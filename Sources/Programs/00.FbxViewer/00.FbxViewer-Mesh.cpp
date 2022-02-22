#include "pch.h"
#include "00.FbxViewer.h"

#include "Xin.FBX/Xin.FBX.h"


FBX::FFbxModel LoadMeshWithCache(FEngine & Engine, FStringV FbxFilePath)
{
	FString FbxFilePathAbsolute = FPath::GetAbsolutePath(FbxFilePath);
	FString FbxCacheFileName = FPath::GetBaseName(FbxFilePath) + u8"_"V + MD5EncryptToString(FbxFilePathAbsolute) + u8".xmod"V;
	FString CacheDirectory = FPath::Combine(Engine.DocumentsPath, u8"FbxMeshCache"V);

	FDirectory::EnsureExists(CacheDirectory);

	FString CacheFilePath = FPath::Combine(CacheDirectory, FbxCacheFileName);

	if (FFile::CreationTime(CacheFilePath) > FFile::CreationTime(FbxFilePathAbsolute))
	{
		FConsole::WriteLine(u8"Load {} from cache."V, CacheFilePath);
		FFileStream FileStream(CacheFilePath, EFileModes::Input);
		FBX::FFbxModel FbxModel;
		FileStream >> FbxModel;
		return FbxModel;
	}
	else
	{
		FConsole::WriteLine(u8"Prepareing model cache {}."V, CacheFilePath);
		FConsole::WriteLine(u8"Loading {}."V, FbxFilePathAbsolute);
		FBX::FFbxModel FbxModel = FBX::LoadStaticMeshFromFile(FbxFilePathAbsolute);
		FFileStream FileStream(CacheFilePath, EFileModes::Output);
		FileStream << FbxModel;
		return FbxModel;
	}
}

class FStaticMeshVS : public IShaderSource
{
public:
	DEFINE_SHADER_INSTANCE(FStaticMeshVS);

	FStaticMeshVS() : IShaderSource(EShaderStage::Vertex, u8"Shaders/Image.xsf"V, u8"MainVS"V) {}
};

class FStaticMeshPS : public IShaderSource
{
public:
	DEFINE_SHADER_INSTANCE(FStaticMeshPS);

	FStaticMeshPS() : IShaderSource(EShaderStage::Pixel, u8"Shaders/Image.xsf"V, u8"MainPS"V) {}
};

