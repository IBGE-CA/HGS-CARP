#pragma once

#include <vector>

class Node
{
public:
	Node(int id = -1) : id(id) {}

	int id;

	std::vector<int> edges;
};
