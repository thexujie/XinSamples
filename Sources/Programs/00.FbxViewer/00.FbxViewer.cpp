#include "pch.h"
#include "00.FbxViewer.h"

#pragma comment(lib, "imm32.lib")

LRESULT CALLBACK MainWndProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
	if (Message == WM_CLOSE)
		PostQuitMessage(0);

	return DefWindowProcW(hWnd, Message, wParam, lParam);
}

HWND FbxViewer_CreateWindow()
{
	new char[9] { 't', 'e', 's', 't', ' ', 'l', 'e', 'a', 'k' };

	HINSTANCE hInstance = (HINSTANCE)FSystem::Instance();
	WNDCLASSEXW wcex = { sizeof(WNDCLASSEXW) };
	wcex.cbSize = sizeof(WNDCLASSEXW);
	wcex.style = 0;
	wcex.lpfnWndProc = MainWndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = NULL;
	wcex.hCursor = ::LoadCursor(NULL, IDC_ARROW);
	//wcex.hbrBackground = (HBRUSH)(COLOR_BACKGROUND + 1);
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = WindowClassName;
	wcex.hIconSm = NULL;

	RegisterClassExW(&wcex);

	RECT WindowRect = { 0, 0, (LONG)1920, (LONG)1080 };
	AdjustWindowRect(&WindowRect, WS_OVERLAPPEDWINDOW, FALSE);

	HWND WindowHandle = CreateWindowExW(
		0, WindowClassName, WindowTitle.Data, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, WindowRect.right - WindowRect.left, WindowRect.bottom - WindowRect.top,
		NULL, NULL, hInstance, NULL);
	SetPropW(HWND(WindowHandle), GuidWindowPointer, (HANDLE)(void *)nullptr);

	::ImmAssociateContext(HWND(WindowHandle), NULL);
	::ShowWindow(HWND(WindowHandle), SW_NORMAL);
	::UpdateWindow(HWND(WindowHandle));

	return WindowHandle;
}

int FbxViewer_Main(FEngine & Engine);

#include "Xin.RHI.D3D12/Xin.RHI.D3D12.h"

int main(int argc, const char * args[])
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	//_CrtSetBreakAlloc(11113);

	Xin::RHI::IRHIDeviceRef Device = Xin::RHI::D3D12::CreateDevice();

	FEngine Engine { *Device };
	FbxViewer_Main(Engine);
	//TestRHI_Mesh(Device);

	return 0;
}
