#include "Common.hlsli"
#include "Utility.hlsli"

Texture2D<float3> Input;
RWTexture2D<float3> Output;

cbuffer ShaderParameter
{
    float2 InputTexelSize;
    float FilterSigmaS; // 1.0f / (2 * SigmaS * SigmaS)
    //uint SampleRadius; // 采样的像素数量
}

float3 ReadPixel(int2 PixelCoord)
{
    return Input.SampleLevel(PointClampSampler, (PixelCoord + 0.5f) * InputTexelSize, 0);
}

static const int SampleRadius = 5;
static const int SampleCount = SampleRadius * 2 - 1;
static const int CacheDimension = 16;
groupshared float3 CachedPixels[CacheDimension][CacheDimension];

void StorePixel(uint2 PixelCoord, float3 Color)
{
    CachedPixels[PixelCoord.y][PixelCoord.x] = Color;
    //CachedPixels[PixelCoord.x][PixelCoord.y] = Color;
}

float3 LoadPixel(uint2 PixelCoord)
{
    return CachedPixels[PixelCoord.y][PixelCoord.x];
    //return CachedPixels[PixelCoord.x][PixelCoord.y];
}

float3 BlurPixels(float3 Pixels[SampleCount + 1], int SampleCenterIndex)
{
    // 中心像素颜色
    float3 CenterColor = Pixels[SampleCenterIndex];
    float CenterWeight = 1; // 当像素距离差为 0 时（像素本身），权重为 1。

    float3 ColorSum = CenterColor; // 所有像素颜色求和
    float WeightSum = CenterWeight; // 所有像素权重求和

    for (int SampleIndex = 1; SampleIndex < SampleRadius; ++SampleIndex)
    {
	    {
            float3 Color = Pixels[SampleCenterIndex + SampleIndex];
            float WeightSpatial = exp(-1 * pow(SampleIndex, 2) * FilterSigmaS);
            
            ColorSum += Color * WeightSpatial;
            WeightSum += WeightSpatial;
        }
	    {
            float3 Color = Pixels[SampleCenterIndex - SampleIndex];
            float WeightSpatial = exp(-1 * pow(SampleIndex, 2) * FilterSigmaS);
            
            ColorSum += Color * WeightSpatial;
            WeightSum += WeightSpatial;
        }
    }
    return ColorSum / WeightSum;
}

void BlurHorizontally(uint2 OutputPixelCoord, uint2 FirstPixelCoord)
{
    // 每次取 10 个像素，[0,8] 计算第一个像素，[1,9]计算第二个像素
    float3 Pixels[SampleCount + 1];
    for (int PixelIndex = 0; PixelIndex < SampleCount + 1; ++PixelIndex)
        Pixels[PixelIndex] = LoadPixel(FirstPixelCoord + int2(PixelIndex, 0));

    StorePixel(OutputPixelCoord, BlurPixels(Pixels, 4));
    StorePixel(OutputPixelCoord + int2(1, 0), BlurPixels(Pixels, 5));
}

void BlurVertically(uint2 OutputCoord, uint2 FirstPixelCoord)
{
    float3 Pixels[SampleCount + 1];
    for (int PixelIndex = 0; PixelIndex < SampleCount; ++PixelIndex)
        Pixels[PixelIndex] = LoadPixel(FirstPixelCoord + int2(0, PixelIndex));
    Output[OutputCoord] = BlurPixels(Pixels, 4);
}

[numthreads(8, 8, 1)]
void DefaultComputeShader(uint3 GroupID : SV_GroupID, uint3 GroupThreadID : SV_GroupThreadID, uint3 DispatchThreadID : SV_DispatchThreadID)
{
    // Input -> LDS
    {
        int2 InputPixelCoord = (GroupID.xy << 3) + (GroupThreadID.xy << 1) - 4;
        uint2 PixelCoord = GroupThreadID.xy << 1;
        StorePixel(PixelCoord + int2(0, 0), ReadPixel(InputPixelCoord + int2(0, 0)));
        StorePixel(PixelCoord + int2(1, 0), ReadPixel(InputPixelCoord + int2(1, 0)));
        StorePixel(PixelCoord + int2(0, 1), ReadPixel(InputPixelCoord + int2(0, 1)));
        StorePixel(PixelCoord + int2(1, 1), ReadPixel(InputPixelCoord + int2(1, 1)));
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
