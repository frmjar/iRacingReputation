/*
MIT License - iRacing Reputation System
Implementación de la ventana independiente para etiquetar pilotos - Versión modular
*/

#include "DriverTagWindow.h"
#include <tchar.h>
#include <cmath>
#include <algorithm>
#include <vector>
// Implementación modular: ver DriverTagWindow_Init.cpp, DriverTagWindow_Render.cpp, DriverTagWindow_Persistence.cpp, DriverTagWindow_Helpers.cpp, DriverTagWindow_Views.cpp

// Forward declare message handler from imgui_impl_win32.cpp
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT CALLBACK DriverTagWindow::StaticWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    DriverTagWindow *window = nullptr;

    if (msg == WM_NCCREATE)
    {
        CREATESTRUCT *cs = reinterpret_cast<CREATESTRUCT *>(lParam);
        window = reinterpret_cast<DriverTagWindow *>(cs->lpCreateParams);
        SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(window));
    }
    else
    {
        window = reinterpret_cast<DriverTagWindow *>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
    }

    if (window)
    {
        return window->WndProc(hwnd, msg, wParam, lParam);
    }

    return DefWindowProc(hwnd, msg, wParam, lParam);
}

LRESULT DriverTagWindow::WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (m_imguiContext)
        ImGui::SetCurrentContext(m_imguiContext);

    if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wParam, lParam))
        return true;

    switch (msg)
    {
    case WM_SIZE:
        if (m_device != nullptr && wParam != SIZE_MINIMIZED)
        {
            CleanupRenderTarget();
            m_swapChain->ResizeBuffers(0, (UINT)LOWORD(lParam), (UINT)HIWORD(lParam), DXGI_FORMAT_UNKNOWN, 0);
            CreateRenderTarget();
        }
        return 0;
    case WM_SYSCOMMAND:
        if ((wParam & 0xfff0) == SC_KEYMENU)
            return 0;
        break;
    case WM_CLOSE:
        m_shouldClose = true;
        m_visible = false;
        return 0;
    case WM_DESTROY:
        m_shouldClose = true;
        m_visible = false;
        return 0;
    }
    return DefWindowProc(hwnd, msg, wParam, lParam);
}

// EOF
