#pragma once

#include "Singleton.h"

#include <vector>
#include <limits>
#include <string>

#include "Node.h"
#include "Link.h"
#include "Service.h"

namespace carp_core
{
	class Data : public Singleton<Data>
	{
		friend class Singleton<Data>;
	public:
		enum Type
		{
			CARP,
			MDCARP,
			NEARP,
			NoType,
		};

		std::string name;

		std::vector<Node> nodes;
		std::vector<Link> links;
		std::vector<RLink> arcs;
		std::vector<RLink> edges;
		
		std::vector<RService> services;

		std::vector<int> depots;

		int vehicles = 0;
		int capacity = 0;
		double maxTime = 0.0;

		double lb = 0.0;
		double ub = std::numeric_limits<double>::infinity();

		const static double EPS;

	private:
		Data() = default;
	};
}
