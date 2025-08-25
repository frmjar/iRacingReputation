#ifdef __cplusplus
#endif
#pragma once
#include <string>
#include <vector>
#include <chrono>
#include "../Utils/Common/Types.h"

struct ProximityTagOverlay
{
    int carIdx;
    std::string driverName;
    std::vector<TagInfo> tags;
    std::chrono::steady_clock::time_point timestamp;
    float duration = 3.0f; // segundos
};

class OverlayProximityTagsManager
{
public:
    void ShowOverlay(int carIdx, const std::string &driverName, const std::vector<TagInfo> &tags, float duration = 3.0f);
    void Update();
    void Render();
    void Hide();

private:
    bool visible = false;
    ProximityTagOverlay currentOverlay;
};

extern OverlayProximityTagsManager overlayManager;
extern OverlayProximityTagsManager overlayManager;
