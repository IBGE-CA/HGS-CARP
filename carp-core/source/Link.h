#pragma once

#include "Service.h"

namespace carp_core
{
	class Link : public Service
	{
	public:
		int id = -1;
		int eid = -1;
		int sid = -1;

		enum Type
		{
			Arc,
			Edge,
		} type;

		static Link buildArc(int id, int from, int to, double costFromTo, double costToFrom, int demand = 0)
		{
			return Link(id, from, to, costFromTo, costToFrom, demand, Arc);
		}
		
		static Link buildEdge(int id, int from, int to, double costFromTo, double costToFrom, int demand = 0)
		{
			return Link(id, from, to, costFromTo, costToFrom, demand, Edge);
		}

		bool isArc() const { return type == Arc; }
		bool isEdge() const { return type == Edge; }

		bool operator==(const Link& other) const { return ((from == other.from) && (to == other.to)); }

	private:
		Link(int id, int from, int to, double costFromTo, double costToFrom, int demand, Type type) :
			id(id), Service(from, to, costFromTo, costToFrom, demand), type(type) {}
	};

	typedef std::reference_wrapper<Link> RLink;
}
