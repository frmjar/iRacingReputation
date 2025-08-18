/*
MIT License - iRacing Reputation System
Implementación de la ventana independiente para etiquetar pilotos - Versión modular
*/

#include "DriverTagWindow.h"
#include <tchar.h>
#include <cmath>
#include <algorithm>
#include <vector>
void DriverTagWindow::DrawDriverField(const char *label, const std::string &value, const ImVec4 &color, float labelWidth)
{
    ImGui::PushStyleColor(ImGuiCol_Text, UIColors::Theme::TEXT_LABEL);
    ImGui::Text("%s:", label);
    ImGui::PopStyleColor();
    ImGui::SameLine(labelWidth);
    ImGui::PushStyleColor(ImGuiCol_Text, color);
    ImGui::Text("%s", value.c_str());
    ImGui::PopStyleColor();
}

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

bool DriverTagWindow::CreateDeviceD3D()
{
    DXGI_SWAP_CHAIN_DESC sd = {};
    sd.BufferCount = 2;
    sd.BufferDesc.Width = 0;
    sd.BufferDesc.Height = 0;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferDesc.RefreshRate.Numerator = 60;
    sd.BufferDesc.RefreshRate.Denominator = 1;
    sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow = m_hwnd;
    sd.SampleDesc.Count = 1;
    sd.SampleDesc.Quality = 0;
    sd.Windowed = TRUE;
    sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

    UINT createDeviceFlags = 0;
    D3D_FEATURE_LEVEL featureLevel;
    const D3D_FEATURE_LEVEL featureLevelArray[2] = {D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0};

    HRESULT res = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, createDeviceFlags,
                                                featureLevelArray, 2, D3D11_SDK_VERSION, &sd, &m_swapChain,
                                                &m_device, &featureLevel, &m_context);
    if (res == DXGI_ERROR_UNSUPPORTED)
    {
        res = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_WARP, nullptr, createDeviceFlags,
                                            featureLevelArray, 2, D3D11_SDK_VERSION, &sd, &m_swapChain,
                                            &m_device, &featureLevel, &m_context);
    }
    if (res != S_OK)
        return false;
    CreateRenderTarget();
    return true;
}

void DriverTagWindow::CleanupDeviceD3D()
{
    CleanupRenderTarget();
    if (m_swapChain)
    {
        m_swapChain->Release();
        m_swapChain = nullptr;
    }
    if (m_context)
    {
        m_context->Release();
        m_context = nullptr;
    }
    if (m_device)
    {
        m_device->Release();
        m_device = nullptr;
    }
}

void DriverTagWindow::CreateRenderTarget()
{
    ID3D11Texture2D *pBackBuffer;
    m_swapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
    m_device->CreateRenderTargetView(pBackBuffer, nullptr, &m_renderTargetView);
    pBackBuffer->Release();
}

void DriverTagWindow::CleanupRenderTarget()
{
    if (m_renderTargetView)
    {
        m_renderTargetView->Release();
        m_renderTargetView = nullptr;
    }
}

void DriverTagWindow::UpdateTrustLevel(DriverReputation &reputation) const
{
    // Simplificado: asignar trust level basado en comportamientos seleccionados
    int positiveCount = 0;
    int negativeCount = 0;

    if (reputation.HasBehavior(DriverFlags::CLEAN_DRIVER))
        positiveCount += 3;
    if (reputation.HasBehavior(DriverFlags::GOOD_RACER))
        positiveCount += 4;
    if (reputation.HasBehavior(DriverFlags::AGGRESSIVE))
        positiveCount += 1;
    if (reputation.HasBehavior(DriverFlags::DIRTY_DRIVER))
        negativeCount += 3;
    if (reputation.HasBehavior(DriverFlags::RAMMER))
        negativeCount += 5;
    if (reputation.HasBehavior(DriverFlags::BLOCKING))
        negativeCount += 2;
    if (reputation.HasBehavior(DriverFlags::UNSAFE_REJOIN))
        negativeCount += 2;

    int netScore = positiveCount - negativeCount;
    if (netScore >= 4)
        reputation.trustLevel = DriverTrustLevel::TRUSTED;
    else if (netScore >= 1)
        reputation.trustLevel = DriverTrustLevel::NEUTRAL; // ligera mejora positiva
    else if (netScore <= -3)
        reputation.trustLevel = DriverTrustLevel::AVOID;
    else if (netScore <= -1)
        reputation.trustLevel = DriverTrustLevel::CAUTION;
    else
        reputation.trustLevel = DriverTrustLevel::NEUTRAL;
}

