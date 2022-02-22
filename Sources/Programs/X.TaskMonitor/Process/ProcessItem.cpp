#include "PCH.h"
#include "ProcessItem.h"

#include "Utils/ShellItemImage.h"

namespace Xin::TaskMonitor
{
	UElementRef FProcessItemTemplate::Instantiate(UControl & Control, const FVariant & Variant)
	{
		FProcessItem & ProcessItem = Variant.Get<FProcessItem &>();
		UTextBlockRef TextBlock = MakeRefer<UTextBlock>();
		TextBlock->Text = FText(ProcessItem.ProcessName);
		TextBlock->Color = Control.Foreground;
		TextBlock->FontFamily = Binding(Control, UControl::FontFamilyProperty());
		TextBlock->FontSize = Binding(Control, UControl::FontSizeProperty());
		TextBlock->FontWeight = Binding(Control, UControl::FontWeightProperty());
		TextBlock->FontStyle = Binding(Control, UControl::FontStyleProperty());
		TextBlock->HorizontalAlignment = Binding(&Control, UControl::ContentHorizontalAlignmentProperty());
		TextBlock->VerticalAlignment = Binding(&Control, UControl::ContentVerticalAlignmentProperty());
		return TextBlock;
	}

	UElementRef FProcessItemTemplate_IconAndName::Instantiate(UControl & Control, const FVariant & Variant)
	{
		FProcessItem & ProcessItem = Variant.Get<FProcessItem &>();
		UShellItemImageRef ShellItemImage = MakeRefer<UShellItemImage>();
		{
			ShellItemImage->Size = { 32, 32 };
			ShellItemImage->Margin = { 0, 0, 4, 0 };
			ShellItemImage->VerticalAlignment = EElementAlignment::Center;
			ShellItemImage->ItemImageFactory = ShellItemImageCache.FetchModuleIcon(ProcessItem.FilePath);
		}
		UTextBlockRef Text = MakeRefer<UTextBlock>();
		{
			Text->Text = FText(ProcessItem.ProcessName);
			Text->Color = Control.Foreground;
			Text->FontFamily = Binding(Control, UControl::FontFamilyProperty());
			Text->FontSize = Binding(Control, UControl::FontSizeProperty());
			Text->FontWeight = Binding(Control, UControl::FontWeightProperty());
			Text->FontStyle = Binding(Control, UControl::FontStyleProperty());
			Text->HorizontalAlignment = Binding(&Control, UControl::ContentHorizontalAlignmentProperty());
			Text->VerticalAlignment = Binding(&Control, UControl::ContentVerticalAlignmentProperty());
		}
		UStackPanelRef StackPanel = MakeRefer<UStackPanel>();
		StackPanel->Orientation = EOrientation::Horizontal;
		StackPanel->Elements = { ShellItemImage, Text };
		return StackPanel;
	}
}
