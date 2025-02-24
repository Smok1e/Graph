#pragma once

#include <SFML/Graphics.hpp>
#include <filesystem>

//========================================

namespace config
{
	const sf::Color node_default_color(200, 200, 200);
	const float     node_default_radius = 10;

	const sf::Color edge_default_color(0, 170, 255);
	const float     edge_default_thickness = 4;

	const std::filesystem::path resources_path = "./resources";
	const std::filesystem::path font_filename = "CascadiaMono.ttf";

} // namespace config

//========================================