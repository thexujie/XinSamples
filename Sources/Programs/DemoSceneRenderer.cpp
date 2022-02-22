#include "PCH.h"
#include "DefferedSceneRenderer.h"

#include "Engine.h"
#include "RenderGraph.h"
#include "SSAO.h"
#include "PostProcessRenderer.h"
#include "RenderScene.h"
#include "RenderView.h"
#include "PostProcesses/FXAA.h"

namespace Xin::Engine
{
	enum class EVisualBuffer
	{
		SceneOutput = 0,

		BaseColor,
		BaseNormal,
		SceneDepth,

		SSAOOcclusion,
		LinearDepth,
		ShadowDepth,
		FXAA,

		Custom0,
		Custom1,
		Custom2,
		Custom3,
		Custom4,
		Custom5,
		Custom6,
		Custom7,
		Custom8,
		Custom9,
	};

	TArray<IRHIShader *, ShaderCount> FromShaders(TArray<FShader *, ShaderCount> Shaders)
	{
		TArray<IRHIShader *, ShaderCount> Result;
		for (size_t Index = 0; Index < ShaderCount; ++Index)
			Result[Index] = Shaders[Index] ? Shaders[Index]->RHIShader.Get() : nullptr;
		return Result;
	}

	class FMeshCommandCollector
	{
	public:
		FMeshCommandCollector(FRenderGraph & RenderGraph) : RenderGraph(RenderGraph) {}
		virtual ~FMeshCommandCollector() = default;

		virtual TArray<FShader *, ShaderCount> CollectShaders(const FMeshBatch & MeshBatch, const FMeshBatchElement & MeshBatchElement, EMeshPass MeshPass)
		{
			TArray<FShader *, ShaderCount> Shaders = InterpretCastRef<const FMaterial>(MeshBatchElement.Material).GetShaders(InterpretCastRef<const FMeshAssembler>(MeshBatch.MeshAssember), MeshPass);
			return Shaders;
		}

		void CollectMeshCommands(EMeshPass MeshPass, FScene & Scene,
			FResource * GlobalParameter,
			FResource * ViewParameter,
			FRGResource * Occlusion,
			FRGResource * ShadowDepth,
			const FRasterizeState & RasterizeState,
			const FDepthStencilState & DepthStencilState)
		{
			for (size_t PrimitiveIndex = 0; PrimitiveIndex < Scene.Primitives.Size; ++PrimitiveIndex)
			{
				IRenderPrimitive & RenderPrimitive = Scene.Primitives[PrimitiveIndex].Ref();
				FPrimitiveParameter PrimitiveParameterData;
				PrimitiveParameterData.LocalToWorld = RenderPrimitive.GetWorldMatrix();
				FResource * PrimitiveParameter = RenderGraph.CreateShaderParameterT(PrimitiveParameterData);

				for (const FMeshBatch & MeshBatch : RenderPrimitive.MeshBatchs)
				{
					FMeshGeometry MeshGeometry;
					MeshBatch.RenderMesh->GetMeshGeometry(MeshPass, MeshGeometry);

					for (size_t ElementIndex : IRange(MeshBatch.Elements.Size))
					{
						const FMeshBatchElement & MeshBatchElement = MeshBatch.Elements[ElementIndex];

						TArray<FShader *, ShaderCount> Shaders = CollectShaders(MeshBatch, MeshBatchElement, MeshPass);
						FMeshRenderState MeshRenderState;
						{
							MeshRenderState.Shaders = FromShaders(Shaders);
							if (MeshPass == EMeshPass::Depth)
							{
								MeshRenderState.InputElements[0] = MeshBatch.InputElements[0];
								MeshRenderState.InputElements[1] = MeshBatch.InputElements[1];
							}
							else
								MeshRenderState.InputElements = MeshBatch.InputElements;
							MeshRenderState.PrimitiveType = EPrimitiveType::Triangle;
							MeshRenderState.RasterizeState = RasterizeState;
							MeshRenderState.BlendState = FBlendState::Default;
							MeshRenderState.DepthStencilState = DepthStencilState;
						}

						FVisibleMeshCommand & MeshCommand = MeshCommands.Add(EInitializeMode::Zero);
						MeshCommand.Initialize(RenderGraph.Engine.PipelineStateCache, MeshRenderState);

						if (GlobalParameter)
							MeshCommand.RenderResources.SetResource(u8"GlobalParameter"S, GlobalParameter);

						if (Occlusion)
							MeshCommand.RenderResources.SetResource(u8"Occlusion"S, Occlusion);

						if (ShadowDepth)
							MeshCommand.RenderResources.SetResource(u8"ShadowDepth"S, ShadowDepth);

						if (ViewParameter)
							MeshCommand.RenderResources.SetResource(u8"ViewParameter"S, ViewParameter);

						MeshCommand.RenderResources.SetResource(u8"PrimitiveParameter"S, PrimitiveParameter);

						const FMaterial & Material = InterpretCastRef<const FMaterial>(MeshBatchElement.Material);
						const FMeshAssembler & MeshAssembler = InterpretCastRef<const FMeshAssembler>(MeshBatch.MeshAssember);

						//MeshAssembler.GetParameters(MeshCommand.Resources);
						Material.GetParameters(MeshCommand.RenderResources);

						//for (const FRenderResources::FResource & Resource : MeshCommand.Resources.Resources)
						//	AssertExpresion(Resource.RHIResource);

						MeshCommand.PrimitiveParameter = MeshBatch.PrimitiveParameter;

						MeshCommand.Topology = MeshGeometry.Topology;
						MeshCommand.VertexBuffers = MeshGeometry.VertexBuffers;
						MeshCommand.VertexStrides = MeshGeometry.VertexStrides;
						MeshCommand.VertexBufferCount = MeshGeometry.VertexBufferCount;
						MeshCommand.VertexCount = MeshGeometry.VertexCount;
						MeshCommand.IndexBuffer = MeshGeometry.IndexBuffer;
						MeshCommand.IndexFormat = MeshGeometry.IndexFormat;
						MeshCommand.IndexCount = MeshGeometry.IndexCount;

						if (MeshBatchElement.IndexCount > 0)
						{
							MeshCommand.IndexedInstanced.VertexOffset = MeshBatchElement.VertexOffset;
							MeshCommand.IndexedInstanced.IndexOffset = MeshBatchElement.IndexOffset;
							MeshCommand.IndexedInstanced.IndexCount = MeshBatchElement.IndexCount;
						}
						else
						{
							MeshCommand.Instanced.VertexOffset = MeshBatchElement.VertexOffset;
							MeshCommand.Instanced.VertexCount = MeshBatchElement.VertexCount;
						}
#ifdef XIN_DEBUG
						MeshCommand.RenderPrimitive = &RenderPrimitive;
						MeshCommand.MeshAssembler = MeshBatch.MeshAssember;
						MeshCommand.Material = MeshBatchElement.Material;
#endif
					}
				}
			}

			//std::sort(PassMeshCommands.Begin(), PassMeshCommands.End(), [](const FVisibleMeshCommand & A, const FVisibleMeshCommand & B) { return A.MeshRenderStateIndex < B.MeshRenderStateIndex; });
		}

