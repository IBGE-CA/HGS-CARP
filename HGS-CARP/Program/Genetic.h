#pragma once

#include <vector>

class Population;
class Individual;
class Params;

class Genetic
{
private:

	// number of iterations without improvement (during the execution of the HGA)
	int nbIterNonProd;

	// number of iterations (during the execution of the HGA)
	int nbIter;

public:

	// allowed time
	clock_t ticks;

	// printing search traces or not
	bool traces;

	// pointer towards the population
	Population* population;

	// working individuals (for the local search and crossover)
	// to work on some solutions we first create a copy in this kind of individuals
	// because the individual used for storage in the population do not contain all search data structures
	Individual* rejeton;
	Individual* rejeton2;
	Individual* rejetonP1;
	Individual* rejetonP2;
	Individual* rejetonBestFound;
	Individual* rejetonBestFoundAll;

	// Pointer towards the parameters of the problem
	Params* params;

	// Running the algorithm until "maxIterations" total iterations have been reached, 
	// or "maxIterNonProd" consecutive iterations without improvement have been reached
	// nbRec is a parameter that says if we are in the main loop of the algorithm, or inside a decomposition phase
	void evolve(int maxIterNonProd, int nbRec);

	// sub functions to differenciate the behavior with a GA and with an ILS.
	void evolveHGA(int maxIterNonProd, int nbRec);
	void evolveILS();

	// Repairing an infeasible solution
	void reparer();

	// OX Crossover
	void crossOX();

	// PIX Crossover
	void crossPIX();

	// temporary structures used in the crossover
	std::vector<int> freqClient;

	// regular management of the penalty coefficients
	void gererPenalites();

	// Constructor
	Genetic(Params* params, Population* population, clock_t ticks, bool traces);

	~Genetic();
};
