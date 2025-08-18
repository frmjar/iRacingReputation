#include "DriverInfoComponent.h"
#include "../../../Utils/Common/UIColors.h"

DriverInfoComponent::DriverInfoComponent(
    const std::vector<DriverData> &sessionDrivers,
    const int &selectedDriverIndex,
    std::function<DriverReputation &(int, const std::string &)> getOrCreateReputationFunc) : m_sessionDrivers(sessionDrivers),
                                                                                             m_selectedDriverIndex(selectedDriverIndex),
                                                                                             m_getOrCreateReputation(getOrCreateReputationFunc)
{
}

bool DriverInfoComponent::Initialize()
{
    return true;
}

void DriverInfoComponent::Shutdown()
{
    // Cleanup si es necesario
}

void DriverInfoComponent::Render()
{
    ImGui::BeginChild("DriverDetails", ImVec2(0, 0), true);

    if (m_selectedDriverIndex >= 0 && m_selectedDriverIndex < (int)m_sessionDrivers.size())
    {
        const auto &driver = m_sessionDrivers[m_selectedDriverIndex];
        auto &reputation = m_getOrCreateReputation(driver.customerId, driver.displayName);

        ImGui::PushStyleColor(ImGuiCol_Text, UIColors::Theme::TEXT);
        ImGui::Text("Detalles del Piloto");
        ImGui::PopStyleColor();
        ImGui::Separator();

        // Información básica del piloto
        ImGui::PushStyleColor(ImGuiCol_ChildBg, UIColors::Theme::PANEL_BG);
        ImGui::BeginChild("DriverInfo", ImVec2(0, 180), true, ImGuiWindowFlags_NoScrollbar);
        ImGui::PushStyleColor(ImGuiCol_Text, UIColors::Theme::TEXT);

        DrawDriverField("Nombre:", driver.displayName, UIColors::Theme::TEXT);
        DrawDriverField("Numero:", std::string("#") + driver.carNumber, UIColors::Theme::TEXT);
        DrawDriverField("Posicion:", driver.position, UIColors::Special::POSITION_GOLD);
        DrawDriverField("iRating:", driver.iRating, UIColors::GetIRatingColor(driver.iRating));
        DrawDriverField("Licencia:", driver.licenseLevel, UIColors::Theme::TEXT);
        DrawDriverField("Safety:", driver.safetyRating, UIColors::GetSafetyRatingColor(driver.safetyRating), "%.1f");

        if (driver.isPlayer)
        {
            ImGui::Spacing();
            ImGui::TextColored(UIColors::Special::PLAYER_HIGHLIGHT, ">>> (ESTE ERES TU) <<<");
        }

        ImGui::PopStyleColor();
        ImGui::EndChild();
        ImGui::PopStyleColor();

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        // Renderizar componente de tags si está disponible
        if (m_tagsComponent)
        {
            ImGui::Text("Etiquetas de Comportamiento:");
            m_tagsComponent->Render();
            ImGui::Spacing();
            ImGui::Separator();
            ImGui::Spacing();
        }

        // Renderizar componente de notas si está disponible
        if (m_notesComponent)
        {
            m_notesComponent->Render();
        }
    }
    else
    {
        ImGui::Text("Selecciona un piloto de la lista para ver sus detalles");
    }

    ImGui::EndChild();
}

void DriverInfoComponent::DrawDriverField(const char *label, const std::string &value, const ImVec4 &color, float labelWidth)
{
    ImGui::TextUnformatted(label);
    ImGui::SameLine(labelWidth);
    ImGui::TextColored(color, "%s", value.c_str());
}

void DriverInfoComponent::DrawDriverField(const char *label, int value, const ImVec4 &color, float labelWidth)
{
    ImGui::TextUnformatted(label);
    ImGui::SameLine(labelWidth);
    ImGui::TextColored(color, "%d", value);
}

void DriverInfoComponent::DrawDriverField(const char *label, float value, const ImVec4 &color, const char *fmt, float labelWidth)
{
    ImGui::TextUnformatted(label);
    ImGui::SameLine(labelWidth);
    ImGui::TextColored(color, fmt, value);
}
