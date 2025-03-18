#pragma once

#include <concepts>
#include <vector>
#include <set>

#include <SFML/Graphics.hpp>

#include <Graph/Objects/Object.hpp>
#include <Graph/Objects/Node.hpp>
#include <Graph/Objects/Edge.hpp>
#include <Graph/Path.hpp>

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

	void pathSearchSrc(Node* node);
	void pathSearchDst(Node* node);
	void cancelPathSearch();

	Node* getPathSrc();
	Node* getPathDst();

	template<std::derived_from<Object> T>
	T* addObject(T* object);

	template<std::derived_from<Object> T>
	T* operator+=(T* object);

	template<std::derived_from<Object> T>
	std::vector<T*> findAll();

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

	Node* m_path_src = nullptr;
	Node* m_path_dst = nullptr;
	Path m_shortest_path {};
	bool m_pathfind_overlay_show = false;
	std::string m_path_text = "";

	void setPathIndication(bool enable);

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

template<std::derived_from<Object> T>
std::vector<T*> ObjectManager::findAll()
{
	std::vector<T*> objects;
	for (auto* object: m_objects)
		if (T* casted_object = dynamic_cast<T*>(object))
			objects.push_back(casted_object);

	return objects;
}

//========================================