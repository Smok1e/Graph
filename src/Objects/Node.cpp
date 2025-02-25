#include <format>
#include <iostream>

#include <Graph/Objects/Node.hpp>
#include <Graph/Objects/Edge.hpp>
#include <Graph/Objects/ObjectManager.hpp>

#include <Graph/Config.hpp>
#include <Graph/Utils.hpp>
#include <Graph/ImGuiExtra.hpp>

//========================================

size_t Node::s_node_index = 0;

//========================================

Node::Node():
	Object()
{
	constexpr char letter_begin = 'A';
	constexpr char letter_end   = 'F' + 1;

	auto index = s_node_index++;

	char letter = letter_begin + index % (letter_end - letter_begin);
	if (auto suffix = index / (letter_end - letter_begin))
		setLabel(std::format("{}{}", letter, suffix));

	else
		setLabel(std::string_view(&letter, 1));
				 
	m_text.setCharacterSize(config::font_size);
}

//========================================

void Node::setPosition(const sf::Vector2f& position)
{
	m_circle.setPosition(position);
}

const sf::Vector2f& Node::getPosition() const
{
	return m_circle.getPosition();
}

void Node::setRadius(float radius)
{
	m_radius = radius;
}

float Node::getRadius() const
{
	return m_radius;
}

void Node::setColor(sf::Color color)
{
	m_color = color;
}

sf::Color Node::getColor() const
{
	return m_color;
}

void Node::setLabel(std::string_view label)
{
	m_label = label;
	m_text.setString(std::string(label));
}

std::string_view Node::getLabel() const
{
	return m_label;
}

const char* Node::getName() const
{
	return "Node";
}

int Node::getPriority() const
{
	return 1;
}

Edge* Node::isAdjacent(Node* node) const
{
	if (node == this)
		return nullptr;

	auto iter = std::find_if(
		m_connected_edges.begin(),
		m_connected_edges.end(),
		[node](Edge* edge) -> bool
		{
			return 
				edge->getNodeA() == node ||
				edge->getNodeB() == node;
		}
	);

	return iter != m_connected_edges.end()
		? *iter
		: nullptr;
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

	m_text.setString(m_label);

	auto bounds = m_text.getGlobalBounds();
	m_text.setOrigin(bounds.width / 2, 0);
	m_text.setPosition(m_circle.getPosition() + sf::Vector2f(0, m_circle.getRadius() + 10));

	m_object_manager->getWindow()->draw(m_text);
}

bool Node::intersect(const sf::Vector2f& point) const
{				
	auto distance = point - m_circle.getPosition();
	return distance.x*distance.x + distance.y*distance.y < pow(m_circle.getRadius(), 2);
}

//========================================

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

void Node::onPropertiesShow()
{
	ImGui::SliderFloat("Radius", &m_radius, 5, 100);
	ImGui::ColorEdit3("Color", &m_color);
	ImGui::InputText("Label", &m_label);

	if (!m_connected_edges.empty())
	{
		ImGui::Text("Connected edges:");
		if (ImGui::BeginTable("table_connected_edges", 2, ImGuiTableFlags_Borders))
		{
			size_t i = 0;
			for (auto* edge: m_connected_edges)
			{
				ImGui::TableNextRow();

				ImGui::TableNextColumn();
				ImGui::Selectable(std::to_string(++i).c_str());

				ImGui::TableNextColumn();
				edge->insertSelectableReference();
			}

			ImGui::EndTable();
		}
	}

	else
		ImGui::Text("No edges are connected");
}

bool Node::onRMBMenuShow()
{
	Object::onRMBMenuShow();

	if (ImGui::Selectable("Set as source"))
	{
		m_object_manager->pathSearchSrc(this);
		return true;
	}

	if (m_object_manager->getPathSrc() && ImGui::Selectable("Set as destination"))
	{
		m_object_manager->pathSearchDst(this);
		return true;
	}

	return false;
}

//========================================

void Node::onAdded(ObjectManager* manager)
{
	Object::onAdded(manager);
	m_text.setFont(*manager->getFont());
}

void Node::onDelete()
{
	auto copy = m_connected_edges;
	for (auto* edge: copy)
		m_object_manager->deleteObject(edge);

	if (m_object_manager->getPathSrc() == this)
		m_object_manager->pathSearchSrc(nullptr);

	if (m_object_manager->getPathDst() == this)
		m_object_manager->pathSearchDst(nullptr);
}

void Node::onEdgeConnected(Edge* edge)
{
	if (
		std::find(
			m_connected_edges.begin(), 
			m_connected_edges.end(), 
			edge
		) == m_connected_edges.end()
	)
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

const std::vector<Edge*>& Node::getConnectedEdges() const
{
	return m_connected_edges;
}

//========================================