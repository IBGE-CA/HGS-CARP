#pragma once

#include "../carp-core/Singleton.h"
#include "../carp-core/BaseParameters.h"

namespace carp_one
{
	class Parameters : public carp_core::Singleton<Parameters>, public carp_core::BaseParameters
	{
		friend class Singleton<Parameters>;
	public:
		enum Solver
		{
			Gurobi,
			CPLEX,
			GLOP,
			GLPK,
			SCIP,
			COIN,
			None,
		};

		bool load(int argc, char* argv[]);

		const std::string& getSolverName() const;

		Solver solver = Solver::Gurobi;
	
	protected:
		Parameters() {};

	private:
		std::vector<std::string> solverToStr = { "Gurobi", "CPLEX", "GLOP", "GLPK", "SCIP", "COIN", "None" };
		std::unordered_map<std::string, int> strToSolver = {
			{ "Gurobi", Solver::Gurobi },
			{ "CPLEX", Solver::CPLEX },
			{ "GLOP", Solver::GLOP },
			{ "GLPK", Solver::GLPK },
			{ "SCIP", Solver::SCIP },
			{ "COIN", Solver::COIN },
			{ "None", Solver::None },
		};
	};
}
