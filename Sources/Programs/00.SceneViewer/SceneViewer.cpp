#include "PCH.h"
#include "SceneViewer.h"

#include "Xin.Direct2D/Xin.Direct2D.h"
#include "Xin.Direct2D/Xin.Direct2D.Private.h"
#include "Xin.Skia/Xin.Skia.h"
#include "Xin.RHI.D3D12/Xin.RHI.D3D12.Private.h"

#include "Programs/00.Test.UI/00.Test.UI.h"

namespace Xin
{
	//static FStringV XMeshFilePath = u8"Sponza/sponza.xmsh"V;
	static FStringV XMeshFilePath = u8"SponzaPBR/sponza.xmsh"V;
	//static FStringV XMeshFilePath = u8"PowerPlant/powerplant.xmsh"V;

	USceneViewer::USceneViewer(IRHIDevice & Device, EGraphicsMode OfflineGraphicsMode)
		: Device(Device), Engine(Device), Executor(Engine), RHIGraphics(Executor)
	{
		//RenderSettings.TransientResourceEnabled = false;
		//DataSource.SSAOMode = ESSAOMode::PS;
		//DataSource.PostProcessMode = EPostProcessMode::PS;
		//DataSource.BloomMode = EBloomMode::PS;
		//DataSource.FXAAMode = EFXAAMode::PS;

		IGraphicsRef OfflineGraphics;
		switch (OfflineGraphicsMode)
		{
		default:
		case EGraphicsMode::D2D:
			OfflineGraphics = D2D::D2DCreateGraphics();
			break;
		case EGraphicsMode::Skia:
			OfflineGraphics = Skia::SkiaCreateGraphics();
			break;
		case EGraphicsMode::D2DOn11On12:
			OfflineGraphics = new D2D::FD2DGraphics(StaticCast<D3D12::FD3D12CommandQueue>(Executor.PrimaryQueue.Ref()).d3d12CommandQueue);
			break;
		}
		RHIGraphics.Initialize(OfflineGraphics);

		Size = { 1920, 1080 };
		Foreground = Colors::White;
		AtlasPanel = CreateElement_Atlas();
		ChromePanel = CreateElement_Chrome();
		AtlasPanel->Size = {FDimen::Percent50, FDimen::Percent50};
		AtlasPanel->HorizontalAlignment = EElementAlignment::Near;
		AtlasPanel->VerticalAlignment = EElementAlignment::Near;
		AtlasPanel->GroundingFill = MakeRefer<FSolidColorBrush>(0x80FFFFFF);

		ControlPanel->MinWidth = 400;
		ControlPanel->MinHeight = 400;
		ControlPanel->HorizontalAlignment = EElementAlignment::Far;
		ControlPanel->VerticalAlignment = EElementAlignment::Near;
		ControlPanel->ShowAtlas = [this](auto)
		{
			if (MainPanel->Elements.Contains(AtlasPanel))
			{
				MainPanel->Elements.RemoveItem(AtlasPanel);	
			}
			else
			{
				MainPanel->Elements.AddItem(AtlasPanel);	
			}
		};
		ControlPanel->ShowChrome = [this](auto)
			{
				if (ChromePanel->Shown)
				{
					ChromePanel->ShowChromes();
				}
				else
				{
					MainPanel->Elements.AddItem(ChromePanel);	
				}
			};
		MainPanel->Elements = {ControlPanel};

		Content = MainPanel;
	}

	USceneViewer::~USceneViewer()
	{
		Executor.WaitRefresh();
	}

	void USceneViewer::OnInitialize()
	{
		UWindow::OnInitialize();
		InitializeScene();

		//Logo = Graphics.CreateBitmap(u8"../../Xin/Assets/Data/Xin_128x72.png"V);
	}

	void USceneViewer::RunLoop()
	{
		UpdateTime = FDateTime::Steady();
		FApplication::Instance().Loop([&]
		{
			if (Shown)
			{
				OnRender();
			}
		});
	}

	void USceneViewer::UpdateScene(float32 DeltaTime)
	{
		ControlPanel->UpdateLabels(DeltaTime, Engine.Device, RenderSettings);

		CameraController->Update(DeltaTime);
		World->Update(DeltaTime);
		Scene->Update(Executor);

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
		RenderSettings.PostProcessMode = DataSource.PostProcessMode;
		RenderSettings.SSAOMode = DataSource.SSAOMode;
		RenderSettings.BloomMode = DataSource.BloomMode;
		RenderSettings.FXAAMode = DataSource.FXAAMode;

		Scene->RenderSettings = RenderSettings;
	}

	void USceneViewer::OnRender()
	{
		float64 SteadyTime = FDateTime::Steady();
		float32 DeltaTime = float32(SteadyTime - UpdateTime);
		UpdateTime = SteadyTime;

		UpdateViewport();
		UpdateScene(DeltaTime);

		Executor.UpdateContext();

		uint32 BackBufferIndex = Viewport->BeginFrame();
		if (BackBufferIndex == NullIndex)
			return;

		IRHIResource * BackBuffer = BackBuffers[BackBufferIndex];
		{
			FCommandList CommandList { Executor };

			FRenderGraph RenderGraph { Executor, *Scene, CommandList };

			CommandList.ResourceAccess(BackBuffer, EResourceAccess::RenderTarget);
			DefferedSceneRenderer.Render(RenderGraph, BackBuffer, nullptr);

			CommandList.Execute();

			auto FrameStatics = CommandList.GetFrameStatics();
		}

		if constexpr (true)
		{
			FRHIPainter RHIPainter(RHIGraphics);
			RHIPainter.BeginPaint(BackBitmaps[BackBufferIndex], false);
			Paint(RHIPainter);

			//RHIPainter.FillRect({ 100, 100, 400, 200 }, Colors::Red);
			RHIPainter.EndPaint();
		}

		{
			FCommandList CommandList { Executor };
			CommandList.ResourceAccess(BackBuffer, EResourceAccess::Present);
			CommandList.Execute();
		}

		Viewport->EndFrame(0);
		++FrameIndex;
	}

