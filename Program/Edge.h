#pragma once

class Edge
{
public:
	Edge(int id, int from, int to, int cost = 0, int demand = 0, int zz_cost = 0, int other = -1)
		: id(id), from(from), to(to), cost(cost), demand(demand) {}

	int id = -1;
	int from = -1;
	int to = -1;
	int cost;
	int demand;

	bool operator==(const Edge& edge) const { return ((from == edge.from) && (to == edge.to)); }
	bool operator<(const Edge& edge) const { return (cost == edge.cost) ? (id < edge.id) : (cost < edge.cost); }
	bool operator>(const Edge& edge) const { return (cost == edge.cost) ? (id > edge.id) : (cost > edge.cost); }

	bool isRequired() const { return demand > 0; }
};
