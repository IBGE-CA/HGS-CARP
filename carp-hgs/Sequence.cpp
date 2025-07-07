#include "Sequence.h"

#include "../carp-core/Data.h"
#include "../carp-core/BaseProcessedData.h"
#include "../carp-core/BaseParameters.h"

using namespace std;
using namespace carp_core;
using namespace carp_hgs;

void Sequence::init(int current, Individual& individual, bool isForPathTracking)
{
	const Service& service = data.services[current];

	load = service.demand;
	cost = min(service.costFromTo, service.costToFrom);

	// Cannot start and finish in the same extremity if we have a single node in the sequence
	bestCost[FromFrom] = numeric_limits<double>::infinity();
	bestCost[ToTo] = numeric_limits<double>::infinity();
	bestCost[FromTo] = service.costFromTo;
	bestCost[ToFrom] = service.costToFrom;

	// Initializing the structure for tracking the path
	if (isForPathTracking)
	{
		for (int m = 0; m < ModesSize; m++)
			bestCostArcs[m].clear();

		bestCostArcs[FromTo].push_back(pair<int, int>(service.from, service.to));
		bestCostArcs[ToFrom].push_back(pair<int, int>(service.to, service.from));
	}

	first = current;
	last = current;
}

void Sequence::concatOneAfter(const Sequence& sequence, int current)
{
	const Service& lastService = data.services[sequence.last];
	const Service& service = data.services[current];

	auto& distancesFrom = pd.distances[lastService.from];
	auto& distancesTo = pd.distances[lastService.to];

	bestCost[FromTo] = min(sequence.bestCost[FromFrom] + distancesFrom[service.from],
		sequence.bestCost[FromTo] + distancesTo[service.from]) + service.costFromTo;

	bestCost[ToTo] = min(sequence.bestCost[ToFrom] + distancesFrom[service.from],
		sequence.bestCost[ToTo] + distancesTo[service.from]) + service.costFromTo;

	bestCost[FromFrom] = min(sequence.bestCost[FromFrom] + distancesFrom[service.to],
		sequence.bestCost[FromTo] + distancesTo[service.to]) + service.costToFrom;

	bestCost[ToFrom] = min(sequence.bestCost[ToFrom] + distancesFrom[service.to],
		sequence.bestCost[ToTo] + distancesTo[service.to]) + service.costToFrom;

	cost = min(min(bestCost[FromTo], bestCost[ToTo]), min(bestCost[FromFrom], bestCost[ToFrom]));
	load = sequence.load + service.demand;

	first = sequence.first;
	last = current;
}

void Sequence::concatOneAfterWithPathTracking(const Sequence& sequence, int current)
{
	const Service& lastService = data.services[sequence.last];
	const Service& service = data.services[current];

	auto& distancesFrom = pd.distances[lastService.from];
	auto& distancesTo = pd.distances[lastService.to];
	
	double bestCostFromToA = sequence.bestCost[FromFrom] + distancesFrom[service.from] + service.costFromTo;
	double bestCostFromToB = sequence.bestCost[FromTo] + distancesTo[service.from] + service.costFromTo;
	if (bestCostFromToA < bestCostFromToB)
	{
		bestCost[FromTo] = bestCostFromToA;
		bestCostArcs[FromTo] = sequence.bestCostArcs[FromFrom];
	}
	else
	{
		bestCost[FromTo] = bestCostFromToB;
		bestCostArcs[FromTo] = sequence.bestCostArcs[FromTo];
	}
	bestCostArcs[FromTo].push_back(pair<int, int>(service.from, service.to));

	double bestCostToToA = sequence.bestCost[ToFrom] + distancesFrom[service.from] + service.costFromTo;
	double bestCostToToB = sequence.bestCost[ToTo] + distancesTo[service.from] + service.costFromTo;
	if (bestCostToToA < bestCostToToB)
	{
		bestCost[ToTo] = bestCostToToA;
		bestCostArcs[ToTo] = sequence.bestCostArcs[ToFrom];
	}
	else
	{
		bestCost[ToTo] = bestCostToToB;
		bestCostArcs[ToTo] = sequence.bestCostArcs[ToTo];
	}
	bestCostArcs[ToTo].push_back(pair<int, int>(service.from, service.to));

	double bestCostFromFromA = sequence.bestCost[FromFrom] + distancesFrom[service.to] + service.costToFrom;
	double bestCostFromFromB = sequence.bestCost[FromTo] + distancesTo[service.to] + service.costToFrom;
	if (bestCostFromFromA < bestCostFromFromB)
	{
		bestCost[FromFrom] = bestCostFromFromA;
		bestCostArcs[FromFrom] = sequence.bestCostArcs[FromFrom];
	}
	else
	{
		bestCost[FromFrom] = bestCostFromFromB;
		bestCostArcs[FromFrom] = sequence.bestCostArcs[FromTo];
	}
	bestCostArcs[FromFrom].push_back(pair<int, int>(service.to, service.from));

	double bestCostToFromA = sequence.bestCost[ToFrom] + distancesFrom[service.to] + service.costToFrom;
	double bestCostToFromB = sequence.bestCost[ToTo] + distancesTo[service.to] + service.costToFrom;
	if (bestCostToFromA < bestCostToFromB)
	{
		bestCost[ToFrom] = bestCostToFromA;
		bestCostArcs[ToFrom] = sequence.bestCostArcs[ToFrom];
	}
	else
	{
		bestCost[ToFrom] = bestCostToFromB;
		bestCostArcs[ToFrom] = sequence.bestCostArcs[ToTo];
	}
	bestCostArcs[ToFrom].push_back(pair<int, int>(service.to, service.from));

	cost = min(min(bestCost[FromTo], bestCost[ToTo]), min(bestCost[FromFrom], bestCost[ToFrom]));
	load = sequence.load + service.demand;
	
	first = sequence.first;
	last = current;
}

