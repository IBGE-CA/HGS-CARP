#include "Loader.h"

#include <fstream>
#include <iostream>

#include "Data.h"

using namespace std;
using namespace carp_core;

bool Loader::load(const string& file_name)
{
	ifstream in(file_name, ifstream::in);
	if (!in.good())
	{
		cout << "File " << file_name << " not found!" << endl;
		return false;
	}

	bool ret = loadCARP(in);

	Data& data = Data::getInstance();
	data.name = file_name.substr(file_name.find_last_of('/') + 1, file_name.find_last_of('.') - file_name.find_last_of('/') - 1);

	simplifyDemands();
	loadKnwonUpperBound();

	return ret;
}

bool Loader::loadCARP(ifstream& in)
{
	Data& data = Data::getInstance();

	int n = 0, r = 0, m = 0, p = 0, d = 0;
	char colon;
	string option;
	while (in.good())
	{
		in >> option;
		if (!in.good()) break;
		if (option == "VERTICES")
		{
			in >> colon >> n;
			data.nodes.resize(n);
			for (int v = 0; v < n; v++) data.nodes[v].id = v;
		}
		else if (option == "ARISTAS_REQ") in >> colon >> r;
		else if (option == "D_ARISTAS_REQ") in >> colon >> d;
		else if (option == "ARISTAS_NOREQ") in >> colon >> m;
		else if (option == "VEHICULOS") in >> colon >> data.vehicles;
		else if (option == "CAPACIDAD") in >> colon >> data.capacity;
		else if (option == "LISTA_ARISTAS_REQ")
		{
			in >> colon;
			readRequireds(in, r + d);
		}
		else if (option == "LISTA_ARISTAS_NOREQ")
		{
			in >> colon;
			readNonRequired(in, m);
		}
		else if (option == "DEPOSITO")
		{
			in >> colon >> p;
			closeRead(p);
		}
	}

	data.maxTime = numeric_limits<double>::infinity();

	return true;
}

void Loader::readRequireds(ifstream& in, int r)
{
	string trash;
	for (int i = 0; i < r; i++)
	{
		int from, to, cost, demand;
		in >> trash >> from >> trash >> to >> trash >> trash >> cost >> trash >> demand;
		createEdge(from, to, cost, demand);
	}
}

void Loader::readNonRequired(ifstream& in, int m)
{
	string trash;
	for (int i = 0; i < m; i++)
	{
		int from, to, cost;
		in >> trash >> from >> trash >> to >> trash >> trash >> cost;
		createEdge(from, to, cost);
	}
}

void Loader::createEdge(int from, int to, int cost, int demand)
{
	if (to < from) swap(from, to);

	Data& data = Data::getInstance();

	data.links.emplace_back(Link::buildEdge((int)data.links.size(), from - 1, to - 1, cost, demand));
	Link& link = data.links.back();

	link.eid = (int)data.edges.size();
	data.edges.emplace_back(link);

	if (demand > 0)
	{
		link.sid = (int)data.services.size();
		data.services.emplace_back(link);
	}
}

void Loader::closeRead(int d)
{
	Data& data = Data::getInstance();

	if (--d != 0)
	{
		swap(data.nodes[0], data.nodes[d]);
		data.nodes[0].id = 0;
		data.nodes[d].id = d;
		for (Link& link : data.links)
		{
			if (link.from == d) link.from = 0;
			else if (link.from == 0) link.from = d;
			if (link.to == d) link.to = 0;
			else if (link.to == 0) link.to = d;

			if (link.from > link.to) swap(link.from, link.to);
		}
	}

	for (Link& link : data.links)
	{
		data.nodes[link.from].links.push_back(link);
		data.nodes[link.to].links.push_back(link);
	}
}

void Loader::simplifyDemands()
{
	Data& data = Data::getInstance();
	Service& first = data.services[0];

	int value = first.demand;
	int limit = value / 2;

	vector<pair<int, bool>> factors;
	for (int i = 2; (i <= limit) && (value > 1); i++)
		while (value % i == 0)
		{
			value /= i;
			factors.emplace_back(i, true);
		}

	for (int s = 1; s < (int)data.edges.size(); s++)
	{
		Service& service = data.services[s];

		value = service.demand;
		if (value == 0) continue;

		for (auto& factor : factors)
			if (factor.second)
				if (value % factor.first == 0)
					value /= factor.first;
				else
					factor.second = false;
	}

	value = 1;
	for (auto& factor : factors)
		if (factor.second)
			value *= factor.first;

	for (Service& service : data.services)
		service.demand /= value;
	data.capacity /= value;

	if (value > 1) cout << "Demands simplified by a factor of " << value << "." << endl;
}

void Loader::loadKnwonUpperBound()
{
	ifstream in("bks.txt", ios::in);
	if (in.fail()) return;

	Data& data = Data::getBuilt();
	while (!in.eof())
	{
		string name;
		double ub;

		in >> name >> ub;
		if (name == data.name) data.ub = ub;
	}

	in.close();
}
