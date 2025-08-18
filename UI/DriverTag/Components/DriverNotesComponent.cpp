#include "DriverNotesComponent.h"
#include "../../../Utils/Logging/Logger.h"
#include <cstring>
#include <ctime>

DriverNotesComponent::DriverNotesComponent(
    const std::vector<DriverData> &sessionDrivers,
    const int &selectedDriverIndex,
    char *notesBuffer,
    size_t notesBufferSize,
    std::function<DriverReputation &(int, const std::string &)> getOrCreateReputationFunc,
    std::function<void(int)> markDirtyFunc) : m_sessionDrivers(sessionDrivers),
                                              m_selectedDriverIndex(selectedDriverIndex),
                                              m_notesBuffer(notesBuffer),
                                              m_notesBufferSize(notesBufferSize),
                                              m_getOrCreateReputation(getOrCreateReputationFunc),
                                              m_markDirty(markDirtyFunc)
{
}

bool DriverNotesComponent::Initialize()
{
    return true;
}

void DriverNotesComponent::Shutdown()
{
    // Cleanup si es necesario
}

void DriverNotesComponent::Render()
{
    if (m_selectedDriverIndex < 0 || m_selectedDriverIndex >= (int)m_sessionDrivers.size())
    {
        return;
    }

    const auto &driver = m_sessionDrivers[m_selectedDriverIndex];
    auto &reputation = m_getOrCreateReputation(driver.customerId, driver.displayName);

    ImGui::Text("Notas personales:");

    // Sincronizar notas actuales al buffer si está vacío
    SyncNotesToBuffer(reputation);

    if (ImGui::InputTextMultiline("##notes", m_notesBuffer, m_notesBufferSize,
                                  ImVec2(-1, 100), ImGuiInputTextFlags_AllowTabInput))
    {
        reputation.notes = std::string(m_notesBuffer);
        reputation.lastUpdated = std::time(nullptr);
        m_markDirty(reputation.customerId); // persist live edit
    }

    ImGui::Spacing();
    if (ImGui::Button("Guardar Cambios", ImVec2(120, 30)))
    {
        SaveNotesFromBuffer(reputation);
    }

    ImGui::SameLine();
    if (ImGui::Button("Limpiar Notas", ImVec2(120, 30)))
    {
        ClearNotes(reputation);
    }
}

void DriverNotesComponent::SyncNotesToBuffer(DriverReputation &reputation)
{
    // Copiar notas actuales al buffer si está vacío
    if (strlen(m_notesBuffer) == 0 && !reputation.notes.empty())
    {
        strncpy_s(m_notesBuffer, m_notesBufferSize, reputation.notes.c_str(), m_notesBufferSize - 1);
    }
}

void DriverNotesComponent::SaveNotesFromBuffer(DriverReputation &reputation)
{
    reputation.notes = std::string(m_notesBuffer);
    reputation.lastUpdated = std::time(nullptr);
    Logger::Info("Notas guardadas para: " + reputation.userName);
    m_markDirty(reputation.customerId);
}

void DriverNotesComponent::ClearNotes(DriverReputation &reputation)
{
    reputation.notes.clear();
    memset(m_notesBuffer, 0, m_notesBufferSize);
    reputation.lastUpdated = std::time(nullptr);
    m_markDirty(reputation.customerId);
}
