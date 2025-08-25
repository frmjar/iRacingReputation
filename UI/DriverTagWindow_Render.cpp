#include "../../Overlay/OverlayProximityTags.h"
#include "DriverTagWindow.h"

void DriverTagWindow::Render()
{
    if (!m_initialized || !m_visible || !m_imguiContext)
        return;
    ImGui::SetCurrentContext(m_imguiContext);
    const float clear_color[4] = {0.45f, 0.55f, 0.60f, 1.00f};
    m_context->OMSetRenderTargets(1, &m_renderTargetView, nullptr);
    m_context->ClearRenderTargetView(m_renderTargetView, clear_color);
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);
    if (ImGui::Begin("Driver Tagging", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse))
    {
        float fullHeight = ImGui::GetContentRegionAvail().y;
        if (m_sideMenu)
        {
            m_sideMenu->Render(fullHeight, CountDriversWithFlags());
            m_currentView = m_sideMenu->GetCurrentView();
        }
        ImGui::SameLine();
        ImGui::BeginChild("MainContent", ImVec2(0, fullHeight), false, ImGuiWindowFlags_None);
        switch (m_currentView)
        {
        case AppView::DRIVERS_WITH_FLAGS:
            RenderDriversWithFlagsView();
            break;
        case AppView::CURRENT_SESSION:
            RenderCurrentSessionView();
            break;
        default:
            ImGui::Text("Vista desconocida");
            break;
        }
        ImGui::EndChild();
    }
    ImGui::End();
    // Renderizar overlay de proximidad aquí
    extern OverlayProximityTagsManager overlayManager;
    overlayManager.Render();
    ImGui::Render();
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
    m_swapChain->Present(1, 0);
}

void DriverTagWindow::Show()
{
    if (m_hwnd)
    {
        ShowWindow(m_hwnd, SW_SHOW);
        SetForegroundWindow(m_hwnd);
        m_visible = true;
    }
}

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
    const D3D_FEATURE_LEVEL featureLevelArray[2] = {D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0};
    D3D_FEATURE_LEVEL featureLevel;
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
