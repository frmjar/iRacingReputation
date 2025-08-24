@echo off
echo iRacing Reputation System - Build Script
echo ==========================================

REM Configurar el entorno de Visual Studio
call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat"

if "%1"=="clean" (
    echo Limpiando archivos...
    del *.exe *.obj *.pdb *.ilk 2>nul
    echo Limpieza completada.
    goto :end
)

if "%1"=="release" (
    echo Compilando en modo Release...
    cl.exe /std:c++17 /utf-8 /EHsc /O2 /MD /DNDEBUG /D_CONSOLE /D_CRT_SECURE_NO_WARNINGS ^
    /I. /I./Core/IRacingSDK /I./External/ImGui /I./External/ImGui/backends /I./External/SQLite /I./Utils/Persistence ^
    main.cpp ^
    Core/Application/iRacingReputationApp.cpp ^
    Utils/Logging/Logger.cpp ^
    Utils/Graphics/IconManager.cpp ^
    Utils/IRacing/StringUtils.cpp ^
    Utils/IRacing/YAMLDriverParser.cpp ^
    Utils/IRacing/SessionInfoProvider.cpp ^
    UI/DriverTagWindow.cpp ^
    UI/DriverTagWindow_Init.cpp ^
    UI/DriverTagWindow_Render.cpp ^
    UI/DriverTagWindow_Persistence.cpp ^
    UI/DriverTagWindow_Helpers.cpp ^
    UI/DriverTagWindow_Views.cpp ^
    UI/DriverTag/DriverTagManager.cpp ^
    UI/DriverTag/Components/DriverListComponent.cpp ^
    UI/DriverTag/Components/DriverInfoComponent.cpp ^
    UI/DriverTag/Components/DriverTagsComponent.cpp ^
    UI/DriverTag/Components/DriverNotesComponent.cpp ^
    UI/Components/SideMenu.cpp ^
    Core/IRacingSDK/irsdk_client.cpp ^
    Core/IRacingSDK/irsdk_utils.cpp ^
    Core/IRacingSDK/yaml_parser.cpp ^
    Core/IRacingSDK/IRacingVariables.cpp ^
    Core/IRacingSDK/IRacingConnection.cpp ^
    Utils/Persistence/Database.cpp ^
    Utils/Persistence/ReputationRepository.cpp ^
    External/SQLite/sqlite3.c ^
    External/ImGui/imgui.cpp ^
    External/ImGui/imgui_draw.cpp ^
    External/ImGui/imgui_tables.cpp ^
    External/ImGui/imgui_widgets.cpp ^
    External/ImGui/backends/imgui_impl_win32.cpp ^
    External/ImGui/backends/imgui_impl_dx11.cpp ^
    /Fe:iRacingReputation.exe ^
    /link d3d11.lib d3dcompiler.lib dxgi.lib user32.lib gdi32.lib
) else (
    echo Compilando en modo Debug...
    cl.exe /std:c++17 /utf-8 /EHsc /Zi /MDd /DDEBUG /D_CONSOLE /D_CRT_SECURE_NO_WARNINGS ^
    /I. /I./Core/IRacingSDK /I./External/ImGui /I./External/ImGui/backends /I./External/SQLite /I./Utils/Persistence ^
    main.cpp ^
    Core/Application/iRacingReputationApp.cpp ^
    Utils/Logging/Logger.cpp ^
    Utils/Graphics/IconManager.cpp ^
    Utils/IRacing/StringUtils.cpp ^
    Utils/IRacing/YAMLDriverParser.cpp ^
    Utils/IRacing/SessionInfoProvider.cpp ^
    UI/DriverTagWindow.cpp ^
    UI/DriverTagWindow_Init.cpp ^
    UI/DriverTagWindow_Render.cpp ^
    UI/DriverTagWindow_Persistence.cpp ^
    UI/DriverTagWindow_Helpers.cpp ^
    UI/DriverTagWindow_Views.cpp ^
    UI/DriverTag/DriverTagManager.cpp ^
    UI/DriverTag/Components/DriverListComponent.cpp ^
    UI/DriverTag/Components/DriverInfoComponent.cpp ^
    UI/DriverTag/Components/DriverTagsComponent.cpp ^
    UI/DriverTag/Components/DriverNotesComponent.cpp ^
    UI/Components/SideMenu.cpp ^
    Core/IRacingSDK/irsdk_client.cpp ^
    Core/IRacingSDK/irsdk_utils.cpp ^
    Core/IRacingSDK/yaml_parser.cpp ^
    Core/IRacingSDK/IRacingVariables.cpp ^
    Core/IRacingSDK/IRacingConnection.cpp ^
    Utils/Persistence/Database.cpp ^
    Utils/Persistence/ReputationRepository.cpp ^
    External/SQLite/sqlite3.c ^
    External/ImGui/imgui.cpp ^
    External/ImGui/imgui_draw.cpp ^
    External/ImGui/imgui_tables.cpp ^
    External/ImGui/imgui_widgets.cpp ^
    External/ImGui/backends/imgui_impl_win32.cpp ^
    External/ImGui/backends/imgui_impl_dx11.cpp ^
    /Fe:iRacingReputation.exe ^
    /link d3d11.lib d3dcompiler.lib dxgi.lib user32.lib gdi32.lib
)

if %ERRORLEVEL% EQU 0 (
    echo Compilacion exitosa!
    if "%2"=="run" (
        echo Ejecutando...
        iRacingReputation.exe
    )
) else (
    echo Error en la compilacion.
)

:end
pause
