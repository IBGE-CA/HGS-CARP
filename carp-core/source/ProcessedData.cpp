#include "ProcessedData.h"

#include "Data.h"

#include "FloydWarshall.h"

using namespace std;
using namespace carp_core;

ProcessedData::ProcessedData() : data(Data::getBuilt())
{
	buildDistances();
}

void ProcessedData::buildDistances()
{
	double infinity = FloydWarshall<double>::infinity();

	distances.resize(data.nodes.size());
	for (int i = 0; i < (int)data.nodes.size(); i++)
	{
		distances[i].resize(data.nodes.size(), infinity);
		distances[i][i] = 0.0;
	}

	for (const Link& link : data.links)
	{
		distances[link.from][link.to] = link.costFromTo;
		distances[link.to][link.from] = link.costToFrom;
	}

	FloydWarshall<double> fw(distances);
	fw.run();
}
