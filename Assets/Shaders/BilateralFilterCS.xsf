#include "Common.hlsli"
#include "Utility.hlsli"

#define XIN_BILATERAL_FILTER_PACKED 1

Texture2D<float3> Input;
RWTexture2D<float3> Output;

cbuffer ShaderParameter
{
    float2 InputTexelSize;
    float FilterSigmaS; // 1.0f / (2 * SigmaS * SigmaS)
    float FilterSigmaR; // 1.0f / (2 * SigmaR * SigmaR)
    //uint SampleRadius; // 采样的像素数量
}

float3 FetchPixel(int2 PixelCoord)
{
    return Input[PixelCoord];
}

float3 SamplePixel(float2 Texcoord, int2 Offset)
{
    return Input.SampleLevel(PointClampSampler, Texcoord, 0, Offset);
}

static const int SampleRadius = 5;
static const int SampleCount = SampleRadius * 2 - 1;

#if XIN_BILATERAL_FILTER_PACKED
groupshared uint2 CachedPixels[256];
#else
groupshared float4 CachedPixels[256];
#endif


void StorePixel(uint2 PixelCoord, float3 Color)
{
    uint PixelIndex = PixelCoord.x + (PixelCoord.y << 4);
#if XIN_BILATERAL_FILTER_PACKED
    CachedPixels[PixelIndex] = uint2((f32tof16(Color.r) << 16) | f32tof16(Color.g), (f32tof16(Color.b) << 16) | f32tof16(RGBToLuminance(Color)));
#else
    CachedPixels[PixelIndex] = float4(Color, RGBToLuminance(Color));
#endif
}

float4 LoadPixel(uint2 PixelCoord)
{
    uint PixelIndex = PixelCoord.x + (PixelCoord.y << 4);
#if XIN_BILATERAL_FILTER_PACKED
    uint2 PackedPixel = CachedPixels[PixelIndex];
    return float4(f16tof32(PackedPixel.x >> 16), f16tof32(PackedPixel.x & 0xFFFF), f16tof32(PackedPixel.y >> 16), f16tof32(PackedPixel.y & 0xFFFF));
#else
    return CachedPixels[PixelIndex];
#endif
}

float3 BlurPixels(float4 Pixels[SampleCount + 1], int SampleCenterIndex)
{
    // 中心像素颜色
    float4 PixelCenter = Pixels[SampleCenterIndex];

    float3 ColorSum = PixelCenter.rgb; // 所有像素颜色求和
    float WeightSum = 1; // 所有像素权重求和

    [unroll]
    for (int SampleIndex = 1; SampleIndex < SampleRadius; ++SampleIndex)
    {
        float4 PixelA = Pixels[SampleCenterIndex + SampleIndex];
        float4 PixelB = Pixels[SampleCenterIndex - SampleIndex];
        
        float WeightS = exp(-1 * pow(SampleIndex, 2) * FilterSigmaS);

        //ColorSum += PixelA.rgb * WeightS;
        //ColorSum += PixelB.rgb * WeightS;
        //WeightSum += WeightS;
        //WeightSum += WeightS;

        float WeightA = WeightS * exp(-1 * pow(PixelA.a - PixelCenter.a, 2) * FilterSigmaR);
        float WeightB = WeightS * exp(-1 * pow(PixelB.a - PixelCenter.a, 2) * FilterSigmaR);
        
        ColorSum += PixelA.rgb * WeightA;
        ColorSum += PixelB.rgb * WeightB;
        WeightSum += WeightA;
        WeightSum += WeightB;
    }
    return ColorSum / WeightSum;
}

void BlurHorizontally(uint2 OutputPixelCoord, uint2 FirstPixelCoord)
{
    float4 Pixels[SampleCount + 1];
    [unroll]
    for (int PixelIndex = 0; PixelIndex < SampleCount + 1; ++PixelIndex)
        Pixels[PixelIndex] = LoadPixel(FirstPixelCoord + int2(PixelIndex, 0));

    StorePixel(OutputPixelCoord, BlurPixels(Pixels, 4));
    StorePixel(OutputPixelCoord + int2(1, 0), BlurPixels(Pixels, 5));
}

void BlurVertically(uint2 OutputCoord, uint2 FirstPixelCoord)
{
    float4 Pixels[SampleCount + 1];
    [unroll]
    for (int PixelIndex = 0; PixelIndex < SampleCount; ++PixelIndex)
        Pixels[PixelIndex] = LoadPixel(FirstPixelCoord + int2(0, PixelIndex));
    Output[OutputCoord] = BlurPixels(Pixels, 4);
}

[numthreads(8, 8, 1)]
void DefaultComputeShader(uint3 GroupID : SV_GroupID, uint3 GroupThreadID : SV_GroupThreadID, uint3 DispatchThreadID : SV_DispatchThreadID)
{
    // Input -> LDS
    {
        int2 InputPixelCoord = (GroupID.xy << 3) - 4 + (GroupThreadID.xy << 1);
        uint2 PixelCoord = GroupThreadID.xy << 1;
        //StorePixel(PixelCoord + int2(0, 0), FetchPixel(InputPixelCoord + int2(0, 0)));
        //StorePixel(PixelCoord + int2(1, 0), FetchPixel(InputPixelCoord + int2(1, 0)));
        //StorePixel(PixelCoord + int2(0, 1), FetchPixel(InputPixelCoord + int2(0, 1)));
        //StorePixel(PixelCoord + int2(1, 1), FetchPixel(InputPixelCoord + int2(1, 1)));

        float2 Texcoord = InputTexelSize * (InputPixelCoord.xy + 0.5);
        StorePixel(PixelCoord + int2(0, 0), SamplePixel(Texcoord, int2(0, 0)));
        StorePixel(PixelCoord + int2(1, 0), SamplePixel(Texcoord, int2(1, 0)));
        StorePixel(PixelCoord + int2(0, 1), SamplePixel(Texcoord, int2(0, 1)));
        StorePixel(PixelCoord + int2(1, 1), SamplePixel(Texcoord, int2(1, 1)));
        GroupMemoryBarrierWithGroupSync();
    }

    // 16x16 -> 8x16
    // 一次 Blur，只会在一个维度上完成 16->8，所以，一次 BlurHorizontally 我们得计算两个像素
    {
        uint2 PixelCoord = uint2((GroupThreadID.x & 0x3) << 1, (GroupThreadID.y << 1) + (GroupThreadID.x >> 2));
        uint2 OutputPixelCoord = uint2((GroupThreadID.x & 0x3) << 1, (GroupThreadID.y << 1) + (GroupThreadID.x >> 2));
        BlurHorizontally(PixelCoord, PixelCoord);
        GroupMemoryBarrierWithGroupSync();
    }

    // 8x16 -> 8x8
    // 一次 BlurVertically 我们只需要计算一个像素
    BlurVertically(DispatchThreadID.xy, GroupThreadID.xy);
}
