#include <algorithm>

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

sf::Color HSV(int h, int s, int v, int a /*= 0xFF*/)
{
	sf::Color rgb;
	if (s == 0)
	{
		rgb.r = v;
		rgb.g = v;
		rgb.b = v;

		return rgb;
	}

	int region = h / 43;
	int remainder = (h - (region * 43)) * 6;

	int p = (v * (255 - s)) >> 8;
	int q = (v * (255 - ((s * remainder) >> 8))) >> 8;
	int t = (v * (255 - ((s * (255 - remainder)) >> 8))) >> 8;

	switch (region)
	{
		case 0:
			rgb.r = v;
			rgb.g = t;
			rgb.b = p;
			break;

		case 1:
			rgb.r = q;
			rgb.g = v;
			rgb.b = p;
			break;

		case 2:
			rgb.r = p;
			rgb.g = v;
			rgb.b = t;
			break;

		case 3:
			rgb.r = p;
			rgb.g = q;
			rgb.b = v;
			break;

		case 4:
			rgb.r = t;
			rgb.g = p;
			rgb.b = v;
			break;

		default:
			rgb.r = v;
			rgb.g = p;
			rgb.b = q;
			break;

	}

	rgb.a = a;
	return rgb;
}

//========================================