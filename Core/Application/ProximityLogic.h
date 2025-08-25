#include <map>
#pragma once
#include <vector>
#include "../../Utils/Common/Types.h"
#include "../../Overlay/OverlayProximityTags.h"

class ProximityLogic
{
public:
    ProximityLogic(OverlayProximityTagsManager *overlayManager) : overlayManager(overlayManager) {}
    void CheckAndShowOverlay(int playerCarIdx, const std::vector<DriverData> &drivers, const std::map<int, DriverReputation> &reputations, float threshold = 10.0f);

private:
    OverlayProximityTagsManager *overlayManager;
};
