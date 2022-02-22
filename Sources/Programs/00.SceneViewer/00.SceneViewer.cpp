#include "PCH.h"

#include "00.SceneViewer.Types.h"

#include "Xin.RHI.Vulkan/Xin.RHI.Vulkan.h"
#include "Xin.RHI.D3D12/Xin.RHI.D3D12.h"

#if 0
#include "Xin.RHI/Xin.RHI.h"
#include "Xin.Rendering/Xin.Rendering.h"
#include "Xin.Engine/Xin.Engine.h"
#include "Xin.Direct2D/Xin.Direct2D.h"
#include "Xin.RHI.D3D12/Xin.RHI.D3D12.Private.h"

#include "Xin.Direct2D/Xin.Direct2D.Private.h"
#include "Xin.Skia/Xin.Skia.h"

using namespace Xin;
using namespace Xin::RHI;
using namespace Xin::RHI::D3D12;
using namespace Xin::Rendering;
using namespace Xin::Engine;

#define XIN_PAINTER 1


//static FStringV XMeshFilePath = u8"Sponza/sponza.xmsh"V;
static FStringV XMeshFilePath = u8"SponzaPBR/sponza.xmsh"V;
//static FStringV XMeshFilePath = u8"PowerPlant/powerplant.xmsh"V;


static EGraphicsMode OnlineGraphicsMode = EGraphicsMode::RHI;
static EGraphicsMode OfflineGraphicsMode = EGraphicsMode::Skia;

