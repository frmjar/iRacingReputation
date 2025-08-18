#pragma once

#include "../../External/ImGui/imgui.h"

namespace UIColors
{
    // Colores de estilo ImGui para tema oscuro gaming
    namespace Theme
    {
        // Fondos
        constexpr ImVec4 WINDOW_BG = ImVec4(0.06f, 0.06f, 0.08f, 1.00f); // Fondo muy oscuro
        constexpr ImVec4 PANEL_BG = ImVec4(0.08f, 0.08f, 0.10f, 1.00f);  // Fondo paneles/popups/scrollbar
        constexpr ImVec4 FRAME_BG = ImVec4(0.12f, 0.12f, 0.15f, 1.00f);  // Fondo frames

        // Grises interactivos (del más oscuro al más claro)
        constexpr ImVec4 INTERACTIVE_1 = ImVec4(0.15f, 0.15f, 0.18f, 1.00f); // Headers/Botones base
        constexpr ImVec4 INTERACTIVE_2 = ImVec4(0.18f, 0.18f, 0.22f, 1.00f); // Frame hover
        constexpr ImVec4 INTERACTIVE_3 = ImVec4(0.20f, 0.20f, 0.25f, 1.00f); // Separadores/Header hover
        constexpr ImVec4 INTERACTIVE_4 = ImVec4(0.25f, 0.25f, 0.30f, 1.00f); // Estados activos
        constexpr ImVec4 INTERACTIVE_5 = ImVec4(0.35f, 0.35f, 0.40f, 1.00f); // Button active
        constexpr ImVec4 INTERACTIVE_6 = ImVec4(0.45f, 0.45f, 0.50f, 1.00f); // Scrollbar active

        // Texto
        constexpr ImVec4 TEXT = ImVec4(0.90f, 0.90f, 0.90f, 1.00f);          // Texto principal
        constexpr ImVec4 TEXT_DISABLED = ImVec4(0.50f, 0.50f, 0.50f, 1.00f); // Texto deshabilitado
        constexpr ImVec4 TEXT_LABEL = ImVec4(0.7f, 0.7f, 0.8f, 1.0f);        // Texto de etiquetas

        // Bordes
        constexpr ImVec4 BORDER = ImVec4(0.15f, 0.15f, 0.20f, 1.00f);        // Bordes sutiles
        constexpr ImVec4 BORDER_SHADOW = ImVec4(0.00f, 0.00f, 0.00f, 0.00f); // Sin sombra
    }

    // Colores para tags de comportamiento de pilotos
    namespace DriverTags
    {
        constexpr ImVec4 CLEAN_DRIVER = ImVec4(0.2f, 0.8f, 0.2f, 1.0f);  // Verde
        constexpr ImVec4 GOOD_RACER = ImVec4(0.0f, 0.8f, 1.0f, 1.0f);    // Cian
        constexpr ImVec4 AGGRESSIVE = ImVec4(1.0f, 0.7f, 0.0f, 1.0f);    // Naranja
        constexpr ImVec4 DIRTY_DRIVER = ImVec4(1.0f, 0.4f, 0.0f, 1.0f);  // Naranja rojizo
        constexpr ImVec4 RAMMER = ImVec4(1.0f, 0.0f, 0.0f, 1.0f);        // Rojo
        constexpr ImVec4 BLOCKING = ImVec4(0.8f, 0.8f, 0.0f, 1.0f);      // Amarillo
        constexpr ImVec4 UNSAFE_REJOIN = ImVec4(1.0f, 0.5f, 0.0f, 1.0f); // Naranja
        constexpr ImVec4 NEWBIE = ImVec4(0.0f, 1.0f, 1.0f, 1.0f);        // Cian brillante
    }

    // Colores para niveles de iRating
    namespace IRating
    {
        constexpr ImVec4 GOLD = ImVec4(1.0f, 0.84f, 0.0f, 1.0f);   // 4000+ Dorado
        constexpr ImVec4 PURPLE = ImVec4(0.5f, 0.0f, 0.5f, 1.0f);  // 3000+ Púrpura
        constexpr ImVec4 BLUE = ImVec4(0.0f, 0.5f, 1.0f, 1.0f);    // 2000+ Azul
        constexpr ImVec4 GREEN = ImVec4(0.0f, 0.8f, 0.0f, 1.0f);   // 1500+ Verde
        constexpr ImVec4 ORANGE = ImVec4(1.0f, 0.65f, 0.0f, 1.0f); // 1000+ Naranja
        constexpr ImVec4 RED = ImVec4(1.0f, 0.25f, 0.25f, 1.0f);   // <1000 Rojo
    }

    // Colores para Safety Rating
    namespace SafetyRating
    {
        constexpr ImVec4 EXCELLENT = ImVec4(0.0f, 0.9f, 0.3f, 1.0f); // 4.0+ Verde brillante
        constexpr ImVec4 GOOD = ImVec4(0.0f, 0.8f, 0.2f, 1.0f);      // 3.0+ Verde
        constexpr ImVec4 AVERAGE = ImVec4(0.9f, 0.8f, 0.0f, 1.0f);   // 2.5+ Amarillo
        constexpr ImVec4 POOR = ImVec4(1.0f, 0.6f, 0.0f, 1.0f);      // 2.0+ Naranja
        constexpr ImVec4 BAD = ImVec4(1.0f, 0.25f, 0.25f, 1.0f);     // <2.0 Rojo
    }

