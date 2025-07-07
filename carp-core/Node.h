#pragma once

#include "Service.h"

#include <vector>

#include "Link.h"

namespace carp_core
{
	class Node : public Service
	{
	public:
		Node(int id = -1, int demand = 0) : id(id), Service(id, id, 0.0, 0.0, demand) {}

		int id;
		int sid = -1;

		std::vector<std::reference_wrapper<Link>> links;
	};

	typedef std::reference_wrapper<Node> RNode;
}
