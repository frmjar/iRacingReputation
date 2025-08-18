#pragma once

#include "Components/IDriverTagComponent.h"
#include "Components/DriverListComponent.h"
#include "Components/DriverInfoComponent.h"
#include "Components/DriverTagsComponent.h"
#include "Components/DriverNotesComponent.h"
#include "../../Utils/Common/Types.h"
#include <memory>
#include <vector>
#include <functional>
#include <map>
#include <functional>
#include <d3d11.h>

// Manager que orquesta todos los componentes de DriverTag
class DriverTagManager
{
public:
    DriverTagManager(
        std::vector<DriverData> &sessionDrivers,
        std::map<int, DriverReputation> &driverReputations,
        int &selectedDriverIndex,
        char *notesBuffer,
        size_t notesBufferSize,
        const std::vector<TagInfo> &availableTags,
        ID3D11ShaderResourceView *fallbackIcon);

    bool Initialize();
    void Shutdown();
    void Render();

    // Configurar callbacks adicionales
    void SetGetOrCreateReputationFunc(std::function<DriverReputation &(int, const std::string &)> func)
    {
        m_getOrCreateReputationFunc = func;
    }

    void SetUpdateTrustLevelFunc(std::function<void(DriverReputation &)> func)
    {
        m_updateTrustLevelFunc = func;
    }

    void SetMarkDirtyFunc(std::function<void(int)> func)
    {
        m_markDirtyFunc = func;
    }

private:
    // Referencias a datos principales
    std::vector<DriverData> &m_sessionDrivers;
    std::map<int, DriverReputation> &m_driverReputations;
    int &m_selectedDriverIndex;
    char *m_notesBuffer;
    size_t m_notesBufferSize;
    const std::vector<TagInfo> &m_availableTags;
    ID3D11ShaderResourceView *m_fallbackIcon;

    // Componentes
    std::shared_ptr<DriverListComponent> m_listComponent;
    std::shared_ptr<DriverInfoComponent> m_infoComponent;
    std::shared_ptr<DriverTagsComponent> m_tagsComponent;
    std::shared_ptr<DriverNotesComponent> m_notesComponent;

    // Callbacks opcionales
    std::function<DriverReputation &(int, const std::string &)> m_getOrCreateReputationFunc;
    std::function<void(DriverReputation &)> m_updateTrustLevelFunc;
    std::function<void(int)> m_markDirtyFunc;

    // Helpers internos
    void OnDriverSelectionChanged(int newIndex);
};
