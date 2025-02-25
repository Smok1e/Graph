#pragma once

#include <SFML/Graphics.hpp>

//========================================

#ifndef IM_VEC2_CLASS_EXTRA

// Extends ImVec2 to be compatiable with sf::Vector2
#define IM_VEC2_CLASS_EXTRA                                             \
template<typename T> ImVec2(const sf::Vector2<T>& vec): ImVec2(x, y) {} \
template<typename T> operator sf::Vector2<T> 							\
{																		\
	return sf::Vector2<T>(x, y);										\
}			

#endif // #ifndef IM_VEC2_CLASS_EXTRA

#ifndef IM_VEC4_CLASS_EXTRA

// Extends ImVec4 to be cmpatiable with sf::Color
#define IM_VEC4_CLASS_EXTRA					\
ImVec4(sf::Color color): 					\
	ImVec4(									\
		static_cast<float>(color.r) / 0xFF, \
		static_cast<float>(color.g) / 0xFF,	\
		static_cast<float>(color.b) / 0xFF,	\
		static_cast<float>(color.a) / 0xFF	\
	)
#endif // #ifndef IM_VEC4_CLASS_EXTRA

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