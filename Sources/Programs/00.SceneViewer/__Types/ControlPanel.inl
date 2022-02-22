// 55008C9512EFFB086ED2F9E6E69C88A3 2025-04-02 15:59:12
// Generated code begin.

#pragma once
#include "PCH.h"
#include "ControlPanel.h"

namespace Xin
{
	class FControlPanelDataSource_Type : public TClass<FControlPanelDataSource>
	{
	public:
		// --- 0 Constructors

		// --- 1 Methods
		struct Method___FType_lref___Type____const
		{
			static inline const TMethod<FControlPanelDataSource, FType &()> Method { u8"Type"N, &FControlPanelDataSource::Type, FMetadata::None };
		};
		static inline const TReferenceWrapper<const FMethod> ReflectedMethods[] =
		{
			Method___FType_lref___Type____const::Method,
		};

		// --- 2 Fields
		struct Field___SSAOLevel
		{
			static inline const TField Field { u8"SSAOLevel"N, &FControlPanelDataSource::SSAOLevel, FMetadata::None };
		};
		struct Field___SSAOQuality
		{
			static inline const TField Field { u8"SSAOQuality"N, &FControlPanelDataSource::SSAOQuality, FMetadata::None };
		};
		static inline const TReferenceWrapper<const FField> ReflectedFields[] =
		{
			Field___SSAOLevel::Field,
			Field___SSAOQuality::Field,
		};

		// --- 3 Properties
		struct Property___SSAOMode
		{
			static inline const TProxyProperty Property { u8"SSAOMode"N, &FControlPanelDataSource::__SSAOMode, &FControlPanelDataSource::__GetSSAOMode, &FControlPanelDataSource::__SetSSAOMode, FMetadata::None };
		};
		struct Property___PostProcessMode
		{
			static inline const TProxyProperty Property { u8"PostProcessMode"N, &FControlPanelDataSource::__PostProcessMode, &FControlPanelDataSource::__GetPostProcessMode, &FControlPanelDataSource::__SetPostProcessMode, FMetadata::None };
		};
		struct Property___BloomMode
		{
			static inline const TProxyProperty Property { u8"BloomMode"N, &FControlPanelDataSource::__BloomMode, &FControlPanelDataSource::__GetBloomMode, &FControlPanelDataSource::__SetBloomMode, FMetadata::None };
		};
		struct Property___FXAAMode
		{
			static inline const TProxyProperty Property { u8"FXAAMode"N, &FControlPanelDataSource::__FXAAMode, &FControlPanelDataSource::__GetFXAAMode, &FControlPanelDataSource::__SetFXAAMode, FMetadata::None };
		};
		static inline const TReferenceWrapper<const FProperty> ReflectedProperties[] =
		{
			Property___SSAOMode::Property,
			Property___PostProcessMode::Property,
			Property___BloomMode::Property,
			Property___FXAAMode::Property,
		};

		// --- 4 Base Types
		static inline const TReferenceWrapper<const FClass> ReflectedBaseTypes[] =
		{
			Classof<IDependency>(),
		};

		// --- 5 Constructor
		FControlPanelDataSource_Type(FName Name = u8"FControlPanelDataSource"N) : TClass(Name, Classof<IDependency>())
		{
			FClass::Fields = ReflectedFields;
			FClass::Methods = ReflectedMethods;
			FClass::Properties = ReflectedProperties;
			FClass::BaseTypes = ReflectedBaseTypes;
		}
	};

	const TProperty<Engine::ESSAOMode> & FControlPanelDataSource::SSAOModeProperty() { return FControlPanelDataSource_Type::Property___SSAOMode::Property; }

	const TProperty<Engine::EPostProcessMode> & FControlPanelDataSource::PostProcessModeProperty() { return FControlPanelDataSource_Type::Property___PostProcessMode::Property; }

	const TProperty<Engine::EBloomMode> & FControlPanelDataSource::BloomModeProperty() { return FControlPanelDataSource_Type::Property___BloomMode::Property; }

	const TProperty<Engine::EFXAAMode> & FControlPanelDataSource::FXAAModeProperty() { return FControlPanelDataSource_Type::Property___FXAAMode::Property; }

	template<> struct XIN_EXPORT TStaticType<FControlPanelDataSource>
	{
		static inline FControlPanelDataSource_Type FControlPanelDataSourceType { u8"FControlPanelDataSource"N };

		static FType & Type()
		{
			return FControlPanelDataSourceType;
		}
	};

	FType & FControlPanelDataSource::StaticType()
	{
		return TStaticType<FControlPanelDataSource>().Type();
	}

	static TTypeRegister<FControlPanelDataSource> FControlPanelDataSourceTypeRegister { TStaticType<FControlPanelDataSource>().Type() };
}

// Generated code end.
