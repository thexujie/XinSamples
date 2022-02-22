#include "pch.h"
#include "00.Test.RHI.h"

#pragma comment(lib, "imm32.lib")

LRESULT CALLBACK MainWndProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
	if (Message == WM_CLOSE)
		PostQuitMessage(0);

	return DefWindowProcW(hWnd, Message, wParam, lParam);
}

void TestRHI_Initialize()
{
	FApplication::Instance().LoadModule(u8"Xin.ImageCodec.FreeImage.dll"V);
}

HWND TestRHI_CreateWindow()
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
