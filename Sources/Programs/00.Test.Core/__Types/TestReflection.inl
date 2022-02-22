// A412CCE4E020349B8EA92349B0B46D6F 2023-09-20 10:13:46
// Generated code begin.

#pragma once
#include "TestReflection.h"

// FGlobalParameters
class FGlobalParameters_Type : public TClass<FGlobalParameters>
{
public:
	// --- 0 Constructors
	static inline const TConstructor<FGlobalParameters, const FGlobalParameters &> Constructor___const_FGlobalParameters_lref { u8"!Constructor___const_FGlobalParameters_lref"N, FMetadata::None };
	static inline const TConstructor<FGlobalParameters, FGlobalParameters &&> Constructor___FGlobalParameters_rref { u8"!Constructor___FGlobalParameters_rref"N, FMetadata::None };
	static inline const TReferenceWrapper<const FConstructor> ReflectedConstructors[] =
	{
		Constructor___const_FGlobalParameters_lref,
		Constructor___FGlobalParameters_rref,
	};

	// --- 1 Methods

	// --- 2 Properties

	// --- 4 Type
	FGlobalParameters_Type(FName Name = u8"FGlobalParameters"N) : TClass<FGlobalParameters>(Name)
	{
	}
};

inline FType & FGlobalParameters::StaticType()
{
	static FGlobalParameters_Type FGlobalParametersType;
	return FGlobalParametersType;
}


// FTestClassBaseA
class FTestClassBaseA_Type : public TClass<FTestClassBaseA>
{
public:
	// --- 0 Constructors
	static inline const TConstructor<FTestClassBaseA> Constructor { u8"!Constructor"N, FMetadata::None };
	static inline const TConstructor<FTestClassBaseA, const FTestClassBaseA &> Constructor___const_FTestClassBaseA_lref { u8"!Constructor___const_FTestClassBaseA_lref"N, FMetadata::None };
	static inline const TConstructor<FTestClassBaseA, FTestClassBaseA &&> Constructor___FTestClassBaseA_rref { u8"!Constructor___FTestClassBaseA_rref"N, FMetadata::None };
	static inline const TReferenceWrapper<const FConstructor> ReflectedConstructors[] =
	{
		Constructor,
		Constructor___const_FTestClassBaseA_lref,
		Constructor___FTestClassBaseA_rref,
	};

	// --- 1 Methods
	struct int32___AddBasic__int32__int32
	{
		static inline const TMethod<FTestClassBaseA, int32(int32, int32)> Method { u8"AddBasic"N, &FTestClassBaseA::AddBasic, FMetadata::None };
	};

	static inline const TReferenceWrapper<const FMethod> ReflectedMethods[] =
	{
		int32___AddBasic__int32__int32::Method,
	};

	// --- 2 Properties


	// --- 4 Type
	FTestClassBaseA_Type(FName Name = u8"FTestClassBaseA"N) : TClass<FTestClassBaseA>(Name)
	{
	}
};

inline FType & FTestClassBaseA::StaticType()
{
	static FTestClassBaseA_Type FTestClassBaseAType;
	return FTestClassBaseAType;
}


// FTestClassBaseB
class FTestClassBaseB_Type : public TClass<FTestClassBaseB>
{
public:
	// --- 0 Constructors
	static inline const TConstructor<FTestClassBaseB> Constructor { u8"!Constructor"N, FMetadata::None };
	static inline const TConstructor<FTestClassBaseB, const FTestClassBaseB &> Constructor___const_FTestClassBaseB_lref { u8"!Constructor___const_FTestClassBaseB_lref"N, FMetadata::None };
	static inline const TConstructor<FTestClassBaseB, FTestClassBaseB &&> Constructor___FTestClassBaseB_rref { u8"!Constructor___FTestClassBaseB_rref"N, FMetadata::None };
	static inline const TReferenceWrapper<const FConstructor> ReflectedConstructors[] =
	{
		Constructor,
		Constructor___const_FTestClassBaseB_lref,
		Constructor___FTestClassBaseB_rref,
	};

	// --- 1 Methods
	struct int32___AddBasic__int32__int32
	{
		static inline const TMethod<FTestClassBaseB, int32(int32, int32)> Method { u8"AddBasic"N, &FTestClassBaseB::AddBasic, FMetadata::None };
	};

	static inline const TReferenceWrapper<const FMethod> ReflectedMethods[] =
	{
		int32___AddBasic__int32__int32::Method,
	};

	// --- 2 Properties

	// --- 4 Type
	FTestClassBaseB_Type(FName Name = u8"FTestClassBaseB"N) : TClass<FTestClassBaseB>(Name, FClass::None)
	{
	}
};

inline FType & FTestClassBaseB::StaticType()
{
	static FTestClassBaseB_Type FTestClassBaseBType;
	return FTestClassBaseBType;
}


// FTestClass
class FTestClass_Type : public TClass<FTestClass>
{
public:
	// --- 0 Constructors
	static inline const TConstructor<FTestClass> Constructor { u8"!Constructor"N, FMetadata::None };
	static inline const TConstructor<FTestClass, int32> Constructor___int32 { u8"!Constructor___int32"N, FMetadata::None };
	static inline const TConstructor<FTestClass, int32, float32, bool> Constructor___int32__float32__bool { u8"!Constructor___int32__float32__bool"N, FMetadata::None };
	static inline const TConstructor<FTestClass, const FTestClass &> Constructor___const_FTestClass_lref { u8"!Constructor___const_FTestClass_lref"N, FMetadata::None };
	static inline const TConstructor<FTestClass, FTestClass &&> Constructor___FTestClass_rref { u8"!Constructor___FTestClass_rref"N, FMetadata::None };
	static inline const TReferenceWrapper<const FConstructor> ReflectedConstructors[] =
	{
		Constructor,
		Constructor___int32,
		Constructor___int32__float32__bool,
		Constructor___const_FTestClass_lref,
		Constructor___FTestClass_rref,
	};

