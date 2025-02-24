#include <Graph/Utils.hpp>

//========================================

sf::Color Interpolate(sf::Color a, sf::Color b, float t)
{
	return sf::Color(
		a.r + t * (b.r - a.r),
		a.g + t * (b.g - a.g),
		a.b + t * (b.b - a.b),
		a.a + t * (b.a - a.a)
	);
}

//========================================