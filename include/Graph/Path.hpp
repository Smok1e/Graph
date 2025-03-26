#pragma once

#include <vector>

#include <Graph/Objects/Node.hpp>

//========================================

class Path
{
public:
	Path(const Path&) = delete;
	Path(Path&& path) noexcept;
	~Path();

	Path& operator=(const Path&) = delete;
	Path& operator=(Path&& path) noexcept;

	size_t getLength() const;
	int getWeight() const;
	std::string_view getString() const;

	Node* getFirstNode() const;
	Node* getLastNode() const;

	bool contains(Node* node) const;
	bool contains(Edge* edge) const;

	bool empty() const;
	operator bool() const;

	static Path Empty();
	static Path Shortest(Node* src, Node* dst);

private:
	Path() = default;

	void update();
	void setIndication(bool enable);

	std::vector<std::pair<Node*, Edge*>> m_path {};
	std::string m_string {};
	size_t m_length { 0 };
	int m_weight { 0 };

};

//========================================