	// --- 1 Methods
	struct int32___GetIntValue____const
	{
		static inline const TMethod<FTestClass, int32()> Method { u8"GetIntValue"N, &FTestClass::GetIntValue, FMetadata::None };
	};

	struct void___SetIntValue__int32
	{
		static inline const TMethod<FTestClass, void(int32)> Method { u8"SetIntValue"N, &FTestClass::SetIntValue, FMetadata::None };
	};

	struct void___OnIntValueChanged__const_int32_lref
	{
		static inline const TMethod<FTestClass, void(const int32 &)> Method { u8"OnIntValueChanged"N, &FTestClass::OnIntValueChanged, FMetadata::None };
	};

	struct float32___GetFloatValue____const
	{
		static inline const TMethod<FTestClass, float32()> Method { u8"GetFloatValue"N, &FTestClass::GetFloatValue, FMetadata::None };
	};

	struct void___SetBoolValue__bool
	{
		static inline const TMethod<FTestClass, void(bool)> Method { u8"SetBoolValue"N, &FTestClass::SetBoolValue, FMetadata::None };
	};

	struct int32___Add__int32__int32
	{
		static inline const TMethod<FTestClass, int32(int32, int32)> Method { u8"Add"N, &FTestClass::Add, FMetadata::None };
	};

	struct int32___Subtract__int32__int32____const
	{
		static inline const TMethod<FTestClass, int32(int32, int32)> Method { u8"Subtract"N, &FTestClass::Subtract, FMetadata::None };
	};

	struct int32___Mulitply__const_int32_lref__int32_lref____const
	{
		static inline const TMethod<FTestClass, int32(const int32 &, int32 &)> Method { u8"Mulitply"N, &FTestClass::Mulitply, FMetadata::None };
	};

	struct int32___AddNoneConst__int32__int32
	{
		static inline const TMethod<FTestClass, int32(int32, int32)> Method { u8"AddNoneConst"N, &FTestClass::AddNoneConst, FMetadata::None };
	};

	struct void___Print__float32____const
	{
		static inline const TMethod<FTestClass, void(float32)> Method { u8"Print"N, &FTestClass::Print, FMetadata::None };
	};

	struct TView__char8_____GetName
	{
		static inline const TMethod<FTestClass, TView<char8>()> Method { u8"GetName"N, &FTestClass::GetName, FMetadata::None };
	};

	struct TView__char8_____FormatValue__TView__char8____float32
	{
		static inline const TMethod<FTestClass, TView<char8>(TView<char8>, float32)> Method { u8"FormatValue"N, &FTestClass::FormatValue, FMetadata::None };
	};

	static inline const TReferenceWrapper<const FMethod> ReflectedMethods[] =
	{
		int32___GetIntValue____const::Method,
		void___SetIntValue__int32::Method,
		void___OnIntValueChanged__const_int32_lref::Method,
		float32___GetFloatValue____const::Method,
		void___SetBoolValue__bool::Method,
		int32___Add__int32__int32::Method,
		int32___Subtract__int32__int32____const::Method,
		int32___Mulitply__const_int32_lref__int32_lref____const::Method,
		int32___AddNoneConst__int32__int32::Method,
		void___Print__float32____const::Method,
		TView__char8_____GetName::Method,
		TView__char8_____FormatValue__TView__char8____float32::Method,
	};

	// --- 2 Properties
	struct int32___TheIntValue
	{
		static inline const TFieldProperty Property { u8"TheIntValue"N, &FTestClass::TheIntValue, FMetadata::None };
	};
	struct float32___TheFloatValue
	{
		static inline const TFieldProperty Property { u8"TheFloatValue"N, &FTestClass::TheFloatValue, FMetadata::None };
	};
	struct bool___TheBoolValue
	{
		static inline const TFieldProperty Property { u8"TheBoolValue"N, &FTestClass::TheBoolValue, FMetadata::None };
	};
	struct int32___IntValue
	{
		static inline const TFunctorProperty Property { u8"IntValue"N, &FTestClass::GetIntValue, &FTestClass::SetIntValue, FMetadata::None };
	};
	struct float32___FloatValue
	{
		static inline const TFunctorProperty Property { u8"FloatValue"N, &FTestClass::GetFloatValue, nullptr, FMetadata::None };
	};
	struct bool___BoolValue
	{
		static inline const TFunctorProperty Property { u8"BoolValue"N, nullptr, &FTestClass::SetBoolValue, FMetadata::None };
	};

	static inline const TReferenceWrapper<const FProperty> ReflectedProperties[] =
	{
		int32___TheIntValue::Property,
		float32___TheFloatValue::Property,
		bool___TheBoolValue::Property,
		int32___IntValue::Property,
		float32___FloatValue::Property,
		bool___BoolValue::Property,
	};

	// --- 4 Type
	FTestClass_Type(FName Name = u8"FTestClass"N) : TClass<FTestClass>(Name)
	{
	}
};

inline FType & FTestClass::StaticType()
{
	static FTestClass_Type FTestClassType;
	return FTestClassType;
}

// Generated code end.
