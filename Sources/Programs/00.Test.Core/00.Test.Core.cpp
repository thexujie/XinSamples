#include "PCH.h"

#include <iostream>

#include "00.Test.CoreTypes.h"

#include "TestReflection.h"

Xin::TPackIndexing<0, int &, float, Xin::FString> && Forward2(Xin::TPackIndexing<0, int &, float, Xin::FString> & aa) noexcept
{
	return static_cast<Xin::TPackIndexing<0, int &, float, Xin::FString> && >(aa);
}

int main(int argc, const char * args[])
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	//_CrtSetBreakAlloc(389);
	FConsole::SetEncoding(FEncoding::UTF8);
	FConsole::WriteLine(u8"FString.Name={}"V, Typeof<FString>().Name);

	FString TimeStamp = FDateTime::Current().Format(u8"yy-MM-dd HH:mm:ss"V);

	//FType & StringType = Typeof<FString>();

	FEventArgs Args;

	{

		int a = 12;
		float b = 3;
		FString S = u8"HH"V;
		std::tuple<int &, float, FString> tt { a, b, S };
		std::tuple<int &, double, FString> T2(tt);
		//TTuple<int &, double, FString> T3 = tt;
		//TTuple<int &, double, FString> T2 = Move(tt);
	}
	{
		int a = 12;
		float b = 3;
		FString S = u8"HH"V;

		TTuple<FString> ta { S };
		TTuple<FString> tb { ta };
		TTuple<FString> tc { Move(ta) };

		auto & [ba] = tc;
		ba = FString::None;

		ta = tc;
		Xin::Get<0>(tc) = u8"fff"V;
		tb = Move(tc);
		TTuple<int &, float, FString> tt { a, b, S };
		static_assert(IsTupleV<TTuple<int &, float, FString>>);
		static_assert(IsTupleV<TTuple<int &, float, FString>>);
		using XXf = PackIndexingT<0, int &, float, FString>;
		XXf cc = a;

		static_assert(IsTupleStorageV<TTuple<int &, float, FString>::TupleStorageT>);
		TTuple<int &, double, FString> T2(tt);
		TTuple<int &, double, FString> T3(Move(tt));
		cc = a;
		
	}

	//FName2 Used;

	//auto & UInt32Type = Typeof<uint32>();
	constexpr FName NameFF = u8"方法"N;
	constexpr FName NameXJ = MakeFixedName<u8"许杰">();

	FConsole::WriteLine(u8"NameXJ={} and {}"V, NameFF.Resolve(), NameXJ.Resolve());

	const FType & Ta = Typeof<FString>();
	TReferenceWrapper<const FType> TT = Typeof<FString>();
	bool TTE = TT == Typeof<FString>();
	bool TTE2 = TT == Typeof<IObject>();

	FName NameTest(u8"事实名字"V);
	const TClass<FTestClass> & Class = static_cast<TClass<FTestClass> &>(FTestClass::StaticType());

	const TConstructor<FTestClass, int, float, bool> & Constructor = Class.FindConstructorT<FTestClass, int, float, bool>();
	const TConstructor<FTestClass, int, float, double> & Constructorx = Class.FindConstructorT<FTestClass, int, float, double>();

	FVariant Instance = Constructor.Invoke({ 111, 234.0, 0.0f });
	//FVariant Instance = Constructor.Invoke();
	//FVariant Instance = Constructor3.Invoke(33, 23.0f, true);
	FTestClass & TestClass = Instance.Get<FTestClass &>();
	//FTestClass & TestClass = *Instance.Detatch<FTestClass>();
	TestClass.IntValue = 34;

	constexpr FName NID = u8"FriendlyName"N;

	const TProperty<FName> & FriendlyName = Class.FindPropertyT<FName>(u8"FriendlyName"N);
	const TProperty<FColor> & DisplayColor = Class.FindPropertyT<FColor>(u8"DisplayColor"N);

	const TInstanceProperty<FTestClass, int32> & IntValueProperty = Class.FindPropertyT<int32>(u8"IntValue"V);
	IntValueProperty.SetValue(TestClass, 23);

	//const TProperty<FTestClass, bool> & BoolValueProperty = Class.FindPropertyT<bool>(u8"BoolValue"V);
	//BoolValueProperty.SetValue(TestClass, true);

	//const TProperty<FTestClass, float32> & FloatValueProperty = Class.FindPropertyT<float32>(u8"FloatValue"V);
	//FloatValueProperty.SetValue(TestClass, 233);

	FVariant Var;
	bool bb = Var.Get<bool>();

	auto & Add = Class.FindMethodT<int32(int32, int32)>(u8"Add"V);
	int32 Result2 = Add.Invoke(TestClass, 10, 22);

	auto & Mulitply = Class.FindMethodT<int32(const int32 &, int32 &)>(u8"Mulitply"V);
	int32 Result2a = Mulitply.Invoke(TestClass, 10, Result2);

	auto & AddConst = Class.FindMethodT<int32(int32, int32)>(u8"Add"V);
	int32 Result20 = AddConst.Invoke(TestClass, 10, 22);

	auto & AddNoneConst = Class.FindMethodT<int32(int32, int32)>(u8"AddNoneConst"V);
	int32 Result21 = AddNoneConst.Invoke(TestClass, 10, 22); 

	auto & Subtract = Class.FindMethodT<int32(int32, int32)>(u8"Subtract"V);
	int32 Result3 = Subtract.Invoke(TestClass, 10, 22);

	auto & Print = Class.FindMethodT<void(float32)>(u8"Print"V);
	Print.Invoke(TestClass, 10.0f);

	auto & GetName = Class.FindMethodT<FStringV()>(u8"GetName"V);
	auto Name = GetName.Invoke(TestClass);

	auto & FormatValue = Class.FindMethodT<FStringV(FStringV, float32)>(u8"FormatValue"V);
	auto Name22 = FormatValue.Invoke(TestClass, u8"{:10.4}"V, 3.1415926f);

	const FMethod & FormatValue2 = Class.FindMethod(u8"FormatValue"V);
	FVariant Name222 = FormatValue2.Invoke(&TestClass, { u8"{:10.4}"V, 3.1415926f });
	auto Result = Name222.Get<const FString &>();
	const FProperty & NameField = Class.FindProperty(u8"Name"N);
	NameField.SetValue(TestClass, u8"大大的惊喜"V);

	const FProperty & Vector4Field = Class.FindProperty(u8"Vector4Value"N);
	Vector4Field.SetValue(TestClass, FVector4 { 1, 2, 3, 4 });

	auto Result22 = Vector4Field.GetValue(TestClass);
	//FConsole::ReadKey(u8"..."V);
	return 0;
}
