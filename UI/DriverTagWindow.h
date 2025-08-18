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

    // Font setup
    bool LoadFonts();
    bool LoadIcons();          // Load PNG icon textures
    bool CreateFallbackIcon(); // Crea textura fallback
    void DrawDriverField(const char *label, const std::string &value, const ImVec4 &color, float labelWidth = 100.0f);
    void DrawDriverField(const char *label, int value, const ImVec4 &color, float labelWidth = 100.0f);
    void DrawDriverField(const char *label, float value, const ImVec4 &color, const char *fmt = "%.1f", float labelWidth = 100.0f);
    ImVec4 GetIRatingColor(int iRating) const;
    ImVec4 GetSafetyColor(float sr) const;
    void UpdateTrustLevel(DriverReputation &rep) const;                      // Centraliza la lógica de asignar trust
    bool RenderSingleTagButton(const TagInfo &tag, bool isSelected);         // Devuelve true si se hizo click
    bool IsPlaceholderDriver(const DriverData &d) const;                     // Determina si es un placeholder vacío
    std::string TruncateText(const std::string &text, float maxWidth) const; // Trunca con algoritmo O(log n)

    void DrawSortAndOptionsBar();                    // Barra de orden y opciones
    std::vector<int> BuildSortedDriverIndex() const; // Indices ordenados

    // Helper methods
    DriverReputation &GetOrCreateReputation(int customerId, const std::string &userName);
    void CreateMockData(); // Para pruebas

    // Persistencia
    bool InitPersistence();
    void FlushDirty(bool force = false);
    void MarkDirty(int customerId);

    // Navegación / vistas
    AppView m_currentView = AppView::DRIVERS_WITH_FLAGS;
    std::unique_ptr<SideMenu> m_sideMenu;
    void RenderDriversWithFlagsView();
    void RenderCurrentSessionView();
    int CountDriversWithFlags() const;

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
};

// Implementación inline para métodos simples
inline DriverTagWindow::~DriverTagWindow()
{
    Shutdown();
}

inline void DriverTagWindow::Show()
{
    if (m_hwnd)
    {
        ShowWindow(m_hwnd, SW_SHOW);
        SetForegroundWindow(m_hwnd);
        m_visible = true;
    }
}

inline void DriverTagWindow::Hide()
{
    if (m_hwnd)
    {
        ShowWindow(m_hwnd, SW_HIDE);
        m_visible = false;
    }
}

inline const DriverReputation *DriverTagWindow::GetDriverReputation(int customerId) const
{
    auto it = m_driverReputations.find(customerId);
    return it != m_driverReputations.end() ? &it->second : nullptr;
}

