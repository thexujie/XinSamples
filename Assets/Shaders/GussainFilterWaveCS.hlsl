#include "Common.hlsli"
#include "Utility.hlsli"

Texture2D<float3> Input;
RWTexture2D<float3> Output;

cbuffer ShaderParameter
{
    float2 InputTexelSize;
    float FilterSigmaS; // 1.0f / (2 * SigmaS * SigmaS)
}

float3 ReadPixel(int2 PixelCoord)
{
    return Input.SampleLevel(PointClampSampler, (PixelCoord + 0.5f) * InputTexelSize, 0);
}

static const int SampleRadius = 5;
static const int SampleCount = SampleRadius * 2 - 1;
static const int CacheDimensionX = 8;
static const int CacheDimensionY = 16;
groupshared float3 CachedPixels[CacheDimensionX * CacheDimensionY];

void StorePixel(uint2 PixelCoord, float3 Color)
{
    CachedPixels[PixelCoord.x + PixelCoord.y * CacheDimensionX] = Color;
    //CachedPixels[PixelCoord.x * CacheDimensionY + PixelCoord.y] = Color;
}

float3 LoadPixel(uint2 PixelCoord)
{
    return CachedPixels[PixelCoord.x + PixelCoord.y * CacheDimensionX];
    //return CachedPixels[PixelCoord.x * CacheDimensionY + PixelCoord.y];
}

float3 BlurPixels10(float3 Pixels[10], int SampleCenterIndex)
{
    // 中心像素颜色
    float3 PixelCenter = Pixels[SampleCenterIndex];

    float3 ColorSum = PixelCenter.rgb; // 所有像素颜色求和
    float WeightSum = 1; // 所有像素权重求和

    [unroll]
    for (int SampleIndex = 1; SampleIndex < SampleRadius; ++SampleIndex)
    {
        float3 PixelA = Pixels[SampleCenterIndex + SampleIndex];
        float3 PixelB = Pixels[SampleCenterIndex - SampleIndex];
        
        float WeightS = exp(-1 * pow(SampleIndex, 2) * FilterSigmaS);

        ColorSum += PixelA.rgb * WeightS;
        ColorSum += PixelB.rgb * WeightS;
        WeightSum += WeightS;
        WeightSum += WeightS;
    }
    return ColorSum / WeightSum;
}

float3 BlurPixels12(float3 Pixels[12], int SampleCenterIndex)
{
    // 中心像素颜色
    float3 PixelCenter = Pixels[SampleCenterIndex];

    float3 ColorSum = PixelCenter.rgb; // 所有像素颜色求和
    float WeightSum = 1; // 所有像素权重求和

    [unroll]
    for (int SampleIndex = 1; SampleIndex < SampleRadius; ++SampleIndex)
    {
        float3 PixelA = Pixels[SampleCenterIndex + SampleIndex];
        float3 PixelB = Pixels[SampleCenterIndex - SampleIndex];
        
        float WeightS = exp(-1 * pow(SampleIndex, 2) * FilterSigmaS);

        ColorSum += PixelA.rgb * WeightS;
        ColorSum += PixelB.rgb * WeightS;
        WeightSum += WeightS;
        WeightSum += WeightS;
    }
    return ColorSum / WeightSum;
}

void BlurHorizontally(uint2 OutputPixelCoord, uint2 FirstPixelCoord)
{
    // 每次取 10 个像素，[0,8] 计算第一个像素，[1,9]计算第二个像素
    float3 Pixels[SampleCount + 1];
    for (int PixelIndex = 0; PixelIndex < SampleCount + 1; ++PixelIndex)
    {
        Pixels[PixelIndex] = LoadPixel(FirstPixelCoord + int2(PixelIndex, 0));
    }

    StorePixel(OutputPixelCoord, BlurPixels10(Pixels, 4));
    StorePixel(OutputPixelCoord + int2(1, 0), BlurPixels10(Pixels, 5));
}

void BlurVertically(uint2 OutputCoord, uint2 FirstPixelCoord)
{
    float3 Pixels[10];
    [unroll]
    for (int PixelIndex = 0; PixelIndex < SampleCount; ++PixelIndex)
        Pixels[PixelIndex] = LoadPixel(FirstPixelCoord + int2(0, PixelIndex));
    Output[OutputCoord] = BlurPixels10(Pixels, 4);
}

