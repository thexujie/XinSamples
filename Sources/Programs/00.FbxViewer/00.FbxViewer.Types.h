#pragma once

#include "Xin.Core/Xin.Core.h"
#include "Xin.RHI/Xin.RHI.h"
#include "Xin.Rendering/Xin.Rendering.h"
#include "Xin.ShaderCompiler/Xin.ShaderCompiler.h"

using namespace Xin;
using namespace Xin::RHI;
using namespace Xin::Rendering;

#if XIN_WINDOWS
#ifdef _RHITest_Module_
#define RHITEST_API __declspec(dllexport)
#else
#define RHITEST_API __declspec(dllimport)
#endif
#endif
