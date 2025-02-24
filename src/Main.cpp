#include <vector>
#include <concepts>
#include <iostream>
#include <numbers>
#include <random>
#include <format>
#include <numeric>

#include <SFML/Graphics.hpp>

#include <Graph/Objects/Node.hpp>
#include <Graph/Objects/Edge.hpp>
#include <Graph/Objects/ObjectManager.hpp>

#include <Graph/ImmersiveDarkMode.hpp>
#include <Graph/ImGuiExtra.hpp>
#include <Graph/Config.hpp>
#include <Graph/Utils.hpp>

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

	sf::Font m_font {};
	sf::Cursor m_default_cursor {};
	sf::Cursor m_drag_cursor {};

	bool m_rmb_menu_show = false;
	sf::Vector2i m_rmb_menu_pos {};

	bool m_dragging = false;
	sf::Vector2f m_dragging_start_pos = {};
	sf::View m_dragging_start_view = {};

	bool m_imgui_demo_show = false;
	bool m_objects_show = false;

	void onEvent(const sf::Event& event);
	void processInterface();
	void generateRandomGraph();

};

//======================================== Main loop

void Main::run()
{
	if (!m_font.loadFromFile((config::resources_path/config::font_filename).string()))
		return;

	m_object_manager.setFont(&m_font);

	m_drag_cursor.loadFromSystem(sf::Cursor::SizeAll);
	m_default_cursor.loadFromSystem(sf::Cursor::Arrow);

	sf::ContextSettings settings;
	settings.antialiasingLevel = 8;

	m_render_window.create(sf::VideoMode(1000, 1000), "Graph editor", sf::Style::Default, settings);
	m_render_window.setVerticalSyncEnabled(true);
	m_object_manager.setWindow(&m_render_window);

	TryEnableImmersiveDarkMode(m_render_window);

	ImGui::SFML::Init(m_render_window);

	auto& style = ImGui::GetStyle();
	style.WindowRounding = 4;
	style.WindowBorderSize = 0;

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
			// ImGui::MenuItem("Objects",    nullptr, &m_objects_show   );
			ImGui::MenuItem("ImGui demo", nullptr, &m_imgui_demo_show);
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Edit"))
		{
			if (ImGui::MenuItem("Clear"))
				m_object_manager.clear();

			if (ImGui::MenuItem("Reset view"))
				m_render_window.setView(m_render_window.getDefaultView());

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
			ImGui::SeparatorText("Create");

			if (ImGui::Selectable("Node"))
			{
				m_object_manager.addObject(new Node)->setPosition(
					m_render_window.mapPixelToCoords(m_rmb_menu_pos)
				);

				m_rmb_menu_show = false;
			}

			if (ImGui::Selectable("Random graph"))
			{
				generateRandomGraph();
				m_rmb_menu_show = false;
			}

			ImGui::End();
		}
	}

	// ImGui demo
	if (m_imgui_demo_show)
		ImGui::ShowDemoWindow(&m_imgui_demo_show);

	/*
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
					ImGui::Selectable(std::to_string(++i).c_str(), ImGuiSelectableFlags_SpanAllColumns);

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
	*/
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

		case sf::Event::Resized:
			{
				auto view = m_render_window.getView();
				view.setSize  (event.size.width,     event.size.height    );
				view.setCenter(event.size.width / 2, event.size.height / 2);

				m_render_window.setView(view);
			}

			break;

		case sf::Event::MouseButtonPressed:
			if (!io.WantCaptureMouse)
			{
				m_rmb_menu_show = false;

				switch (event.mouseButton.button)
				{
					case sf::Mouse::Middle:
						m_dragging = true;
						m_dragging_start_pos = sf::Vector2f(
							event.mouseButton.x,
							event.mouseButton.y
						);

						m_dragging_start_view = m_render_window.getView();
						m_render_window.setMouseCursor(m_drag_cursor);
						break;

				}
			}

			break;

		case sf::Event::MouseButtonReleased:
			if (!io.WantCaptureMouse)
			{
				switch (event.mouseButton.button)
				{
					case sf::Mouse::Right:
						m_rmb_menu_show = true;
						m_rmb_menu_pos = sf::Vector2i(
							event.mouseButton.x,
							event.mouseButton.y
						);

						break;

					case sf::Mouse::Middle:
						m_dragging = false;
						m_render_window.setMouseCursor(m_default_cursor);
						break;

				}
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

		case sf::Event::MouseMoved:
			if (m_dragging)
			{
				auto delta = m_dragging_start_pos - sf::Vector2f(
					event.mouseMove.x,
					event.mouseMove.y
				);

				m_render_window.setView(
					sf::View(
						m_dragging_start_view.getCenter() + delta, 
						m_dragging_start_view.getSize()
					)
				);
			}

			break;

		case sf::Event::MouseWheelMoved:
			if (!io.WantCaptureMouse)
			{
				constexpr float scroll_speed = 20;

				auto view = m_render_window.getView();
				if (sf::Keyboard::isKeyPressed(sf::Keyboard::LControl))
					view.move(scroll_speed * event.mouseWheel.delta, 0);

				else 
					view.move(0, -scroll_speed * event.mouseWheel.delta);

				m_render_window.setView(view);
			}

			break;

	}
}