int FbxViewer_Main(FEngine & Engine)
{
	float32 FovY = Float32PI * 0.333333f;

	FCommandExecutor CommandExecutor { Engine };

	Engine.AddAssetsPath(GetProjectAssetsPath());

	HWND WindowHandle = FbxViewer_CreateWindow();

	SizeU ClientSize = Windows::GetClientSize(WindowHandle);

	IRHIViewportRef Viewport = Engine.Device.CreateViewport(CommandExecutor.PrimaryQueue, WindowHandle, ClientSize, NumBackBuffers, EFormat::R8G8B8A8UF, EPresentMode::FlipDiscard);
	IRHIResourceRef DepthStencil = CommandExecutor.CreateTexture({ EFormat::D32F, ClientSize, EResourceUsage::DepthStencil, { 1.0f, 0 } });

	IShaderInstance * VertexShader = Engine.ShaderBundle.GetShaderT<FStaticMeshVS>();
	//VertexShader->SetResourceLabel(u8"MainVS"V);
	IShaderInstance * PixelShader = Engine.ShaderBundle.GetShaderT<FStaticMeshPS>();
	//PixelShader->SetResourceLabel(u8"MainPS"V);

	FPipelineStateDesc PipelineStateDesc;
	PipelineStateDesc.Shaders[EShaderStage::Vertex] = VertexShader->GetShaderModule();
	PipelineStateDesc.Shaders[EShaderStage::Pixel] = PixelShader->GetShaderModule();
	PipelineStateDesc.Topology = ETopology::TriangleList;
	PipelineStateDesc.InputFormat.Streams[0].Stride = sizeof(FBX::FMeshVertex);
	PipelineStateDesc.InputFormat.Elements[0] = { EInputSemantic::Position, 0, EFormat::Float3, 0, 0 };
	PipelineStateDesc.InputFormat.Elements[1] = { EInputSemantic::Texcoord, 0, EFormat::Float2, 0, sizeof(float3) };
	PipelineStateDesc.OutputFormat.RenderTargets[0] = EFormat::R8G8B8A8UF;
	PipelineStateDesc.OutputFormat.DepthStencil = DepthStencil->ResourceDesc.Format;

	PipelineStateDesc.RasterizeState.CullMode = ECullMode::Back;
	PipelineStateDesc.DepthStencilState.DepthTest = true;
	PipelineStateDesc.DepthStencilState.DepthWrite = true;

	PipelineStateDesc.StaticSamplers[0] = { EShaderRelevance::Pixel, 0, 0, ESampleFilter::Linear, EAddressMode::Clamp, EAddressMode::Clamp, EAddressMode::Clamp };

	IRHIPipelineStateRef PipelineState = Engine.Device.CreatePipelineState(PipelineStateDesc);

	IImageRef Image = IImage::Load(u8"../../Data/Cat.jpg"V);
	IRHIResourceRef TextureA = CommandExecutor.CreateTexture( { Image->ImageData.Format, { Image->ImageData.Width, Image->ImageData.Height }, EResourceUsage::SampledTexture | EResourceUsage::CopyWrite }, FTextureImageSource{ Image->ImageData });
	IRHIResourceRef TextureB = CommandExecutor.CreateTexture({ Image->ImageData.Format, { Image->ImageData.Width, Image->ImageData.Height }, EResourceUsage::SampledTexture | EResourceUsage::CopyWrite }, FTextureImageSource { Image->ImageData });

	FVector3 BBoxMin = FVector3::Max;
	FVector3 BBoxMax = FVector3::Min;
	//FBX::FFbxModel FbxModel = LoadMeshWithCache(u8"../../Data/SM_ColorCalibrator.fbx"V);
	FBX::FFbxModel FbxModel = LoadMeshWithCache(Engine, u8"../../Data/SKM_Manny.fbx"V);


	//FBX::FFbxModel FbxModel = LoadMeshWithCache(u8"../../Data/Preview.fbx"V);
	//FBX::FFbxModel FbxModel = LoadMeshWithCache(u8"../../Data/MatineeCam_SM.fbx"V);
	//FBX::FFbxModel FbxModel = LoadMeshWithCache(u8"H:\\Files\\Models\\33-fbx\\fbx\\Wolf_fbx.fbx"V);
	//FBX::FFbxModel FbxModel = LoadMeshWithCache(u8R"(H:\Files\Models\Bistro_v5_1\BistroInterior.fbx)"V);

	FBX::FFbxMesh & FbxMesh = FbxModel.LodMeshs[0];

	for (const FBX::FMeshVertex & MeshVertex : FbxMesh.Vertices)
	{
		BBoxMin = Min(BBoxMin, MeshVertex.Position);
		BBoxMax = Max(BBoxMax, MeshVertex.Position);
	}

	FVector3 BBoxExtent = BBoxMax - BBoxMin;
	float32 BBoxRadius = Max(BBoxExtent.X, BBoxExtent.Y, BBoxExtent.Z);
	FVector3 BBoxCenter = (BBoxMin + BBoxMax) * 0.5f;

	IRHIResourceRef VertexBuffer = CommandExecutor.CreateBuffer(
		FBufferDesc { uint32(sizeof(FBX::FMeshVertex) * FbxMesh.Vertices.Size), EResourceUsage::VertexBuffer | EResourceUsage::CopyWrite },
		ReinterpretView<byte>(FbxMesh.Vertices.View()));

	EIndexFormat IndexFormat = FbxMesh.Indices.Index32 ? EIndexFormat::Index32 : EIndexFormat::Index16;
	IRHIResourceRef IndexBuffer = CommandExecutor.CreateBuffer(
		FBufferDesc { uint32((IndexFormat == EIndexFormat::Index32 ? sizeof(uint32) : sizeof(uint16)) * FbxMesh.Indices.GetNumIndices()), EResourceUsage::IndexBuffer | EResourceUsage::CopyWrite },
		ReinterpretView<byte>(FbxMesh.Indices.GetIndices()));

	//FMatrix WorldMatrix = FMatrix::Scale({ 500, 200, 1 }) * FMatrix::Translate(-400, 0, 0);

	float1 LookDistance = 1.0f / std::sin(FovY * 0.5f) * BBoxRadius * 0.5f;
	float3 LookForward = float3(0, -1, -1);
	float3 LookCenter = { 0, BBoxExtent.Y * 0.5f, 0 };
	float3 LookAt = LookCenter - LookForward * LookDistance;

	FMatrix ViewMatrix = FMatrix::LookAtLH(LookAt, LookCenter, { 0, 1, -1 });
	//FMatrix ViewMatrix = FMatrix::LookAtLH({0, 0, -1}, { 0, 0, 0 }, { 0, 1, 0 });
	//FMatrix ProjectionMatrix = FMatrix::OrthographicLH(ClientSize.Width, ClientSize.Height, 0.01f, 10000.0f);
	FMatrix ProjectionMatrix = FMatrix::PerspectiveLH(FovY, float32(ClientSize.Width) / ClientSize.Height, 0.01f, 10000.0f);
	FMatrix ViewProjectionMatrix = ViewMatrix * ProjectionMatrix;
	//ViewMatrix = FMatrix::Identity;
	IRHIResourceRef ViewParameters = CommandExecutor.CreateBuffer(FBufferDesc { sizeof(FMatrix), EResourceUsage::UniformBuffer | EResourceUsage::HostWrite }, ViewBytes(ViewProjectionMatrix));

	float4 Color = { 1, 1, 1, 1 };
	IRHIResourceRef PixelParameters = CommandExecutor.CreateBuffer(FBufferDesc { sizeof(float4), EResourceUsage::UniformBuffer | EResourceUsage::HostWrite }, ViewBytes(Color));

	//IRHIResourceRef WorldParameters = CommandExecutor.CreateResource(FBufferDesc { sizeof(FMatrix), EResourceUsage::ConstBuffer | EResourceUsage::HostWrite });

	float64 StartTime = FDateTime::Steady();
	float64 LastTime = StartTime;

	FCommandList CommandList { CommandExecutor };
	MSG msg = {};
	FCounter<float32, 3> FpsCounter { 1000 };
	float64 LastTitleUpdateTime = 0;

	FApplication::Instance().Loop([&]
	{
		float64 CurrentTime = FDateTime::Steady();
		float32 DeltaTime = float32(CurrentTime - LastTime);
		LastTime = CurrentTime;
		uint32 BackBufferIndex = Viewport->BeginFrame();
		if (BackBufferIndex == NullIndex)
			return;

		FpsCounter.Accumulate(1);
		if (CurrentTime - LastTitleUpdateTime >= 1)
		{
			FWString Title = Format(L"FPS={:.2f}"V, FpsCounter.Average());
			::SetWindowTextW(WindowHandle, Title.Data);
			LastTitleUpdateTime = CurrentTime;
		}

		IRHIResource * BackBuffer = Viewport->GetBackBuffer(BackBufferIndex);

		CommandExecutor.Update(DeltaTime);
		//Sleep(500);

		{
			float32 RotateAngle = (CurrentTime - StartTime) * Float32PI * 0.5f;
			//RotateAngle = Float32PI * 0.25f;
			//RotateAngle = 0;

			FMatrix WorldMatrix = FMatrix::Translate(-BBoxCenter.X, -BBoxMin.Y, -BBoxCenter.Z) * FMatrix::Scale({ 1, 1, 1 }) * FMatrix::RotateAxis(FVector4::AxisY, RotateAngle);

			//WorldMatrix = FMatrix::Scale({ 200, 200, 200 });
			FRHIBlock WorldParameters = CommandList.AllocShaderParameterT(WorldMatrix);
			//MemoryCopy(WorldParameters->BaseAddress.CPUAddress, &WorldMatrix, sizeof(FMatrix));

			CommandList.ResourceAccess(BackBuffer, EResourceAccess::RenderTarget);
			CommandList.ResourceAccess(DepthStencil, EResourceAccess::DepthWrite);
			{
				CommandList.BeginPass({ { { BackBuffer, EResourceLoad::Clear, Colors::LightSkyBlue } }, { DepthStencil, EResourceLoad::Clear } });

				CommandList.SetPipelineState(PipelineState);

				CommandList.IASetVertexBuffers(0, { VertexBuffer }, { sizeof(FBX::FMeshVertex) }, FbxMesh.Vertices.Size);
				CommandList.IASetIndexBuffer(IndexBuffer, IndexFormat, FbxMesh.Indices.GetNumIndices());

				CommandList.SetParameter(u8"WorldParameters"N, WorldParameters);
				CommandList.SetParameter(u8"ViewParameters"N, ViewParameters);
				CommandList.SetParameter(u8"PixelParameters"N, PixelParameters);
				CommandList.SetParameter(u8"ImageA"N, TextureA);
				CommandList.SetParameter(u8"ImageB"N, TextureB);

				//const FBX::FFbxMeshElement & MeshElement = FbxMesh.Elements[1];
				//CommandList.DrawIndexedInstanced(MeshElement.NumIndices, MeshElement.IndexOffset, MeshElement.BaseVertexIndex);
				for (uintx MeshElementIndex = 0; MeshElementIndex < FbxMesh.Elements.Size; ++MeshElementIndex)
				{
					const FBX::FFbxMeshElement & MeshElement = FbxMesh.Elements[MeshElementIndex];
					CommandList.DrawIndexedInstanced(MeshElement.IndexOffset, MeshElement.IndexCount, MeshElement.VertexOffset);
				}

				CommandList.EndPass();
			}
			CommandList.ResourceAccess(BackBuffer, EResourceAccess::Present);
			CommandList.Flush();

			Viewport->EndFrame();
		}

		Viewport->EndFrame();
	});

	CommandExecutor.WaitFinish();
	return 0;
}
