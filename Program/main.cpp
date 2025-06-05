#include <iostream>
#include <string>

#include "CommandLineInterface.h"
#include "Loader.h"

#include "Genetic.h"

using namespace std;

int main(int argc, char* argv[])
{
	try
	{
		CommandLineInterface cli;
		if (!cli.load(argc, argv)) return -1;

		// For the PCARP, we need to minimize fleet size as first objective, then minimize distance as second objective.
		bool minFleetSize = (cli.type == CommandLineInterface::PCARP);
		// For the MM-kWRPP, we need to minimize the length of the maximum route.
		bool minMaxTour = (cli.type == CommandLineInterface::MM_kWRPP);

		/* CLASSIC CASE OF OPTIMIZATION, BASED ON DISTANCE : for the CVRP, CARP, NEARP, MDCARP... */
		/* THIS IS THE MAIN START OF THE PROGRAM */
		if (!minFleetSize && !minMaxTour)
		{
			clock_t nb_ticks_allowed = cli.timeLimit * CLOCKS_PER_SEC;

			Loader loader;
			if (!loader.load(cli.instanceFile))
			{
				cout << "Error while loading the instance file : " << cli.instanceFile << endl;
				return -1;
			}

			// initialisation of the Parameters
			Params mesParametres(cli.instanceFile, cli.outputFile, cli.bksFile, cli.seed, cli.type, cli.timeCapacitated,
				cli.softConstraints, cli.nbVeh, cli.nbDep, false, cli.deadheadingArcs);

			// Running the algorithm
			Population population(mesParametres);
			Genetic solver(&mesParametres, &population, nb_ticks_allowed, true);

			solver.evolve(20000, 1); // First parameter (20000) controls the number of iterations without improvement before termination

			// Printing the solution
			population.ExportBest(cli.outputFile);
			population.ExportBKS(cli.bksFile);

			cout << endl;
			return 0;
		}

		/* SOME PROBLEMS CONSIDERED IN THE PAPER INVOLVE ANOTHER OBJECTIVE, such as fleet size minimization, or minimization of the maximum tour */
		/* THIS IS DONE HERE BY RUNNING ITERATIVELY THE ALGORITHM with a decreasing fleet or distance constraint */
		// fleet size minimization (minFleetSize = true) -- applying the algorithm with a decreasing fleet size, as long as a feasible solution is found
		// or minimization of the maximum tour (minMaxTour = true) -- applying the algorithm with a decreasing tour duration constraint
		else
		{
			int veh = cli.nbVeh; // start with an upper bound on the number of vehicles
			clock_t nb_ticks_allowed = cli.timeLimit * CLOCKS_PER_SEC;
			double distConstraint = 1.e30; // or with a permissive distance constraint
			bool validExist = true;

			int nbpop = 0;
			int nbOverallLoop = 0; 
			vector<Params> mesParametresTab; // vector of parameters for each subproblem
			vector<Population> populationTab; // vector of populations for each subproblem
			Population* lastPop = NULL; // pointer to the last population, used to keep the penalty values

			while (validExist) // A feasible solution has been found, we can continue to decrease (either the number of vehicles or the distance constraint, depending on the case)
			{
				// Setting the parameters of the next problem
				mesParametresTab.push_back(Params(cli.instanceFile, cli.outputFile, cli.bksFile, cli.seed, cli.type,
					cli.timeCapacitated, cli.softConstraints, veh, cli.nbDep, true, cli.deadheadingArcs));
				nbpop = (int)mesParametresTab.size();
				nbOverallLoop++; // counting the number of subproblems which have been resolved

				// For safety, to evacuate any chance of infinite loop and printout.
				// No considered instances should lead to more than 10000 overall modifications of the fleet size or distance constraint
				if (nbOverallLoop >= 10000)
					throw string("Fleet or distance minimization, too many overall loops, there must be a problem, aborting the run");

				// Setting the distance constraint (only effective for the MM-kWRPP)
				for (int v = 0; v < mesParametresTab[nbpop - 1].nbVehiculesPerDep; v++)
					mesParametresTab[nbpop - 1].ordreVehicules[1][v].maxRouteTime = distConstraint;

				if (minMaxTour && minFleetSize) throw string("This program was not designed to optimize jointly the fleet size and length of the maximum tour");

				// Keeping the current penalty values
				if (lastPop != NULL)
				{
					mesParametresTab[nbpop - 1].penalityCapa = lastPop->params.penalityCapa;
					mesParametresTab[nbpop - 1].penalityLength = lastPop->params.penalityLength;
				}

				// Constructing the new population
				populationTab.push_back(Population(mesParametresTab[nbpop - 1]));

				// Adding the individuals found in previous iterations to help the search to start
				if (nbpop >= 2) populationTab[nbpop - 1].addAllIndividus(&populationTab[nbpop - 2]);
				if (nbpop >= 3) populationTab[nbpop - 1].addAllIndividus(&populationTab[nbpop - 3]);
				// Solving
				Genetic solver(&mesParametresTab[nbpop - 1], &populationTab[nbpop - 1], nb_ticks_allowed, true);
				cout << "######### GA evolution ######### : " << "| FLEET SIZE : " << veh << " | DIST CONSTRAINT : " <<
					mesParametresTab[nbpop - 1].ordreVehicules[1][0].maxRouteTime << endl;
				solver.evolve(2000, 1);

				// Checking if we need to go to the next fleet or distance constraint value
				if (populationTab[nbpop - 1].getIndividuBestValide() != NULL)
				{
					if (minFleetSize)
						veh--; // reducing the fleet size (PCARP)
					else if (minMaxTour)
						distConstraint = populationTab[nbpop - 1].getIndividuBestValide()->maxRoute - 1; // or reducing the distance below the best current solution (MM-kWRPP)
				}
				else
					validExist = false;

				// in the case of fleet size minimization for the PCARP (type == 32), we can test to see if there is enough capacity left to service all customers (trivial lower bound on fleet size)
				// in this case, don't need to pursue the search further
				if (cli.type == CommandLineInterface::PCARP && mesParametresTab[nbpop - 1].totalDemand > veh *
					mesParametresTab[nbpop - 1].ordreVehicules[1][0].vehicleCapacity * mesParametresTab[nbpop - 1].nbDays)
				{
					cout << "Insufficient capacity -- we can stop decreasing the fleet size" << endl;
					validExist = false;
				}

				cout << "  " << endl;
			}

			// Case of the minimization of the max route length
			// At the end of the process, the search is finished, we return the solution
			if (minMaxTour)
			{
				populationTab[nbpop - 2].ExportBest(cli.outputFile);
				populationTab[nbpop - 2].ExportBKS(cli.bksFile);
			}
			else
				// Case of the minimization of the fleet size
				// We should not forget the secondary objective in the hierarchy, which is now to minimize the distance for the resulting fleet size
				// Thus, a last optimization run is done
			{
				veh++;
				cout << "######### Second phase : minimizing Distance with " << veh << " vehicles" << endl;
				Params mesParametres(cli.instanceFile, cli.outputFile, cli.bksFile, cli.seed, cli.type, cli.timeCapacitated,
					cli.softConstraints, veh, cli.nbDep, false, cli.deadheadingArcs);
				Population population(mesParametres);
				if (nbpop >= 1 && populationTab[nbpop - 1].getIndividuBestValide() != NULL)
					population.addAllIndividus(&populationTab[nbpop - 1]);
				else if (nbpop >= 2)
					population.addAllIndividus(&populationTab[nbpop - 2]);
				Genetic solver(&mesParametres, &population, nb_ticks_allowed, true);
				solver.evolve(10000, 1);

				// Returning the final solution
				population.ExportBest(cli.outputFile);
				population.ExportBKS(cli.bksFile);
			}

			cout << endl;
		}
	}
	catch (const string& e)
	{
		cout << e << endl;
		cout << endl;
		return -1;
	}

	return 0;
}
