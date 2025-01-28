#include "WindowHelper.h"

#include <iostream>

// Window procedure to handle messages sent to the window
LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
	switch (message) {

		// Handle window destruction
	case WM_DESTROY: {
		PostQuitMessage(0);
		return 0;
	}
	default:
		break;
	}

	// Default window handling
	return DefWindowProc(hWnd, message, wParam, lParam);
}

// Function to set up and create a window
bool SetupWindow(HINSTANCE instance, UINT width, UINT height, int nCmdShow, HWND& window)
{
	const wchar_t CLASS_NAME[] = L"WINDOW CLASS";

	// Define window class properties
	WNDCLASS wc = {
		.lpfnWndProc = WindowProc,
		.hInstance = instance,
		.lpszClassName = CLASS_NAME
	};

	// Register the window class
	if (!RegisterClass(&wc)) {
		std::wcerr << L"Failed to register window class, last error: " << GetLastError() << std::endl;
		return false;
	}

	// Create the window
	window = CreateWindowEx(0, CLASS_NAME, L"Rasterizer Window", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, 0, width, height, nullptr, nullptr, instance, nullptr);

	// Check if window creation was successful
	if (window == nullptr) {
		std::wcerr << L"Failed to create window, last error: " << GetLastError() << std::endl;
		return false;
	}

	// Show the window
	ShowWindow(window, nCmdShow);
	return true;
}