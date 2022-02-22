#include "Common.hlsli"
#include "Utility.hlsli"

Texture2D Source;

#define DefaultSampler PointClampSampler
//#define DefaultSampler PointBorderSampler

cbuffer ShaderParameter
{
    float2 BlurTexelSize;
    float FilterSigmaS; // 1.0f / (2 * SigmaS * SigmaS)
    float FilterSigmaR; // 1.0f / (2 * SigmaR * SigmaR)
    uint SampleCount;
}

float4 DefaultPixelShader(float4 Position : SV_POSITION, float2 Texcoord : TEXCOORD) : SV_Target
{
    float3 PixelCenter = Source.SampleLevel(DefaultSampler, Texcoord, 0);
    float LuminanceCenter = RGBToLuminance(PixelCenter);

    float3 ColorSum = PixelCenter;
    float WeightSum = 1;
    for (int SampleIndex = 1; SampleIndex < SampleCount; ++SampleIndex)
    {
        float3 PixelA = Source.SampleLevel(DefaultSampler, Texcoord + BlurTexelSize * SampleIndex, 0);
        float3 PixelB = Source.SampleLevel(DefaultSampler, Texcoord - BlurTexelSize * SampleIndex, 0);
        float LuminanceA = RGBToLuminance(PixelA);
        float LuminanceB = RGBToLuminance(PixelB);

        // 位置权重，左右两个像素共用
        float WeightS = exp(-1 * pow(SampleIndex, 2) * FilterSigmaS);

        // 亮度权重，两个像素分别计算
        // Pixel A
        float WeightA = WeightS * exp(-1 * pow(LuminanceA - LuminanceCenter, 2) * FilterSigmaR);
        ColorSum += PixelA * WeightA;
        WeightSum += WeightA;
        // Pixel B
        float WeightB = WeightS * exp(-1 * pow(LuminanceB - LuminanceCenter, 2) * FilterSigmaR);
        ColorSum += PixelB * WeightB;
        WeightSum += WeightB;
    }
    return float4(ColorSum / WeightSum, 1);
}