class FSceneViewer
{
public:
	FSceneViewer(IRHIDevice & Device) : Device(Device)
	{
		static TkeybordInteractiveDebug DebugValue([this](FKey Key)
		{
			switch (Key.KeyCode)
			{
			case EKeyCode::Numpad0:
				RenderSettings.VisualBuffer = EVisualBuffer::Custom0;
				break;
			case EKeyCode::Numpad1:
				RenderSettings.VisualBuffer = EVisualBuffer::Custom1;
				break;
			case EKeyCode::Numpad2:
				RenderSettings.VisualBuffer = EVisualBuffer::Custom2;
				break;
			case EKeyCode::Numpad3:
				RenderSettings.VisualBuffer = EVisualBuffer::Custom3;
				break;
			case EKeyCode::Numpad4:
				RenderSettings.VisualBuffer = EVisualBuffer::Custom4;
				break;
			case EKeyCode::Numpad5:
				RenderSettings.VisualBuffer = EVisualBuffer::Custom5;
				break;
			case EKeyCode::Numpad6:
				RenderSettings.VisualBuffer = EVisualBuffer::Custom6;
				break;
			case EKeyCode::Numpad7:
				RenderSettings.VisualBuffer = EVisualBuffer::Custom7;
				break;
			case EKeyCode::Numpad8:
				RenderSettings.VisualBuffer = EVisualBuffer::Custom8;
				break;
			case EKeyCode::Numpad9:
				RenderSettings.VisualBuffer = EVisualBuffer::Custom9;
				break;
			case EKeyCode::Num1:
				RenderSettings.VisualBuffer = EVisualBuffer::SceneOutput;
				break;
			case EKeyCode::Num2:
				RenderSettings.VisualBuffer = EVisualBuffer::SceneColor;
				break;
			case EKeyCode::Num3:
				RenderSettings.VisualBuffer = EVisualBuffer::BaseNormal;
				break;
			case EKeyCode::Num4:
				RenderSettings.VisualBuffer = EVisualBuffer::SceneDepth;
				break;
			case EKeyCode::Num5:
				RenderSettings.VisualBuffer = EVisualBuffer::SSAOOcclusion;
				break;
			case EKeyCode::Num6:
				RenderSettings.VisualBuffer = EVisualBuffer::LinearDepth;
				break;
			case EKeyCode::Num7:
				RenderSettings.VisualBuffer = EVisualBuffer::ShadowDepth;
				break;
			case EKeyCode::F:
				RenderSettings.WireFrameRender = !RenderSettings.WireFrameRender;
				break;
			case EKeyCode::J:
				if (Key.ControlKey & EControlKey::Ctrl)
					RenderSettings.ReversedZ = !RenderSettings.ReversedZ;
				else
					RenderSettings.PreZ = !RenderSettings.PreZ;
				break;
			case EKeyCode::C:
				RenderSettings.CascadeLevel = (RenderSettings.CascadeLevel + 1) % (CSMCascadeMax + 1);
				switch (RenderSettings.CascadeLevel)
				{
				case 0:
				case 1:
					RenderSettings.CascadePercentages = { 0, 100 };
					break;
				case 2:
					RenderSettings.CascadePercentages = { 0, 10, 100 };
					break;
				case 3:
					RenderSettings.CascadePercentages = { 0, 5, 15, 100 };
					break;
				case 4:
					RenderSettings.CascadePercentages = { 0, 5, 15, 50, 100 };
					break;
				}
				break;
			case EKeyCode::P:
				if (Key.ControlKey & EControlKey::Alt)
				{
					if (Key.ControlKey & EControlKey::Shift)
						RenderSettings.PostProcessMode = EPostProcessMode((int(RenderSettings.PostProcessMode) + 1) % int(EPostProcessMode::Count));
					else
					{
						RenderSettings.PostProcessMode = RenderSettings.PostProcessMode == EPostProcessMode::CS ? EPostProcessMode::PS : EPostProcessMode::CS;
						if (RenderSettings.PostProcessMode == EPostProcessMode::CS)
						{
							RenderSettings.SSAOMode = ESSAOMode::CS;
							RenderSettings.BloomMode = EBloomMode::CS;
							RenderSettings.FXAAMode = EFXAAMode::CS;
						}
						else
						{
							RenderSettings.SSAOMode = ESSAOMode::PS;
							RenderSettings.BloomMode = EBloomMode::PS;
							RenderSettings.FXAAMode = EFXAAMode::PS;
						}
					}
				}
				break;
			case EKeyCode::X:
				if (Key.ControlKey & EControlKey::Alt)
				{
					if (Key.ControlKey & EControlKey::Shift)
						RenderSettings.FXAAMode = EFXAAMode((int(RenderSettings.FXAAMode) + 1) % int(EFXAAMode::Count));
					else
						RenderSettings.FXAAMode = RenderSettings.FXAAMode == EFXAAMode::PS ? EFXAAMode::CS : EFXAAMode::PS;
				}
				else
					RenderSettings.FXAALevel = EFXAALevel((int(RenderSettings.FXAALevel) + 1) % int(EFXAALevel::Count));
				break;
			case EKeyCode::O:
				if (Key.ControlKey & EControlKey::Alt)
				{
					if (Key.ControlKey & EControlKey::Shift)
						RenderSettings.SSAOMode = ESSAOMode((int(RenderSettings.SSAOMode) + 1) % int(ESSAOMode::Count));
					else
						RenderSettings.SSAOMode = RenderSettings.SSAOMode == ESSAOMode::CS ? ESSAOMode::PS : ESSAOMode::CS;
				}
				else if (Key.ControlKey & EControlKey::Shift)
					RenderSettings.SSAOQuality = ESSAOQuality((int(RenderSettings.SSAOQuality) + 1) % Min(int(ESSAOQuality::Count), int(RenderSettings.SSAOLevel) + 2));
				else
				{
					RenderSettings.SSAOLevel = ESSAOLevel((int(RenderSettings.SSAOLevel) + 1) % int(ESSAOLevel::Count));
					RenderSettings.SSAOQuality = ESSAOQuality::Quality0;
				}
				break;
			case EKeyCode::B:
				if (Key.ControlKey & EControlKey::Alt)
				{
					if (Key.ControlKey & EControlKey::Shift)
						RenderSettings.BloomMode = EBloomMode((int(RenderSettings.BloomMode) + 1) % int(EBloomMode::Count));
					else
						RenderSettings.BloomMode = RenderSettings.BloomMode == EBloomMode::CS ? EBloomMode::PS : EBloomMode::CS;
				}
				else
					RenderSettings.BloomLevel = EBloomLevel((int(RenderSettings.BloomLevel) + 1) % int(EBloomLevel::Count));
				break;
			case EKeyCode::G:
				switch (RenderSettings.GussianBlurMode)
				{
				case EGussianBlurMode::PixelShader:
					RenderSettings.GussianBlurMode = EGussianBlurMode::ComputerShader;
					break;
				case EGussianBlurMode::ComputerShader:
					RenderSettings.GussianBlurMode = EGussianBlurMode::ComputerShader_Basic;
					break;
				case EGussianBlurMode::ComputerShader_Basic:
				default:
					RenderSettings.GussianBlurMode = EGussianBlurMode::PixelShader;
				}
				break;
			case EKeyCode::T:
				SetTransientResourceEnabled(!RenderSettings.TransientResourceEnabled);
				break;
			default:
				break;
			}
		});
	}