    // Colores especiales para UI
    namespace Special
    {
        constexpr ImVec4 SELECTED_ITEM = ImVec4(0.2f, 0.6f, 1.0f, 1.0f); // Azul para seleccionado
        constexpr ImVec4 POSITION_GOLD = ImVec4(1.0f, 0.8f, 0.0f, 1.0f); // Color posición (diferente al dorado)
        constexpr ImVec4 ERROR_RED = ImVec4(1.0f, 0.3f, 0.3f, 1.0f);     // Rojo error

        // Colores para botones seleccionados (rojo oscuro con transparencia)
        constexpr ImVec4 SELECTED_BASE = ImVec4(0.8f, 0.1f, 0.1f, 0.7f);     // Rojo oscuro con transparencia base
        constexpr ImVec4 SELECTED_HOVER = ImVec4(0.9f, 0.2f, 0.2f, 0.8f);    // Rojo oscuro hover más visible
        constexpr ImVec4 SELECTED_ACTIVE = ImVec4(0.7f, 0.05f, 0.05f, 0.9f); // Rojo muy oscuro active

        // Alias para compatibilidad con código existente
        constexpr ImVec4 PLAYER_HIGHLIGHT = IRating::GOLD;   // Usa el dorado del iRating
        constexpr ImVec4 WARNING = SafetyRating::POOR;       // Usa el naranja del safety rating
        constexpr ImVec4 SUCCESS = DriverTags::CLEAN_DRIVER; // Usa el verde de tags
    }

    // Funciones utilitarias para obtener colores dinámicos
    inline ImVec4 GetIRatingColor(int iRating)
    {
        if (iRating >= 4000)
            return IRating::GOLD;
        if (iRating >= 3000)
            return IRating::PURPLE;
        if (iRating >= 2000)
            return IRating::BLUE;
        if (iRating >= 1500)
            return IRating::GREEN;
        if (iRating >= 1000)
            return IRating::ORANGE;
        return IRating::RED;
    }

    inline ImVec4 GetSafetyRatingColor(float sr)
    {
        if (sr >= 4.0f)
            return SafetyRating::EXCELLENT;
        if (sr >= 3.0f)
            return SafetyRating::GOOD;
        if (sr >= 2.5f)
            return SafetyRating::AVERAGE;
        if (sr >= 2.0f)
            return SafetyRating::POOR;
        return SafetyRating::BAD;
    }

    // Función para aplicar el tema gaming oscuro a ImGui
    inline void ApplyDarkGamingTheme()
    {
        ImGuiStyle &style = ImGui::GetStyle();

        // Aplicar colores del tema
        style.Colors[ImGuiCol_WindowBg] = Theme::WINDOW_BG;
        style.Colors[ImGuiCol_ChildBg] = Theme::PANEL_BG;
        style.Colors[ImGuiCol_PopupBg] = Theme::PANEL_BG;

        style.Colors[ImGuiCol_Header] = Theme::INTERACTIVE_1;
        style.Colors[ImGuiCol_HeaderHovered] = Theme::INTERACTIVE_3;
        style.Colors[ImGuiCol_HeaderActive] = Theme::INTERACTIVE_4;
        style.Colors[ImGuiCol_Separator] = Theme::INTERACTIVE_3;

        style.Colors[ImGuiCol_Button] = Theme::INTERACTIVE_1;
        style.Colors[ImGuiCol_ButtonHovered] = Theme::INTERACTIVE_4;
        style.Colors[ImGuiCol_ButtonActive] = Theme::INTERACTIVE_5;

        style.Colors[ImGuiCol_Text] = Theme::TEXT;
        style.Colors[ImGuiCol_TextDisabled] = Theme::TEXT_DISABLED;

        style.Colors[ImGuiCol_Border] = Theme::BORDER;
        style.Colors[ImGuiCol_BorderShadow] = Theme::BORDER_SHADOW;
        style.Colors[ImGuiCol_FrameBg] = Theme::FRAME_BG;
        style.Colors[ImGuiCol_FrameBgHovered] = Theme::INTERACTIVE_2;
        style.Colors[ImGuiCol_FrameBgActive] = Theme::INTERACTIVE_4;

        style.Colors[ImGuiCol_ScrollbarBg] = Theme::PANEL_BG;
        style.Colors[ImGuiCol_ScrollbarGrab] = Theme::INTERACTIVE_4;
        style.Colors[ImGuiCol_ScrollbarGrabHovered] = Theme::INTERACTIVE_5;
        style.Colors[ImGuiCol_ScrollbarGrabActive] = Theme::INTERACTIVE_6;

        // Configuración de bordes y redondeo - gaming moderno
        style.WindowRounding = 12.0f;
        style.ChildRounding = 8.0f;
        style.FrameRounding = 8.0f;
        style.ScrollbarRounding = 12.0f;
        style.GrabRounding = 6.0f;
        style.WindowBorderSize = 1.0f;
        style.ChildBorderSize = 1.0f;
        style.FrameBorderSize = 0.0f;

        // Espaciado gaming moderno
        style.WindowPadding = ImVec2(16, 16);
        style.FramePadding = ImVec2(12, 8);
        style.ItemSpacing = ImVec2(12, 8);
        style.ItemInnerSpacing = ImVec2(8, 6);
        style.IndentSpacing = 30.0f;
    }
}
