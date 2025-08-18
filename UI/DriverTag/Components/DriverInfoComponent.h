#pragma once

#include "IDriverTagComponent.h"
#include "../../../Utils/Common/Types.h"
#include <vector>
#include <functional>
#include <imgui.h>

// Componente responsable de mostrar la información detallada del piloto seleccionado
class DriverInfoComponent : public IDriverTagComponent
{
public:
    DriverInfoComponent(
        const std::vector<DriverData> &sessionDrivers,
        const int &selectedDriverIndex,
        std::function<DriverReputation &(int, const std::string &)> getOrCreateReputationFunc);

    void Render() override;
    bool Initialize() override;
    void Shutdown() override;

    // Configurar componente de tags para renderizar dentro de la información
    void SetTagsComponent(std::shared_ptr<IDriverTagComponent> tagsComponent)
    {
        m_tagsComponent = tagsComponent;
    }

    // Configurar componente de notas
    void SetNotesComponent(std::shared_ptr<IDriverTagComponent> notesComponent)
    {
        m_notesComponent = notesComponent;
    }

private:
    // Referencias a los datos principales
    const std::vector<DriverData> &m_sessionDrivers;
    const int &m_selectedDriverIndex;
    std::function<DriverReputation &(int, const std::string &)> m_getOrCreateReputation;

    // Componentes anidados
    std::shared_ptr<IDriverTagComponent> m_tagsComponent;
    std::shared_ptr<IDriverTagComponent> m_notesComponent;

    // Helpers para renderizado
    void DrawDriverField(const char *label, const std::string &value, const ImVec4 &color, float labelWidth = 100.0f);
    void DrawDriverField(const char *label, int value, const ImVec4 &color, float labelWidth = 100.0f);
    void DrawDriverField(const char *label, float value, const ImVec4 &color, const char *fmt = "%.1f", float labelWidth = 100.0f);
};
