#include <numbers>

#include <SFML/Graphics.hpp>

//========================================

float VectorLength(const sf::Vector2f& vec);
sf::Vector2f NormalizeVector(const sf::Vector2f& vec);

sf::Vector2f ChangeBasis(
	const sf::Vector2f& e1,
	const sf::Vector2f& e2,
	const sf::Vector2f& vec
);

void DrawLine(
	sf::RenderTarget& target,
	const sf::Vector2f& a,
	const sf::Vector2f& b,
	sf::Color color = sf::Color::White,
	float thickness = 4
);

bool LineIntersect(
	const sf::Vector2f& a,
	const sf::Vector2f& b,
	float thickness,
	const sf::Vector2f& point
);

//========================================

int main()
{
	sf::RenderWindow window(sf::VideoMode(500, 500), "Test");
	window.setVerticalSyncEnabled(true);

	auto a = .2f * sf::Vector2f(window.getSize());
	auto b = .8f * sf::Vector2f(window.getSize());
	auto thickness = 10;

	sf::Sprite sprite;

	while (window.isOpen())
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			switch (event.type)
			{
				case sf::Event::Closed:
					window.close();
					break;

				case sf::Event::KeyPressed:
					if (event.key.code == sf::Keyboard::Escape)
						window.close();

					break;
			}
		}

		sf::Vector2f mouse(sf::Mouse::getPosition(window));
		if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
			a = mouse;
		
		if (sf::Mouse::isButtonPressed(sf::Mouse::Right))
			b = mouse;

		window.clear();

		auto size = window.getSize();

		sf::Image image;
		image.create(size.x, size.y);
		for (size_t x = 0; x < size.x; x++)
			for (size_t y = 0; y < size.y; y++)
				if (LineIntersect(a, b, thickness, sf::Vector2f(x, y)))
					image.setPixel(x, y, sf::Color::Green);

		sf::Texture texture;
		texture.loadFromImage(image);
		sprite.setTexture(texture);

		window.draw(sprite);
		DrawLine(window, a, b, sf::Color(255, 255, 255, 100), thickness);

		window.display();
	}

	return 0;
}

//========================================

float VectorLength(const sf::Vector2f& vec)
{
	return sqrt(vec.x*vec.x + vec.y*vec.y);
}

sf::Vector2f NormalizeVector(const sf::Vector2f& vec)
{
	return vec * (1.f / VectorLength(vec));
}

// Returns the vector rotated by 90 degrees 
sf::Vector2f Perpendicular(const sf::Vector2f& vec)
{
	return sf::Vector2f(-vec.y, vec.x);
}

sf::Vector2f ChangeBasis(
	const sf::Vector2f& e1,
	const sf::Vector2f& e2,
	const sf::Vector2f& vec
)
{
	auto inverse_determinant = 1.f / (e1.x * e2.y - e2.x * e1.y);
	return inverse_determinant * sf::Vector2f(
		 e2.y * vec.x - e2.x * vec.y,
		-e1.y * vec.x + e1.x * vec.y
	);
}

void DrawLine(
	sf::RenderTarget& target,
	const sf::Vector2f& a,
	const sf::Vector2f& b,
	sf::Color color /*= sf::Color::White*/,
	float thickness /*= 4*/
)
{
	auto distance = b - a;
	auto angle = atan2(distance.y, distance.x);

	sf::RectangleShape rect;
	rect.setFillColor(color);
	rect.setSize(sf::Vector2f(VectorLength(distance), thickness));
	rect.setRotation((180.0 / std::numbers::pi) * angle);
	rect.setPosition(a + .5f * distance);
	rect.setOrigin(.5f * rect.getSize());

	target.draw(rect);
}

bool LineIntersect(
	const sf::Vector2f& a,
	const sf::Vector2f& b,
	float thickness,
	const sf::Vector2f& point
)
{
	auto distance = b - a;
	auto length = VectorLength(distance);

	auto e1 = NormalizeVector(distance);
	auto e2 = Perpendicular(e1);
	auto e_point = ChangeBasis(e1, e2, point - a);

	return 0 <= e_point.x && e_point.x < length && abs(e_point.y) < .5f * thickness;
}

//========================================