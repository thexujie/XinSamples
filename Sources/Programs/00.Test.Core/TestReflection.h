#pragma once

#include "Xin.Core/Xin.Core.h"

using namespace Xin;

struct NONE_API FGlobalParameters
{
	XinType(FriendlyName("测试基类型A"), DisplayColor(10, 23, 23));
	FVector3 LightPositin;
	FVector3 LightDirection;
};

class NONE_API FTestClassBaseA
{
public:
	XinType(FriendlyName("测试基类型A"), DisplayColor(10, 23, 23));

	FTestClassBaseA() = default;

	XinFunction(FriendlyName("挺好的"));

	int AddBasic(int ValueA, int ValueB)
	{
		return ValueA + ValueB;
	}
};

class NONE_API FTestClassBaseB
{
public:
	XinType(FriendlyName("测试基类型B"), DisplayColor(10, 23, 23));

public:
	FTestClassBaseB() = default;

	XinFunction(FriendlyName("挺好的2"));

	int AddBasic(int ValueA, int ValueB)
	{
		return ValueA + ValueB;
	}
};

class NONE_API FTestClass : public FTestClassBaseA, public FTestClassBaseB /*, public FObject*/
{
public:
	XinType(FriendlyName("测试类型"), DisplayColor(12, 23, 23));

	FTestClass() = default;
	FTestClass(int IntValue) : TheIntValue(IntValue) {}
	FTestClass(int IntValue, float FloatValue, bool BoolValue) : TheIntValue(IntValue), TheFloatValue(FloatValue), TheBoolValue(BoolValue) { }

	XinFunction(FriendlyName ="Get Int Value", Internal, EditorOnly);
	int GetIntValue() const { return TheIntValue; }

	void SetIntValue(int Value) { TheIntValue = Value; }
	void OnIntValueChanged(const int & NewValue) {}
	float GetFloatValue() const { return TheFloatValue; }
	void SetBoolValue(bool Value) { TheBoolValue = Value; }

	//[[Xin(FriendlyName="加")]]
	int Add(int ValueA, int ValueB)
	{
		return TheIntValue + ValueA + ValueB;
	}

	//int Add(int ValueA, int ValueB) const
	//{
	//	return TheIntValue + ValueA + ValueB + 10000;
	//}

	int Subtract(int ValueA, int ValueB) const
	{
		return TheIntValue - ValueA - ValueB;
	}

	int Mulitply(const int & ValueA, int & ValueB) const
	{
		return TheIntValue - ValueA - ValueB;
	}

	int AddNoneConst(int ValueA, int ValueB)
	{
		return TheIntValue + ValueA + ValueB + 10000;
	}

	void Print(float Value) const { }

	FStringV GetName() { return u8"Test Name"; }

	FStringV FormatValue(FStringV Formal, float Value)
	{
		return Format(Formal, Value);
	}

public:
	int TheIntValue;
	float TheFloatValue;
	bool TheBoolValue;
	FString Name;
	FVector4 Vector4Value;
	static char TheCharValue;

public:
	PropertyRW(int, GetIntValue, SetIntValue) IntValue;

	PropertyR(float, GetFloatValue) FloatValue;
	PropertyW(bool, SetBoolValue) BoolValue;
};
