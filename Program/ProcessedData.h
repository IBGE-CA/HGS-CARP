#pragma once

#include "Singleton.h"

#include <vector>

#include "Edge.h"

class Data;

class ProcessedData : public Singleton<ProcessedData>
{
	friend class Singleton<ProcessedData>;
public:

	std::vector<int> requireds;
	std::vector<std::reference_wrapper<Edge>> requiredEdges;

	// Distance matrix. Old ar_distanceNodes.
	std::vector<std::vector<double>> distances;

	// FW predecesors. Old ar_predNodes.
	std::vector<std::vector<int>> pred;

private:
	Data& data;

	void buildRequireds();
	void buildDistances();

	ProcessedData();
};