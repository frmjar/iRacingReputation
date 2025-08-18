#include "DriverListComponent.h"
#include "../../../Utils/Common/UIColors.h"

DriverListComponent::DriverListComponent(
    const std::vector<DriverData> &sessionDrivers,
    int &selectedDriverIndex,
    char *notesBuffer,
    size_t notesBufferSize) : m_sessionDrivers(sessionDrivers),
                              m_selectedDriverIndex(selectedDriverIndex),
                              m_notesBuffer(notesBuffer),
                              m_notesBufferSize(notesBufferSize)
{
}

bool DriverListComponent::Initialize()
{
    return true;
}

void DriverListComponent::Shutdown()
{
    // Cleanup si es necesario
}

void DriverListComponent::Render()
{
    ImGui::BeginChild("DriverList", ImVec2(400, 0), true);

    // Contar solo pilotos reales (excluyendo placeholders "Piloto #")
    int realCount = 0;
    for (const auto &d : m_sessionDrivers)
    {
        if (!IsPlaceholderDriver(d))
        {
            realCount++;
        }
    }

    ImGui::Text("Pilotos mostrados (%d)", realCount);
    ImGui::Separator();

    for (int i = 0; i < static_cast<int>(m_sessionDrivers.size()); i++)
    {
        const auto &driver = m_sessionDrivers[i];

        // Saltar placeholders vacíos
        if (IsPlaceholderDriver(driver))
        {
            continue;
        }

        bool isSelected = (m_selectedDriverIndex == i);
        RenderDriverButton(driver, i, isSelected);

        if (ImGui::IsItemHovered())
        {
            RenderDriverTooltip(driver);
        }
    }

    ImGui::EndChild();
}

bool DriverListComponent::IsPlaceholderDriver(const DriverData &driver) const
{
    return (driver.displayName.rfind("Piloto #", 0) == 0 &&
            driver.customerId == driver.carIdx + 1000);
}

void DriverListComponent::RenderDriverButton(const DriverData &driver, int index, bool isSelected)
{
    // Resaltar al jugador
    if (driver.isPlayer)
    {
        ImGui::PushStyleColor(ImGuiCol_Text, UIColors::Special::PLAYER_HIGHLIGHT); // Dorado para el jugador
    }

    // Crear etiqueta simple para el piloto
    std::string label = std::to_string(driver.position > 0 ? driver.position : (driver.carIdx + 1)) +
                        ": #" + driver.carNumber + " " + driver.displayName;

    if (isSelected)
    {
        ImGui::PushStyleColor(ImGuiCol_Button, UIColors::Special::SELECTED_ITEM); // Azul para seleccionado
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, UIColors::Special::SELECTED_ITEM);
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, UIColors::Special::SELECTED_ITEM);
    }

    // Botón simple con ID único
    std::string buttonId = "pilot_" + std::to_string(index);
    if (ImGui::Button((label + "##" + buttonId).c_str(), ImVec2(-1, 30)))
    {
        bool changed = (m_selectedDriverIndex != index);
        m_selectedDriverIndex = index;

        if (changed)
        {
            // Reset buffer de notas para evitar mezclar textos si se dejó vacío
            memset(m_notesBuffer, 0, m_notesBufferSize);

            // Notificar cambio de selección
            if (m_onSelectionChanged)
            {
                m_onSelectionChanged(index);
            }
        }

        Logger::Info("Piloto " + std::to_string(index) + " seleccionado: " + driver.displayName);
    }

    if (isSelected)
    {
        ImGui::PopStyleColor(3);
    }

    if (driver.isPlayer)
    {
        ImGui::PopStyleColor();
    }
}

void DriverListComponent::RenderDriverTooltip(const DriverData &driver)
{
    ImGui::BeginTooltip();
    ImGui::Text("Posición: %d", driver.position);
    ImGui::Text("iRating: %d", driver.iRating);
    ImGui::Text("Licencia: %s", driver.licenseLevel.c_str());
    ImGui::Text("SR: %.1f", driver.safetyRating);
    if (driver.isPlayer)
    {
        ImGui::Text("(TÚ)");
    }
    ImGui::EndTooltip();
}
