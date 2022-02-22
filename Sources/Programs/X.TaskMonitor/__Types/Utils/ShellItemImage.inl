// 2CC80AD702D2E746A8605AC479D9A7AC 2025-03-03 17:41:01
// Generated code begin.

#pragma once
#include "PCH.h"
#include "Utils/ShellItemImage.h"

namespace Xin
{
	class UShellItemImage_Type : public TClass<TaskMonitor::UShellItemImage>
	{
	public:
		using UShellItemImage = TaskMonitor::UShellItemImage;

		// --- 0 Constructors
		static inline const TConstructor<UShellItemImage> Constructor { u8"!Constructor"N, FMetadata::None };
		static inline const TReferenceWrapper<const FConstructor> ReflectedConstructors[] =
		{
			Constructor,
		};

		// --- 1 Methods
		struct Method___FType_lref___Type____const
		{
			static inline const TMethod<UShellItemImage, FType &()> Method { u8"Type"N, &UShellItemImage::Type, FMetadata::None };
		};
		struct Method___void___OnConstruct
		{
			static inline const TMethod<UShellItemImage, void()> Method { u8"OnConstruct"N, &UShellItemImage::OnConstruct, FMetadata::None };
		};
		struct Method___void___OnFinalize
		{
			static inline const TMethod<UShellItemImage, void()> Method { u8"OnFinalize"N, &UShellItemImage::OnFinalize, FMetadata::None };
		};
		struct Method___TVec2__float64_____OnMeasure__TVec2__float64__
		{
			static inline const TMethod<UShellItemImage, FSize(FSize)> Method { u8"OnMeasure"N, &UShellItemImage::OnMeasure, FMetadata::None };
		};
		struct Method___void___OnPaint__IPainter_lref
		{
			static inline const TMethod<UShellItemImage, void(IPainter &)> Method { u8"OnPaint"N, &UShellItemImage::OnPaint, FMetadata::None };
		};
		static inline const TReferenceWrapper<const FMethod> ReflectedMethods[] =
		{
			Method___FType_lref___Type____const::Method,
			Method___void___OnConstruct::Method,
			Method___void___OnFinalize::Method,
			Method___TVec2__float64_____OnMeasure__TVec2__float64__::Method,
			Method___void___OnPaint__IPainter_lref::Method,
		};

		// --- 2 Fields
		struct Field___Bitmap
		{
			static inline const TField Field { u8"Bitmap"N, &UShellItemImage::Bitmap, FMetadata::None };
		};
		struct Field___ItemImageFactory
		{
			static inline const TField Field { u8"ItemImageFactory"N, &UShellItemImage::ItemImageFactory, FMetadata::None };
		};
		static inline const TReferenceWrapper<const FField> ReflectedFields[] =
		{
			Field___Bitmap::Field,
			Field___ItemImageFactory::Field,
		};

		// --- 3 Properties
		struct Property___Stretch
		{
			static inline const TDependencyProperty Property { u8"Stretch"N, &UShellItemImage::__Stretch, &UShellItemImage::__GetStretch, &UShellItemImage::__SetStretchValue, UShellItemImage::__Stretch__, UShellItemImage::FPropertyMetadata::None };
		};
		static inline const TReferenceWrapper<const FProperty> ReflectedProperties[] =
		{
			Property___Stretch::Property,
		};

		// --- 4 Base Types
		static inline const TReferenceWrapper<const FClass> ReflectedBaseTypes[] =
		{
			Classof<UI::UElement>(),
		};

		// --- 5 Constructor
		UShellItemImage_Type(FName Name = u8"UShellItemImage"N) : TClass(Name, Classof<UI::UElement>())
		{
			FClass::Constructors = ReflectedConstructors;
			FClass::Fields = ReflectedFields;
			FClass::Methods = ReflectedMethods;
			FClass::Properties = ReflectedProperties;
			FClass::BaseTypes = ReflectedBaseTypes;
		}
	};

	const TProperty<UI::EStretch> & TaskMonitor::UShellItemImage::StretchProperty() { return UShellItemImage_Type::Property___Stretch::Property; }

	template<> struct XIN_EXPORT TStaticType<TaskMonitor::UShellItemImage>
	{
		static FType & Type()
		{
			static UShellItemImage_Type UShellItemImageType { u8"UShellItemImage"N };
			return UShellItemImageType;
		}
	};

	FType & TaskMonitor::UShellItemImage::StaticType()
	{
		return TStaticType<TaskMonitor::UShellItemImage>().Type();
	}

	static TTypeRegister<TaskMonitor::UShellItemImage> UShellItemImageTypeRegister { TStaticType<TaskMonitor::UShellItemImage>().Type() };
}

// Generated code end.
