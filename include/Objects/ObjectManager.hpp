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

	ObjectManager() = default;
	ObjectManager(const ObjectManager& copy) = delete;
	~ObjectManager();

	void setWindow(sf::RenderWindow* window);
	sf::RenderWindow* getWindow();

	void setFont(sf::Font* font);
	sf::Font* getFont() const;

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

	size_t size() const;
	container::iterator begin();
	container::iterator end();

private:
	sf::RenderWindow* m_window = nullptr;
	sf::Font* m_font = nullptr;

	container m_objects {};

	std::vector<container::iterator> m_deleted_objects {};
	bool m_clear = false;

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