bool DriverTagWindow::LoadIcons()
{
    if (!m_iconManager)
    {
        Logger::Error("IconManager not initialized");
        return false;
    }

    // Cargar iconos para cada tag
    for (auto &tag : m_availableTags)
    {
        if (tag.iconPath && strlen(tag.iconPath) > 0)
        {
            // Usamos el nombre lógico del tag como key
            if (m_iconManager->LoadIcon(tag.name, tag.iconPath))
            {
                if (auto *texInfo = m_iconManager->GetIcon(tag.name))
                    tag.iconTexture = texInfo->texture;
            }
            if (!tag.iconTexture)
            {
                Logger::Warning(std::string("Failed to load icon: ") + tag.iconPath);
                if (!CreateFallbackIcon())
                {
                    Logger::Error("Failed to create fallback icon");
                    return false;
                }
                tag.iconTexture = m_fallbackIcon;
            }
        }
        else
        {
            if (!CreateFallbackIcon())
            {
                Logger::Error("Failed to create fallback icon");
                return false;
            }
            tag.iconTexture = m_fallbackIcon;
        }
    }

    return true;
}

bool DriverTagWindow::IsPlaceholderDriver(const DriverData &d) const
{
    return d.displayName.find("Piloto #") == 0 || d.displayName.empty() || !d.isValid;
}

std::string DriverTagWindow::TruncateText(const std::string &text, float maxWidth) const
{
    if (text.empty())
        return text;

    ImVec2 textSize = ImGui::CalcTextSize(text.c_str());
    if (textSize.x <= maxWidth)
        return text;

    std::string truncated = text;
    while (!truncated.empty() && ImGui::CalcTextSize((truncated + "...").c_str()).x > maxWidth)
    {
        truncated.pop_back();
    }

    return truncated.empty() ? "..." : truncated + "...";
}

void DriverTagWindow::DrawDriverField(const char *label, int value, const ImVec4 &color, float labelWidth)
{
    DrawDriverField(label, std::to_string(value), color, labelWidth);
}

void DriverTagWindow::DrawDriverField(const char *label, float value, const ImVec4 &color, const char *fmt, float labelWidth)
{
    char buffer[64];
    snprintf(buffer, sizeof(buffer), fmt, value);
    DrawDriverField(label, std::string(buffer), color, labelWidth);
}

ImVec4 DriverTagWindow::GetIRatingColor(int iRating) const
{
    return UIColors::GetIRatingColor(iRating);
}

ImVec4 DriverTagWindow::GetSafetyColor(float sr) const
{
    return UIColors::GetSafetyRatingColor(sr);
}

bool DriverTagWindow::CreateFallbackIcon()
{
    if (m_fallbackIcon)
        return true;
    // Crear una textura 32x32 gris con un "?" simple dibujable (usaremos color sólido, el texto se pinta encima si quisiéramos)
    const int W = 32, H = 32;
    std::vector<unsigned int> pixels(W * H, 0xFF555555); // ARGB (DX11 usará RGBA reinterpretar)
    // Dibujar borde claro
    for (int x = 0; x < W; ++x)
    {
        pixels[x] = 0xFFAAAAAA;
        pixels[x + (H - 1) * W] = 0xFFAAAAAA;
    }
    for (int y = 0; y < H; ++y)
    {
        pixels[y * W] = 0xFFAAAAAA;
        pixels[(y * W) + (W - 1)] = 0xFFAAAAAA;
    }
    // Crear textura
    D3D11_TEXTURE2D_DESC desc = {};
    desc.Width = W;
    desc.Height = H;
    desc.MipLevels = 1;
    desc.ArraySize = 1;
    desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    desc.SampleDesc.Count = 1;
    desc.Usage = D3D11_USAGE_DEFAULT;
    desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    D3D11_SUBRESOURCE_DATA init = {};
    init.pSysMem = pixels.data();
    init.SysMemPitch = W * 4;
    ID3D11Texture2D *tex = nullptr;
    if (FAILED(m_device->CreateTexture2D(&desc, &init, &tex)))
        return false;
    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
    srvDesc.Format = desc.Format;
    srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MipLevels = 1;
    if (FAILED(m_device->CreateShaderResourceView(tex, &srvDesc, &m_fallbackIcon)))
    {
        tex->Release();
        return false;
    }
    tex->Release();
    return true;
}

// EOF
