#include "DriverTagsComponent.h"
#include "../../../Utils/Common/UIColors.h"
#include "../../../Utils/Logging/Logger.h"
#include <ctime>
#include <cmath>
#include <algorithm>

DriverTagsComponent::DriverTagsComponent(
    const std::vector<DriverData> &sessionDrivers,
    const int &selectedDriverIndex,
    const std::vector<TagInfo> &availableTags,
    std::function<DriverReputation &(int, const std::string &)> getOrCreateReputationFunc,
    std::function<void(int)> markDirtyFunc,
    ID3D11ShaderResourceView *fallbackIcon) : m_sessionDrivers(sessionDrivers),
                                              m_selectedDriverIndex(selectedDriverIndex),
                                              m_availableTags(availableTags),
                                              m_getOrCreateReputation(getOrCreateReputationFunc),
                                              m_markDirty(markDirtyFunc),
                                              m_fallbackIcon(fallbackIcon)
{
}

bool DriverTagsComponent::Initialize()
{
    return true;
}

void DriverTagsComponent::Shutdown()
{
    // Cleanup si es necesario
}

void DriverTagsComponent::Render()
{
    if (m_selectedDriverIndex < 0 || m_selectedDriverIndex >= (int)m_sessionDrivers.size())
    {
        return;
    }

    const auto &driver = m_sessionDrivers[m_selectedDriverIndex];
    auto &reputation = m_getOrCreateReputation(driver.customerId, driver.displayName);

    RenderTagButtons(reputation);
    RenderActiveTags(reputation);
}

void DriverTagsComponent::RenderTagButtons(DriverReputation &reputation)
{
    ImGui::Text("Selecciona el comportamiento:");
    ImGui::Spacing();

    // Configurar estilo de botones
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 12.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(16, 12));
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(16, 12));

    // Calcular columnas basado en el ancho disponible
    float avail = ImGui::GetContentRegionAvail().x;
    int rawCols = (int)std::floor(avail / 170.0f);
    rawCols = std::clamp(rawCols, 1, 6);
    int buttonsPerRow = rawCols;

    int buttonCount = 0;
    for (const auto &tag : m_availableTags)
    {
        bool isSelected = reputation.HasBehavior(tag.behavior);

        if (RenderSingleTagButton(tag, isSelected))
        {
            const DriverData &currentDriver = m_sessionDrivers[m_selectedDriverIndex];

            if (isSelected)
            {
                reputation.RemoveBehavior(tag.behavior);
                Logger::Info("Piloto " + currentDriver.displayName + " - removido tag: " + tag.name);
                m_markDirty(reputation.customerId);
            }
            else
            {
                reputation.AddBehavior(tag.behavior);
                reputation.lastUpdated = std::time(nullptr);
                Logger::Info("Piloto " + currentDriver.displayName + " - agregado tag: " + tag.name);
                UpdateTrustLevel(reputation);
                m_markDirty(reputation.customerId);
            }
        }

        buttonCount++;
        // Organizar en grid basado en el número de columnas calculado
        if (buttonCount % buttonsPerRow != 0 && buttonCount < static_cast<int>(m_availableTags.size()))
        {
            ImGui::SameLine();
        }
    }

    // Restaurar estilos de botones
    ImGui::PopStyleVar(3);
}

void DriverTagsComponent::RenderActiveTags(const DriverReputation &reputation)
{
    // Mostrar tags actuales si hay alguno seleccionado
    if (reputation.behaviorFlags != 0)
    {
        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        ImGui::Text("Tags actuales:");
        bool firstTag = true;
        for (const auto &tag : m_availableTags)
        {
            if (reputation.HasBehavior(tag.behavior))
            {
                if (!firstTag)
                {
                    ImGui::SameLine();
                    ImGui::Text(" | ");
                    ImGui::SameLine();
                }
                ImGui::TextColored(tag.color, "%s", tag.name);
                if (!firstTag)
                {
                    ImGui::SameLine();
                }
                firstTag = false;
            }
        }
    }
}