void Sequence::concatOneBefore(const Sequence& sequence, int current)
{
	const Service& firstService = data.services[sequence.first];
	const Service& service = data.services[current];

	auto& distancesFrom = pd.distances[service.from];
	auto& distancesTo = pd.distances[service.to];

	bestCost[FromFrom] = min(sequence.bestCost[FromFrom] + distancesTo[firstService.from],
		sequence.bestCost[ToFrom] + distancesTo[firstService.to]) + service.costFromTo;
	
	bestCost[FromTo] = min(sequence.bestCost[FromTo] + distancesTo[firstService.from],
		sequence.bestCost[ToTo] + distancesTo[firstService.to]) + service.costFromTo;

	bestCost[ToFrom] = min(sequence.bestCost[FromFrom] + distancesFrom[firstService.from],
		sequence.bestCost[ToFrom] + distancesFrom[firstService.to]) + service.costToFrom;
	
	bestCost[ToTo] = min(sequence.bestCost[FromTo] + distancesFrom[firstService.from],
		sequence.bestCost[ToTo] + distancesFrom[firstService.to]) + service.costToFrom;
	
	cost = min(min(bestCost[FromTo], bestCost[ToTo]), min(bestCost[FromFrom], bestCost[ToFrom]));
	load = sequence.load + service.demand;
	
	first = current;
	last = sequence.last;
}

double Sequence::evaluate(const Sequence& sequence) const
{
	return sequence.bestCost[FromFrom] +
		max(sequence.load - data.capacity, 0) * cli.penaltyCapacity +
		max(sequence.bestCost[FromFrom] - data.maxTime, 0.0) * cli.penaltyTime;
}

double Sequence::evaluate(const Sequence& sequence1, const Sequence& sequence2) const
{
	const Service& service1 = data.services[sequence1.last];
	const Service& service2 = data.services[sequence2.first];

	auto& distancesFrom = pd.distances[service1.from];
	auto& distancesTo = pd.distances[service1.to];

	double cost = min(
		min(sequence1.bestCost[FromFrom] + distancesFrom[service2.from] + sequence2.bestCost[FromFrom],
			sequence1.bestCost[FromFrom] + distancesFrom[service2.to] + sequence2.bestCost[ToFrom]),
		min(sequence1.bestCost[FromTo] + distancesTo[service2.from] + sequence2.bestCost[FromFrom],
			sequence1.bestCost[FromTo] + distancesTo[service2.to] + sequence2.bestCost[ToFrom])
	);

	return cost +
		max(sequence1.load + sequence2.load - data.capacity, 0) * cli.penaltyCapacity +
		max(cost - data.maxTime, 0.0) * cli.penaltyTime;
}

double Sequence::evaluate(const Sequence& sequence1, const Sequence& sequence2, double& cost, double& time, double& load) const
{
	const Service& service1 = data.services[sequence1.last];
	const Service& service2 = data.services[sequence2.first];

	auto& distancesFrom = pd.distances[service1.from];
	auto& distancesTo = pd.distances[service1.to];

	cost = min(
		min(sequence1.bestCost[FromFrom] + distancesFrom[service2.from] + sequence2.bestCost[FromFrom],
			sequence1.bestCost[FromFrom] + distancesFrom[service2.to] + sequence2.bestCost[ToFrom]),
		min(sequence1.bestCost[FromTo] + distancesTo[service2.from] + sequence2.bestCost[FromFrom],
			sequence1.bestCost[FromTo] + distancesTo[service2.to] + sequence2.bestCost[ToFrom])
	);

	load = max(sequence1.load + sequence2.load - data.capacity, 0);
	time = max(cost - data.maxTime, 0.0);

	return cost + load * cli.penaltyCapacity + time * cli.penaltyTime;
}