[numthreads(8, 8, 1)]
void DefaultComputeShader(uint3 GroupID : SV_GroupID, uint3 GroupThreadID : SV_GroupThreadID, uint3 DispatchThreadID : SV_DispatchThreadID)
{
    uint LaneIndex = WaveGetLaneIndex();
    // 16x16 -> 8x16
    // 一次 Blur，只会在一个维度上完成 16->8，所以，一次 BlurHorizontally 我们得计算两个像素
    {
        int2 BasePixelCoord = (GroupID.xy << 3) - 4;
        uint2 OutputPixelCoord = uint2((GroupThreadID.x & 0x3) << 1, (GroupThreadID.y << 1) + (GroupThreadID.x >> 2));

        float3 PixelA = ReadPixel(int2(BasePixelCoord.x + (OutputPixelCoord.x << 1) + 0, BasePixelCoord.y + OutputPixelCoord.y));
        float3 PixelB = ReadPixel(int2(BasePixelCoord.x + (OutputPixelCoord.x << 1) + 1, BasePixelCoord.y + OutputPixelCoord.y));
        float3 PixelC = ReadPixel(int2(BasePixelCoord.x + (OutputPixelCoord.x << 1) + 2, BasePixelCoord.y + OutputPixelCoord.y));
        float3 PixelD = ReadPixel(int2(BasePixelCoord.x + (OutputPixelCoord.x << 1) + 3, BasePixelCoord.y + OutputPixelCoord.y));

        uint LaneBaseIndex = (LaneIndex & ~0x3) + (OutputPixelCoord.x >> 2);
        float3 Pixels[12];

        Pixels[+0] = WaveReadLaneAt(PixelA, LaneBaseIndex + 0);
        Pixels[+1] = WaveReadLaneAt(PixelB, LaneBaseIndex + 0);
        Pixels[+2] = WaveReadLaneAt(PixelC, LaneBaseIndex + 0);
        Pixels[+3] = WaveReadLaneAt(PixelD, LaneBaseIndex + 0);
        Pixels[+4] = WaveReadLaneAt(PixelA, LaneBaseIndex + 1);
        Pixels[+5] = WaveReadLaneAt(PixelB, LaneBaseIndex + 1);
        Pixels[+6] = WaveReadLaneAt(PixelC, LaneBaseIndex + 1);
        Pixels[+7] = WaveReadLaneAt(PixelD, LaneBaseIndex + 1);
        Pixels[+8] = WaveReadLaneAt(PixelA, LaneBaseIndex + 2);
        Pixels[+9] = WaveReadLaneAt(PixelB, LaneBaseIndex + 2);
        Pixels[10] = WaveReadLaneAt(PixelC, LaneBaseIndex + 2);
        Pixels[11] = WaveReadLaneAt(PixelD, LaneBaseIndex + 2);

        StorePixel(OutputPixelCoord, BlurPixels12(Pixels, (OutputPixelCoord.x & 0x3) + 4));
        StorePixel(OutputPixelCoord + int2(1, 0), BlurPixels12(Pixels, (OutputPixelCoord.x & 0x3) + 5));
    }
    GroupMemoryBarrierWithGroupSync();

    // 8x16 -> 8x8
    // 一次 BlurVertically 我们只需要计算一个像素
    //{
    //    float3 PixelA = LoadPixel(int2(GroupThreadID.y, (GroupThreadID.x << 1) + 0));
    //    float3 PixelB = LoadPixel(int2(GroupThreadID.y, (GroupThreadID.x << 1) + 1));

    //    uint LaneBaseIndex = LaneIndex & ~0x7;
    //    float3 Pixels[10];

    //    Pixels[+0] = WaveReadLaneAt(PixelA, LaneBaseIndex + (GroupThreadID.x >> 1) + 0);
    //    Pixels[+1] = WaveReadLaneAt(PixelB, LaneBaseIndex + (GroupThreadID.x >> 1) + 0);
    //    Pixels[+2] = WaveReadLaneAt(PixelA, LaneBaseIndex + (GroupThreadID.x >> 1) + 1);
    //    Pixels[+3] = WaveReadLaneAt(PixelB, LaneBaseIndex + (GroupThreadID.x >> 1) + 1);
    //    Pixels[+4] = WaveReadLaneAt(PixelA, LaneBaseIndex + (GroupThreadID.x >> 1) + 2);
    //    Pixels[+5] = WaveReadLaneAt(PixelB, LaneBaseIndex + (GroupThreadID.x >> 1) + 2);
    //    Pixels[+6] = WaveReadLaneAt(PixelA, LaneBaseIndex + (GroupThreadID.x >> 1) + 3);
    //    Pixels[+7] = WaveReadLaneAt(PixelB, LaneBaseIndex + (GroupThreadID.x >> 1) + 3);
    //    Pixels[+8] = WaveReadLaneAt(PixelA, LaneBaseIndex + (GroupThreadID.x >> 1) + 4);
    //    Pixels[+9] = WaveReadLaneAt(PixelA, LaneBaseIndex + (GroupThreadID.x >> 1) + 4);
        
    //    int2 BasePixelCoord = GroupID.xy << 3;
    //    Output[BasePixelCoord + GroupThreadID.yx] = BlurPixels10(Pixels, (GroupThreadID.x & 0x1) + 4);
    //}

    // 8x16 -> 8x8
    // 一次 BlurVertically 我们只需要计算一个像素
    BlurVertically(DispatchThreadID.xy, GroupThreadID.xy);

    //Output[DispatchThreadID.xy] = WaveGetLaneIndex() / 32.0f;
}