bool DriverTagsComponent::RenderSingleTagButton(const TagInfo &tag, bool isSelected)
{
    const float btnW = 160.0f;
    const float btnH = 56.0f;
    const float padding = 8.0f;
    const float iconSize = 32.0f;

    // Usar colores del archivo UIColors.h - rojos muy claros
    ImVec4 baseCol = isSelected ? UIColors::Special::SELECTED_BASE : UIColors::Theme::FRAME_BG;
    ImVec4 hoverCol = isSelected ? UIColors::Special::SELECTED_HOVER : UIColors::Theme::INTERACTIVE_2;
    ImVec4 activeCol = isSelected ? UIColors::Special::SELECTED_ACTIVE : UIColors::Theme::INTERACTIVE_4;
    ImVec4 textCol = isSelected ? UIColors::Theme::TEXT : UIColors::Theme::TEXT;

    ImGui::PushStyleColor(ImGuiCol_Button, baseCol);
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, hoverCol);
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, activeCol);
    ImGui::PushStyleColor(ImGuiCol_Text, textCol);
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 10.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(padding, padding));

    // Usamos un ID para no colisionar con otros botones que puedan tener el mismo texto
    ImGui::PushID(tag.name);
    // Botón "vacío": texto lo pintaremos manual para centrar/controlar alineación con icono
    bool clicked = ImGui::Button("##btn", ImVec2(btnW, btnH));
    ImVec2 rectMin = ImGui::GetItemRectMin();
    ImVec2 rectMax = ImGui::GetItemRectMax();
    auto *drawList = ImGui::GetWindowDrawList();

    // Borde de selección más visible
    if (isSelected)
        drawList->AddRect(rectMin, rectMax, IM_COL32(255, 200, 80, 255), 10.0f, 0, 2.0f);
    else
        drawList->AddRect(rectMin, rectMax, IM_COL32(60, 60, 70, 180), 10.0f, 0, 1.0f);

    float xCursor = rectMin.x + padding;
    float yCenter = rectMin.y + (btnH - iconSize) * 0.5f;
    ID3D11ShaderResourceView *tex = tag.iconTexture ? tag.iconTexture : m_fallbackIcon;
    if (tex)
    {
        drawList->AddImage((void *)tex, ImVec2(xCursor, yCenter), ImVec2(xCursor + iconSize, yCenter + iconSize),
                           ImVec2(0, 0), ImVec2(1, 1),
                           isSelected ? IM_COL32(255, 255, 255, 255) : IM_COL32(200, 200, 200, 255));
        xCursor += iconSize + padding;
    }

    // Texto multiline (nombre + descripción breve) centrado verticalmente respecto al botón
    std::string title = tag.name;
    std::string desc = tag.description;
    float maxTextW = rectMax.x - padding - xCursor; // ancho asignado
    if (maxTextW < 10.0f)
        maxTextW = 10.0f;

    auto shrinkToFit = [&](std::string &s)
    {
        const char *ellipsis = "...";
        while (!s.empty() && ImGui::CalcTextSize(s.c_str()).x > maxTextW)
        {
            if (s.size() <= 4)
            {
                s = ellipsis;
                break;
            }
            s.pop_back();
        }
        if (ImGui::CalcTextSize(s.c_str()).x > maxTextW && s.size() > 3)
            s = std::string(ellipsis);
    };

    shrinkToFit(title);
    shrinkToFit(desc);
    ImVec2 titleSize = ImGui::CalcTextSize(title.c_str());
    ImVec2 descSize = ImGui::CalcTextSize(desc.c_str());
    float totalTextH = titleSize.y + descSize.y + 2.0f;
    float textStartY = rectMin.y + (btnH - totalTextH) * 0.5f;
    ImVec2 textPos(xCursor, textStartY);
    drawList->AddText(textPos, ImGui::GetColorU32(textCol), title.c_str());
    ImVec4 descCol = isSelected ? UIColors::Theme::TEXT : UIColors::Theme::TEXT_DISABLED;
    drawList->AddText(ImVec2(textPos.x, textPos.y + titleSize.y + 2.0f), ImGui::GetColorU32(descCol), desc.c_str());

    // Tooltip detallado
    if (ImGui::IsItemHovered())
    {
        ImGui::BeginTooltip();
        ImGui::TextColored(isSelected ? UIColors::Special::PLAYER_HIGHLIGHT : UIColors::Theme::TEXT, "%s", tag.name);
        ImGui::Separator();
        ImGui::PushTextWrapPos(ImGui::GetFontSize() * 24.0f);
        ImGui::TextUnformatted(tag.description);
        ImGui::PopTextWrapPos();
        ImGui::EndTooltip();
    }

    ImGui::PopID();
    ImGui::PopStyleVar(2);
    ImGui::PopStyleColor(4);
    return clicked;
}

void DriverTagsComponent::UpdateTrustLevel(DriverReputation &reputation) const
{
    if (reputation.HasBehavior(DriverFlags::RAMMER) || reputation.HasBehavior(DriverFlags::DIRTY_DRIVER))
    {
        reputation.trustLevel = DriverTrustLevel::AVOID;
    }
    else if (reputation.HasBehavior(DriverFlags::AGGRESSIVE) || reputation.HasBehavior(DriverFlags::BLOCKING) || reputation.HasBehavior(DriverFlags::UNSAFE_REJOIN))
    {
        reputation.trustLevel = DriverTrustLevel::CAUTION;
    }
    else if (reputation.HasBehavior(DriverFlags::CLEAN_DRIVER) || reputation.HasBehavior(DriverFlags::GOOD_RACER))
    {
        reputation.trustLevel = DriverTrustLevel::TRUSTED;
    }
    else
    {
        reputation.trustLevel = DriverTrustLevel::NEUTRAL;
    }
}
