#include "PCH.h"
#include "X.TaskMonitor.h"
#include "TaskMonitor.h"
#include "Xin.Core/Platform/Windows/WindowsPrivate.h"
#include "Xin.Direct2D/Xin.Direct2D.h"
#include "Xin.UI/Platform/Windows/Xin.UI.Windows.h"

#define TASK_MONITOR_RHI_PAINTER 0

using namespace Xin;

int main(int argc, const char * args[])
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	_CrtSetReportMode(_CRT_ASSERT, _CRTDBG_MODE_FILE | _CRTDBG_MODE_WNDW);
	_CrtSetReportFile(_CRT_ASSERT, _CRTDBG_FILE_STDERR);
	//_CrtSetBreakAlloc(15497);

	new char[9] { 't', 'e', 's', 't', ' ', 'l', 'e', 'a', 'k' };

	Windows::FCOMInitializer COMInitializer;

	Xin::FGlobalLogger GlobalLogger { u8"X.TaskManager.log"V, Xin::LogLevel_Dbg };
	//Xin::FFileStream LoggerFileSteam;
	//LoggerFileSteam.Open(u8"MT.log"V, Xin::EFileModes::Append);

	IGraphicsRef Graphics = D2D::D2DCreateGraphics();
	//Graphics = Skia::SkiaCreateGraphics();

#if TASK_MONITOR_RHI_PAINTER
	if (!Graphics)
	{
		//RHIDevice = RHI::D3D12::CreateRHIDevice();
		RHIDevice = RHI::Vulkan::CreateDevice();
		RHIDevice->SetDebugName(u8"MainDevice"V);

		Engine = new Rendering::IBaseEngine(*RHIDevice);
		Executor = new Rendering::FCommandExecutor(*Engine);

		static IGraphicsRef OfflineGraphics = Skia::SkiaCreateGraphics();
		//IGraphicsRef OfflineGraphics = D2D::D2DCreateGraphics();
		TReferPtr<Rendering::FRHIGraphics> RHIGraphics = new Rendering::FRHIGraphics(*Executor);
		RHIGraphics->Initialize(OfflineGraphics);

		Graphics = RHIGraphics;
	}
#endif

	IDesktop::Default().SetDefaultGraphics(Graphics);
	Xin::TaskMonitor::FTaskMonitor TaskMonitor;
	int32 ReturnValue = TaskMonitor.Run();
	IDesktop::Default().SetDefaultGraphics(nullptr);
	return ReturnValue;
}