// Implementación de métodos principales
inline bool DriverTagWindow::Initialize()
{
    if (m_initialized)
    {
        Logger::Warning("DriverTagWindow ya está inicializado");
        return true;
    }

    Logger::Info("Inicializando DriverTagWindow...");

    // Registrar clase de ventana
    WNDCLASSEXW wc = {};
    wc.cbSize = sizeof(WNDCLASSEXW);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = StaticWndProc;
    wc.hInstance = GetModuleHandle(nullptr);
    wc.hIcon = LoadIcon(nullptr, IDI_APPLICATION);
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.lpszClassName = L"DriverTagWindow";
    wc.hIconSm = LoadIcon(nullptr, IDI_APPLICATION);

    if (!RegisterClassExW(&wc))
    {
        Logger::Error("Error registrando clase de ventana");
        return false;
    }

    // Crear ventana
    m_hwnd = CreateWindowExW(
        0,
        L"DriverTagWindow",
        L"iRacing Reputation - Driver Tagging",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT,
        1000, 700,
        nullptr,
        nullptr,
        GetModuleHandle(nullptr),
        this);

    if (!m_hwnd)
    {
        Logger::Error("Error creando ventana");
        return false;
    }

    // Inicializar DirectX
    if (!CreateDeviceD3D())
    {
        Logger::Error("Error inicializando DirectX");
        return false;
    }

    // Inicializar IconManager después de crear el dispositivo
    m_iconManager = std::make_unique<IconManager>();
    if (!m_iconManager->Initialize(m_device))
    {
        Logger::Error("Error inicializando IconManager");
        return false;
    }

    // Crear contexto ImGui separado para esta ventana
    m_imguiContext = ImGui::CreateContext();
    ImGui::SetCurrentContext(m_imguiContext);

    ImGuiIO &io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    // Setup estilo personalizado - similar a la imagen de referencia
    ImGui::StyleColorsDark();

    // Aplicar tema gaming oscuro desde helper
    UIColors::ApplyDarkGamingTheme();

    // Setup backends para esta ventana específica
    if (!ImGui_ImplWin32_Init(m_hwnd))
    {
        Logger::Error("Error inicializando ImGui Win32 backend para DriverTagWindow");
        return false;
    }

    if (!ImGui_ImplDX11_Init(m_device, m_context))
    {
        Logger::Error("Error inicializando ImGui DX11 backend para DriverTagWindow");
        return false;
    }

    // Cargar fuentes con soporte Unicode mejorado
    if (!LoadFonts())
    {
        Logger::Error("Error cargando fuentes para DriverTagWindow");
        return false;
    }

    // Cargar iconos PNG
    if (!LoadIcons())
    {
        Logger::Error("Error cargando iconos para DriverTagWindow");
        return false;
    }

    // Inicializar persistencia (no falla la ventana si DB falla, solo log)
    if (InitPersistence())
    {
        Logger::Info("Persistencia SQLite inicializada");
    }
    else
    {
        Logger::Warning("Persistencia SQLite deshabilitada (fallo al inicializar)");
    }

    // Cargar datos mock para pruebas (solo si aún no hay datos de sesión reales)
    LoadMockData();

    // Inicializar el manager de componentes modulares
    m_driverTagManager = std::make_unique<DriverTagManager>(
        m_sessionDrivers,
        m_driverReputations,
        m_selectedDriverIndex,
        m_notesBuffer,
        sizeof(m_notesBuffer),
        m_availableTags,
        m_fallbackIcon);
    // Configurar callbacks requeridos
    m_driverTagManager->SetGetOrCreateReputationFunc([this](int id, const std::string &name) -> DriverReputation &
                                                     { return GetOrCreateReputation(id, name); });
    m_driverTagManager->SetMarkDirtyFunc([this](int id)
                                         { MarkDirty(id); });
    // (Opcional) actualización de trust si en el futuro se externaliza
    m_driverTagManager->Initialize();

    // Crear SideMenu
    m_sideMenu = std::make_unique<SideMenu>([this](AppView v)
                                            { m_currentView = v; });

    m_initialized = true;
    Logger::Info("DriverTagWindow inicializado correctamente");
    return true;
}

inline void DriverTagWindow::Shutdown()
{
    if (m_initialized)
    {
        // Forzar flush de reputaciones pendientes antes de cerrar
        FlushDirty(true);
        if (m_imguiContext)
        {
            ImGui::SetCurrentContext(m_imguiContext);

            // Verificar si los backends están inicializados antes de hacer shutdown
            ImGuiIO &io = ImGui::GetIO();
            if (io.BackendRendererUserData != nullptr)
            {
                ImGui_ImplDX11_Shutdown();
            }
            if (io.BackendPlatformUserData != nullptr)
            {
                ImGui_ImplWin32_Shutdown();
            }

            ImGui::DestroyContext(m_imguiContext);
            m_imguiContext = nullptr;
        }

        CleanupDeviceD3D();

        if (m_hwnd)
        {
            DestroyWindow(m_hwnd);
            m_hwnd = nullptr;
        }

        UnregisterClassW(L"DriverTagWindow", GetModuleHandle(nullptr));

        m_initialized = false;
        Logger::Info("DriverTagWindow cerrado correctamente");
    }
}

inline void DriverTagWindow::Update()
{
    if (!m_initialized)
        return;

    MSG msg;
    while (PeekMessageW(&msg, m_hwnd, 0, 0, PM_REMOVE))
    {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }

    // Flush periódico de reputaciones sucias (debounce)
    FlushDirty(false);
}

