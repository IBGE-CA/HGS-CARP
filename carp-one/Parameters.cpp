#include "Parameters.h"

using namespace std;
using namespace carp_one;

bool Parameters::load(int argc, char* argv[])
{
	addChoiceOption({ "-solver", "Solver to use for solving the problem" }, (int&)solver, strToSolver);

	if (!BaseParameters::load(argc, argv)) return false;
	return true;
}

const string& Parameters::getSolverName() const
{
	return solverToStr[(int)solver];
}
