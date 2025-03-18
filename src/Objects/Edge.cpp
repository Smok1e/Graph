#include <cmath>
#include <numbers>
#include <format>

#include <Graph/Objects/Edge.hpp>
#include <Graph/Objects/Node.hpp>
#include <Graph/Objects/ObjectManager.hpp>

#include <Graph/Config.hpp>
#include <Graph/Utils.hpp>
#include <Graph/ImGuiExtra.hpp>

//========================================

Edge::Edge():
	Object()
{
	m_rectangle.setFillColor(config::edge_default_color);
	m_text.setCharacterSize(config::font_size);

	setWeight(m_weight);
}
						
void Edge::onAdded(ObjectManager* manager)
{
	Object::onAdded(manager);
	m_text.setFont(*manager->getFont());
}

//========================================

void Edge::setThickness(float thickness)
{
	m_thickness = thickness;
}

float Edge::getThickness() const
{
	return m_thickness;
}

void Edge::setColor(sf::Color color)
{
	m_color = color;
}

sf::Color Edge::getColor() const
{
	return m_color;
}

//========================================

void Edge::setNodeA(Node* node)
{
	if (m_node_a)
		m_node_a->onEdgeDisconnected(this);

	m_node_a = node;

	if (m_connecting = !m_node_b)
		m_connecting_end = sf::Vector2f(
			m_object_manager->getWindow()->mapPixelToCoords(
				sf::Mouse::getPosition(*m_object_manager->getWindow())
			)
		);

	node->onEdgeConnected(this);
}

Node* Edge::getNodeA() const
{
	return m_node_a;
}

void Edge::setNodeB(Node* node)
{
	if (m_node_b)
		m_node_b->onEdgeDisconnected(this);

	m_node_b = node;
	m_connecting = false;

	node->onEdgeConnected(this);
}

Node* Edge::getNodeB() const
{
	return m_node_b;
}

void Edge::setWeight(int weight)
{
	m_text.setString(std::to_string(m_weight));
}

int Edge::getWeight() const
{
	return m_weight;
}

Node* Edge::opposite(Node* node) const
{
	assert(node && (node == m_node_a || node == m_node_b));

	return node == m_node_a
		? m_node_b
		: m_node_a;
}

bool Edge::isConnectedTo(Node* node) const
{
	return node == m_node_a || node == m_node_b;
}

//========================================

bool Edge::onEvent(const sf::Event& event)
{
	if (Object::onEvent(event))
		return true;

	switch (event.type)
	{
		case sf::Event::MouseMoved:
			if (m_connecting)
			{
				m_connecting_end = sf::Vector2f(
					m_object_manager->getWindow()->mapPixelToCoords(
						sf::Vector2i(
							event.mouseMove.x,
							event.mouseMove.y
						)
					)
				);

				return false;
			}

			break;

		case sf::Event::MouseButtonReleased:
			if (m_connecting)
			 	m_object_manager->edgeConnectionComplete();

			return false;
			break;

	}

	return false;
}

//========================================

void Edge::draw()
{
	auto a = getAPosition();
	auto b = getBPosition();

	auto direction = b - a;	
	float length = sqrt(direction.x*direction.x + direction.y*direction.y);
	auto angle = atan2(direction.y, direction.x);
	auto center = a + .5f * direction;

	m_rectangle.setPosition(center);
	m_rectangle.setRotation((180.0 / std::numbers::pi) * angle);
	m_rectangle.setSize(sf::Vector2f(length, m_thickness));
	m_rectangle.setOrigin(m_rectangle.getSize() * .5f);

	auto color = m_path_indication
		? config::edge_path_color
		: m_color;

	m_rectangle.setFillColor(
		m_hovered
			? Interpolate(color, sf::Color::White, .5f)
			: color
	);

	m_object_manager->getWindow()->draw(m_rectangle);

	m_text.setFillColor(color);
	m_text.setPosition(center + 20.f * sf::Vector2f(-direction.y, direction.x) / length);

	m_object_manager->getWindow()->draw(m_text);
}

bool Edge::intersect(const sf::Vector2f& point)	const
{
	if (m_connecting)
		return false;

	auto a = getAPosition();
	auto b = getBPosition();

	auto distance = b - a;
	float length = sqrt(distance.x*distance.x + distance.y*distance.y);

	//      _
	//      e1
	//	   b
	//    /
	//   /
	//  /			 _
	// a-----------> i
	// |\
	// | \
	// |  \
	// |   \ _
	// v	 e2
	// _
	// j
	// 
	// e1 = || b - a ||
	// (e1 ^ e2) = 90
	//							 | cos 90 -sin 90 |		   | 0 -1 |		   | -e2_y |
	// => e2 = rotate(90) * e1 = | sin 90  cos 90 | * e1 = | 1  0 | * e1 = |  e2_x |

	auto e1 = distance / length;
	auto e2 = sf::Vector2f(-e1.y, e1.x);

	// | x |   | e1.x e2.x |   | x' |
	// | y | = | e1.y e2.y | * | y' |
	//
	// A = E * A' => A' = E^-1 * A
	// 
	// E^-1 = (1/|E|) * E^T = 1 / (e1.x*e2.y - e2.x*e1.y) * |  e2.y -e2.x |
	// 													    | -e1.y  e1.x |
	//
	// x' = 1 / (e1.x*e2.y - e2.x*e1.y) *  e2.y * x - e2.x * y
	// y' = 1 / (e1.x*e2.y - e2.x*e1.y) * -e1.y * x + e1.x * y

	auto inverse_determinant = 1.f / (e1.x * e2.y - e2.x * e1.y);
	auto e_point = inverse_determinant * sf::Vector2f(
		 e2.y * (point.x - a.x) - e2.x * (point.y - a.y),
		-e1.y * (point.x - a.x) + e1.x * (point.y - a.y)
	);

	auto size = m_rectangle.getSize();
	return 0 <= e_point.x && e_point.x < size.x && abs(e_point.y) <= .5f * size.y;
}

//========================================

const char* Edge::getName() const
{
	return "Edge";
}

void Edge::onPropertiesShow()
{
	ImGui::SliderFloat("Thickness", &m_thickness, 1.f, 20.f);
	ImGui::ColorEdit3("Color", &m_color);

	if (ImGui::SliderInt("Weight", &m_weight, 1, 100))
		setWeight(m_weight);

	ImGui::Text("Connected nodes:");
	if (ImGui::BeginTable("table_connected_nodes", 2, ImGuiTableFlags_Borders))
	{
		ImGui::TableNextRow();

		if (m_node_a)
		{
			ImGui::TableNextColumn();
			ImGui::Selectable("A");

			ImGui::TableNextColumn();
			m_node_a->insertSelectableReference();
		}

		if (m_node_a)
		{
			ImGui::TableNextColumn();
			ImGui::Selectable("B");

			ImGui::TableNextColumn();
			m_node_b->insertSelectableReference();
		}

		ImGui::EndTable();
	}
}

sf::Vector2f Edge::getAPosition() const
{
	return m_node_a->getPosition();
}

sf::Vector2f Edge::getBPosition() const
{
	return m_connecting
		? m_connecting_end
		: m_node_b->getPosition();
}

void Edge::onDelete()
{
	if (m_node_a)
		m_node_a->onEdgeDisconnected(this);

	if (m_node_b)
		m_node_b->onEdgeDisconnected(this);
}

void Edge::setPathIndication(bool enable)
{
	m_path_indication = enable;
}

//========================================
