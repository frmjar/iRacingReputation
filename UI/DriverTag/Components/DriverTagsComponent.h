#pragma once

#include "IDriverTagComponent.h"
#include "../../../Utils/Common/Types.h"
#include <vector>
#include <functional>
#include <memory>
#include <d3d11.h>
#include <imgui.h>

// Componente responsable de manejar las etiquetas/tags de comportamiento
class DriverTagsComponent : public IDriverTagComponent
{
public:
    DriverTagsComponent(
        const std::vector<DriverData> &sessionDrivers,
        const int &selectedDriverIndex,
        const std::vector<TagInfo> &availableTags,
        std::function<DriverReputation &(int, const std::string &)> getOrCreateReputationFunc,
        std::function<void(int)> markDirtyFunc,
        ID3D11ShaderResourceView *fallbackIcon);

    void Render() override;
    bool Initialize() override;
    void Shutdown() override;

private:
    // Referencias a los datos principales
    const std::vector<DriverData> &m_sessionDrivers;
    const int &m_selectedDriverIndex;
    const std::vector<TagInfo> &m_availableTags;
    std::function<DriverReputation &(int, const std::string &)> m_getOrCreateReputation;
    std::function<void(int)> m_markDirty;
    ID3D11ShaderResourceView *m_fallbackIcon;

    // Métodos privados de renderizado
    void RenderTagButtons(DriverReputation &reputation);
    void RenderActiveTags(const DriverReputation &reputation);
    bool RenderSingleTagButton(const TagInfo &tag, bool isSelected);
    void UpdateTrustLevel(DriverReputation &reputation) const;
};
