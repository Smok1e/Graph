#pragma once

#include <SFML/Graphics.hpp>

//========================================

class ObjectManager;

class Object
{
public:
	Object();

	bool isHovered() const;
	virtual const char* getName() const = 0;

	virtual void draw() = 0;
	virtual bool onEvent(const sf::Event& event);
	virtual void onAdded(ObjectManager* manager);
	virtual void onDelete();

	virtual int getPriority() const;
	size_t getID() const;

	void processInterface();
	void insertSelectableReference();

protected:
	static size_t s_id_counter;
	size_t m_id;

	ObjectManager* m_object_manager = nullptr;

	bool m_hovered = false;

	bool m_rmb_menu_show = false;
	sf::Vector2f m_rmb_menu_pos {};

	bool m_properties_show = false;

	virtual bool intersect(const sf::Vector2f& point) const = 0;
	virtual void onHoverChanged();

	virtual bool onRMBMenuShow();
	virtual void onPropertiesShow();

};

//========================================

struct ObjectPtrCmp
{
	bool operator()(const Object* a, const Object* b) const;
};

//========================================