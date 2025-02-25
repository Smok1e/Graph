#pragma once

#include <string>
#include <string_view>

#include <Graph/Objects/Object.hpp>
#include <Graph/Objects/Node.hpp>

#include <Graph/Config.hpp>

#include <SFML/Graphics.hpp>

//========================================

class Edge;

class Node: public Object
{
public:
	Node();

	void setPosition(const sf::Vector2f& position);
	const sf::Vector2f& getPosition() const;

	void setRadius(float radius);
	float getRadius() const;

	void setColor(sf::Color color);
	sf::Color getColor() const;

	void setLabel(std::string_view label);
	std::string_view getLabel() const;

	int getPriority() const override;
	Edge* isAdjacent(Node* node) const;

	void onAdded(ObjectManager* manager) override;
	void draw() override;
	bool onEvent(const sf::Event& event) override;

	void onDelete() override;
	void onEdgeConnected(Edge* edge);
	void onEdgeDisconnected(Edge* edge);

	const std::vector<Edge*>& getConnectedEdges() const;

private:
	static size_t s_node_index;

	float m_radius = config::node_default_radius;
	sf::Color m_color = config::node_default_color;
	std::string m_label = "";

	sf::CircleShape m_circle {};
	sf::Text m_text;

	sf::Vector2f m_capture_offset {};
	bool m_captured = false;

	std::vector<Edge*> m_connected_edges {};

	bool intersect(const sf::Vector2f& point) const override;
	const char* getName() const override;

	void onPropertiesShow() override;
	bool onRMBMenuShow() override;

};

//========================================