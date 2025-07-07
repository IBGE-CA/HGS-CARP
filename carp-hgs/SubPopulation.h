#pragma once

#include <vector>

namespace carp_hgs
{
	// Structure to manage a sub-population(feasible or infeasible solutions)
	class SubPopulation
	{
		// Individuals
		std::vector<Individual*> individuals;

		// Number of Individuals
		int nbIndiv;
	};
}