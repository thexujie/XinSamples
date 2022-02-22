#pragma once

#include "00.FbxViewer.Types.h"
#include "Xin.Core/Platform/Windows/WindowsPrivate.h"

const uint32 NumBackBuffers = 3;

const FWStringV WindowTitle = L"Fbx Viewer"V;

const wchar_t WindowClassName[] = L"E9B39421-7D98-419E-BABA-2A27896B7DCD";
const wchar_t GuidWindowPointer[] = L"3846C5AA-E4A6-42C3-B7E7-ED869E63FE8F";

FString GetProjectAssetsPath();

HWND FbxViewer_CreateWindow();