inline void DriverTagWindow::Render()
{
    if (!m_initialized || !m_visible || !m_imguiContext)
        return;

    // Cambiar al contexto de esta ventana
    ImGui::SetCurrentContext(m_imguiContext);

    // Clear
    const float clear_color[4] = {0.45f, 0.55f, 0.60f, 1.00f};
    m_context->OMSetRenderTargets(1, &m_renderTargetView, nullptr);
    m_context->ClearRenderTargetView(m_renderTargetView, clear_color);

    // Start ImGui frame - crear nuestro propio contexto para esta ventana
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    // Main window (fullscreen)
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);

    if (ImGui::Begin("Driver Tagging", nullptr,
                     ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
                         ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse))
    {
        float fullHeight = ImGui::GetContentRegionAvail().y;
        // Menú lateral
        if (m_sideMenu)
        {
            m_sideMenu->Render(fullHeight, CountDriversWithFlags());
            m_currentView = m_sideMenu->GetCurrentView();
        }

        ImGui::SameLine();

        // Contenido principal
        ImGui::BeginChild("MainContent", ImVec2(0, fullHeight), false, ImGuiWindowFlags_None);
        switch (m_currentView)
        {
        case AppView::DRIVERS_WITH_FLAGS:
            RenderDriversWithFlagsView();
            break;
        case AppView::CURRENT_SESSION:
            RenderCurrentSessionView();
            break;
        default:
            ImGui::Text("Vista desconocida");
            break;
        }
        ImGui::EndChild();
    }
    ImGui::End();

    // Render
    ImGui::Render();
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

    m_swapChain->Present(1, 0); // Present with vsync
}

inline void DriverTagWindow::LoadMockData()
{
    // Crear datos mock para pruebas
    m_sessionDrivers.clear();
    m_usingRealData = false;

    // Agregar pilotos ficticios
    DriverData driver1;
    driver1.customerId = 123456;
    driver1.displayName = "Carlos Rodriguez";
    driver1.carNumber = "42";
    driver1.carIdx = 0;
    driver1.position = 1;
    driver1.iRating = 2400;
    driver1.licenseLevel = "A";
    driver1.safetyRating = 4.2f;
    driver1.isPlayer = false;
    driver1.isValid = true;
    m_sessionDrivers.push_back(driver1);

    DriverData driver2;
    driver2.customerId = 789012;
    driver2.displayName = "Anna Thompson";
    driver2.carNumber = "07";
    driver2.carIdx = 1;
    driver2.position = 2;
    driver2.iRating = 1850;
    driver2.licenseLevel = "B";
    driver2.safetyRating = 3.8f;
    driver2.isPlayer = false;
    driver2.isValid = true;
    m_sessionDrivers.push_back(driver2);

    DriverData driver3;
    driver3.customerId = 345678;
    driver3.displayName = "Mike Johnson";
    driver3.carNumber = "15";
    driver3.carIdx = 2;
    driver3.position = 3;
    driver3.iRating = 1250;
    driver3.licenseLevel = "C";
    driver3.safetyRating = 2.1f;
    driver3.isPlayer = false;
    driver3.isValid = true;
    m_sessionDrivers.push_back(driver3);

    DriverData driver4;
    driver4.customerId = 901234;
    driver4.displayName = "Sarah Wilson";
    driver4.carNumber = "88";
    driver4.carIdx = 3;
    driver4.position = 4;
    driver4.iRating = 3200;
    driver4.licenseLevel = "A";
    driver4.safetyRating = 4.8f;
    driver4.isPlayer = true; // Este es el jugador
    driver4.isValid = true;
    m_sessionDrivers.push_back(driver4);

    DriverData driver5;
    driver5.customerId = 567890;
    driver5.displayName = "Alex Martinez";
    driver5.carNumber = "23";
    driver5.carIdx = 4;
    driver5.position = 5;
    driver5.iRating = 980;
    driver5.licenseLevel = "D";
    driver5.safetyRating = 1.9f;
    driver5.isPlayer = false;
    driver5.isValid = true;
    m_sessionDrivers.push_back(driver5);

    Logger::Info("Datos mock cargados: " + std::to_string(m_sessionDrivers.size()) + " pilotos");
}

