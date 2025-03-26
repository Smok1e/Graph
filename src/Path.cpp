#include <iostream>
#include <ranges>
#include <format>
#include <numeric>

#include <Graph/Path.hpp>
#include <Graph/Objects/Edge.hpp>

//========================================

Path::Path(Path&& path) noexcept:
	m_path(std::move(path.m_path))
{
	update();
}

Path& Path::operator=(Path&& path) noexcept
{
	setIndication(false);

	m_path = std::move(path.m_path);
	setIndication(true);
	update();

	return *this;
}

Path::~Path()
{
	setIndication(false);
}

//========================================

size_t Path::getLength() const
{
	return m_length;
}

int Path::getWeight() const
{
	return m_weight;
}

std::string_view Path::getString() const
{
	return m_string;
}

Node* Path::getFirstNode() const
{
	return m_path.front().first;
}

Node* Path::getLastNode() const
{
	return m_path.back().first;
}

bool Path::contains(Node* node) const
{
	return std::ranges::find_if(
		m_path, 
		[node](const auto& pair) { 
			return pair.first == node; 
		}
	) != m_path.end();
}

bool Path::contains(Edge* edge) const
{
	return std::ranges::find_if(
		m_path, 
		[edge](const auto& pair) { 
			return pair.second == edge; 
		}
	) != m_path.end();	
}

bool Path::empty() const
{
	return m_path.empty();
}

Path::operator bool() const
{
	return !empty();
}

//========================================

Path Path::Empty()
{
	return Path();
}

Path Path::Shortest(Node* src, Node* dst)
{
	struct PassedNode
	{
		Node* node;
		size_t offset;
	};

	std::vector<PassedNode> current_path, shortest_path;
	current_path.emplace_back(src, 0);

	while (!current_path.empty())
	{
		auto& head = current_path.back();
		if (head.node == dst && (!shortest_path.size() || current_path.size() < shortest_path.size()))
			shortest_path = current_path;

		const auto& edges = head.node->getConnectedEdges();
		bool next_found = false;

		while (!next_found && head.offset < edges.size())
		{
			if (shortest_path.size() != 0 && current_path.size() >= shortest_path.size())
			{
				head.offset++;
				break;
			}

			Node* next = edges[head.offset++]->opposite(head.node);

			if (
				std::find_if(
					current_path.begin(),
					current_path.end(),
					[next](const PassedNode& passed_node) -> bool
					{
						return passed_node.node == next;
					}
				) == current_path.end()
			)
			{
				current_path.emplace_back(next, 0);
				next_found = true;
			}
		}

		if (!next_found)
			current_path.pop_back();
	}

	Path result;
	for (size_t i = 0; i < shortest_path.size(); i++)
		result.m_path.emplace_back(
			shortest_path[i].node,
			i < shortest_path.size() - 1
				? shortest_path[i].node->getConnectedEdges()[shortest_path[i].offset - 1]
				: nullptr
		);

	return result;	
}

//========================================

void Path::setIndication(bool enable)
{
	for (auto [node, edge]: m_path)
		if (edge) edge->setPathIndication(enable);
}

void Path::update()
{
	m_weight = std::accumulate(
		m_path.begin(),
		m_path.end(),
		0,
		[](int weight, const auto& pair) -> int
		{
			return pair.second
				? weight + pair.second->getWeight()
				: weight;
		}
	);

	m_length = std::accumulate(
		m_path.begin(),
		m_path.end(),
		0,
		[](size_t length, const auto& pair) -> size_t
		{
			return pair.second
				? length + 1
				: length;
		}
	);

	if (m_path.empty())
	{
		m_string = "Empty path";
		return;
	}

	setIndication(true);

	m_string.clear();
	for (auto [node, edge]: m_path)
		m_string += std::format(
			"{}{}", 
			node->getLabel(), 
			edge
				? " -> ": 
				"; "
		);
}

//========================================