#include "OneIndexSolver.h"

#include <fstream>

#include "../carp-core/Data.h"
#include "Parameters.h"
#include "ProcessedData.h"

#include "../carp-core/Node.h"
#include "../carp-core/Link.h"
#include "../carp-core/Service.h"

#include "CapacityCut.h"

//#include "Solution.h"
//
//#include "Templates.h"
#include "Timer.h"

using namespace std;
using namespace carp_core;
using namespace carp_one;
using namespace operations_research;

OneIndexSolver::OneIndexSolver(const string& solverName, bool deleteCuts) :
	solver(MPSolver::CreateSolver(solverName == "" ? params.getSolverName() : solverName))
, deleteCuts(deleteCuts)
{
	createDeadArcsVars();
	createObjectiveFunction();
}

OneIndexSolver::~OneIndexSolver()
{
	solver->Clear();
	if (deleteCuts) for (auto& cut : cuts) delete cut;
}

bool OneIndexSolver::solve()
{
	Timer timer;
	timer.start();

#ifdef DEBUG
	writeLP();
#endif

	const MPSolver::ResultStatus status = solver->Solve();

	timer.stop();

	//Solution solution;
	//solution->setTime(timer.get());

	//if (status == MPSolver::OPTIMAL || status == MPSolver::FEASIBLE)
	//{
	//	solution->setCost(objective->Value());
	//	return true;
	//}
	return false;
}

void OneIndexSolver::createDeadArcsVars()
{
	for (Link& arc : data.arcs)
		deadArcs.push_back(solver->MakeNumVar(0.0, MPSolver::infinity(), format("za({},{})", arc.from, arc.to)));
}

void OneIndexSolver::createDeadEdgesVars()
{
	for (Link& edge : data.edges)
	{
		deadArcs.push_back(solver->MakeNumVar(0.0, MPSolver::infinity(), format("ze({},{})", edge.from, edge.to)));
		deadArcs.push_back(solver->MakeNumVar(0.0, MPSolver::infinity(), format("ze({},{})", edge.to, edge.from)));
	}
}

void OneIndexSolver::createObjectiveFunction()
{
	objective = solver->MutableObjective();
	objective->SetMinimization();

	for (const Link& arc : data.arcs)
		objective->SetCoefficient(deadArcs[arc.aid], arc.costFromTo);

	for (const Link& edge : data.edges)
	{
		objective->SetCoefficient(deadArcs[2 * edge.eid], edge.costFromTo);
		objective->SetCoefficient(deadArcs[2 * edge.eid + 1], edge.costToFrom);
	}
}

//void OneIndexSolver::addCuts(const vector<CapacityCut*>& cuts)
//{
//	for (int c = 0; c < (int)cuts.size(); c++)
//		addCut(cuts[c]);
//}
//
//void OneIndexSolver::addCut(CapacityCut* cut)
//{
//	IloExpr expr(env);
//	for (int v = 0; v < (int)data.getVertices().size(); v++)
//		for (int w = v + 1; w < (int)data.getVertices().size(); w++)
//			if (cut->contains(v) != cut->contains(w))
//				expr += deadheadeds[indexes[v][w]];
//	constraints.add(IloRange(expr >= cut->getAlpha()));
//	cuts.push_back(cut);
//#ifdef DEBUG
//	stringstream ss;
//	ss << "Cut(" << constraints.getSize() - 1 << ")";
//	constraints[constraints.getSize() - 1].setName(ss.str().c_str());
//#endif
//	model.add(constraints[constraints.getSize() - 1]);
//}
//
//const vector<double>& OneIndexSolver::getValues()
//{
//	//values.resize(data.getEdges().size(), 0.0);
//	//for (int e = 0; e < (int)data.getEdges().size(); e++)
//	//	values[e] = cplex.getValue(deadheadeds[e]);
//	return values;
//}
//
//void OneIndexSolver::purgeCuts()
//{
//	//if (deadheadeds[0].getType() == IloNumVar::Int) return;
//
//	//vector<CapacityCut*> newCuts;
//	//for (int c = 0; c < (int)cuts.size(); c++)
//	//	if (cplex.getDual(constraints[c]) > EPS)
//	//		newCuts.push_back(cuts[c]);
//	//	else
//	//		delete cuts[c];
//	//cuts.clear();
//
//	////model.remove(constraints);
//	////constraints.clear();
//
//	//addCuts(newCuts);
//}
//
//void OneIndexSolver::goInt()
//{
//	model.add(IloConversion(env, deadheadeds, IloNumVar::Int));
//}
//

void OneIndexSolver::setSolverParameters()
{
	switch (params.solver)
	{
	case Parameters::Solver::Gurobi:
		solver->SetSolverSpecificParametersAsString("Method=1, Threads=1, OutputFlag=0");
		break;
	case Parameters::Solver::CPLEX:
		solver->SetSolverSpecificParametersAsString("1062=2, 1067=1, 2012=0");
		break;
	case Parameters::Solver::GLOP:
		break;
	case Parameters::Solver::GLPK:
		// Not tested.
		solver->SetSolverSpecificParametersAsString("simplex_method=dual");
		break;
	case Parameters::Solver::SCIP:
		break;
	case Parameters::Solver::COIN:
		// Not tested.
		solver->SetSolverSpecificParametersAsString("dualSimplex");
		break;
	}
}

void OneIndexSolver::writeModel()
{
	string buffer;
	solver->ExportModelAsLpFormat(false, &buffer);
	ofstream fout(format("lp/{}-one.lp", data.name), ios::out);
	fout << buffer;
	fout.close();
}
