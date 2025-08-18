#pragma once

#include "IDriverTagComponent.h"
#include "../../../Utils/Common/Types.h"
#include <functional>
#include <imgui.h>

// Componente responsable de manejar las notas personales del piloto
class DriverNotesComponent : public IDriverTagComponent
{
public:
    DriverNotesComponent(
        const std::vector<DriverData> &sessionDrivers,
        const int &selectedDriverIndex,
        char *notesBuffer,
        size_t notesBufferSize,
        std::function<DriverReputation &(int, const std::string &)> getOrCreateReputationFunc,
        std::function<void(int)> markDirtyFunc);

    void Render() override;
    bool Initialize() override;
    void Shutdown() override;

private:
    // Referencias a los datos principales
    const std::vector<DriverData> &m_sessionDrivers;
    const int &m_selectedDriverIndex;
    char *m_notesBuffer;
    size_t m_notesBufferSize;
    std::function<DriverReputation &(int, const std::string &)> m_getOrCreateReputation;
    std::function<void(int)> m_markDirty;

    // Helpers privados
    void SyncNotesToBuffer(DriverReputation &reputation);
    void SaveNotesFromBuffer(DriverReputation &reputation);
    void ClearNotes(DriverReputation &reputation);
};
