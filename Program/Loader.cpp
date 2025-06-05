#include "Loader.h"

#include <fstream>
#include <iostream>

#include "Data.h"
#include "CommandLineInterface.h"

using namespace std;

int Loader::depot = 0;

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

	return true;
}

void Loader::readRequireds(ifstream& in, int r)
{
	Data& data = Data::getInstance();

	int total = 0;
	string trash;
	for (int i = 0; i < r; i++)
	{
		int from, to, cost, demand, zz_cost = 0, other = -1;
		in >> trash >> from >> trash >> to >> trash >> trash >> cost >> trash >> demand;
		if (to < from) swap(from, to);
		data.edges.emplace_back(total++, from - 1, to - 1, cost, demand, zz_cost, other);
	}
}

void Loader::readNonRequired(ifstream& in, int m)
{
	Data& data = Data::getInstance();

	string trash;
	for (int i = 0; i < m; i++)
	{
		int from, to, cost;
		in >> trash >> from >> trash >> to >> trash >> trash >> cost;
		if (to < from) swap(from, to);
		data.edges.emplace_back(i, from - 1, to - 1, cost, 0);
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
		for (auto& edge : data.edges)
		{
			if (edge.from == d) edge.from = 0;
			else if (edge.from == 0) edge.from = d;
			if (edge.to == d) edge.to = 0;
			else if (edge.to == 0) edge.to = d;

			if (edge.from > edge.to) swap(edge.from, edge.to);
		}
	}

	for (auto& edge : data.edges)
	{
		data.nodes[edge.from].edges.push_back(edge.id);
		data.nodes[edge.to].edges.push_back(edge.id);
	}
}

void Loader::simplifyDemands()
{
	Data& data = Data::getInstance();
	vector<pair<int, bool>> factors;

	int e = 0;
	while (data.edges[e].demand == 0) e++;

	int value = data.edges[e].demand;
	int limit = value / 2;

	for (int i = 2; (i <= limit) && (value > 1); i++)
		while (value % i == 0)
		{
			value /= i;
			factors.emplace_back(i, true);
		}

	for (e++; e < (int)data.edges.size(); e++)
	{
		value = data.edges[e].demand;
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

	for (auto& edge : data.edges)
		edge.demand /= value;
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
