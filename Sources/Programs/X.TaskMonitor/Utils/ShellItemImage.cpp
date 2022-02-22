#include "PCH.h"
#include "ShellItemImage.h"

#include <TlHelp32.h>
#include <shobjidl_core.h>

#include "Xin.Core/Platform/Windows/WindowsPrivate.h"

namespace Xin::TaskMonitor
{
	IShellItemImageFactoryRef FShellItemImageCache::FetchModuleIcon(FStringV ShellItemPath)
	{
		if (auto Iter = IconCache.Find(ShellItemPath); Iter != IconCache.End())
			return Iter->Second;

		FWString ShellItemPathW = FEncoding::ToWide(ShellItemPath);
		ShellItemPathW.Replace(L"/"V, L"\\"V);

		TReferPtr<IShellItemImageFactory> ItemImageFactory;
		HRESULT Result = SHCreateItemFromParsingName(ShellItemPathW.Data, nullptr, IID_IShellItemImageFactory, ItemImageFactory.GetVV());
		if (Windows::Succeeded(Result))
		{
			IconCache.Insert({ ShellItemPath, ItemImageFactory });
			return ItemImageFactory;
		}
		return nullptr;
	}

	void UShellItemImage::OnConstruct()
	{
		UElement::OnConstruct();
		HitInside = EHitResult::Ignore;
	}

	void UShellItemImage::OnFinalize()
	{
		Bitmap = nullptr;
		UElement::OnFinalize();
	}

	FSize UShellItemImage::OnMeasure(FSize ConstrainedSize)
	{
		if (!Bitmap)
			return DefaultSize;
		return Bitmap->Size;
	}

	void UShellItemImage::OnPaint(IPainter & Painter)
	{
		if (ItemImageFactory && !Bitmap)
		{
			SIIGBF Flags = 0;
			HBITMAP hBitmap = NULL;
			HRESULT Result = ItemImageFactory->GetImage({ (LONG)FinalSize.Width, (LONG)FinalSize.Height }, Flags, &hBitmap);
			if (Windows::Succeeded(Result))
			{
				Bitmap = Graphics.CreateBitmap(Windows::FPixmapHBITMAP(hBitmap));
				DeleteObject(hBitmap);
			}
		}

		if (!Bitmap)
			return;

		switch (Stretch)
		{
		case EStretch::None:
			Painter.DrawBitmap(Bitmap.Ref(), { FPoint::Zero, Bitmap->Size });
			break;
		case EStretch::Fill:
			Painter.DrawBitmap(Bitmap.Ref(), { FPoint::Zero, FinalSize });
			break;
		case EStretch::Uniform:
		{
			float32 ScaleValue = (float32)Min(FinalSize.Width / Bitmap->Size.Width, FinalSize.Height / Bitmap->Size.Height);
			SizeF Size = Bitmap->Size * ScaleValue;
			RectF Rect { float32(FinalSize.Width - Size.Width) * 0.5f, float32(FinalSize.Height - Size.Height) * 0.5f, Size.Width, Size.Height };
			Painter.DrawBitmap(Bitmap.Ref(), Rect);
			break;
		}
		case EStretch::UniformFill:
			break;
		default:;
		}
	}
}
