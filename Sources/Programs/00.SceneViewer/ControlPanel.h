#pragma once

#include "00.SceneViewer.Types.h"

namespace Xin
{
	struct FControlPanelDataSource : IDependency
	{
		META_DECL

		ESSAOLevel SSAOLevel = ESSAOLevel::Level1;
		ESSAOQuality SSAOQuality = ESSAOQuality::Quality0;

		ProxyPropertyRW(ESSAOMode, SSAOMode) = ESSAOMode::CS;
		ProxyPropertyRW(EPostProcessMode, PostProcessMode) = EPostProcessMode::CS;
		ProxyPropertyRW(EBloomMode, BloomMode) = EBloomMode::CS;
		ProxyPropertyRW(EFXAAMode, FXAAMode) = EFXAAMode::CS;
	};

	class UControlPanel : public UStackPanel
	{
	public:
		UControlPanel(FControlPanelDataSource & DataSource);
		~UControlPanel();

	public:
		void OnConstruct() override;
		void OnInitialize() override;

	public:
		void UpdateLabels(float32 DeltaTime, IRHIDevice & Device, const FSceneRenderSettings & RenderSettings);

	public:
		FControlPanelDataSource & DataSource;

	public:
		UTextBlockRef DeviceRHI = MakeRefer<UTextBlock>();
		UTextBlockRef FPS= MakeRefer<UTextBlock>();
		UTextBlockRef Bloom = MakeRefer<UTextBlock>();
		URadioButtonRef Bloom_CS = MakeRefer<URadioButton>();
		URadioButtonRef Bloom_PS = MakeRefer<URadioButton>();

		UTextBlockRef SSAO = MakeRefer<UTextBlock>();
		URadioButtonRef SSAO_CS = MakeRefer<URadioButton>();
		URadioButtonRef SSAO_PS = MakeRefer<URadioButton>();

		UTextBlockRef FXAA = MakeRefer<UTextBlock>();
		URadioButtonRef FXAA_CS = MakeRefer<URadioButton>();
		URadioButtonRef FXAA_PS = MakeRefer<URadioButton>();

	private:
		FCounter<float32, 3> FpsCounter { 1000 };
		FCounter<float32, 10> FpsCounter10 { 1000 };
		uint64 LabelsUpdateTime = 0;

	public:
		FDelegate ShowAtlas;
		FDelegate ShowChrome;
	};
}