	~FSceneViewer()
	{
		Executor.WaitRefresh();
	}

	void InitializeWindow(FText Title)
	{
		FWindowPlacement WindowPlacement {};
		WindowPlacement.Title = Title;
		WindowPlacement.WindowRect = { PointF::NaN, { 1920, 1080 } };
		WindowHandle = IDesktop::Default().PlaceWindow(WindowPlacement);
		WindowHandle->ShowWindow(EWindowShowMode::None, EWindowShowPosition::ScreenCenter);

		WindowHandle->OnClose = []() { FApplication::Instance().Exit(0); };

		WindowHandle->OnSize = { this, &FSceneViewer::OnResize };

		WindowHandle->OnKeyDown = { this, &FSceneViewer::OnKeyDown };
		WindowHandle->OnKeyUp = { this, &FSceneViewer::OnKeyUp };
		WindowHandle->OnMouseDown = { this, &FSceneViewer::OnMouseDown };
		WindowHandle->OnMouseUp = { this, &FSceneViewer::OnMouseUp };
		WindowHandle->OnMouseMove = { this, &FSceneViewer::OnMouseMove };
		WindowHandle->OnMouseWhell = { this, &FSceneViewer::OnMouseWheel };
	}

	void InitializePipeline()
	{
		World = new FWorld(Engine);
		Scene = new FWorldScene(*World);

		{
			Camera = new FCameraActor();
			CameraController.SetActor(Camera.Get());
			World->AddActor(Camera);
			Camera->SetWorldTransform(FTransform { FQuat::Rotate(0.17f, -Float32PI * 0.5f, 0.0f), { 800, 560, -100 }, { 1, 1, 1 } });
			//Camera->SetWorldTransform(FTransform { FQuat::Rotate(0, 0, 0.0f), { 800, 560, -100 }, { 1, 1, 1 } });
		}
		{
			Sponza = new FActor();
			auto MeshComponnet = new FStaticMeshComponent();
			MeshComponnet->SetStaticMesh(LoadStaticMesh(Engine, XMeshFilePath));
			Sponza->AddComponent(MeshComponnet);
			World->AddActor(Sponza);
		}

		switch (GraphicsMode)
		{
		default:
		case EGraphicsMode::D2D:
			Graphics = new D2D::FD2DGraphics();
			break;
		case EGraphicsMode::D2DOn11On12:
			Graphics = new D2D::FD2DGraphics(StaticCast<FD3D12CommandQueue>(Executor.PrimaryQueue.Ref()).d3d12CommandQueue);
			break;
		case EGraphicsMode::Skia:
			Graphics = Skia::SkiaCreateGraphics();
			break;
		case EGraphicsMode::RHI:
		{
			IGraphicsRef OfflineGraphics;
			switch (OfflineGraphicsMode)
			{
			default:
			case EGraphicsMode::D2D:
				OfflineGraphics = new D2D::FD2DGraphics();
				break;
			case EGraphicsMode::Skia:
				OfflineGraphics = Skia::SkiaCreateGraphics();
				break;
			case EGraphicsMode::D2DOn11On12:
				OfflineGraphics = new D2D::FD2DGraphics(StaticCast<FD3D12CommandQueue>(Executor.PrimaryQueue.Ref()).d3d12CommandQueue);
				break;
			}
			TReferPtr<FRHIGraphics> RHIGraphics = new FRHIGraphics(Executor);
			RHIGraphics->Initialize(OfflineGraphics);
			Graphics = RHIGraphics;
			break;
		}
		}

		Logo = Graphics->CreateBitmap(u8"../../Xin/Assets/Data/Xin_128x72.png"V);
		Painter = Graphics->CreatePainter();
	}

