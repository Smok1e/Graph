#pragma once

#include <Graph/Objects/Object.hpp>
#include <Graph/Objects/Node.hpp>

#include <SFML/Graphics.hpp>

//========================================

class Edge: public Object
{
public:
	Edge();

	void onAdded(ObjectManager* manager);

	void setThickness(float thickness);
	float getThickness() const;

	void setColor(sf::Color color);
	sf::Color getColor() const;

	void setNodeA(Node* node);
	Node* getNodeA() const;

	void setNodeB(Node* node);
	Node* getNodeB() const;

	void setWeight(int weight);
	int getWeight() const;

	Node* opposite(Node* node) const;
	bool isConnectedTo(Node* node) const;

	void draw() override;
	bool onEvent(const sf::Event& event) override;
	void onDelete() override;

	void setPathIndication(bool enable);

private:
	sf::Color m_color     = config::edge_default_color;
	float     m_thickness = config::edge_default_thickness;
	int       m_weight    = config::edge_default_weight;

	bool m_connecting = false;
	sf::Vector2f m_connecting_end {};

	Node* m_node_a = nullptr;
	Node* m_node_b = nullptr;
	bool m_path_indication = false;

	sf::RectangleShape m_rectangle {};
	sf::Text           m_text      {};

	bool intersect(const sf::Vector2f& point) const override;
	const char* getName() const override;
	void onPropertiesShow();

	sf::Vector2f getAPosition() const;
	sf::Vector2f getBPosition() const;

};

//========================================