#pragma once

#include "IDriverTagComponent.h"
#include "../../../Utils/Common/Types.h"
#include "../../../Utils/Logging/Logger.h"
#include <vector>
#include <functional>
#include <string>
#include <imgui.h>

// Componente responsable del listado de pilotos
class DriverListComponent : public IDriverTagComponent
{
public:
    DriverListComponent(
        const std::vector<DriverData> &sessionDrivers,
        int &selectedDriverIndex,
        char *notesBuffer,
        size_t notesBufferSize);

    void Render() override;
    bool Initialize() override;
    void Shutdown() override;

    // Configurar callback para notificar cambios de selección
    void SetOnSelectionChanged(std::function<void(int)> callback) { m_onSelectionChanged = callback; }

private:
    // Referencias a los datos principales
    const std::vector<DriverData> &m_sessionDrivers;
    int &m_selectedDriverIndex;
    char *m_notesBuffer;
    size_t m_notesBufferSize;

    // Callback para notificar cambios
    std::function<void(int)> m_onSelectionChanged;

    // Helpers internos
    bool IsPlaceholderDriver(const DriverData &driver) const;
    void RenderDriverButton(const DriverData &driver, int index, bool isSelected);
    void RenderDriverTooltip(const DriverData &driver);
};
