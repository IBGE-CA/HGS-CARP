#include "ProcessedData.h"

#include "Data.h"

#include "FloydWarshall.h"

ProcessedData::ProcessedData() : data(Data::getBuilt())
{
	buildRequireds();
	buildDistances();
}

void ProcessedData::buildRequireds()
{
	for (Edge& edge : data.edges)
		if (edge.isRequired())
		{
			requireds.push_back(edge.id);
			requiredEdges.emplace_back(edge);
		}
}

void ProcessedData::buildDistances()
{
	distances.resize(data.nodes.size());
	for (int i = 0; i < (int)data.nodes.size(); i++)
	{
		distances[i].resize(data.nodes.size(), 0.0);
		for (int e : data.nodes[i].edges)
		{
			int other = (data.edges[e].from == i ? data.edges[e].to : data.edges[e].from);
			distances[i][other] = data.edges[e].cost;
		}
	}

	FloydWarshall<double> fw(distances);
	fw.run();
}
