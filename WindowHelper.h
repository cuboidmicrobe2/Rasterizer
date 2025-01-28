#pragma once

#include <Windows.h>

/// <summary>
/// Sets up a window with the specified parameters.
/// </summary>
/// <param name="instance">- Handle to the instance.</param>
/// <param name="width">- Width of the window.</param>
/// <param name="height">- Height of the window.</param>
/// <param name="nCmdShow">- Specifies how the window is to be shown.</param>
/// <param name="window">- Reference to the window handle.</param>
/// <returns>True if the window was successfully created, otherwise false.</returns>
bool SetupWindow(HINSTANCE instance, UINT width, UINT height, int nCmdShow, HWND& window);