inline void DriverTagWindow::UpdateDriverList(const std::vector<DriverData> &drivers)
{
    m_sessionDrivers = drivers;
    Logger::InfoF("Lista de pilotos actualizada: %d pilotos", static_cast<int>(drivers.size()));
}

inline DriverReputation &DriverTagWindow::GetOrCreateReputation(int customerId, const std::string &userName)
{
    auto it = m_driverReputations.find(customerId);
    if (it == m_driverReputations.end())
    {
        DriverReputation newRep;
        newRep.customerId = customerId;
        newRep.userName = userName;
        newRep.behaviorFlags = static_cast<uint32_t>(DriverFlags::UNKNOWN);
        newRep.trustLevel = DriverTrustLevel::NEUTRAL;
        newRep.notes = "";
        newRep.lastUpdated = std::time(nullptr);
        newRep.lastSeen = "";
        m_driverReputations[customerId] = newRep;
        return m_driverReputations[customerId];
    }
    return it->second;
}

inline void DriverTagWindow::LoadSessionData(const std::vector<DriverData> &sessionDrivers)
{
    m_sessionDrivers = sessionDrivers;
    m_usingRealData = true;
    Logger::Info("Datos de sesión cargados: " + std::to_string(sessionDrivers.size()) + " pilotos");

    // Crear entradas de reputación para todos los pilotos si no existen
    for (const auto &driver : sessionDrivers)
    {
        GetOrCreateReputation(driver.customerId, driver.displayName);
    }
}

// =================== Persistencia ===================
inline bool DriverTagWindow::InitPersistence()
{
    if (m_persistenceInitialized)
        return true;
    try
    {
        // Ruta base: mismo directorio del ejecutable
        char modulePath[MAX_PATH];
        if (GetModuleFileNameA(nullptr, modulePath, MAX_PATH) == 0)
            return false;
        std::filesystem::path exePath(modulePath);
        auto baseDir = exePath.parent_path();
        auto dbPath = baseDir / "reputation.db";

        if (!m_db.Open(dbPath.string()))
        {
            Logger::Error("No se pudo abrir/crear la base de datos: " + dbPath.string());
            return false;
        }
        if (!m_repo.Init(m_db))
        {
            Logger::Error("No se pudo inicializar el repositorio de reputaciones");
            return false;
        }
        if (!m_repo.LoadAll(m_db, m_driverReputations))
        {
            Logger::Warning("No se pudieron cargar reputaciones existentes (continuando vacío)");
        }
        m_persistenceInitialized = true;
        m_lastFlush = std::time(nullptr);
        return true;
    }
    catch (const std::exception &ex)
    {
        Logger::Error(std::string("Excepción inicializando persistencia: ") + ex.what());
        return false;
    }
}

inline void DriverTagWindow::MarkDirty(int customerId)
{
    if (!m_persistenceInitialized)
        return;
    // Evitar duplicados simples
    if (std::find(m_dirtyIds.begin(), m_dirtyIds.end(), customerId) == m_dirtyIds.end())
    {
        m_dirtyIds.push_back(customerId);
    }
    m_reputationsDirty = true;
}

inline void DriverTagWindow::FlushDirty(bool force)
{
    if (!m_persistenceInitialized || !m_reputationsDirty)
        return;
    const int debounceSeconds = 3; // No escribir más de ~cada 3s salvo fuerza
    std::time_t now = std::time(nullptr);
    if (!force && (now - m_lastFlush) < debounceSeconds)
        return;

    int flushed = 0;
    for (int id : m_dirtyIds)
    {
        auto it = m_driverReputations.find(id);
        if (it == m_driverReputations.end())
            continue;
        // Actualizar metadatos
        it->second.lastUpdated = now;
        if (it->second.lastSeen.empty())
        {
            // Guardar fecha simple YYYY-MM-DD
            std::time_t t = now;
            std::tm tmStruct;
#if defined(_WIN32)
            localtime_s(&tmStruct, &t);
#else
            localtime_r(&t, &tmStruct);
#endif
            char buf[32];
            std::strftime(buf, sizeof(buf), "%Y-%m-%d", &tmStruct);
            it->second.lastSeen = buf;
        }
        if (!m_repo.Upsert(m_db, it->second))
        {
            Logger::Warning("Fallo guardando reputación para id=" + std::to_string(id));
        }
        else
        {
            flushed++;
        }
    }
    m_dirtyIds.clear();
    m_reputationsDirty = false;
    m_lastFlush = now;
    if (flushed > 0)
        Logger::Info("Reputaciones guardadas: " + std::to_string(flushed));
}

