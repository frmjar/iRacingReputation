#include "ProximityLogic.h"
#include <chrono>

void ProximityLogic::CheckAndShowOverlay(int playerCarIdx, const std::vector<DriverData> &drivers, const std::map<int, DriverReputation> &reputations, float threshold)
{
    const DriverData *player = nullptr;
    for (const auto &d : drivers)
    {
        if (d.carIdx == playerCarIdx)
        {
            player = &d;
            break;
        }
    }
    if (!player)
        return;
    for (const auto &d : drivers)
    {
        if (d.carIdx == playerCarIdx)
            continue;
        auto it = reputations.find(d.customerId);
        if (it == reputations.end())
            continue;
        const DriverReputation &rep = it->second;
        if (rep.behaviorFlags == 0 || rep.behaviorFlags == static_cast<uint32_t>(DriverFlags::UNKNOWN))
            continue;
        float dist = d.distanceToPlayer;
        if (dist <= threshold)
        {
            // Aquí deberías obtener los tags activos del piloto
            std::vector<TagInfo> tags; // TODO: obtener tags reales desde rep o lógica
            overlayManager->ShowOverlay(d.carIdx, d.displayName, tags, 3.0f);
            return;
        }
    }
    overlayManager->Hide();
}
