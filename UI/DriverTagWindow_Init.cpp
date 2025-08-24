#include "DriverTagWindow.h"

DriverTagWindow::~DriverTagWindow()
{
    Shutdown();
}

bool DriverTagWindow::Initialize()
{
    if (m_initialized)
    {
        Logger::Warning("DriverTagWindow ya está inicializado");
        return true;
    }
    Logger::Info("Inicializando DriverTagWindow...");
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
    m_hwnd = CreateWindowExW(0, L"DriverTagWindow", L"iRacing Reputation - Driver Tagging", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 1000, 700, nullptr, nullptr, GetModuleHandle(nullptr), this);
    if (!m_hwnd)
    {
        Logger::Error("Error creando ventana");
        return false;
    }
    if (!CreateDeviceD3D())
    {
        Logger::Error("Error inicializando DirectX");
        return false;
    }
    m_iconManager = std::make_unique<IconManager>();
    if (!m_iconManager->Initialize(m_device))
    {
        Logger::Error("Error inicializando IconManager");
        return false;
    }
    m_imguiContext = ImGui::CreateContext();
    ImGui::SetCurrentContext(m_imguiContext);
    ImGuiIO &io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    ImGui::StyleColorsDark();
    UIColors::ApplyDarkGamingTheme();
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
    if (!LoadFonts())
    {
        Logger::Error("Error cargando fuentes para DriverTagWindow");
        return false;
    }
    if (!LoadIcons())
    {
        Logger::Error("Error cargando iconos para DriverTagWindow");
        return false;
    }
    if (InitPersistence())
    {
        Logger::Info("Persistencia SQLite inicializada");
    }
    else
    {
        Logger::Warning("Persistencia SQLite deshabilitada (fallo al inicializar)");
    }
    LoadMockData();
    m_driverTagManager = std::make_unique<DriverTagManager>(m_sessionDrivers, m_driverReputations, m_selectedDriverIndex, m_notesBuffer, sizeof(m_notesBuffer), m_availableTags, m_fallbackIcon);
    m_driverTagManager->SetGetOrCreateReputationFunc([this](int id, const std::string &name) -> DriverReputation &
                                                     { return GetOrCreateReputation(id, name); });
    m_driverTagManager->SetMarkDirtyFunc([this](int id)
                                         { MarkDirty(id); });
    m_driverTagManager->Initialize();
    m_sideMenu = std::make_unique<SideMenu>([this](AppView v)
                                            { m_currentView = v; });
    m_initialized = true;
    Logger::Info("DriverTagWindow inicializado correctamente");
    return true;
}

void DriverTagWindow::Shutdown()
{
    if (m_initialized)
    {
        FlushDirty(true);
        if (m_imguiContext)
        {
            ImGui::SetCurrentContext(m_imguiContext);
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

void DriverTagWindow::LoadMockData()
{
    m_sessionDrivers.clear();
    m_usingRealData = false;
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
    driver4.isPlayer = true;
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

void DriverTagWindow::UpdateDriverList(const std::vector<DriverData> &drivers)
{
    m_sessionDrivers = drivers;
    Logger::InfoF("Lista de pilotos actualizada: %d pilotos", static_cast<int>(drivers.size()));
}

void DriverTagWindow::LoadSessionData(const std::vector<DriverData> &sessionDrivers)
{
    m_sessionDrivers = sessionDrivers;
    m_usingRealData = true;
    Logger::Info("Datos de sesión cargados: " + std::to_string(sessionDrivers.size()) + " pilotos");
    for (const auto &driver : sessionDrivers)
    {
        GetOrCreateReputation(driver.customerId, driver.displayName);
    }
}

void DriverTagWindow::UpdateSessionData(const std::vector<DriverData> &currentDrivers)
{
    m_sessionDrivers = currentDrivers;
    m_usingRealData = true;
    for (const auto &driver : currentDrivers)
    {
        GetOrCreateReputation(driver.customerId, driver.displayName);
    }
}

bool DriverTagWindow::LoadFonts()
{
    ImGuiIO &io = ImGui::GetIO();
    ImFontConfig config;
    config.MergeMode = false;
    const ImWchar *ranges = io.Fonts->GetGlyphRangesDefault();
    ImFont *font = io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\seguisym.ttf", 16.0f, &config, ranges);
    if (!font)
    {
        Logger::Warning("No se pudo cargar Segoe UI Symbol, usando fuente por defecto");
        font = io.Fonts->AddFontDefault();
    }
    io.Fonts->Build();
    Logger::Info("Fuentes cargadas correctamente");
    return true;
}

bool DriverTagWindow::LoadIcons()
{
    if (!m_iconManager)
    {
        Logger::Error("IconManager not initialized");
        return false;
    }
    // Cargar iconos para cada tag
    for (auto &tag : m_availableTags)
    {
        if (tag.iconPath && strlen(tag.iconPath) > 0)
        {
            // Usamos el nombre lógico del tag como key
            if (m_iconManager->LoadIcon(tag.name, tag.iconPath))
            {
                if (auto *texInfo = m_iconManager->GetIcon(tag.name))
                    tag.iconTexture = texInfo->texture;
            }
            if (!tag.iconTexture)
            {
                Logger::Warning(std::string("Failed to load icon: ") + tag.iconPath);
                if (!CreateFallbackIcon())
                {
                    Logger::Error("Failed to create fallback icon");
                    return false;
                }
                tag.iconTexture = m_fallbackIcon;
            }
        }
        else
        {
            if (!CreateFallbackIcon())
            {
                Logger::Error("Failed to create fallback icon");
                return false;
            }
            tag.iconTexture = m_fallbackIcon;
        }
    }
    return true;
}

void DriverTagWindow::Update()
{
    if (!m_initialized)
        return;
    MSG msg;
    while (PeekMessageW(&msg, m_hwnd, 0, 0, PM_REMOVE))
    {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }
    FlushDirty(false);
}
