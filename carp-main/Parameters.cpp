#include "Parameters.h"

#include <unordered_map>

#include "../carp-one/Parameters.h"
//#include "../carp-hgs/Parameters.h"

using namespace std;
using namespace carp_core;
using namespace carp_main;

bool Parameters::load(int argc, char* argv[])
{
	unordered_map<string, int> algorithms =
	{
		{ "one", Algorithm::OneIndex },
		{ "hgs", Algorithm::HGS },
		{ "none", Algorithm::None }
	};

	addChoiceOption({ "algorithm", "Algorithm for solving the problem (one - One-Index Formulation, hgs - Hybrid Genetic Search)." }, (int&)algorithm, algorithms);

	BaseParameters::load(argc, argv);
	if (algorithm == Algorithm::None) return false;

	BaseParameters& params = getParameters(algorithm);
	params.addChoiceOption({ "algorithm", "Algorithm for solving the problem (one - One-Index Formulation, hgs - Hybrid Genetic Search)." }, (int&)algorithm, algorithms);
	if (!params.load(argc, argv)) return false;

	return true;
}

BaseParameters& Parameters::getParameters(Algorithm algorithm)
{
	switch (algorithm)
	{
	case Algorithm::OneIndex: return carp_one::Parameters::getInstance();
	}
}
