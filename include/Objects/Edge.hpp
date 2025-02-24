#pragma once

#include <Objects/Object.hpp>
#include <Objects/Node.hpp>

#include <SFML/Graphics.hpp>

//========================================

class Edge: public Object
{
public:
	Edge();

	void setThickness(float thickness);
	float getThickness() const;

	void setColor(sf::Color color);
	sf::Color getColor() const;

	void setNodeA(Node* node);
	Node* getNodeA() const;

	void setNodeB(Node* node);
	Node* getNodeB() const;

	void draw() override;
	bool onEvent(const sf::Event& event) override;

	void onDelete() override;

private:
	sf::Color m_color = config::edge_default_color;
	float m_thickness = config::edge_default_thickness;

	bool m_connecting = false;
	sf::Vector2f m_connecting_end {};

	Node* m_node_a = nullptr;
	Node* m_node_b = nullptr;

	sf::RectangleShape m_rectangle {};

	bool intersect(const sf::Vector2f& point) const override;
	const char* getName() const override;
	void onPropertiesShow();

	sf::Vector2f getAPosition() const;
	sf::Vector2f getBPosition() const;

};

//========================================