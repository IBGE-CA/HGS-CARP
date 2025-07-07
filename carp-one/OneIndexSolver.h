#pragma once

#include <vector>

#include <ortools/linear_solver/linear_solver.h>

#include "InputUser.h"

namespace carp_one
{
	class CapacityCut;

	class OneIndexSolver : public InputUser
	{
	public:
		OneIndexSolver(const std::string& solverName = "", bool deleteCuts = true);
		~OneIndexSolver();

		bool solve();

		void addCuts(const std::vector<CapacityCut*>& cuts);
		void addCut(CapacityCut* cut);

		const std::vector<CapacityCut*>& getCuts() const { return cuts; }

		const std::vector<double>& getValues();

		void purgeCuts();
		void goInt();

	private:
		operations_research::MPSolver* solver;

		std::vector<operations_research::MPVariable*> deadArcs;
		std::vector<operations_research::MPVariable*> deadEdges;

		operations_research::MPObjective* objective;

		std::vector<operations_research::MPConstraint*> constraints;
		std::vector<operations_research::MPConstraint*> degreeCons;

		std::vector<CapacityCut*> cuts;
		std::vector<double> values;
		std::vector<double> duals;

		std::vector<std::vector<int>> indexes;

		bool deleteCuts;

		void createDeadArcsVars();
		void createDeadEdgesVars();
		void createObjectiveFunction();

		void setSolverParameters();
		
		void writeModel();
	};
}
