#pragma once

#include <Objects/Object.hpp>
#include <Objects/Node.hpp>

#include <Config.hpp>

#include <SFML/Graphics.hpp>

//========================================

class Edge;

class Node: public Object
{
public:
	using Object::Object;

	void setPosition(const sf::Vector2f& position);
	const sf::Vector2f& getPosition() const;

	void draw() override;
	bool onEvent(const sf::Event& event) override;

	int getPriority() const override;

	void onDelete() override;
	void onEdgeConnected(Edge* edge);
	void onEdgeDisconnected(Edge* edge);

private:
	float m_radius = config::node_default_radius;
	sf::Color m_color = config::node_default_color;

	sf::CircleShape m_circle {};
	sf::Vector2f m_capture_offset {};
	bool m_captured = false;

	std::vector<Edge*> m_connected_edges {};

	bool intersect(const sf::Vector2f& point) const override;
	const char* getName() const override;

	void onPropertiesShow() override;

};

//========================================