	public:
		FRenderGraph & RenderGraph;
		TList<FVisibleMeshCommand> MeshCommands;
	};

	class FPreZMeshCommandCollector : public FMeshCommandCollector
	{
	public:
		FPreZMeshCommandCollector(FRenderGraph & RenderGraph)
			: FMeshCommandCollector(RenderGraph)
		{
		}

		//TArray<FShader *, ShaderCount> CollectShaders(const FMeshBatch & MeshBatch, const FMeshBatchElement & MeshBatchElement, EMeshPass MeshPass) override
		//{
		//	TArray<FShader *, ShaderCount> Shaders;
		//	Shaders[STI_Vertex] = RenderGraph.Engine.Resources.DepthVS.Get();
		//	//Shaders[STI_Pixel] = RenderGraph.Engine.BuiltinResources.DepthPS.Get();
		//	//Shaders[STI_Pixel] = nullptr;
		//	return Shaders;
		//}

	private:
	};

	class FBasePassMeshCommandCollector : public FMeshCommandCollector
	{
	public:
		FBasePassMeshCommandCollector(FRenderGraph & RenderGraph)
			: FMeshCommandCollector(RenderGraph)
		{

		}
	};

	class FShadowDepthMeshCommandCollector : public FMeshCommandCollector
	{
	public:
		FShadowDepthMeshCommandCollector(FRenderGraph & RenderGraph)
			: FMeshCommandCollector(RenderGraph)
		{
		}

		//TArray<FShader *, ShaderCount> CollectShaders(const FMeshBatch & MeshBatch, const FMeshBatchElement & MeshBatchElement, EMeshPass MeshPass) override
		//{
		//	TArray<FShader *, ShaderCount> Shaders;
		//	Shaders[STI_Vertex] = RenderGraph.Engine.Resources.DepthVS.Get();
		//	//Shaders[STI_Pixel] = Engine.BuiltinResources.DepthPS.Get();
		//	return Shaders;
		//}

	private:
		FShaderInstanceRef PixelShader;
	};

	FDefferedSceneRenderer::FDefferedSceneRenderer(FEngine & Engine)
		: Engine(Engine)
	{
		TransientAllocators.Resize(Engine.BackBufferCount);
		for (uint32 BackBufferIndex = 0; BackBufferIndex < Engine.BackBufferCount; ++BackBufferIndex)
		{
			TransientAllocators[BackBufferIndex] = MakeRefer<FTransientAllocator>(Engine);
			TransientAllocators[BackBufferIndex]->SetName(Format(u8"TransientAllocator{}"S, BackBufferIndex));
		}

		if (!SceneExposure)
		{
			//ExpVar Exposure("Graphics/HDR/Exposure", 2.0f, -8.0f, 8.0f, 0.25f);
			float InitialExposure = 2.0f;

			float LuminanceMin = 0.0001f;
			float LuminanceMax = 16.0f;
			float LuminanceMinLog = log2(LuminanceMin);
			float LuminanceMaxLog = log2(LuminanceMax);

			float InitExposure[8] =
			{
				InitialExposure, 1.0f / InitialExposure, InitialExposure, 0.0f,
				LuminanceMinLog, LuminanceMaxLog, LuminanceMaxLog - LuminanceMinLog, 1.0f / (LuminanceMaxLog - LuminanceMinLog)
			};
			SceneExposure = Engine.Allocator.CreateResource({ { 8, sizeof(float1) }, EResourceUsage::ShaderResource | EResourceUsage::UnorderedAccess, EFormat::None, EResourceAccess::UnorderedAccess }, ViewDataContent(InitExposure));
		}
	}

