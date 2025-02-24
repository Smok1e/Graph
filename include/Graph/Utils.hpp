#pragma once

#include <SFML/Graphics.hpp>

//========================================

sf::Color Interpolate(sf::Color a, sf::Color b, float t);
sf::Color HSV(int h, int s, int v, int a = 0xFF);

//========================================