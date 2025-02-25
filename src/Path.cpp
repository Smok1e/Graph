#include <Graph/Path.hpp>
#include <Graph/Objects/Edge.hpp>

//========================================

Path FindShortestPath(Node* src, Node* dst)
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
	for (const auto& passed_node: shortest_path)
		result.push_back(passed_node.node);

	return result;
}

//========================================