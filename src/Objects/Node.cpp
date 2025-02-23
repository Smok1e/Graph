#include <format>
#include <iostream>

#include <Objects/Node.hpp>
#include <Objects/Edge.hpp>
#include <Objects/ObjectManager.hpp>

#include <Config.hpp>
#include <Utils.hpp>
#include <ImGuiExtra.hpp>

//========================================

void Node::setPosition(const sf::Vector2f& position)
{
	m_circle.setPosition(position);
}

const sf::Vector2f& Node::getPosition() const
{
	return m_circle.getPosition();
}

//========================================

void Node::draw()
{
	m_circle.setRadius(m_radius);
	m_circle.setOrigin(m_radius, m_radius);
	m_circle.setFillColor(
		m_hovered
			? Interpolate(m_color, sf::Color::White, .5f)
			: m_color
	);

	m_object_manager->getWindow()->draw(m_circle);
}

bool Node::intersect(const sf::Vector2f& point) const
{				
	auto distance = point - m_circle.getPosition();
	return distance.x*distance.x + distance.y*distance.y < pow(m_circle.getRadius(), 2);
}

bool Node::onEvent(const sf::Event& event)
{
	if (Object::onEvent(event))
		return true;

	switch (event.type)
	{
		case sf::Event::MouseButtonPressed:
			switch (event.mouseButton.button)
			{
				case sf::Mouse::Left:
					if (m_hovered)
					{
						// When ctrl is pressed, create new edge
						if (sf::Keyboard::isKeyPressed(sf::Keyboard::LControl))
							m_object_manager->edgeConnectionStart(this);

						// Otherwise, drag the node
						else
						{
							m_captured = true;
							m_capture_offset = sf::Vector2f(
								event.mouseButton.x,
								event.mouseButton.y
							) - getPosition();
						}

						return true;
					}

					break;
			}

			break;

		case sf::Event::MouseButtonReleased:
			switch (event.mouseButton.button)
			{
				case sf::Mouse::Left:
					if (m_captured)
					{
						m_captured = false;
						return true;
					}

					if (m_hovered)
						return m_object_manager->edgeConnectionComplete(this);

					break;

			}

			break;

		case sf::Event::MouseMoved:
			if (m_captured)
			{
				setPosition(
					sf::Vector2f(
						event.mouseMove.x, 
						event.mouseMove.y
					) - m_capture_offset
				);
			}

			break;
	}

	return false;
}

const char* Node::getName() const
{
	return "Node";
}

int Node::getPriority() const
{
	return 1;
}

void Node::onPropertiesShow()
{
	ImGui::SliderFloat("Radius", &m_radius, 5, 100);
	ImGui::ColorEdit3("Color", &m_color);

	ImGui::Text("Connected edges:");
	if (ImGui::BeginTable("table_connected_edges", 2, ImGuiTableFlags_Borders))
	{
		for (size_t i = 0; i < m_connected_edges.size(); i++)
		{
			ImGui::TableNextRow();

			ImGui::TableNextColumn();
			ImGui::Selectable(std::format("{}", i + 1).c_str());

			ImGui::TableNextColumn();
			m_connected_edges[i]->insertSelectableReference();
		}

		ImGui::EndTable();
	}
}

void Node::onDelete()
{
	for (auto* edge: m_connected_edges)
		m_object_manager->deleteObject(edge);
}

void Node::onEdgeConnected(Edge* edge)
{
	m_connected_edges.push_back(edge);
}

void Node::onEdgeDisconnected(Edge* edge)
{
	auto iter = std::find(
		m_connected_edges.begin(), 
		m_connected_edges.end(), 
		edge
	);

	if (iter != m_connected_edges.end())
		m_connected_edges.erase(iter);
}

//========================================