	void USceneViewer::InitializeScene()
	{
		CameraController->SetActor(Camera.Get());
		World->AddActor(Camera);
		Camera->SetWorldTransform(FTransform { FQuat::Rotate(0.17f, -Float32PI * 0.5f, 0.0f), { 800, 560, -100 }, { 1, 1, 1 } });

		auto MeshComponnet = new FStaticMeshComponent();
		MeshComponnet->SetStaticMesh(LoadStaticMesh(Engine, XMeshFilePath));
		Actor->AddComponent(MeshComponnet);
		World->AddActor(Actor);
	}

	void USceneViewer::UpdateViewport()
	{
		SizeU ViewportSize = WindowSize;
		if (!Viewport)
		{
			Viewport = Engine.Device.CreateViewport(Executor.PrimaryQueue.Get(), voidp(WindowHandle->GetWindowHandle()), ViewportSize, BackBufferCount, EFormat::R10G10B10A2UF, EPresentMode::FlipDiscard);
			Viewport->SetDebugName(u8"MainViewport"V);
			BackBitmaps.Clear();
			BackBuffers.Clear();
		}
		else
		{
			if (Viewport->GetSize() != ViewportSize)
			{
				Executor.WaitRefresh();
				BackBitmaps.Clear();
				BackBuffers.Clear();
				Viewport->Resize(ViewportSize);
			}
		}

		if (BackBuffers.Size == 0)
		{
			BackBitmaps.Reset(Viewport->GetNumBackBuffers(), nullptr);
			BackBuffers.Reset(Viewport->GetNumBackBuffers(), nullptr);

			for (uint32 BufferIndex = 0; BufferIndex < Viewport->GetNumBackBuffers(); ++BufferIndex)
			{
				BackBuffers[BufferIndex] = Viewport->GetBackBuffer(BufferIndex);
				BackBitmaps[BufferIndex] = RHIGraphics.CreateBitmap(BackBuffers[BufferIndex], EBitmapUsage::RenderTarget);
				//BackBitmaps[BufferIndex] = RHIGraphics.CreateBitmap(EFormat::R8G8B8A8UF, WindowSize, EBitmapUsage::RenderTarget);
			}
		}
	}

	void USceneViewer::OnMouseDown(FMouseButtonEventArgs & Args)
	{
		UWindow::OnMouseDown(Args);
		if (Args.Button == EMouseButton::Middle)
		{
			IsObjectRotating = true;
			ObjectTransformStart = ObjectTransform;
			ObjectRotateMousePointStart = Args.Position;
			ObjectRotationStart = ObjectRotation;
		}
		else
			CameraController->OnMouseDown(Args.Position, Args.Button);
	}

	void USceneViewer::OnMouseUp(FMouseButtonEventArgs & Args)
	{
		UWindow::OnMouseUp(Args);
		if (Args.Button == EMouseButton::Middle)
		{
			IsObjectRotating = false;
		}
		else
			CameraController->OnMouseUp(Args.Position, Args.Button);
	}

	void USceneViewer::OnMouseMove(FMouseEventArgs & Args)
	{
		UWindow::OnMouseMove(Args);
		if (IsObjectRotating)
		{
			PointF Offset = Args.Position - ObjectRotateMousePointStart;
			ObjectRotation = FVector3(/*ObjectRotationStart.X - Offset.Y * RotateSpeed*/0, ObjectRotationStart.Y - Offset.X * ObjectRotateSpeed, 0.0f);
			ObjectTransform.Rotation = FQuat::Rotate(ObjectRotation);
			Actor->SetWorldTransform(ObjectTransform);
		}
		else
			CameraController->OnMouseMove(Args.Position);
	}

	void USceneViewer::OnMouseWhell(FMouseWhellEventArgs & Args)
	{
		UWindow::OnMouseWhell(Args);
		CameraController->OnMouseWheel(Args.Position, Args.Whell);
	}

	void USceneViewer::OnKeyDown(FKeyboardEventArgs & Args)
	{
		UWindow::OnKeyDown(Args);

		switch (Args.Key)
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
			CameraController->OnKeyDown(Args.Key);
			break;
		}

		FKey Key { Args.Key };
		SetFlags(Key.ControlKey, EControlKey::Ctrl, GetKeyState(VK_CONTROL) & 0x8000);
		SetFlags(Key.ControlKey, EControlKey::Shift, GetKeyState(VK_SHIFT) & 0x8000);
		SetFlags(Key.ControlKey, EControlKey::Alt, GetKeyState(VK_MENU) & 0x8000);

		FDebug::Instance().OnKeyDown(Key);
	}

	void USceneViewer::OnKeyUp(FKeyboardEventArgs & Args)
	{
		UWindow::OnKeyUp(Args);

		CameraController->OnKeyUp(Args.Key);
	}
}
