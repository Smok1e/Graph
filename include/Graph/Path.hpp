#pragma once

#include <vector>

#include <Graph/Objects/Node.hpp>

//========================================

using Path = std::vector<Node*>;

Path FindShortestPath(Node* src, Node* dst);

//========================================