#pragma once

#include "X.TaskMonitor.Types.h"
#include <shobjidl_core.h>

namespace Xin::TaskMonitor
{
	using IShellItemImageFactoryRef = TReferPtr<IShellItemImageFactory>;

	class FShellItemImageCache
	{
	public:
		FShellItemImageCache() = default;
		~FShellItemImageCache() = default;

		IShellItemImageFactoryRef FetchModuleIcon(FStringV ShellItemPath);
		TMap<FString, IShellItemImageFactoryRef> IconCache;
	};

	class UShellItemImage : public UElement
	{
	public:
		static FType & StaticType();
		FType & Type() const override { return StaticType(); }

	public:
		UShellItemImage() = default;
		~UShellItemImage() = default;

	public:
		void OnConstruct() override;
		void OnFinalize() override;
		FSize OnMeasure(FSize ConstrainedSize) override;
		void OnPaint(IPainter & Painter) override;

	public:
		IBitmapRef Bitmap;
		TReferPtr<IShellItemImageFactory> ItemImageFactory;

	public:
		DependencyPropertyRW(EStretch, Stretch) = EStretch::Fill;
	};
	using UShellItemImageRef = TReferPtr<UShellItemImage>;
}