	void Finalize()
	{
		Executor.WaitRefresh();
	}

	void RenderFrame()
	{
		// Render UI
		IRHIResource * HudTexture = HudTextures[BackBufferIndex];
		IBitmap * HudBitmap = Bitmaps[BackBufferIndex];
		if (HudBitmap)
		{
			IRHICommandQueue::FScopedEvent RenderUIEvent { Executor.PrimaryQueue.Ref(), u8"UI"V };

			SizeU WindowSize = Windows::GetClientSize(HWND(WindowHandle->GetWindowHandle()));
			PointF TextPosition { 10, 10 };

			IPainter & ThePainter = Painter.Ref();

			ThePainter.BeginPaint(HudBitmap);
			//ThePainter.Clear(HudTexture->ResourceDesc.ClearValue.Color);
			ThePainter.FillRect({ TextPosition, TextBlob->FinalSize }, 0x20500050);
			//Painter.DrawTextLayout(TextLayout.Ref(), TextPosition + Point2F { -1, -1 }, 0x40000000);
			//Painter.DrawTextLayout(TextLayout.Ref(), TextPosition + Point2F { 1, -1 }, 0x40000000);
			//Painter.DrawTextLayout(TextLayout.Ref(), TextPosition + Point2F { 1, 1 }, 0x40000000);
			//Painter.DrawTextLayout(TextLayout.Ref(), TextPosition + Point2F { -1, 1 }, 0x40000000);
			//Painter.FillGeometry(TextOutline.Ref(), TextPosition, FSolidColorBrush { 0xA0808080 });
			//Painter.FillGeometry(TextOutline.Ref(), TextPosition, FSolidColorBrush { Colors::Red });

			ThePainter.DrawTextBlob(TextBlob.Ref(), TextPosition, Colors::White);
			ThePainter.DrawBitmap(Logo.Ref(), { WindowSize.Width - 20.f - 128, 10, 128, 72 });

			//XuiPainter.FillRect({ 860, 440, 200, 200 }, 0x80FF0000);
			//XuiPainter.FillRect({ 960, 540, 100, 100}, 0x80FF0000);
			ThePainter.EndPaint();
		}

		if (true)
		{
			FCameraViewInfo CameraViewInfo;
			Camera->CameraComponent->GetViewInfo(CameraViewInfo);

			SizeU ViewportSize = Viewport->GetSize();
			FSceneView SceneView;
			{
				SceneView.FrameIndex = FrameIndex;
				SceneView.NearZ = 0.1f;
				SceneView.FarZ = 10000.0f;

				SceneView.ViewRect = { 0, 0, (int32)ViewportSize.Width, (int32)ViewportSize.Height };
				SceneView.Viewport = { 0, 0, (float32)ViewportSize.Width, (float32)ViewportSize.Height, 0.0f, 1.0f };

				SceneView.Position = CameraViewInfo.Position;
				SceneView.Direction = World->AxisSystem.Forward * CameraViewInfo.Rotation;

				SceneView.ViewMatrix = FMatrix::LookToLH(CameraViewInfo.Position, SceneView.Direction, World->AxisSystem.Upward);
				SceneView.ProjectMatrix = FMatrix::PerspectiveLH(Float32PI * 0.25f, (float32)ViewportSize.Width / ViewportSize.Height, SceneView.NearZ, SceneView.FarZ);
			}
			Scene->SceneView = SceneView;

			FDirectionalLight MainLight;
			{
				MainLight.NearZ = 0.0f;
				MainLight.FarZ = 4000.0f;
				MainLight.FrustumFarZ = 4000.0f;

				MainLight.Position = { 0, 2000, 0 };
				MainLight.Rotation = FQuat::Rotate(Float32PI * 0.35f, 0, 0);
			}
			Scene->MainLight = MainLight;

			//RenderSettings.TransientResourceEnabled = false;
			//RenderSettings.PostProcessMode = EPostProcessMode::PS;
			//RenderSettings.SSAOMode = ESSAOMode::PS;
			//RenderSettings.BloomMode = EBloomMode::PS;
			//RenderSettings.FXAAMode = EFXAAMode::PS;

			Scene->RenderSettings = RenderSettings;

			IRHIResource * BackBuffer = BackBuffers[BackBufferIndex];
			FCommandList CommandList { Executor };

			if (/*HudBitmap && !SharedHudTextures*/GraphicsMode != EGraphicsMode::D2DOn11On12 && GraphicsMode != EGraphicsMode::RHI)
			{
				if (FLockedBitmap LockedBitmap = HudBitmap->Lock(EBitmapLock::Read))
				{
					CommandList.UpdateTexture(HudTexture, FTextureBitmapSource {
						HudBitmap->Format, HudBitmap->Size.Width, HudBitmap->Size.Height, LockedBitmap.Pitch,
						{ LockedBitmap.Bytes, LockedBitmap.Pitch * HudBitmap->Size.Height }
					});
					CommandList.Execute();
				}
			}

			FRenderGraph RenderGraph { Executor, *Scene, CommandList };

			//CommandList.ResourceBarrier(HudTexture, EResourceAccess::ShaderResource);
			CommandList.ResourceAccess(BackBuffer, EResourceAccess::RenderTarget);
			DefferedSceneRenderer.Render(RenderGraph, BackBuffer, HudTexture);
			CommandList.ResourceAccess(BackBuffer, EResourceAccess::Present);
			CommandList.Execute();

			FrameStatics = CommandList.GetFrameStatics();
		}
	}

