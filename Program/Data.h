#pragma once

#include "Singleton.h"

#include <vector>
#include <limits>
#include <string>

#include "Node.h"
#include "Edge.h"

class Data : public Singleton<Data>
{
	friend class Singleton<Data>;
public:
	enum Type
	{
		CARP,
		MDCARP,
		PCARP,
		NEARP,
		NEARP_TP,
		MM_kWRPP,
		NoType,
	};

	int capacity = 0;
	int vehicles = 0;

	double lb = 0.0;
	double ub = std::numeric_limits<double>::infinity();
	std::string name;

	std::vector<Node> nodes;
	std::vector<Edge> edges;

	const static double EPS;

private:
	Data() = default;
};
