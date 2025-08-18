#pragma once
#include "ViewDefs.h"
#include <functional>
#include <string>
#include <vector>
#include "../../Utils/Common/UIColors.h"
#include "../../External/ImGui/imgui.h"

class SideMenu
{
public:
    using OnChangeCallback = std::function<void(AppView)>;
    explicit SideMenu(OnChangeCallback cb = nullptr) : m_onChange(cb) {}
    void Render(float fullHeight, int flaggedCount);
    AppView GetCurrentView() const { return m_currentView; }
    void SetCurrentView(AppView v) { m_currentView = v; }
    void SetOnChange(OnChangeCallback cb) { m_onChange = cb; }
    float GetWidth() const { return 160.0f; } // Reducido de 210 a 160px

private:
    AppView m_currentView = AppView::DRIVERS_WITH_FLAGS;
    OnChangeCallback m_onChange;
    void DrawMenuButton(const char *label, AppView view, bool showCounter = false, int counter = 0);
};