	void UpdateViewport()
	{
		SizeU WindowSize = Windows::GetClientSize(HWND(WindowHandle->GetWindowHandle()));
		if (!Viewport)
		{
			Viewport = Engine.Device.CreateViewport(Executor.PrimaryQueue.Get(), WindowHandle->GetWindowHandle(), WindowSize, BackBufferCount, EFormat::R10G10B10A2UF, EPresentMode::FlipDiscard);
			Viewport->SetDebugName(u8"MainViewport"V);
			UpdateBackBuffers();
		}
		else
		{
			if (Viewport->GetSize() != WindowSize)
			{
				Executor.WaitRefresh();

				BackBuffers.Clear();
				HudTextures.Clear();
				Viewport->Resize(WindowSize);
				UpdateBackBuffers();
			}
		}
	}

	void Render()
	{
		UpdateViewport();

		BackBufferIndex = Viewport->BeginFrame();
		if (BackBufferIndex != NullIndex)
		{
			RenderFrame();

			Viewport->EndFrame();
			Viewport->Present(false);
		}
		++FrameIndex;
	}

	void OnWindowResized(SizeU Size) {}

	void SetTransientResourceEnabled(bool Enabled)
	{
		if (Enabled == RenderSettings.TransientResourceEnabled)
			return;

		RenderSettings.TransientResourceEnabled = Enabled;

		Executor.WaitRefresh();
	}

	void UpdateBackBuffers()
	{
		if (!Viewport)
		{
			BackBuffers.Clear();
			return;
		}

		EResourceUsage HudTextureUsage = EResourceUsage::RenderTarget | EResourceUsage::SampledTexture;
		if (GraphicsMode == EGraphicsMode::D2DOn11On12)
			HudTextureUsage |= EResourceUsage::Interoperable;

		SizeU WindowSize = Windows::GetClientSize(HWND(WindowHandle->GetWindowHandle()));

		BackBuffers.Reset(Viewport->GetNumBackBuffers(), nullptr);
		HudTextures.Reset(Viewport->GetNumBackBuffers(), nullptr);
		Bitmaps.Reset(Viewport->GetNumBackBuffers(), nullptr);

		for (uint32 BufferIndex = 0; BufferIndex < Viewport->GetNumBackBuffers(); ++BufferIndex)
		{
			BackBuffers[BufferIndex] = Viewport->GetBackBuffer(BufferIndex);
			HudTextures[BufferIndex] = Engine.Device.CreateCommitedResource(FTextureDesc(EFormat::R8G8B8A8UF, WindowSize, HudTextureUsage, 1, 1, Colors::None));
			HudTextures[BufferIndex]->SetDebugName(u8"HudTexture [{}]"V.Format(BufferIndex));

			if (GraphicsMode == EGraphicsMode::RHI)
				Bitmaps[BufferIndex] = Graphics->CreateSharedBitmap(HudTextures[BufferIndex], EBitmapUsage::RenderTarget);
			else if (GraphicsMode == EGraphicsMode::D2DOn11On12)
				Bitmaps[BufferIndex] = Graphics->CreateSharedBitmap(HudTextures[BufferIndex]->GetSharedHandle(), EBitmapUsage::RenderTarget);
			else
				Bitmaps[BufferIndex] = Graphics->CreateBitmap(EFormat::R8G8B8A8UF, WindowSize, EBitmapUsage::RenderTarget);
		}
	}

public:
	static constexpr uint32 BackBufferCount = 3;