inline void DriverTagWindow::UpdateSessionData(const std::vector<DriverData> &currentDrivers)
{
    m_sessionDrivers = currentDrivers;
    m_usingRealData = true; // mantener aunque venga vacía; el fallback se controla fuera

    // Actualizar entradas existentes y crear nuevas si es necesario
    for (const auto &driver : currentDrivers)
    {
        GetOrCreateReputation(driver.customerId, driver.displayName);
    }
}

inline bool DriverTagWindow::LoadFonts()
{
    ImGuiIO &io = ImGui::GetIO();

    // Cargar fuente por defecto de Windows con soporte Unicode mejorado
    ImFontConfig config;
    config.MergeMode = false;

    // Intentar cargar Segoe UI Symbol que tiene mejor soporte para emojis/símbolos
    // Rango extendido latin para acentos y eñes
    const ImWchar *ranges = io.Fonts->GetGlyphRangesDefault();
    ImFont *font = io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\seguisym.ttf", 16.0f, &config, ranges);

    if (!font)
    {
        // Si falla, usar la fuente por defecto
        Logger::Warning("No se pudo cargar Segoe UI Symbol, usando fuente por defecto");
        font = io.Fonts->AddFontDefault();
    }

    // Construir atlas de fuentes
    io.Fonts->Build();

    Logger::Info("Fuentes cargadas correctamente");
    return true;
}

// =================== Navegación / Vistas ===================

inline int DriverTagWindow::CountDriversWithFlags() const
{
    int count = 0;
    for (const auto &kv : m_driverReputations)
    {
        if (kv.second.behaviorFlags != static_cast<uint32_t>(DriverFlags::UNKNOWN) && kv.second.behaviorFlags != 0)
            count++;
    }
    return count;
}

inline void DriverTagWindow::RenderDriversWithFlagsView()
{
    // Vista de pilotos registrados: mostrar solo pilotos de la base de datos
    if (m_driverTagManager)
    {
        // Crear lista temporal de pilotos que están en la base de datos
        std::vector<DriverData> databaseDrivers;
        for (const auto &[customerId, reputation] : m_driverReputations)
        {
            // Solo incluir pilotos que tienen banderas asignadas (no UNKNOWN)
            if (reputation.behaviorFlags != static_cast<uint32_t>(DriverFlags::UNKNOWN) &&
                reputation.behaviorFlags != 0)
            {
                // Crear datos básicos desde la reputación de la base de datos
                DriverData dbDriver;
                dbDriver.customerId = customerId;
                dbDriver.displayName = reputation.userName;
                dbDriver.carNumber = "DB"; // Indicador de que viene de base de datos
                dbDriver.carIdx = static_cast<int>(databaseDrivers.size());
                dbDriver.position = 0; // Sin posición en carrera
                dbDriver.iRating = 0;  // Sin datos de iRating disponibles
                dbDriver.licenseLevel = "?";
                dbDriver.safetyRating = 0.0f;
                dbDriver.isPlayer = false;
                dbDriver.isValid = true;
                databaseDrivers.push_back(dbDriver);
            }
        }

        // Temporalmente reemplazar la lista de pilotos para mostrar solo los de la base de datos
        auto originalDrivers = m_sessionDrivers;
        m_sessionDrivers = databaseDrivers;

        m_driverTagManager->Render();

        // Restaurar la lista original
        m_sessionDrivers = originalDrivers;
    }
    else
    {
        ImGui::Text("Manager no disponible");
    }
}

inline void DriverTagWindow::RenderCurrentSessionView()
{
    // Vista de sesión actual: mostrar todos los pilotos de la sesión actual
    if (m_driverTagManager)
    {
        m_driverTagManager->Render();
    }
    else
    {
        ImGui::Text("Manager no disponible");
    }
}