//========================================

void Main::generateRandomGraph()
{
	constexpr size_t nodes_min =  3;
	constexpr size_t nodes_max = 10;

	auto window_size = m_render_window.getSize();
	std::mt19937 gen(std::random_device{}());

	std::uniform_int_distribution<int> x_dist(
		config::node_default_radius, 
		window_size.x - config::node_default_radius
	);

	std::uniform_int_distribution<int> y_dist(
		config::node_default_radius, 
		window_size.y - config::node_default_radius
	);

	std::vector<Node*> nodes(
		std::uniform_int_distribution<size_t>(nodes_min, nodes_max)(gen)
	);

	for (size_t i = 0; i < nodes.size(); i++)
	{
		nodes[i] = m_object_manager += new Node;
		nodes[i]->setPosition(m_render_window.mapPixelToCoords(
			sf::Vector2i(
				x_dist(gen),
				y_dist(gen)
			)
		));
	}

	std::vector<int> indices(nodes.size());
	std::iota(indices.begin(), indices.end(), 0);

	auto edge_color = HSV(
		std::uniform_int_distribution<int>(0x00, 0xFF)(gen),
		0xFF,
		0xFF
	);

	// Connect each node to at least one another
	for (auto* node: nodes)
	{
		// Shuffle nodes indices to iterate them in random order
		std::shuffle(indices.begin(), indices.end(), gen);

		for (auto i: indices)
		{
			Node* item = nodes[i];
			if (item == node)
				continue;

			if (!node->isAdjacent(item))
			{
				Edge* edge = m_object_manager += new Edge;
				edge->setNodeA(node);
				edge->setNodeB(item);
				edge->setColor(edge_color);

				break;
			}
		}
	}

	auto random_node = [&nodes, &gen]() -> Node*
	{
		return nodes[std::uniform_int_distribution<size_t>(0, nodes.size()-1)(gen)];
	};

	// C_2_n = n! / 2*(n - 2)!
	size_t edges_max = tgamma(nodes.size() + 1) / (2 * tgamma(nodes.size() - 2 + 1));
	if (edges_max > nodes.size())
	{
		size_t additional_edge_count = std::uniform_int_distribution<size_t>(
			0, 
			edges_max - nodes.size()
		)(gen);
	
		for (size_t i = 0; i < additional_edge_count; )
		{
			Node* a = random_node();
			Node* b = random_node();
	
			if (a == b || a->isAdjacent(b))
				continue;
	
			Edge* edge = m_object_manager += new Edge;
			edge->setNodeA(a);
			edge->setNodeB(b);
			edge->setColor(edge_color);
	
			i++;
		}
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