	void FDefferedSceneRenderer::Render(FRenderGraph & RenderGraph, FResource * BackBuffer)
	{
		//static FString PreviewImageFilePath = u8"Data/CharlizeTheron.jfif"S;
		static FString PreviewImageFilePath;
		static bool PreviewImageFilePathChanged = true;
		static uint32_t ShadowDepthSize = 1024;
		static int ShadowDepthBias = -200;
		static float ShadowDepthBiasClamp = 0.0f;
		static float ShadowSlopDepthBias = -1.5f;
		static bool GussianLinearSample = false;
		static EVisualBuffer VisualBuffer = EVisualBuffer::SceneOutput;

		static float ZoomRatio = 1.0f;
		static float2 ZoomPosition = { 0.5f, 0.5f };

		static TkeybordInteractiveDebug DebugValue([&](FKey Key)
			{
				switch (Key.KeyCode)
				{
				case EKeyCode::SemiColon:
					GussianLinearSample = !GussianLinearSample;
					break;
				case EKeyCode::Numpad0:
					VisualBuffer = EVisualBuffer::Custom0;
					break;
				case EKeyCode::Numpad1:
					VisualBuffer = EVisualBuffer::Custom1;
					break;
				case EKeyCode::Numpad2:
					VisualBuffer = EVisualBuffer::Custom2;
					break;
				case EKeyCode::Numpad3:
					VisualBuffer = EVisualBuffer::Custom3;
					break;
				case EKeyCode::Numpad4:
					VisualBuffer = EVisualBuffer::Custom4;
					break;
				case EKeyCode::Numpad5:
					VisualBuffer = EVisualBuffer::Custom5;
					break;
				case EKeyCode::Numpad6:
					VisualBuffer = EVisualBuffer::Custom6;
					break;
				case EKeyCode::Numpad7:
					VisualBuffer = EVisualBuffer::Custom7;
					break;
				case EKeyCode::Numpad8:
					VisualBuffer = EVisualBuffer::Custom8;
					break;
				case EKeyCode::Numpad9:
					VisualBuffer = EVisualBuffer::Custom9;
					break;

				case EKeyCode::Num1:
					VisualBuffer = EVisualBuffer::SceneOutput;
					break;
				case EKeyCode::Num2:
					VisualBuffer = EVisualBuffer::BaseColor;
					break;
				case EKeyCode::Num3:
					VisualBuffer = EVisualBuffer::BaseNormal;
					break;
				case EKeyCode::Num4:
					VisualBuffer = EVisualBuffer::SceneDepth;
					break;
				case EKeyCode::Num5:
					VisualBuffer = EVisualBuffer::SSAOOcclusion;
					break;
				case EKeyCode::Num6:
					VisualBuffer = EVisualBuffer::LinearDepth;
					break;
				case EKeyCode::Num7:
					VisualBuffer = EVisualBuffer::ShadowDepth;
					break;
				case EKeyCode::V:
					break;
				//case EKeyCode::Up:
				//	if (Key.ControlKey * EControlKey::Shift)
				//		ShadowDepthBiasClamp = Clamp(ShadowDepthBiasClamp + 100.0f, -2000.0f, 0.0f);
				//	else if (Key.ControlKey * EControlKey::Alt)
				//		ShadowDepthBias = Clamp(ShadowDepthBias + 100, -500, 0);
				//	else
				//		ShadowSlopDepthBias = Clamp(ShadowSlopDepthBias + 0.5f, -4.0f, 0.0f);
				//	break;
				//case EKeyCode::Down:
				//	if (Key.ControlKey * EControlKey::Shift)
				//		ShadowDepthBiasClamp = Clamp(ShadowDepthBiasClamp - 100.0f, -2000.0f, 0.0f);
				//	else if (Key.ControlKey * EControlKey::Alt)
				//		ShadowDepthBias = Clamp(ShadowDepthBias - 100, -500, 0);
				//	else
				//		ShadowSlopDepthBias = Clamp(ShadowSlopDepthBias - 0.5f, -4.0f, 0.0f);
				//	break;
				//case EKeyCode::Left:
				//	ShadowDepthSize = Clamp(ShadowDepthSize / 2, 256u, 2048u);
				//	break;
				//case EKeyCode::Right:
				//	ShadowDepthSize = Clamp(ShadowDepthSize * 2, 256u, 2048u);
				//	break;

				case EKeyCode::Z:
				case EKeyCode::Left:
				case EKeyCode::Right:
				case EKeyCode::Up:
				case EKeyCode::Down:
				{
					switch(Key.KeyCode)
					{
					case EKeyCode::Z:
						if (Key.ControlKey * EControlKey::Shift)
							ZoomRatio = Clamp(ZoomRatio / 1.1f, 0.01f, 100.0f);
						else if (Key.ControlKey * EControlKey::Ctrl)
							ZoomRatio = 1.0f;
						else
							ZoomRatio = Clamp(ZoomRatio * 1.1f, 0.01f, 100.0f);
						break;
					case EKeyCode::Left:
						ZoomPosition.X = ZoomPosition.X - 0.1f / ZoomRatio;
						break;
					case EKeyCode::Right:
						ZoomPosition.X = ZoomPosition.X + 0.1f / ZoomRatio;
						break;
					case EKeyCode::Up:
						ZoomPosition.Y = ZoomPosition.Y - 0.1f / ZoomRatio;
						break;
					case EKeyCode::Down:
						ZoomPosition.Y = ZoomPosition.Y + 0.1f / ZoomRatio;
						break;
					default:
						break;
					}

					if (ZoomRatio <= 1.0f)
						ZoomPosition = {0.5f, 0.5f};
					else
					{
						float MinTexcoord = -(ZoomRatio - 1.0f);
						float MaxTexcoord = 0.0f;
						ZoomPosition.X = Clamp(ZoomPosition.X, MinTexcoord + ZoomRatio * 0.5f, MaxTexcoord + ZoomRatio * 0.5f);
						ZoomPosition.Y = Clamp(ZoomPosition.Y, MinTexcoord + ZoomRatio * 0.5f, MaxTexcoord + ZoomRatio * 0.5f);
					}
					break;
				}
				case EKeyCode::L:
					PreviewImageFilePath = FPath::OpenFileDialog(u8"选择一个图像文件"S, u8"图像文件\0*.*"S);
					PreviewImageFilePathChanged = true;
					break;
				default:
					break;
				}
			});

		RectI ViewRect = RenderGraph.Scene.Viewport.ViewRect;

		FViewParameter ViewParameterData;
		ViewParameterData.ViewMatrix = RenderGraph.Scene.Viewport.ViewMatrix;
		ViewParameterData.ProjectionMatrix = RenderGraph.Scene.Viewport.ProjectionMatrix;
		ViewParameterData.ViewProjectionMatrix = RenderGraph.Scene.Viewport.ViewProjectionMatrix;
		FResource * ViewParameter = RenderGraph.CreateShaderParameterT(ViewParameterData);

		SizeU SceneSize = BackBuffer->Desc.Size;

		FRGBuffer * Exposure = RenderGraph.RegisterBuffer(u8"SceneExposure"S, SceneExposure.Get());
		FRGTexture * ViewOutput = RenderGraph.RegisterTexture(u8"ViewOutput"S, BackBuffer, EResourceAccess::Present, EResourceAccess::Present);
		FRGTexture * BaseNormal = RenderGraph.CreateResource(u8"BaseNormal"S, { EFormat::R11G11B10F, SceneSize, Colors::AliceBlue });
		FRGTexture * BaseColor = RenderGraph.CreateResource(u8"BaseColor"S, { EFormat::R16G16B16A16F, SceneSize, Colors::AliceBlue });
		FRGTexture * SceneDepth = RenderGraph.CreateResource(u8"SceneDepth"S, { EFormat::R32, SceneSize, Engine.ReversedZ ? 0.0f : 1.0f });
		FRGTexture * ShadowDepth = RenderGraph.CreateResource(u8"ShadowDepth"S, { EFormat::D16, { ShadowDepthSize, ShadowDepthSize }, Engine.ReversedZ ? 0.0f : 1.0f });
		FRGTexture * SceneZoomed = RenderGraph.CreateResource(u8"SceneZoomed"S, { EFormat::R16G16B16A16F, SceneSize, Colors::AliceBlue });
		FRGTexture * SceneOutput = RenderGraph.CreateResource(u8"SceneOutput"S, { EFormat::R16G16B16A16F, SceneSize, Colors::AliceBlue });

		FSSAO SSAORenderer { RenderGraph };
		FPostProcessRenderer PostProcessRenderer { RenderGraph };

		FShadowDepthMeshCommandCollector ShadowDepthMeshCommandCollector { RenderGraph };
		FPreZMeshCommandCollector PreZMeshCommandCollector { RenderGraph };
		FBasePassMeshCommandCollector BasePassMeshCommandCollector { RenderGraph };

		FGlobalParameter GlobalParameterData;

		{
			GlobalParameterData.ShadowViewMatrix = RenderGraph.Scene.MainLight.ViewMatrix;
			GlobalParameterData.ShadowProjectionMatrix = RenderGraph.Scene.MainLight.ProjectionMatrix * FMatrix::Transform({ 0.5f, 0.5f, 0.0f }, FQuat::Identity, { 0.5f, -0.5f, 1.0f });
			GlobalParameterData.ShadowViewProjectionMatrix = RenderGraph.Scene.MainLight.ViewMatrix * RenderGraph.Scene.MainLight.ProjectionMatrix * FMatrix::Transform({ 0.5f, 0.5f, 0.0f }, FQuat::Identity, { 0.5f, -0.5f, 1.0f });
			GlobalParameterData.ViewPosition = RenderGraph.Scene.Viewport.ViewPosition;
			GlobalParameterData.LightDirection = -RenderGraph.Scene.MainLight.ViewDirection;
			GlobalParameterData.SunColor = { 8.0, 8.0, 8.0, 1.0 };
			GlobalParameterData.AmbientColor = { 0.2, 0.2, 0.2, 1.0 };
			GlobalParameterData.ShadowTexelSize = 1.0f / float32(ShadowDepthSize);
			GlobalParameterData.ShadowDepthBias = ShadowDepthBias * 0.01f;
		}
		FResource * GlobalParameter = RenderGraph.CreateShaderParameterT(GlobalParameterData);

		const FDepthStencilState DefaultDepthStencilState = Engine.ReversedZ ? FDepthStencilState::DefaultReversedZ : FDepthStencilState::Default;

		FRasterizeState ShadowDepthRasterizeState;
		ShadowDepthRasterizeState.DepthBias = ShadowDepthBias * (Engine.ReversedZ ? 1 : -1);
		ShadowDepthRasterizeState.DepthBiasClamp = ShadowDepthBiasClamp / 65535.0f * (Engine.ReversedZ ? 1 : -1);
		ShadowDepthRasterizeState.SlopScaledDepthBias = ShadowSlopDepthBias * (Engine.ReversedZ ? 1 : -1);

		FViewParameter LightViewParameterData;
		LightViewParameterData.ViewMatrix = RenderGraph.Scene.MainLight.ViewMatrix;
		LightViewParameterData.ProjectionMatrix = RenderGraph.Scene.MainLight.ProjectionMatrix;
		LightViewParameterData.ViewProjectionMatrix = RenderGraph.Scene.MainLight.ViewProjectionMatrix;
		FResource * LightViewParameter = RenderGraph.CreateShaderParameterT(LightViewParameterData);
		ShadowDepthMeshCommandCollector.CollectMeshCommands(EMeshPass::Depth, RenderGraph.Scene, nullptr, LightViewParameter, nullptr, nullptr, ShadowDepthRasterizeState, DefaultDepthStencilState);

		BasePassMeshCommandCollector.CollectMeshCommands(EMeshPass::Base, RenderGraph.Scene, GlobalParameter, ViewParameter, SSAORenderer.SSAOOcclusion, ShadowDepth,
			Engine.WireFrameRender ? FRasterizeState::DefaultWireframe : FRasterizeState::Default, FDepthStencilState(EComparison::Equal, false));
		PreZMeshCommandCollector.CollectMeshCommands(EMeshPass::Depth, RenderGraph.Scene, nullptr, ViewParameter, nullptr, nullptr, FRasterizeState::Default, DefaultDepthStencilState);

		RenderGraph.AddPass(u8"PreZ"S,
			[&](FCompileContext & CompileContext)
			{
				CompileContext.AccessResource(SceneDepth, EResourceAccess::DepthWrite, ERGResourceLoad::Clear);
			},
			[&](FCommandContext & CommandContext)
			{
				CommandContext.SetViewPorts({ FViewport { 0.0f, 0.0f, (float32_t)ViewRect.Width, (float32_t)ViewRect.Height, 0.0f, 1.0f } });
				CommandContext.SetScissorRects({ RectI { 0, 0, (int32_t)ViewRect.Width, (int32_t)ViewRect.Height } });
				CommandContext.SetRenderTargets({}, SceneDepth);
				CommandContext.DispatchMeshCommands(PreZMeshCommandCollector.MeshCommands);
			});
		SSAORenderer.Render(SceneDepth);

		RenderGraph.AddPass(u8"Shadow Depth"S,
			[&](FCompileContext & CompileContext)
			{
				CompileContext.AccessResource(ShadowDepth, EResourceAccess::DepthWrite, ERGResourceLoad::Clear);
			},
			[&](FCommandContext & CommandContext)
			{
				CommandContext.SetViewPorts({ FViewport { 0.0f, 0.0f, (float32_t)ShadowDepthSize, (float32_t)ShadowDepthSize, 0.0f, 1.0f } });
				CommandContext.SetScissorRects({ RectI { 0, 0, (int32_t)ShadowDepthSize, (int32_t)ShadowDepthSize } });

				CommandContext.SetRenderTargets({}, ShadowDepth);
				CommandContext.DispatchMeshCommands(ShadowDepthMeshCommandCollector.MeshCommands);
			});

		RenderGraph.AddPass(u8"Base Pass"S,
			[&](FCompileContext & CompileContext)
			{
				CompileContext.AccessResource(BaseColor, EResourceAccess::RenderTarget, ERGResourceLoad::Clear);
				CompileContext.AccessResource(BaseNormal, EResourceAccess::RenderTarget, ERGResourceLoad::Clear); // Discard for SRV is invalid.
				CompileContext.AccessResource(ShadowDepth, EResourceAccess::PixelShaderResource, ERGResourceLoad::None);
				CompileContext.AccessResource(SceneDepth, EResourceAccess::DepthRead, ERGResourceLoad::None);
				if (Engine.SSAOMode != ESSAOMode::Disabled)
					CompileContext.AccessResource(SSAORenderer.SSAOOcclusion, EResourceAccess::PixelShaderResource);
			},
			[&](FCommandContext & CommandContext)
			{
				CommandContext.SetViewPorts({ FViewport { 0.0f, 0.0f, (float32_t)ViewRect.Width, (float32_t)ViewRect.Height, 0.0f, 1.0f } });
				CommandContext.SetScissorRects({ RectI { 0, 0, (int32_t)ViewRect.Width, (int32_t)ViewRect.Height } });

				CommandContext.SetRenderTargets({ BaseColor, BaseNormal }, SceneDepth);
				CommandContext.DispatchMeshCommands(BasePassMeshCommandCollector.MeshCommands);
			});

		FRGTexture * FinalOutput = BaseColor;
		FRGResourceView * FinalOutputSRV = nullptr; // Default use FinalOutput->RHITexture->SRView
		FShader * FinalShader = Engine.Resources.HDR2SDR->GetShader({});
		FResource * ShaderParameter = nullptr;
		EBlitTextureFlag BlitTextureFlag = EBlitTextureFlag::Fill;

		if (VisualBuffer != EVisualBuffer::BaseColor)
		{
			if (Engine.PostProcessMode != EPostProcessMode::Disabled)
				PostProcessRenderer.Render(BaseColor);

			if (Engine.FXAAMode != EFXAAMode::Disabled)
			{
				FFXAA FXAA(RenderGraph, (VisualBuffer == EVisualBuffer::FXAA) ? EFXAAFlag::Debug : EFXAAFlag::None);
				FXAA.Render(BaseColor, SceneOutput);
				FinalOutput = SceneOutput;
			}
		}

		FRGTexture * PreviewImage = nullptr;
		if (PreviewImageFilePath.Size)
		{
			FTextureRef PreviewImageTexture = Engine.Resources.LoadTexture(PreviewImageFilePath);
			if (PreviewImageTexture)
			{
				PreviewImageTexture->UpdateRHI();
				PreviewImage = RenderGraph.RegisterTexture(u8"PreviewImage"S, PreviewImageTexture->TextureResource.Get());
				RenderGraph.BlitTextureT(FinalOutput, PreviewImage, Engine.Resources.GussainBlur->GetShader(), float2(1.0f / PreviewImage->Desc.Size.X, 0), EBlitTextureFlag::PassName | EBlitTextureFlag::Center);
			}
		}

		switch (VisualBuffer)
		{
		case EVisualBuffer::SceneOutput:
			break;
		case EVisualBuffer::BaseColor:
			FinalOutput = BaseColor;
			break;
		case EVisualBuffer::BaseNormal:
			FinalOutput = BaseNormal;
			FinalShader = Engine.Resources.RGBAVisualizePS.Get();
			break;
		case EVisualBuffer::SceneDepth:
			FinalOutput = SceneDepth;
			FinalShader = Engine.Resources.ZndcVisualizePS.Get();
			//ShaderParameter = RenderGraph.CreateShaderParameterT(EResourceUsage::Dynamic, float2 { 0.1f, 3000.0f });
			ShaderParameter = RenderGraph.CreateShaderParameterT(float2 { /*3000.0f*/400.0f, 0.1f });
			break;
		case EVisualBuffer::SSAOOcclusion:
			FinalOutput = SSAORenderer.SSAOOcclusion;
			FinalShader = Engine.Resources.RRR1VisualizePS.Get();
			break;
		case EVisualBuffer::LinearDepth:
			FinalOutput = SSAORenderer.SSAODepth1x;
			FinalShader = Engine.Resources.RRR1VisualizePS.Get();
			break;
		case EVisualBuffer::ShadowDepth:
			FinalOutput = ShadowDepth;
			FinalShader = Engine.Resources.RRR1VisualizePS.Get();
			BlitTextureFlag = EBlitTextureFlag::Center;
			break;
		case EVisualBuffer::FXAA:
			FinalOutput = SSAORenderer.SSAODepthTiled8x;
			FinalOutputSRV = RenderGraph.CreateResourceView(SSAORenderer.SSAODepthTiled8x, EResourceView::ShaderResource, 15);
			FinalShader = Engine.Resources.RRR1VisualizePS.Get();
			break;
		case EVisualBuffer::Custom0:
			if (Engine.BloomMode != EBloomMode::Disabled)
			{
				FinalOutput = PostProcessRenderer.Bloom.BloomLuminance;
				FinalOutputSRV = RenderGraph.CreateResourceView(PostProcessRenderer.Bloom.BloomLuminance, { EResourceView::ShaderResource, EFormat::R8UF });
			}
			else
			{
				FinalOutput = PostProcessRenderer.SceneLuminance;
				FinalOutputSRV = RenderGraph.CreateResourceView(PostProcessRenderer.SceneLuminance, { EResourceView::ShaderResource, EFormat::R8UF });
			}
			FinalShader = Engine.Resources.RRR1VisualizePS.Get();
			break;
		case EVisualBuffer::Custom1:
			FinalOutput = SSAORenderer.SSAOOcclusionMerged2x;
			FinalShader = Engine.Resources.RRR1VisualizePS.Get();
			break;
		case EVisualBuffer::Custom2:
			FinalOutput = SSAORenderer.SSAODepth2x;
			FinalShader = Engine.Resources.RRR1VisualizePS.Get();
			break;
		case EVisualBuffer::Custom3:
			FinalOutput = SSAORenderer.SSAODepthTiled8x;
			FinalOutputSRV = RenderGraph.CreateResourceView(SSAORenderer.SSAODepthTiled8x, EResourceView::ShaderResource, 0);
			FinalShader = Engine.Resources.RRR1VisualizePS.Get();
			break;
		case EVisualBuffer::Custom4:
			FinalOutput = SSAORenderer.SSAODepthTiled8x;
			FinalOutputSRV = RenderGraph.CreateResourceView(SSAORenderer.SSAODepthTiled8x, EResourceView::ShaderResource, 12);
			FinalShader = Engine.Resources.RRR1VisualizePS.Get();
			break;
		case EVisualBuffer::Custom5:
			FinalOutput = SSAORenderer.SSAODepthTiled8x;
			FinalOutputSRV = RenderGraph.CreateResourceView(SSAORenderer.SSAODepthTiled8x, EResourceView::ShaderResource, 13);
			FinalShader = Engine.Resources.RRR1VisualizePS.Get();
			break;
		case EVisualBuffer::Custom6:
		{
			FRGTexture * SceneOutputB = RenderGraph.CreateResource(u8"SceneOutputB"S, { EFormat::R16G16B16A16F, SceneSize, Colors::AliceBlue });
			FShaderPermutation ShaderPermutation;
			ShaderPermutation.Defines.Add({ u8"XIN_BLUR_GUSSAIN_LINEAR_SAMPLE"S, u8"1"S });
			RenderGraph.BlitTextureT(SceneOutputB, FinalOutput, Engine.Resources.GussainBlur->GetShader(ShaderPermutation), float2(1.0f / FinalOutput->Desc.Size.X, 0), EBlitTextureFlag::PassName);
			RenderGraph.BlitTextureT(SceneOutput, SceneOutputB, Engine.Resources.GussainBlur->GetShader(ShaderPermutation), float2(0, 1.0f / SceneOutputB->Desc.Size.Y), EBlitTextureFlag::PassName);
			FinalOutput = SceneOutput;
			break;
		}
		case EVisualBuffer::Custom7:
		{
			FRGTexture * SceneOutputB = RenderGraph.CreateResource(u8"SceneOutputB"S, { EFormat::R16G16B16A16F, SceneSize, Colors::AliceBlue });
			//FShaderPermutation ShaderPermutation;
			//RenderGraph.BlitTextureT(SceneOutputB, FinalOutput, Engine.Resources.BuiltinResources.GussainBlur->GetShader(ShaderPermutation), float2(1.0f / FinalOutput->Desc.Size.X, 0), EBlitTextureFlag::PassName);
			//RenderGraph.BlitTextureT(SceneOutput, SceneOutputB, Engine.Resources.BuiltinResources.GussainBlur->GetShader(ShaderPermutation), float2(0, 1.0f / SceneOutputB->Desc.Size.Y), EBlitTextureFlag::PassName);
			//FinalOutput = SceneOutput;

			float SigmaS = 6;
			float FilterSigmaS = 1.0f / (2 * SigmaS * SigmaS);
			RenderGraph.BlitTextureT(SceneOutputB, FinalOutput, Engine.Resources.LoadShader(u8"Shaders/GussainFilter.hlsl"S, EShaderType::Pixel).Get(), float3(1.0f / FinalOutput->Desc.Size.X, 0, FilterSigmaS), EBlitTextureFlag::PassName);
			RenderGraph.BlitTextureT(SceneOutput, SceneOutputB, Engine.Resources.LoadShader(u8"Shaders/GussainFilter.hlsl"S, EShaderType::Pixel).Get(), float3(0, 1.0f / FinalOutput->Desc.Size.Y, FilterSigmaS), EBlitTextureFlag::PassName);
			break;
		}
		case EVisualBuffer::Custom8:
		{
			FRGTexture * SceneOutputB = RenderGraph.CreateResource(u8"SceneOutputB"S, { EFormat::R16G16B16A16F, SceneSize, Colors::AliceBlue });
			// GussainFilter.hlsl
			//float SigmaS = 6;
			//float FilterSigmaS = 1.0f / (2 * SigmaS * SigmaS);
			//RenderGraph.BlitTextureT(SceneOutputB, FinalOutput, Engine.Resources.LoadShader(u8"Shaders/GussainFilter.hlsl"S, EShaderType::Pixel).Get(), float3(1.0f / FinalOutput->Desc.Size.X, 0, FilterSigmaS), EBlitTextureFlag::PassName);
			//RenderGraph.BlitTextureT(SceneOutput, SceneOutputB, Engine.Resources.LoadShader(u8"Shaders/GussainFilter.hlsl"S, EShaderType::Pixel).Get(), float3(0, 1.0f / FinalOutput->Desc.Size.Y, FilterSigmaS), EBlitTextureFlag::PassName);

			// GussainFilterCS.hlsl
			float SigmaS = 6;
			float FilterSigmaS = 1.0f / (2 * SigmaS * SigmaS);
			RenderGraph.ComputeTextureT(SceneOutputB, FinalOutput, Engine.Resources.LoadShader(u8"Shaders/GussainFilterCS.hlsl"S, EShaderType::Compute).Get(), float3(1.0f / FinalOutput->Desc.Size, FilterSigmaS), EBlitTextureFlag::PassName);
			FinalOutput = SceneOutputB;

			// BilateralFilterCS.hlsl
			//int FilterSize = 5;
			//float SigmaS = 6;
			//float SigmaR = 8 / 255.0;
			//float FilterSigmaS = 1.0f / (2 * SigmaS * SigmaS);
			//float FilterSigmaR = 1.0f / (2 * SigmaR * SigmaR);
			//RenderGraph.ComputeTextureT(SceneOutputB, FinalOutput, Engine.Resources.LoadShader(u8"Shaders/BilateralFilterCS.hlsl"S, EShaderType::Compute).Get(), float4(1.0f / FinalOutput->Desc.Size.XY, FilterSigmaS, FilterSigmaR), EBlitTextureFlag::PassName);
			//FinalOutput = SceneOutputB;
			break;
		}
		case EVisualBuffer::Custom9:
		{
			FRGTexture * SceneOutputB = RenderGraph.CreateResource(u8"SceneOutputB"S, { EFormat::R16G16B16A16F, SceneSize, Colors::AliceBlue });
			float SigmaS = 6;
			float FilterSigmaS = 1.0f / (2 * SigmaS * SigmaS);
			RenderGraph.ComputeTextureT(SceneOutputB, FinalOutput, Engine.Resources.LoadShader(u8"Shaders/GussainFilterWaveCS.hlsl"S, EShaderType::Compute).Get(), float3(1.0f / FinalOutput->Desc.Size, FilterSigmaS), EBlitTextureFlag::PassName);
			FinalOutput = SceneOutputB;

			//int FilterSize = 5;
			//float SigmaS = 6;
			//float SigmaR = 8 / 255.0;
			//float FilterSigmaS = 1.0f / (2 * SigmaS * SigmaS);
			//float FilterSigmaR = 1.0f / (2 * SigmaR * SigmaR);
			//using ParamT = TTuple<float1, float1, float1, float1,uint1>;

			//RenderGraph.BlitTextureT(SceneOutputB, FinalOutput, Engine.LoadShader(u8"Shaders/BilateralFilter.hlsl"S, EShaderType::Pixel).Get(), ParamT(1.0f / FinalOutput->Desc.Size.X, 0.0f, FilterSigmaS, FilterSigmaR, 5), EBlitTextureFlag::PassName);
			//RenderGraph.BlitTextureT(SceneOutput, SceneOutputB, Engine.LoadShader(u8"Shaders/BilateralFilter.hlsl"S, EShaderType::Pixel).Get(), ParamT(0.0f, 1.0f / BaseColor->Desc.Size.Y, FilterSigmaS, FilterSigmaR, 5), EBlitTextureFlag::PassName);
			//FinalOutput = SceneOutput;
			break;
		}
		default:
			break;
		}

		if (ZoomRatio != 1.0f)
		{
			float4 ZoomParameter = {};
			float ZoomRatioRcp = 1.0f / ZoomRatio;
			ZoomParameter.XY = { ZoomPosition.X - ZoomRatioRcp * 0.5f, ZoomPosition.Y - ZoomRatioRcp * 0.5f };
			ZoomParameter.ZW = { ZoomRatioRcp, ZoomRatioRcp };
			RenderGraph.BlitTextureT(SceneZoomed, FinalOutput, Engine.Resources.Zoom->GetShader({}), ZoomParameter, EBlitTextureFlag::PassName);
			FinalOutput = SceneZoomed;
			FinalOutputSRV = RenderGraph.CreateResourceView(SceneZoomed, { EResourceView::ShaderResource });
		}

		RenderGraph.BlitTexture(ViewOutput, FinalOutput, FinalOutputSRV, FinalShader, ShaderParameter, BlitTextureFlag | EBlitTextureFlag::PassName);
		RenderGraph.Execute();
	}
}
