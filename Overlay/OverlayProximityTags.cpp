#include "OverlayProximityTags.h"
#include "../External/ImGui/imgui.h"

OverlayProximityTagsManager overlayManager;

void OverlayProximityTagsManager::ShowOverlay(int carIdx, const std::string &driverName, const std::vector<TagInfo> &tags, float duration)
{
    currentOverlay.carIdx = carIdx;
    currentOverlay.driverName = driverName;
    currentOverlay.tags = tags;
    currentOverlay.timestamp = std::chrono::steady_clock::now();
    currentOverlay.duration = duration;
    visible = true;
}

void OverlayProximityTagsManager::Update()
{
    if (!visible)
        return;
    auto now = std::chrono::steady_clock::now();
    if (std::chrono::duration<float>(now - currentOverlay.timestamp).count() > currentOverlay.duration)
    {
        Hide();
    }
}

void OverlayProximityTagsManager::Render()
{
    if (!visible)
        return;
    ImGui::SetNextWindowBgAlpha(0.7f);
    ImGui::SetNextWindowPos(ImVec2(30, 100), ImGuiCond_Always);
    ImGui::Begin("##ProximityTagsOverlay", nullptr,
                 ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoInputs);

    // driverName sí es std::string, necesitas .c_str()
    ImGui::Text("%s cerca!", currentOverlay.driverName.c_str());

    for (const auto &tag : currentOverlay.tags)
    {
        ImGui::SameLine();
        ImGui::TextColored(tag.color, "%s", tag.name); // <-- corregido
    }
    ImGui::End();
}

void OverlayProximityTagsManager::Hide()
{
    visible = false;
}
