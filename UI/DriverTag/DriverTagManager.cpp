#include "DriverTagManager.h"

DriverTagManager::DriverTagManager(
    std::vector<DriverData> &sessionDrivers,
    std::map<int, DriverReputation> &driverReputations,
    int &selectedDriverIndex,
    char *notesBuffer,
    size_t notesBufferSize,
    const std::vector<TagInfo> &availableTags,
    ID3D11ShaderResourceView *fallbackIcon) : m_sessionDrivers(sessionDrivers),
                                              m_driverReputations(driverReputations),
                                              m_selectedDriverIndex(selectedDriverIndex),
                                              m_notesBuffer(notesBuffer),
                                              m_notesBufferSize(notesBufferSize),
                                              m_availableTags(availableTags),
                                              m_fallbackIcon(fallbackIcon)
{
}

bool DriverTagManager::Initialize()
{
    // Crear componente de lista de pilotos
    m_listComponent = std::make_shared<DriverListComponent>(
        m_sessionDrivers,
        m_selectedDriverIndex,
        m_notesBuffer,
        m_notesBufferSize);

    // Callback de selección (si en el futuro necesitamos más lógica)
    m_listComponent->SetOnSelectionChanged([this](int idx)
                                           { OnDriverSelectionChanged(idx); });

    // (Opcional) Configurar callbacks adicionales si se añaden en el futuro

    if (!m_listComponent->Initialize())
    {
        return false;
    }

    // Crear componente de información del piloto
    m_infoComponent = std::make_shared<DriverInfoComponent>(
        m_sessionDrivers,
        m_selectedDriverIndex,
        m_getOrCreateReputationFunc ? m_getOrCreateReputationFunc : [this](int id, const std::string &name) -> DriverReputation &
        { return m_getOrCreateReputationFunc ? m_getOrCreateReputationFunc(id, name) : m_driverReputations[id]; });

    if (!m_infoComponent->Initialize())
    {
        return false;
    }

    // Crear componente de tags
    m_tagsComponent = std::make_shared<DriverTagsComponent>(
        m_sessionDrivers,
        m_selectedDriverIndex,
        m_availableTags,
        m_getOrCreateReputationFunc,
        m_markDirtyFunc,
        m_fallbackIcon);

    if (!m_tagsComponent->Initialize())
    {
        return false;
    }

    // Crear componente de notas
    m_notesComponent = std::make_shared<DriverNotesComponent>(
        m_sessionDrivers,
        m_selectedDriverIndex,
        m_notesBuffer,
        m_notesBufferSize,
        m_getOrCreateReputationFunc,
        m_markDirtyFunc);

    if (!m_notesComponent->Initialize())
    {
        return false;
    }

    // Enlazar subcomponentes en info (tags / notas) para layout integrado si se desea
    if (m_infoComponent)
    {
        if (m_tagsComponent)
            m_infoComponent->SetTagsComponent(m_tagsComponent);
        if (m_notesComponent)
            m_infoComponent->SetNotesComponent(m_notesComponent);
    }

    return true;
}

void DriverTagManager::Shutdown()
{
    if (m_listComponent)
    {
        m_listComponent->Shutdown();
        m_listComponent.reset();
    }

    if (m_infoComponent)
    {
        m_infoComponent->Shutdown();
        m_infoComponent.reset();
    }

    if (m_tagsComponent)
    {
        m_tagsComponent->Shutdown();
        m_tagsComponent.reset();
    }

    if (m_notesComponent)
    {
        m_notesComponent->Shutdown();
        m_notesComponent.reset();
    }
}

void DriverTagManager::Render()
{
    // Renderizar lista de pilotos en la izquierda
    if (m_listComponent)
    {
        m_listComponent->Render();
    }

    ImGui::SameLine();

    // Panel derecho con información, tags y notas
    if (ImGui::BeginChild("DriverDetails", ImVec2(0, 0), true))
    {
        // Información del piloto
        if (m_infoComponent)
        {
            m_infoComponent->Render();
        }

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        // Tags de comportamiento
        if (m_tagsComponent)
        {
            m_tagsComponent->Render();
        }

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        // Notas del piloto
        if (m_notesComponent)
        {
            m_notesComponent->Render();
        }
    }
    ImGui::EndChild();
}

void DriverTagManager::OnDriverSelectionChanged(int newIndex)
{
    // Actualizar el índice en el manager principal
    m_selectedDriverIndex = newIndex;

    // Los componentes obtienen el índice por referencia,
    // por lo que se actualizan automáticamente
}
