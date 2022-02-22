#pragma once

#include "00.SceneViewer.Types.h"
#include "ControlPanel.h"

namespace Xin
{
	enum class EGraphicsMode
	{
		D2D = 0,
		D2DOn11On12,
		Skia,
		RHI,
	};

	class USceneViewer : public UWindow
	{
	public:
		USceneViewer(IRHIDevice & Device, EGraphicsMode OfflineGraphicsMode);
		~USceneViewer();

	public:
		void OnInitialize() override;
		IGraphics & GetGraphics() const override { return ConstCast(RHIGraphics); }
		void OnPaintWindow() override { }

	public:
		void RunLoop();
		void UpdateScene(float32 DeltaTime);
		void OnRender();

	private:
		void InitializeScene();
		void UpdateViewport();

	public:
		void OnMouseDown(FMouseButtonEventArgs & Args) override;
		void OnMouseUp(FMouseButtonEventArgs & Args) override;
		void OnMouseMove(FMouseEventArgs & Args) override;
		void OnMouseWhell(FMouseWhellEventArgs & Args) override;
		void OnKeyDown(FKeyboardEventArgs & Args) override;
		void OnKeyUp(FKeyboardEventArgs & Args) override;

	public:
		IRHIDevice & Device;
		FEngine Engine;
		FCommandExecutor Executor;

	private:
		static constexpr uint32 BackBufferCount = 3;
		IRHIViewportRef Viewport;
		TList<IRHIResourceRef> BackBuffers;
		TList<IBitmapRef> BackBitmaps;

	private:
		EGraphicsMode OnlineGraphicsMode = EGraphicsMode::RHI;
		EGraphicsMode OfflineGraphicsMode = EGraphicsMode::Skia;
		FRHIGraphics RHIGraphics;

	private:
		TReferPtr<FWorld> World= new FWorld(Engine);
		TReferPtr<FScene> Scene = new FWorldScene(*World);

		TReferPtr<FCameraActor> Camera = new FCameraActor();
		TReferPtr<FFirstPersionActorController> CameraController = new FFirstPersionActorController();
		TReferPtr<FActor> Actor = new FActor();

	private:
		uint64 FrameIndex = 0;
		FSceneRenderSettings RenderSettings;

		FDefferedSceneRenderer DefferedSceneRenderer { Engine };
		float64 UpdateTime = 0.0f;

	private:
		bool IsObjectRotating = false;
		FVector3 ObjectRotationStart = FVector3::Zero;
		FVector3 ObjectRotation = FVector3::Zero;
		PointF ObjectRotateMousePointStart = PointF::Zero;
		FTransform ObjectTransform = FTransform::Identity;
		FTransform ObjectTransformStart = FTransform::Identity;
		static inline float32 ObjectRotateSpeed = 0.005f;

	private:
		FControlPanelDataSource DataSource;
		UOverlapPanelRef MainPanel = MakeRefer<UOverlapPanel>();
		TReferPtr<UControlPanel> ControlPanel = MakeRefer<UControlPanel>(DataSource);
		UElementRef AtlasPanel;
		UChromePanelRef ChromePanel;
	};
}
