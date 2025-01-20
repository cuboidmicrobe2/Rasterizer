#include "WindowSetup.h"

#include <iostream>

LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {

	// Finds what code to run for the given message
	switch (message) {

		// This message is read when the window is closed
	case WM_DESTROY: {

		// Closes down the application entirely
		PostQuitMessage(0);
		return 0;
	}
	default:
		break;
	}

	// Default window handling
	return DefWindowProc(hWnd, message, wParam, lParam);
}

bool SetupWindow(HINSTANCE instance, UINT width, UINT height, int nCmdShow, HWND& window)
{
	const wchar_t CLASS_NAME[] = L"WINDOW CLASS";

	WNDCLASS wc = {};

	wc.lpfnWndProc = WindowProc;
	wc.hInstance = instance;
	wc.lpszClassName = CLASS_NAME;

	RegisterClass(&wc);

	window = CreateWindowEx(0, CLASS_NAME, L"SPEEEEEEEEEEEEEEEEEEN", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, 0, width, height, nullptr, nullptr, instance, nullptr);

	if (window == nullptr) {
		std::cerr << "HWND was nullptr, last error: " << GetLastError() << std::endl;
		return false;
	}

	ShowWindow(window, nCmdShow);

	return true;
}