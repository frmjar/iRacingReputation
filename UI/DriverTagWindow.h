/*
MIT License - iRacing Reputation System
Ventana independiente para etiquetar pilotos (no overlay)
*/

#pragma once

#include "../External/ImGui/imgui.h"
#include "../External/ImGui/backends/imgui_impl_win32.h"
#include "../External/ImGui/backends/imgui_impl_dx11.h"
#include "../External/FontAwesome/IconsFontAwesome6.h"
#include "../Utils/Common/Types.h"
#include "../Utils/Logging/Logger.h"
#include "../Utils/Graphics/IconManager.h"
#include "../Utils/Common/UIColors.h"
#include "DriverTag/DriverTagManager.h"
#include "Components/SideMenu.h"
#include <windows.h>
#include <d3d11.h>
#include <vector>
#include <string>
#include <map>
#include <memory>
#include <fstream>
#include <mutex>
#include <filesystem>
#include <chrono>
#include <unordered_set>
#include <algorithm>

// Persistencia SQLite
#include "../Utils/Persistence/Database.h"
#include "../Utils/Persistence/ReputationRepository.h"

// Simple JSON persistencia (manual) para reputaciones

class DriverTagWindow
{
private:
    // Window handles
    HWND m_hwnd = nullptr;
    ID3D11Device *m_device = nullptr;
    ID3D11DeviceContext *m_context = nullptr;
    IDXGISwapChain *m_swapChain = nullptr;
    ID3D11RenderTargetView *m_renderTargetView = nullptr;

    // Icon management
    std::unique_ptr<IconManager> m_iconManager;
    ID3D11ShaderResourceView *m_fallbackIcon = nullptr; // Icono genérico si falta PNG

    // ImGui context separado para esta ventana
    ImGuiContext *m_imguiContext = nullptr;

    bool m_initialized = false;
    bool m_visible = false;
    bool m_shouldClose = false;

    // Datos de la sesión actual
    std::vector<DriverData> m_sessionDrivers;
    std::map<int, DriverReputation> m_driverReputations; // customerId -> reputation (no persistente)
    bool m_usingRealData = false;                        // indica si la lista actual proviene del SDK
    // Persistencia SQLite
    Database m_db;
    ReputationRepository m_repo;
    bool m_persistenceInitialized = false;
    bool m_reputationsDirty = false;
    std::mutex m_repMutex;
    std::vector<int> m_dirtyIds;
    std::time_t m_lastFlush = 0;

    // UI State
    int m_selectedDriverIndex = 0; // Seleccionar el primer piloto por defecto
    char m_notesBuffer[512] = "";
    enum class DriverListSortMode
    {
        POSITION = 0,
        IRATING = 1
    };
    DriverListSortMode m_sortMode = DriverListSortMode::POSITION;
    bool m_highlightTeams = true; // Resaltar miembros de equipo

    // Configuración de tags (usa TagInfo global de Types.h)
    std::vector<TagInfo> m_availableTags = {
        {DriverFlags::CLEAN_DRIVER, "Clean Driver", ICON_CLEAN_DRIVER, nullptr, UIColors::DriverTags::CLEAN_DRIVER, "Piloto limpio y respetuoso"},
        {DriverFlags::GOOD_RACER, "Good Racer", ICON_GOOD_RACER, nullptr, UIColors::DriverTags::GOOD_RACER, "Excelente piloto, recomendado"},
        {DriverFlags::AGGRESSIVE, "Aggressive", ICON_AGGRESSIVE, nullptr, UIColors::DriverTags::AGGRESSIVE, "Agresivo pero justo"},
        {DriverFlags::DIRTY_DRIVER, "Dirty Driver", ICON_DIRTY_DRIVER, nullptr, UIColors::DriverTags::DIRTY_DRIVER, "Piloto sucio"},
        {DriverFlags::RAMMER, "Rammer", ICON_RAMMER, nullptr, UIColors::DriverTags::RAMMER, "Peligroso - contacto intencional"},
        {DriverFlags::BLOCKING, "Blocker", ICON_BLOCKING, nullptr, UIColors::DriverTags::BLOCKING, "Bloqueo excesivo"},
        {DriverFlags::UNSAFE_REJOIN, "Unsafe Rejoin", ICON_UNSAFE_REJOIN, nullptr, UIColors::DriverTags::UNSAFE_REJOIN, "Reentradas peligrosas"},
        {DriverFlags::NEWBIE, "Rookie", ICON_NEWBIE, nullptr, UIColors::DriverTags::NEWBIE, "Piloto novato"}};

    // Manager de componentes modulares
    std::unique_ptr<DriverTagManager> m_driverTagManager;
    std::unordered_set<int> m_dirtyReputations;

    // Window procedure
    static LRESULT CALLBACK StaticWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
    LRESULT WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

    // DirectX setup
    bool CreateDeviceD3D();
    void CleanupDeviceD3D();
    void CreateRenderTarget();
    void CleanupRenderTarget();

    bool LoadFonts();
    bool LoadIcons();
    bool CreateFallbackIcon();
    void DrawDriverField(const char *label, const std::string &value, const ImVec4 &color, float labelWidth = 100.0f);
    void DrawDriverField(const char *label, int value, const ImVec4 &color, float labelWidth = 100.0f);
    void DrawDriverField(const char *label, float value, const ImVec4 &color, const char *fmt = "%.1f", float labelWidth = 100.0f);
    ImVec4 GetIRatingColor(int iRating) const;
    ImVec4 GetSafetyColor(float sr) const;
    void UpdateTrustLevel(DriverReputation &reputation) const;
    bool IsPlaceholderDriver(const DriverData &d) const;
    std::string TruncateText(const std::string &text, float maxWidth) const;
    int CountDriversWithFlags() const;
    DriverReputation &GetOrCreateReputation(int customerId, const std::string &userName);
    bool InitPersistence();
    void FlushDirty(bool force = false);
    void MarkDirty(int customerId);
    AppView m_currentView = AppView::DRIVERS_WITH_FLAGS;
    std::unique_ptr<SideMenu> m_sideMenu;
    void RenderDriversWithFlagsView();
    void RenderCurrentSessionView();

public:
    DriverTagWindow() = default;
    ~DriverTagWindow();

    bool Initialize();
    void Shutdown();
    void Update();
    void Render();

    // Control de ventana
    void Show();
    void Hide();
    void Toggle() { m_visible ? Hide() : Show(); }
    bool IsVisible() const { return m_visible; }
    bool ShouldClose() const { return m_shouldClose; }
    bool IsUsingRealData() const { return m_usingRealData; }

    // Gestión de datos
    void UpdateDriverList(const std::vector<DriverData> &drivers);
    void LoadMockData();                                                   // Para pruebas sin iRacing
    void LoadSessionData(const std::vector<DriverData> &sessionDrivers);   // Cargar datos de sesión real
    void UpdateSessionData(const std::vector<DriverData> &currentDrivers); // Actualizar datos durante sesión

    // Obtener reputación de un piloto
    const DriverReputation *GetDriverReputation(int customerId) const;
    const std::map<int, DriverReputation> &GetDriverReputations() const { return m_driverReputations; }
};