	IRHIDevice & Device;

	static_assert(sizeof(FEngine) > 0);

	FEngine Engine { Device };

	FCommandExecutor Executor { Engine };

	IWindowHandleRef WindowHandle;

	IRHIViewportRef Viewport;
	TList<IRHIResourceRef> BackBuffers;
	FFrameStatics FrameStatics;

	uint32 FrameIndex = 0;
	uint32 BackBufferIndex = 0;

	FSceneRenderSettings RenderSettings;

	TReferPtr<FWorld> World;
	TReferPtr<FScene> Scene;
	TReferPtr<FCameraActor> Camera;
	FFirstPersionActorController CameraController {};
	TReferPtr<FActor> Sponza;

	FDefferedSceneRenderer DefferedSceneRenderer { Engine };

	TList<IRHIResourceRef> HudTextures;

	//TReferPtr<Direct2D::FD2DDeviceOn12> D2DDeviceOn12;


	EGraphicsMode GraphicsMode = OnlineGraphicsMode;

	IGraphicsRef Graphics;
	IPainterRef Painter;
	TList<IBitmapRef> Bitmaps;

	IBitmapRef Logo;

	ITextBlobRef TextBlob;
	IPathRef TextPath;

public:
	void OnResize(SizeF Size) {}

	void OnKeyDown(EKeyCode KeyCode)
	{
		switch (KeyCode)
		{
		case EKeyCode::R:
			//CameraController.Position = { 800, 560, -100 };
			//CameraController.Rotate = { 0.17f, -Float32PI * 0.5f, 0 };
			//CameraController.Changed = true;
			break;
		case EKeyCode::Escape:
			::PostQuitMessage(0);
			break;
		default:
			CameraController.OnKeyDown(KeyCode);
			break;
		}

		FKey Key { KeyCode };
		SetFlags(Key.ControlKey, EControlKey::Ctrl, GetKeyState(VK_CONTROL) & 0x8000);
		SetFlags(Key.ControlKey, EControlKey::Shift, GetKeyState(VK_SHIFT) & 0x8000);
		SetFlags(Key.ControlKey, EControlKey::Alt, GetKeyState(VK_MENU) & 0x8000);
		FDebug::Instance().OnKeyDown(Key);
	}

	void OnKeyUp(EKeyCode KeyCode)
	{
		CameraController.OnKeyUp(KeyCode);
	}

	void OnMouseDown(EMouseButton MouseButton, PointF Position)
	{
		if (MouseButton == EMouseButton::Middle)
		{
			IsObjectRotating = true;
			ObjectTransformStart = ObjectTransform;
			ObjectRotateMousePointStart = Position;
			ObjectRotationStart = ObjectRotation;
		}
		else
			CameraController.OnMouseDown(Position, MouseButton);
	}

	void OnMouseUp(const EMouseButton MouseButton, PointF Position)
	{
		if (MouseButton == EMouseButton::Middle)
		{
			IsObjectRotating = false;
		}
		else
			CameraController.OnMouseUp(Position, MouseButton);
	}

	void OnMouseMove(PointF Position)
	{
		if (IsObjectRotating)
		{
			PointF Offset = Position - ObjectRotateMousePointStart;
			ObjectRotation = FVector3(/*ObjectRotationStart.X - Offset.Y * RotateSpeed*/0, ObjectRotationStart.Y - Offset.X * ObjectRotateSpeed, 0.0f);
			ObjectTransform.Rotation = FQuat::Rotate(ObjectRotation);
			UpdateObjectTransform();
		}
		else
			CameraController.OnMouseMove(Position);
	}

