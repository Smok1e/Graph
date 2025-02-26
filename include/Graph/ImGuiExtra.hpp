#pragma once

#include <SFML/Graphics.hpp>

//========================================

#include <imgui.h>
#include <imgui_stdlib.h>
#include <imgui-SFML.h>

//========================================

namespace ImGui
{

// ColorEdit3 overloading that accepts sf::Color pointer
bool ColorEdit3(const char* label, sf::Color* color, ImGuiColorEditFlags flags = 0);

} // namespace ImGui

//========================================
