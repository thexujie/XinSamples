#pragma once

#include "00.Test.RHI.Types.h"

const uint32 NumBackBuffers = 3;

const FWStringV WindowTitle = L"VulkanSampe"V;

const wchar_t WindowClassName[] = L"E9B39421-7D98-419E-BABA-2A27896B7DCD";
const wchar_t GuidWindowPointer[] = L"3846C5AA-E4A6-42C3-B7E7-ED869E63FE8F";

void TestRHI_Initialize();
HWND TestRHI_CreateWindow();

int TestRHI_Empty(Xin::RHI::IRHIDeviceRef Device);
int TestRHI_ColorPS(IRHIDeviceRef Device);
int TestRHI_Texture(Xin::RHI::IRHIDeviceRef Device);
int TestRHI_Mesh(IRHIDeviceRef Device);

FString GetProjectAssetsPath();
