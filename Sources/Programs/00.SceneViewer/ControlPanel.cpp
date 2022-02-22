#include "PCH.h"
#include "ControlPanel.h"

namespace Xin
{
	UControlPanel::UControlPanel(FControlPanelDataSource & DataSource)
		: DataSource(DataSource)
	{
		
	}

	UControlPanel::~UControlPanel() {}

	void UControlPanel::OnConstruct()
	{
		UStackPanel::OnConstruct();

		Orientation = EOrientation::Vertical;
		GroundingFill = MakeRefer<FSolidColorBrush>(0xA0F5F5F5);
		ElementAlignment = EElementAlignment::Near;

		FColor TextColor = 0xFF000000;
		DeviceRHI->Color = TextColor;
		DeviceRHI->FontSize = 24.0f;

		FPS->Color = TextColor;
		SSAO->Color = TextColor;
		Bloom->Color = TextColor;
		FXAA->Color = TextColor;
	}

	void UControlPanel::OnInitialize()
	{
		UStackPanel::OnInitialize();

		TArray<FStringV, 5> FXAAModeString = { u8"Disabled"V, u8"PS_Console"V, u8"PS"V, u8"PS_3_11"V, u8"CS"V };
		TArray<FStringV, 3> SSAOModeString = { u8"Disabled"V, u8"PS"V, u8"CS"V };
		TArray<FStringV, 3> BloolModeString = { u8"Disabled"V, u8"PS"V, u8"CS"V };
		TArray<FStringV, 3> PostProcessModeString = { u8"Disabled"V, u8"PS"V, u8"CS"V };

		//FWString WindowTitle;
		//WindowTitle =  Format(L"FPS={:.0f}, FP10S={:.0f}", FpsCounter.Average(), FpsCounter10.Average());
		//::SetWindowTextW((HWND)WindowHandle, WindowTitle.Data);

		//FString Infomation;
		//Infomation += Format(u8"RHI={}.\n"V, GetDeviceRHIName(Device.GetDeviceRHI()));

		//Infomation += Format(u8"FPS={:.0f}, FP10S={:.0f}.\n"V, FpsCounter.Average(), FpsCounter10.Average());
		//Infomation += Format(u8"AAA,    {{ {} DrawCalls, {} Prims, {} RSs, {} PSOs }}\n"V, FrameStatics.DrawCalls, FrameStatics.Primitives, FrameStatics.RootSignatures, FrameStatics.PipelineStates);

		//Infomation += Format(u8"AAA,    [P]P: {}\n"V, PostProcessModeString[RenderSettings.PostProcessMode]);
		//Infomation += Format(u8"AAA,    SSA[O]: Lv.{},{},{}\n"V, int(RenderSettings.SSAOLevel), int(RenderSettings.SSAOQuality), SSAOModeString[int(RenderSettings.SSAOMode)]);
		//Infomation += Format(u8"AAA,    [B]loom: Lv.{},{}\n"V, (int)RenderSettings.BloomLevel, BloolModeString[RenderSettings.BloomMode]);
		//Infomation += Format(u8"AAA,    F[X]AA: Lv.{},{}\n"V, (int)RenderSettings.FXAALevel, FXAAModeString[RenderSettings.FXAAMode]);

		//TbRHI->GroundingFill = Brushs::BurlyWood;
		//TbFps->GroundingFill = Brushs::IndianRed;

		FPS->VerticalAlignment = EElementAlignment::Far;
		FPS->Margin = { 10, 0, 0, 0 };
		// SSAO
		{
			SSAO->Text = u8"SSAO:"T;
			SSAO_CS->Content = u8"CS"V;
			SSAO_CS->GroupName = u8"RenderingSettings_SSAO"N;
			SSAO_CS->Checked = Binding(DataSource, FControlPanelDataSource::SSAOModeProperty(), [](const FVariant & Value, const FProperty & Property) ->FVariant
				{
					if (Property == FControlPanelDataSource::SSAOModeProperty())
					{
						bool Checked = Value.Get<bool>();
						return Checked ? ESSAOMode::CS : ESSAOMode::PS;
					}
					else
					{
						ESSAOMode SSAOMode = Value.Get<ESSAOMode>();
						return SSAOMode == ESSAOMode::CS;	
					}
				});
			SSAO_PS->Content = u8"PS"V;
			SSAO_PS->GroupName = u8"RenderingSettings_SSAO"N;
			SSAO_PS->Checked = Binding(DataSource, FControlPanelDataSource::SSAOModeProperty(), [](const FVariant & Value, const FProperty & Property) ->FVariant
				{
					if (Property == FControlPanelDataSource::SSAOModeProperty())
					{
						bool Checked = Value.Get<bool>();
						return Checked ? ESSAOMode::PS : ESSAOMode::CS;
					}
					else
					{
						ESSAOMode SSAOMode = Value.Get<ESSAOMode>();
						return SSAOMode == ESSAOMode::PS;	
					}
				});
		}
		// FXAA
		{
			FXAA->Text = u8"FXAA:"T;
			FXAA_CS->Content = u8"CS"V;
			FXAA_CS->GroupName = u8"RenderingSettings_FXAA"N;
			FXAA_CS->Checked = Binding(DataSource, FControlPanelDataSource::FXAAModeProperty(), [](const FVariant & Value, const FProperty & Property) ->FVariant
				{
					if (Property == FControlPanelDataSource::FXAAModeProperty())
					{
						bool Checked = Value.Get<bool>();
						return Checked ? EFXAAMode::CS : EFXAAMode::PS;
					}
					else
					{
						EFXAAMode FXAAMode = Value.Get<EFXAAMode>();
						return FXAAMode == EFXAAMode::CS;	
					}
				});
			FXAA_PS->Content = u8"PS"V;
			FXAA_PS->GroupName = u8"RenderingSettings_FXAA"N;
			FXAA_PS->Checked = Binding(DataSource, FControlPanelDataSource::FXAAModeProperty(), [](const FVariant & Value, const FProperty & Property) ->FVariant
				{
					if (Property == FControlPanelDataSource::FXAAModeProperty())
					{
						bool Checked = Value.Get<bool>();
						return Checked ? EFXAAMode::PS : EFXAAMode::CS;
					}
					else
					{
						EFXAAMode FXAAMode = Value.Get<EFXAAMode>();
						return FXAAMode == EFXAAMode::PS;	
					}
				});
		}

		// Bloom
		{
			Bloom->Text = u8"Bloom:"T;
			Bloom_CS->Content = u8"CS"V;
			Bloom_CS->GroupName = u8"RenderingSettings_Bloom"N;
			Bloom_CS->Checked = Binding(DataSource, FControlPanelDataSource::BloomModeProperty(), [](const FVariant & Value, const FProperty & Property) ->FVariant
				{
					if (Property == FControlPanelDataSource::BloomModeProperty())
					{
						bool Checked = Value.Get<bool>();
						return Checked ? EBloomMode::CS : EBloomMode::PS;
					}
					else
					{
						EBloomMode SSAOMode = Value.Get<EBloomMode>();
						return SSAOMode == EBloomMode::CS;	
					}
				});
			Bloom_PS->Content = u8"PS"V;
			Bloom_PS->GroupName = u8"RenderingSettings_Bloom"N;
			Bloom_PS->Checked = Binding(DataSource, FControlPanelDataSource::BloomModeProperty(), [](const FVariant & Value, const FProperty & Property) ->FVariant
				{
					if (Property == FControlPanelDataSource::BloomModeProperty())
					{
						bool Checked = Value.Get<bool>();
						return Checked ? EBloomMode::PS : EBloomMode::CS;
					}
					else
					{
						EBloomMode SSAOMode = Value.Get<EBloomMode>();
						return SSAOMode == EBloomMode::PS;	
					}
				});
		}

		UStackPanelRef PanelSSAO;
		UStackPanelRef PanelBloom;
		UStackPanelRef PanelFXAA;
		UHLineRef HLine = MakeRefer<UHLine>();
		Elements =
		{
			StackPanel(EOrientation::Horizontal, { DeviceRHI, FPS }),
			LineH(2),
			PanelSSAO = StackPanel(EOrientation::Horizontal, { SSAO, SSAO_CS, SSAO_PS }),
			PanelBloom = StackPanel(EOrientation::Horizontal, { Bloom, Bloom_CS, Bloom_PS }),
			PanelFXAA = StackPanel(EOrientation::Horizontal, { FXAA, FXAA_CS, FXAA_PS }),
			Button(u8"Atlas"T, [this](const FRoutedEventArgs & Args) { ShowAtlas(FRoutedEventArgs(Args)); }, { { UControl::HorizontalAlignmentProperty(), EElementAlignment::Near } }),
			Button(u8"Chrome"T, [this](const FRoutedEventArgs & Args) { ShowChrome(FRoutedEventArgs(Args)); }, { { UControl::HorizontalAlignmentProperty(), EElementAlignment::Near } }),
		};

		//PanelSSAO->ElementAlignment = EElementAlignment::Center;
	}

