#include <format>

#include <Objects/Object.hpp>
#include <Objects/ObjectManager.hpp>
#include <ImGuiExtra.hpp>

//========================================

size_t Object::s_id_counter = 0;

Object::Object():
	m_id(++s_id_counter)
{}

//========================================

bool Object::onEvent(const sf::Event& event)
{
	auto& io = ImGui::GetIO();
	switch (event.type)
	{
		case sf::Event::MouseMoved:
		{
			bool hover = intersect(sf::Vector2f(event.mouseMove.x, event.mouseMove.y));
			if (hover != m_hovered)
			{
				m_hovered = hover;
				onHoverChanged();
				return true;
			}

			return false;
			break;
		}

		case sf::Event::MouseButtonPressed:
			if (m_rmb_menu_show && !io.WantCaptureMouse)
			{
				m_rmb_menu_show = false;
				return true;
			}

			break;

		case sf::Event::MouseButtonReleased:
			switch (event.mouseButton.button)
			{
				case sf::Mouse::Right:
					if (m_hovered && !io.WantCaptureMouse)
					{
						m_rmb_menu_show = true;
						m_rmb_menu_pos = sf::Vector2f(
							event.mouseButton.x,
							event.mouseButton.y
						);

						return true;
					}

					break;

			}

			break;

		case sf::Event::KeyPressed:
			switch (event.key.code)
			{
				case sf::Keyboard::Escape:
					if (m_properties_show)
					{
						m_properties_show = false;
						return true;
					}

					if (m_rmb_menu_show)
					{
						m_rmb_menu_show = false;
						return true;
					}

					break;

			}

	}

	return false;
}

bool Object::isHovered() const
{
	return m_hovered;
}

void Object::onAdded(ObjectManager* manager)
{
	m_object_manager = manager;
}

void Object::onDelete()
{
}

void Object::onHoverChanged()
{
}

int Object::getPriority() const
{
	return 0;
}

size_t Object::getID() const
{
	return m_id;
}

//========================================

void Object::processInterface()
{
	if (m_rmb_menu_show)
	{
		ImGui::SetNextWindowPos(m_rmb_menu_pos);

		if (
			ImGui::Begin(
				getName(), 
				nullptr, 
				ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoTitleBar
			)
		)
		{
			ImGui::SeparatorText(getName());

			if (onRMBMenuShow())
				m_rmb_menu_show = false;
		}

		ImGui::End();
	}

	if (m_properties_show)
	{
		if (
			ImGui::Begin(
				std::format(
					"{} #{} properties", 
					getName(), 
					m_id
				).c_str(), 
				&m_properties_show,
				ImGuiWindowFlags_AlwaysAutoResize
			)
		)
			onPropertiesShow();

		ImGui::End();
	}
}

bool Object::onRMBMenuShow()
{
	if (ImGui::Selectable("Properties"))
	{
		m_properties_show = true;
		return true;
	}

	if (ImGui::Selectable("Delete"))
	{
		m_object_manager->deleteObject(this);
		return true;
	}	
}

void Object::onPropertiesShow()
{

}

void Object::insertSelectableReference()
{
	ImGui::Selectable(
		std::format("{} #{}", getName(), m_id).c_str(), 
		&m_properties_show, 
		ImGuiSelectableFlags_SpanAllColumns
	);
}

//========================================

bool ObjectPtrCmp::operator()(const Object* a, const Object* b)	const
{
	return a->getPriority() < b->getPriority();
}

//========================================