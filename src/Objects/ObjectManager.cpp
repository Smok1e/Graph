#include <cassert>
#include <algorithm>
#include <iostream>

#include <Graph/Objects/ObjectManager.hpp>
#include <Graph/Objects/Object.hpp>

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

	return false;
}

void ObjectManager::processInterface()
{
	for (auto* object: m_objects)
		object->processInterface();
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