	void UControlPanel::UpdateLabels(float32 DeltaTime, IRHIDevice & Device, const FSceneRenderSettings & RenderSettings)
	{
		//static bool Updated = false;
		//if (Updated)
		//	return;
		//Updated = true;

		FpsCounter.Accumulate(1);
		FpsCounter10.Accumulate(1);

		uint64 CurrentTime = FDateTime::SystemMilliseconds();
		if (CurrentTime - LabelsUpdateTime <= 0)
			return;

		TArray<FStringV, 5> FXAAModeString = { u8"Disabled"V, u8"PS_Console"V, u8"PS"V, u8"PS_3_11"V, u8"CS"V };
		TArray<FStringV, 3> SSAOModeString = { u8"Disabled"V, u8"PS"V, u8"CS"V };
		TArray<FStringV, 3> BloolModeString = { u8"Disabled"V, u8"PS"V, u8"CS"V };
		TArray<FStringV, 3> PostProcessModeString = { u8"Disabled"V, u8"PS"V, u8"CS"V };

		DeviceRHI->Text = GetDeviceRHIName(Device.GetDeviceRHI());
		FPS->Text = Format(u8"FPS={:.0f}, FPS10s={:.0f}.\n"V, FpsCounter.Average(), FpsCounter10.Average());
		//TbSSAO->Text = Format(u8"SSA[O]: Lv.{},{},{}\n"V, int(RenderSettings.SSAOLevel), int(RenderSettings.SSAOQuality), SSAOModeString[int(RenderSettings.SSAOMode)]);
		//Bloom->Text = Format(u8"[B]loom: Lv.{},{}\n"V, (int)RenderSettings.BloomLevel, BloolModeString[RenderSettings.BloomMode]);
		//FXAA->Text = Format(u8"F[X]AA: Lv.{}, Mode{}\n"V, (int)RenderSettings.FXAALevel, FXAAModeString[RenderSettings.FXAAMode]);
	}
}
