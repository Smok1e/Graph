#include <vector>
#include <concepts>
#include <iostream>
#include <numbers>
#include <random>
#include <format>

#include <SFML/Graphics.hpp>

#include <Objects/Node.hpp>
#include <Objects/Edge.hpp>
#include <Objects/ObjectManager.hpp>

#include <ImmersiveDarkMode.hpp>
#include <ImGuiExtra.hpp>

//========================================

class Main
{
public:
	Main() = default;

	void run();

private:
	sf::RenderWindow m_render_window {};
	ObjectManager m_object_manager {};

	sf::Clock m_delta_clock {};

	bool m_rmb_menu_show = false;
	sf::Vector2f m_rmb_menu_pos {};

	bool m_imgui_demo_show = false;
	bool m_objects_show = false;

	void onEvent(const sf::Event& event);
	void processInterface();

};

//======================================== Main loop

void Main::run()
{
	sf::Font font;
	if (!font.loadFromFile("resources/CascadiaMono.ttf"))
		return;

	m_object_manager.setFont(&font);

	sf::ContextSettings settings;
	settings.antialiasingLevel = 8;

	m_render_window.create(sf::VideoMode(1000, 1000), "Graph editor", sf::Style::Default & ~sf::Style::Resize, settings);
	m_render_window.setVerticalSyncEnabled(true);
	m_object_manager.setWindow(&m_render_window);

	TryEnableImmersiveDarkMode(m_render_window);

	ImGui::SFML::Init(m_render_window);

	auto& style = ImGui::GetStyle();
	style.WindowRounding = 4;

	while (m_render_window.isOpen())
	{
		sf::Event event;
		while (m_render_window.pollEvent(event))
			onEvent(event);

		m_render_window.clear(sf::Color(24, 24, 24));

		processInterface();
		m_object_manager.drawObjects();

		ImGui::SFML::Render(m_render_window);

		m_render_window.display();
		m_object_manager.cleanup();
	}
}

//======================================== GUI

void Main::processInterface()
{
	ImGui::SFML::Update(m_render_window, m_delta_clock.restart());
	m_object_manager.processInterface();

	// Main menu bar
	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("View"))
		{
			ImGui::MenuItem("Objects",    nullptr, &m_objects_show);
			ImGui::MenuItem("ImGui demo", nullptr, &m_imgui_demo_show);
			ImGui::EndMenu();
		}

		ImGui::EndMainMenuBar();
	}

	// RMB menu
	if (m_rmb_menu_show)
	{
		ImGui::SetNextWindowPos(m_rmb_menu_pos);

		if (
			ImGui::Begin(
				"Create new", 
				nullptr, 
				ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoTitleBar
			)
		)
		{
			ImGui::SeparatorText("Options");

			if (ImGui::Selectable("New node"))
			{
				(m_object_manager += new Node)->setPosition(m_rmb_menu_pos);
				m_rmb_menu_show = false;
			}

			if (ImGui::Selectable("New random graph"))
			{
				std::mt19937 generator(std::random_device{}());

				auto size = m_render_window.getSize();

				std::uniform_int_distribution<> node_count_dist(3, 10);
				std::uniform_int_distribution<> edge_count_dist(3, 10);
				std::uniform_real_distribution<> x_dist(10, size.x - 10);
				std::uniform_real_distribution<> y_dist(10, size.y - 10);

				std::vector<Node*> nodes;
				size_t node_count = node_count_dist(generator);
				for (size_t i = 0; i < node_count; i++)
				{
					auto* node = m_object_manager += new Node;
					node->setPosition(
						sf::Vector2f(
							x_dist(generator),
							y_dist(generator)
						)
					);

					nodes.push_back(node);
				}

				std::uniform_int_distribution<> node_index_dist(0, node_count - 1);
				size_t edge_count = edge_count_dist(generator);
				for (size_t i = 0; i < edge_count; i++)
				{
					auto a = node_index_dist(generator);
					auto b = node_index_dist(generator);
					if (a == b)
					{
						edge_count++;
						continue;
					}

					auto* edge = m_object_manager += new Edge;
					edge->setNodeA(nodes[a]);
					edge->setNodeB(nodes[b]);
				}

				m_rmb_menu_show = false;
			}

			ImGui::End();
		}
	}

	// ImGui demo
	if (m_imgui_demo_show)
		ImGui::ShowDemoWindow(&m_imgui_demo_show);

	// Objects
	if (m_objects_show)
	{
		if (ImGui::Begin("Object manager", &m_objects_show))
		{
			static const char* columns[] = {
				"#",
				"Type"
			};

			ImGui::Text("Objects:");
			if (ImGui::BeginTable("table_objects", std::size(columns), ImGuiTableFlags_ScrollY | ImGuiTableFlags_Borders | ImGuiTableFlags_Resizable))
			{
				for (const char* column: columns)
					ImGui::TableSetupColumn(column);

				ImGui::TableHeadersRow();

				size_t i = 0;
				for (auto* object: m_object_manager)
				{
					ImGui::TableNextRow();

					ImGui::TableNextColumn();
					ImGui::Selectable(std::to_string(++i).c_str());

					ImGui::TableNextColumn();
					object->insertSelectableReference();
				}

				ImGui::EndTable();
			}

			ImGui::Text("%zu objects in total", m_object_manager.size());

			if (ImGui::Button("Delete all"))
				m_object_manager.clear();

		}

		ImGui::End();
	}
}

//======================================== Event processing

void Main::onEvent(const sf::Event& event)
{
	ImGui::SFML::ProcessEvent(event);

	if (m_object_manager.onEvent(event))
		return;

	auto& io = ImGui::GetIO();
	switch (event.type)
	{
		case sf::Event::Closed:
			m_render_window.close();
			break;

		case sf::Event::MouseButtonPressed:
			if (!io.WantCaptureMouse)
				m_rmb_menu_show = false;

			break;

		case sf::Event::MouseButtonReleased:
			switch (event.mouseButton.button)
			{
				case sf::Mouse::Right:
					if (!io.WantCaptureMouse)
					{
						m_rmb_menu_show = true;
						m_rmb_menu_pos = sf::Vector2f(
							event.mouseButton.x,
							event.mouseButton.y
						);
					}

					break;

			}

			break;

		case sf::Event::KeyPressed:
			if (!io.WantCaptureKeyboard)
			{
				switch (event.key.code)
				{
					case sf::Keyboard::Escape:
						m_rmb_menu_show = false;
						break;

				}
			}

			break;

	}
}

//========================================

int main()
{
	Main instance;
	instance.run();

	return 0;
}

//========================================