	void OnMouseWheel(PointF Position, Vec2F Whell)
	{
		CameraController.OnMouseWheel(Position, Whell);
	}

	void Update(float32 DeltaTime)
	{
		Scene->Update(Executor);

		Executor.UpdateContext();
		Device.UpdateContext();

		SizeU WindowSize = Windows::GetClientSize(HWND(WindowHandle->GetWindowHandle()));
		CameraController.Update(DeltaTime);

		FpsCounter.Accumulate(1);
		FpsCounter10.Accumulate(1);

#if XIN_PAINTER
		uint64 CounterTimeInMs = FDateTime::SystemMilliseconds();
		if (CounterTimeInMs - FpsCounterTimeInMs >= 200)
		{
			FpsCounterTimeInMs = CounterTimeInMs;

			TArray<FStringV, 5> FXAAModeString = { u8"Disabled"V, u8"PS_Console"V, u8"PS"V, u8"PS_3_11"V, u8"CS"V };
			TArray<FStringV, 3> SSAOModeString = { u8"Disabled"V, u8"PS"V, u8"CS"V };
			TArray<FStringV, 3> BloolModeString = { u8"Disabled"V, u8"PS"V, u8"CS"V };
			TArray<FStringV, 3> PostProcessModeString = { u8"Disabled"V, u8"PS"V, u8"CS"V };

			//FWString WindowTitle;
			//WindowTitle =  Format(L"FPS={:.0f}, FP10S={:.0f}", FpsCounter.Average(), FpsCounter10.Average());
			//::SetWindowTextW((HWND)WindowHandle, WindowTitle.Data);

			FString Infomation;
			Infomation += Format(u8"RHI={}.\n"V, GetDeviceRHIName(Device.GetDeviceRHI()));

			Infomation += Format(u8"FPS={:.0f}, FP10S={:.0f}.\n"V, FpsCounter.Average(), FpsCounter10.Average());
			Infomation += Format(u8"AAA,    {{ {} DrawCalls, {} Prims, {} RSs, {} PSOs }}\n"V, FrameStatics.DrawCalls, FrameStatics.Primitives, FrameStatics.RootSignatures, FrameStatics.PipelineStates);

			Infomation += Format(u8"AAA,    [P]P: {}\n"V, PostProcessModeString[RenderSettings.PostProcessMode]);
			Infomation += Format(u8"AAA,    SSA[O]: Lv.{},{},{}\n"V, int(RenderSettings.SSAOLevel), int(RenderSettings.SSAOQuality), SSAOModeString[int(RenderSettings.SSAOMode)]);
			Infomation += Format(u8"AAA,    [B]loom: Lv.{},{}\n"V, (int)RenderSettings.BloomLevel, BloolModeString[RenderSettings.BloomMode]);
			Infomation += Format(u8"AAA,    F[X]AA: Lv.{},{}\n"V, (int)RenderSettings.FXAALevel, FXAAModeString[RenderSettings.FXAAMode]);
			Infomation += Format(u8"AAA,    Memory: {:.2f}GB\n"V, Engine.Device.GetMemoryUsage() / 1e9f);

			if (!RenderSettings.TransientResourceEnabled)
				Infomation += u8",    None Transient"V;

			Infomation += u8"\n"
				u8"✋✌️☝️✊✍️⛑️☂️☎♻♿⚠⛔❄️☘️☁️⛅⛈️☂️☔⚡❄️\n"
				u8"☃️⛄☄️👩🌾🎄🍅🛺🚕🚎🚎🚜💗\n"
				u8"☕⛷️⛹️⛹️♂️⛹️♀️⚽⚾⛳\n"
				u8"⛰️⛪⛩⛲⛺⛽⚓⛵⛴️✈️⛱️\n"
				u8"lālálǎlàوالعدل والسلام靐𰻞𰻞𰻞 في العالم.\n"
				u8"♒强烈❤️❤️❤️推荐♒\n✅运行正常✅\n⚡亭亭玉立⚡\n♈恭喜发财♈\n✨新年快乐✨\n🏅大吉大利🏅\n♒今晚吃鸡♒\n"V;

			TextBlob = Graphics->CreateTextBlob(Infomation, {}, 24.0f, SizeF { (float32)WindowSize.Width - 20.0f, TextBlobSizeMax }, ETextLayoutFlags::None);
			//TextGeometry = Engine.PaintDevice.CreateTextGeometry(TextLayout.Ref(), {});
		}
#endif
	}

