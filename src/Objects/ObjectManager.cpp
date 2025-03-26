#include <cassert>
#include <algorithm>
#include <format>

#include <Graph/Objects/ObjectManager.hpp>
#include <Graph/Objects/Object.hpp>

#include <Graph/Path.hpp>
#include <Graph/ImGuiExtra.hpp>

//========================================

ObjectManager::~ObjectManager()
{
	for (auto* object: m_objects)
		delete object;
}

//========================================

void ObjectManager::setWindow(sf::RenderWindow* window)
{
	m_window = window;
}

sf::RenderWindow* ObjectManager::getWindow()
{
	return m_window;
}

void ObjectManager::setFont(sf::Font* font)
{
	m_font = font;
}

sf::Font* ObjectManager::getFont() const
{
	return m_font;
}

//========================================

void ObjectManager::edgeConnectionStart(Node* node)
{
	m_connecting_edge = addObject(new Edge);
	m_connecting_edge->setNodeA(node);
}

bool ObjectManager::edgeConnectionComplete(Node* node /*= nullptr*/)
{
	if (!m_connecting_edge)
		return false;

	if (node)
	{
		if (node == m_connecting_edge->getNodeA())
			return edgeConnectionComplete();

		m_connecting_edge->setNodeB(node);
	}

	else
		deleteObject(m_connecting_edge);

	m_connecting_edge = nullptr;
	return true;
}

//========================================

void ObjectManager::pathSearchSrc(Node* node)
{
	m_path_src = node;
}

void ObjectManager::pathSearchDst(Node* node)
{
	m_path = Path::Empty();

	assert(m_path_src);
	m_path_dst = node;

	if (!node)
	{
		m_path_src = nullptr;
		return;
	}

	m_pathfind_overlay_show = true;
	m_path = Path::Shortest(m_path_src, m_path_dst);
}

Node* ObjectManager::getPathSrc()
{
	return m_path_src;
}

Node* ObjectManager::getPathDst()
{
	return m_path_dst;
}

void ObjectManager::cancelPathSearch()
{
	m_path = Path::Empty();
	m_pathfind_overlay_show = false;
	m_path_src = nullptr;
	m_path_dst = nullptr;
}

//========================================

void ObjectManager::deleteObject(Object* object)
{
	auto iter = std::find(begin(), end(), object);
	assert("deletion of unexisting object" && iter != end());

	if (
		std::find(
			m_deleted_objects.begin(), 
			m_deleted_objects.end(), 
			iter
		) == m_deleted_objects.end()
	)
	{
		m_deleted_objects.push_back(iter);
		object->onDelete();
	}
}

void ObjectManager::operator-=(Object* object)
{
	deleteObject(object);
}

void ObjectManager::clear()
{
	m_clear = true;
}

void ObjectManager::drawObjects()
{
	for (auto* object: m_objects)
		object->draw();
}

bool ObjectManager::onEvent(const sf::Event& event)
{
	for (auto iter = m_objects.rbegin(); iter != m_objects.rend(); iter++)
		if ((*iter)->onEvent(event)) return true;

	switch (event.type)
	{
		case sf::Event::KeyPressed:
			switch (event.key.code)
			{
				case sf::Keyboard::Escape:
					if (m_pathfind_overlay_show)
					{
						cancelPathSearch();
						return true;
					}

					break;
			}

			break;
	}

	return false;
}

void ObjectManager::processInterface()
{
	for (auto* object: m_objects)
		object->processInterface();

	if (m_pathfind_overlay_show)
	{
		constexpr auto padding = 10.f;
		const auto* viewport = ImGui::GetMainViewport();

		ImGui::SetNextWindowBgAlpha(.35f);
		ImGui::SetNextWindowPos(ImVec2(viewport->WorkPos.x + padding, viewport->WorkPos.y + padding));

		if (
			ImGui::Begin(
				"Path finder",
				nullptr,
				ImGuiWindowFlags_NoDecoration       | 
				ImGuiWindowFlags_AlwaysAutoResize   | 
				ImGuiWindowFlags_NoSavedSettings    | 
				ImGuiWindowFlags_NoFocusOnAppearing |
				ImGuiWindowFlags_NoMove
			)
		)
		{							
			ImGui::Text("Path");
			ImGui::Separator();

			auto text = m_path.getString();
			ImGui::Text("%.*s", text.length(), text.data());
			if (m_path)
			{
				ImGui::Text("Lengh: %zu", m_path.getLength());
				ImGui::Text("Weight: %d", m_path.getWeight());
			}
		}

		ImGui::End();
	}
}

void ObjectManager::cleanup()
{
	for (auto iter: m_deleted_objects)
	{
		delete *iter;
		m_objects.erase(iter);
	}

	if (m_clear)
	{
		for (auto object: m_objects)
			delete object;

		m_path = Path::Empty();
		m_pathfind_overlay_show = false;
		m_path_src = nullptr;
		m_path_dst = nullptr;

		m_objects.clear();
		m_clear = false;
	}

	m_deleted_objects.clear();
}

//========================================

void ObjectManager::onNodeDeleted(Node* node)
{
	if (m_path.contains(node))
		cancelPathSearch();
}

void ObjectManager::onEdgeDeleted(Edge* edge)
{
	if (m_path.contains(edge))
		cancelPathSearch();
}

//========================================

size_t ObjectManager::size() const
{
	return m_objects.size();
}

ObjectManager::container::iterator ObjectManager::begin()
{
	return m_objects.begin();
}

ObjectManager::container::iterator ObjectManager::end()
{
	return m_objects.end();
}

//========================================
