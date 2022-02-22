#include "pch.h"
#include "Xin.Direct2D/Xin.Direct2D.h"

using namespace Xin;

int main(int argc, const char * args[])
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	_CrtSetReportMode(_CRT_ASSERT, _CRTDBG_MODE_FILE | _CRTDBG_MODE_WNDW);
	_CrtSetReportFile(_CRT_ASSERT, _CRTDBG_FILE_STDERR);

	new char[9] { 't', 'e', 's', 't', ' ', 'l', 'e', 'a', 'k' };

	IGraphicsRef Graphics = D2D::D2DCreateGraphics();
	//Graphics = Skia::SkiaCreateGraphics();

	//if (!Graphics)
	//{
	//	RHIDevice = RHI::D3D12::CreateRHIDevice();
	//	RHIContext = RHIDevice->CreateContext(0);
	//	RHIContext->SetRHILabel(u8"MainContext"V);

	//	Interface = new Rendering::IRendererInterface(*RHIContext);
	//	Executor = new Rendering::FCommandExecutor(*Interface);

	//	//static IGraphicsRef OfflineGraphics = Skia::SkiaCreateGraphics();
	//	IGraphicsRef OfflineGraphics = D2D::D2DCreateGraphics();
	//	TReferPtr<Rendering::FRHIGraphics> RHIGraphics = new Rendering::FRHIGraphics(*Executor);
	//	RHIGraphics->Initialize(OfflineGraphics);

	//	Graphics = RHIGraphics;
	//}
	return 0;
}
