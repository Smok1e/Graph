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
	setPathIndication(false);
	m_path_src = node;
}

void ObjectManager::pathSearchDst(Node* node)
{
	setPathIndication(false);

	assert(m_path_src);
	m_path_dst = node;

	if (!node)
	{
		m_path_src = nullptr;
		return;
	}

	m_pathfind_overlay_show = true;

	m_shortest_path = FindShortestPath(m_path_src, node);
	if (m_shortest_path.empty())
	{
		m_path_text = "Path not found";
		return;
	}

	m_path_text.clear();
	for (size_t i = 0; i < m_shortest_path.size() - 1; i++)
		m_path_text += std::format("{} -> ", m_shortest_path[i]->getLabel());

	m_path_text += m_shortest_path.back()->getLabel();
	setPathIndication(true);
}

Node* ObjectManager::getPathSrc()
{
	return m_path_src;
}

Node* ObjectManager::getPathDst()
{
	return m_path_dst;
}

void ObjectManager::setPathIndication(bool enable)
{
	if (m_shortest_path.empty())
		return;

	for (size_t i = 0; i < m_shortest_path.size() - 1; i++)
		m_shortest_path[i]->isAdjacent(m_shortest_path[i+1])->setPathIndication(enable);
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
						setPathIndication(m_pathfind_overlay_show = false);
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
			ImGui::Text(
				"Path between %.*s and %.*s", 
				m_path_src->getLabel().size(),
				m_path_src->getLabel().data(), 
				m_path_dst->getLabel().size(),
				m_path_dst->getLabel().data()
			);

			ImGui::Separator();

			ImGui::Text("%s", m_path_text.c_str());
			if (!m_shortest_path.empty())
				ImGui::Text("%zu steps total", m_shortest_path.size() - 1);
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

		m_shortest_path.clear();
		m_pathfind_overlay_show = false;
		m_path_src = nullptr;
		m_path_dst = nullptr;

		m_objects.clear();
		m_clear = false;
	}

	m_deleted_objects.clear();
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
