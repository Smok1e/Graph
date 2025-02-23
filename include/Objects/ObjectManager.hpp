#pragma once

#include <concepts>
#include <vector>
#include <set>

#include <SFML/Graphics.hpp>

#include <Objects/Object.hpp>
#include <Objects/Edge.hpp>

//========================================

class ObjectManager
{
public:
	using container = std::multiset<Object*, ObjectPtrCmp>;
	using iterator = container::iterator;

	ObjectManager() = default;
	~ObjectManager();

	void setWindow(sf::RenderWindow* window);
	sf::RenderWindow* getWindow();

	void edgeConnectionStart(Node* node);
	bool edgeConnectionComplete(Node* node = nullptr);

	template<std::derived_from<Object> T>
	T* addObject(T* object);

	template<std::derived_from<Object> T>
	T* operator+=(T* object);

	void deleteObject(Object* object);
	void operator-=(Object* object);

	void clear();

	void drawObjects();
	bool onEvent(const sf::Event& event);
	void processInterface();
	void cleanup();

	iterator begin();
	iterator end();

private:
	sf::RenderWindow* m_window = nullptr;

	container m_objects {};
	std::vector<iterator> m_deleted_objects {};

	Edge* m_connecting_edge = nullptr;
	bool m_connecting_cancel = false;

};

//========================================

template<std::derived_from<Object> T>
T* ObjectManager::addObject(T* object)
{
	object->onAdded(this);

	m_objects.insert(object);
	return object;
}

template<std::derived_from<Object> T>
T* ObjectManager::operator+=(T* object)
{
	return addObject(object);
}

//========================================