	void UpdateObjectTransform()
	{
		Sponza->SetWorldTransform(ObjectTransform);
	}

private:
	FCounter<float32, 3> FpsCounter { 1000 };
	FCounter<float32, 10> FpsCounter10 { 1000 };
	uint64 FpsCounterTimeInMs = 0;

private:
	bool IsObjectRotating = false;
	FVector3 ObjectRotationStart = FVector3::Zero;
	FVector3 ObjectRotation = FVector3::Zero;
	PointF ObjectRotateMousePointStart = PointF::Zero;
	FTransform ObjectTransform = FTransform::Identity;
	FTransform ObjectTransformStart = FTransform::Identity;
	static inline float32 ObjectRotateSpeed = 0.005f;

public:
	TEvent<void(const FTransform &)> OnObjectTransformChanged;
};

void SceneViewer_Main(IRHIDeviceRef RHIDevice)
{
	new char[9] { 't', 'e', 's', 't', ' ', 'l', 'e', 'a', 'k' };

	FGlobalLogger GlobalLogger;

	RHIDevice->SetDebugName(u8"MainContext"V);

	FSceneViewer SceneViewer { *RHIDevice };
	SceneViewer.InitializeWindow(u8"Scene Viewer"T);
	SceneViewer.InitializePipeline();

	float64 Time = FDateTime::Steady();
	MSG msg = {};
	while (msg.message != WM_QUIT)
	{
		//::GetMessageW(&msg, NULL, 0, 0);
		//TranslateMessage(&msg);
		//DispatchMessageW(&msg);
		float64 TimeTick = FDateTime::Steady();
		float32 DeltaTime = float32(TimeTick - Time);
		Time = TimeTick;
		SceneViewer.Update(DeltaTime);

		SceneViewer.Render();

		DWORD dwWait = MsgWaitForMultipleObjectsEx(0, NULL, 0, QS_ALLEVENTS, MWMO_ALERTABLE);
		switch (dwWait)
		{
		default:
			break;
		}

		while (PeekMessageW(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
				break;

			TranslateMessage(&msg);
			DispatchMessageW(&msg);
		}
	}

	LogInfo(u8"GPU Memory Usage: {0:.2f}GiB"V, RHIDevice->GetMemoryUsage() / (float32)UnitGiB);

	SceneViewer.Finalize();
}
#endif

#include "SceneViewer.h"


int main(int argc, const char * args[])
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	_CrtSetReportMode(_CRT_ASSERT, _CRTDBG_MODE_FILE | _CRTDBG_MODE_WNDW);
	_CrtSetReportFile(_CRT_ASSERT, _CRTDBG_FILE_STDERR);
	//_CrtSetBreakAlloc(11835);
	new char[9] { 't', 'e', 's', 't', ' ', 'l', 'e', 'a', 'k' };

	FResourceTableRef GlobalResourceSheet = MakeRefer<FResourceTable>();
	GlobalResourceSheet->Resources.AddRange(Themes::Default::LoadDefaultThemesResources());

	bool UseD3D12 = false;
	for (int ArgIndex = 0; ArgIndex < argc; ++ArgIndex)
	{
		if (strstr(args[ArgIndex], "-d3d12"))
		{
			UseD3D12 = true;
			break;
		}
	}

	IRHIDeviceRef RHIDevice = UseD3D12 ? D3D12::CreateDevice() : Vulkan::CreateDevice();
	USceneViewer SceneViewer { *RHIDevice, EGraphicsMode::Skia };
	SceneViewer.Construct();
	SceneViewer.ResourceTable.Tables.Add(GlobalResourceSheet);
	SceneViewer.WindowShowPosition = EWindowShowPosition::ScreenCenter;
	SceneViewer.ExitOnClose = true;

	SceneViewer.Show();
	SceneViewer.RunLoop();

	//SceneViewer.Finalize();

	return 0;
}
