#include "SideMenu.h"
#include "../../External/ImGui/imgui.h"
#include "../../Utils/Common/UIColors.h"

void SideMenu::Render(float fullHeight, int flaggedCount)
{
    ImGui::BeginChild("SideMenu", ImVec2(GetWidth(), fullHeight), true, ImGuiWindowFlags_NoScrollbar);
    ImGui::Text("Navegacion");
    ImGui::Separator();
    DrawMenuButton("Pilotos registrados", AppView::DRIVERS_WITH_FLAGS, flaggedCount > 0, flaggedCount);
    DrawMenuButton("Sesion Actual", AppView::CURRENT_SESSION);
    ImGui::EndChild();
}

void SideMenu::DrawMenuButton(const char *label, AppView view, bool showCounter, int counter)
{
    bool active = (m_currentView == view);
    ImVec4 base = active ? UIColors::Theme::INTERACTIVE_4 : UIColors::Theme::INTERACTIVE_2;
    ImVec4 hover = active ? UIColors::Theme::INTERACTIVE_5 : UIColors::Theme::INTERACTIVE_3;
    ImGui::PushStyleColor(ImGuiCol_Button, base);
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, hover);
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, base);
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 6.0f);
    if (ImGui::Button(label, ImVec2(-1, 36)))
    {
        m_currentView = view;
        if (m_onChange)
            m_onChange(view);
    }
    ImGui::PopStyleVar();
    ImGui::PopStyleColor(3);
    if (showCounter)
    {
        ImGui::SameLine();
        ImGui::TextColored(UIColors::DriverTags::GOOD_RACER, "%d", counter);
    }
}
