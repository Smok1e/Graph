#pragma once

#include <SFML/Graphics.hpp>

//========================================

// sf::Vector2 compatibility for ImVec2
#define IM_VEC2_CLASS_EXTRA                                             \
template<typename T> ImVec2(const sf::Vector2<T>& vec): ImVec2(x, y) {} \
template<typename T> operator sf::Vector2<T> 							\
{																		\
	return sf::Vector2<T>(x, y);										\
}			

#include <imgui.h>
#include <imgui_stdlib.h>
#include <imgui-SFML.h>

//========================================

namespace ImGui
{

bool ColorEdit3(const char* label, sf::Color* color, ImGuiColorEditFlags flags = 0);

} // namespace ImGui

//========================================