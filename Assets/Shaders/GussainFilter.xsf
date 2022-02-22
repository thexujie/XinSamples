#include "Common.hlsli"
#include "Utility.hlsli"

//#define NV_SHADER_EXTN_SLOT u0
//#define NV_SHADER_EXTN_REGISTER_SPACE space1
//#include <nvHLSLExtns.h>

Texture2D Source;

cbuffer ShaderParameter
{
    float2 BlurTexelSize; // 采样时的步长，一般是单个像素步长，也可以是多个像素步长
    float FilterSigmaS; // 1.0f / (2 * SigmaS * SigmaS)
}

static const uint SampleRadius = 5;

float4 DefaultPixelShader(float4 Position : SV_POSITION, float2 Texcoord : TEXCOORD) : SV_Target
{
    // 中心像素颜色
    float3 CenterColor = Source.SampleLevel(PointClampSampler, Texcoord, 0);
    float CenterWeight = 1; // 当像素距离差为 0 时（像素本身），权重为 1。

    float3 ColorSum = CenterColor; // 所有像素颜色求和
    float WeightSum = CenterWeight; // 所有像素权重求和
    
#if XIN_BLUR_GUSSAIN_LINEAR_SAMPLE
    for (int SampleIndex = 1; SampleIndex < SampleRadius; SampleIndex += 2)
    {
        float WeightSpatial = exp(-1 * pow(SampleIndex, 2) * FilterSigmaS);
        float WeightSpatialNext = exp(-1 * pow((SampleIndex + 1), 2) * FilterSigmaS);

        float LinearBlurWeight = (WeightSpatial + WeightSpatialNext);
        float LinearBlurOffset = WeightSpatialNext / LinearBlurWeight;

        // 正方向采样、计算亮度、计算权重，然后一起求和
        {
            float3 Color = Source.SampleLevel(LinearClampSampler, Texcoord + BlurTexelSize * (SampleIndex + LinearBlurOffset), 0);
            float Luminance = RGBToLuminance(Color);
            
            ColorSum += Color * (WeightSpatial + WeightSpatialNext);
            WeightSum += (WeightSpatial + WeightSpatialNext);
        }
        // 负方向采样、计算亮度、计算权重，然后一起求和
        {
            float3 Color = Source.SampleLevel(LinearClampSampler, Texcoord - BlurTexelSize * (SampleIndex + LinearBlurOffset), 0);
            float Luminance = RGBToLuminance(Color);
            float WeightSpatial = exp(-1 * pow(SampleIndex, 2) * FilterSigmaS);
            
            ColorSum += Color * (WeightSpatial + WeightSpatialNext);
            WeightSum += (WeightSpatial + WeightSpatialNext);
        }
    }
#else
    for (int SampleIndex = 1; SampleIndex < SampleRadius; ++SampleIndex)
    {
        {
            float3 Color = Source.SampleLevel(PointClampSampler, Texcoord + BlurTexelSize * SampleIndex, 0);
            float Luminance = RGBToLuminance(Color);
            float WeightSpatial = exp(-1 * pow(SampleIndex, 2) * FilterSigmaS);
            
            ColorSum += Color * WeightSpatial;
            WeightSum += WeightSpatial;
        }
        {
            float3 Color = Source.SampleLevel(PointClampSampler, Texcoord - BlurTexelSize * SampleIndex, 0);
            float Luminance = RGBToLuminance(Color);
            float WeightSpatial = exp(-1 * pow(SampleIndex, 2) * FilterSigmaS);
            
            ColorSum += Color * WeightSpatial;
            WeightSum += WeightSpatial;
        }
    }
#endif
    return float4(ColorSum / WeightSum, 1);
}
