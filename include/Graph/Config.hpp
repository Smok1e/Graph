#pragma once

#include <SFML/Graphics.hpp>
#include <filesystem>

//========================================

namespace config
{
	const sf::Color node_default_color(200, 200, 200);
	const float     node_default_radius = 10;

	const sf::Color edge_default_color(0, 210, 163);
	const sf::Color edge_path_color(255, 45, 92);
	const float     edge_default_thickness = 4;

	const sf::Color window_background(30, 31, 34);

	const std::filesystem::path resources_path = "./resources";
	const std::filesystem::path font_filename = "CascadiaMono.ttf";

	constexpr auto font_size = 18.f;

} // namespace config

//========================================