#include "DriverTagWindow.h"

void DriverTagWindow::RenderDriversWithFlagsView()
{
    if (m_driverTagManager)
    {
        std::vector<DriverData> databaseDrivers;
        for (const auto &[customerId, reputation] : m_driverReputations)
        {
            if (reputation.behaviorFlags != static_cast<uint32_t>(DriverFlags::UNKNOWN) && reputation.behaviorFlags != 0)
            {
                DriverData dbDriver;
                dbDriver.customerId = customerId;
                dbDriver.displayName = reputation.userName;
                dbDriver.carNumber = "DB";
                dbDriver.carIdx = static_cast<int>(databaseDrivers.size());
                dbDriver.position = 0;
                dbDriver.iRating = 0;
                dbDriver.licenseLevel = "?";
                dbDriver.safetyRating = 0.0f;
                dbDriver.isPlayer = false;
                dbDriver.isValid = true;
                databaseDrivers.push_back(dbDriver);
            }
        }
        auto originalDrivers = m_sessionDrivers;
        m_sessionDrivers = databaseDrivers;
        m_driverTagManager->Render();
        m_sessionDrivers = originalDrivers;
    }
    else
    {
        ImGui::Text("Manager no disponible");
    }
}

void DriverTagWindow::RenderCurrentSessionView()
{
    if (m_driverTagManager)
    {
        m_driverTagManager->Render();
    }
    else
    {
        ImGui::Text("Manager no disponible");
    }
}
