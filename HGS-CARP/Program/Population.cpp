#include "Population.h"

#include <iostream>

#include "Params.h"
#include "LocalSearch.h"
#include "Route.h"
#include "SeqData.h"

using namespace std;

Population::Population(Params& params) : params(params)
{
	Individual* randomIndiv;
	feasible.nbIndiv = 0;
	unfeasible.nbIndiv = 0;
	double temp, temp2;
	bool feasibleFound = false;

	// Create the trainer
	trainer = new Individual(&params, true);
	delete trainer->localSearch;
	trainer->localSearch = new LocalSearch(&params, trainer); // Initialize the LS structure

	// Creating the initial populations
	for (int i = 0; i < params.mu && (!params.isSearchingFeasible || !feasibleFound); i++)
	{
		randomIndiv = new Individual(&params, true);
		education(randomIndiv);
		addIndividual(randomIndiv);
		updateValidityCounters(randomIndiv);
		if (!randomIndiv->isValide)
		{
			temp = params.penalityCapa;
			temp2 = params.penalityLength;
			params.penalityCapa *= 10;
			params.penalityLength *= 10;

			trainer->recopieIndividu(trainer, randomIndiv);
			trainer->generalSplit();
			trainer->updateLS();
			trainer->localSearch->runSearchTotal();
			trainer->updateIndiv();
			params.penalityCapa = temp;
			params.penalityLength = temp2;
			trainer->generalSplit();
			trainer->recopieIndividu(randomIndiv, trainer);
			addIndividual(randomIndiv);
		}
		if (randomIndiv->isValide) feasibleFound = true;
		delete randomIndiv;
	}

	for (int i = 0; i < 50; i++)
	{
		if (i % 2 == 0) recentLoadFeasibility.push_back(true);
		else recentLoadFeasibility.push_back(false);
		if (i % 2 == 0) recentTimeFeasibility.push_back(true);
		else recentTimeFeasibility.push_back(false);
	}

	temp = params.penalityCapa;
	temp2 = params.penalityLength;

	timeBest = 0;
}

Population::~Population()
{
	delete trainer;
}

void Population::evalExtFit(const SubPopulation& subPop)
{
	int temp;
	vector<int> classement;
	vector<double> distances;

	for (int i = 0; i < subPop.nbIndiv; i++)
	{
		classement.push_back(i);
		distances.push_back(subPop.individuals[i]->distPlusProche(params.nbCountDistMeasure));
	}

	// Ranking the individuals in terms of contribution to diversity
	for (int n = 0; n < subPop.nbIndiv; n++)
	{
		for (int i = 0; i < subPop.nbIndiv - n - 1; i++)
		{
			if (distances[classement[i]] < distances[classement[i + 1]] - 0.000001)
			{
				temp = classement[i + 1];
				classement[i + 1] = classement[i];
				classement[i] = temp;
			}
		}
	}

	// Computing the biased fitness
	for (int i = 0; i < subPop.nbIndiv; i++)
	{
		subPop.individuals[classement[i]]->divRank = (float)i / (float)(subPop.nbIndiv - 1);
		subPop.individuals[classement[i]]->fitRank = (float)classement[i] / (float)(subPop.nbIndiv - 1);
		subPop.individuals[classement[i]]->extendedFitness = subPop.individuals[classement[i]]->fitRank + ((float)1.0 - (float)params.el / (float)subPop.nbIndiv) * subPop.individuals[classement[i]]->divRank;
	}
}

int Population::addIndividual(Individual* indiv)
{
	SubPopulation& subPop = (indiv->isValide ? feasible : unfeasible);
	int k, result;

	result = placeIndividual(subPop, indiv);

	// Keeping only the survivors if the maximum size of the population has been reached
	if (result != -1 && subPop.nbIndiv > params.mu + params.lambda)
	{
		while (subPop.nbIndiv > params.mu)
		{
			k = selectRemovalCandidate(subPop);
			removeIndividual(subPop, k);
		}
	}
	return result;
}

int Population::addAllIndividuals(Population* pop)
{
	Individual* randomIndiv;
	randomIndiv = new Individual(&params, 1.0);

	for (int i = 0; i < pop->feasible.nbIndiv; i++)
	{
		randomIndiv->recopieIndividu(randomIndiv, pop->feasible.individuals[i]);
		education(randomIndiv);
		addIndividual(randomIndiv);
	}

	for (int i = 0; i < pop->unfeasible.nbIndiv; i++)
	{
		randomIndiv->recopieIndividu(randomIndiv, pop->unfeasible.individuals[i]);
		education(randomIndiv);
		addIndividual(randomIndiv);
	}

	delete randomIndiv;
	return 1;
}

void Population::updateProximity(const SubPopulation& subPop, Individual* indiv)
{
	for (int k = 0; k < subPop.nbIndiv; k++)
	{
		if (subPop.individuals[k] != indiv)
		{
			subPop.individuals[k]->addProche(indiv);
			indiv->addProche(subPop.individuals[k]);
		}
	}
}

bool Population::fitExist(const SubPopulation& subPop, Individual* indiv)
{
	int count = 0;
	double distance = indiv->solutionCost.evaluation;
	for (int i = 0; i < (int)subPop.nbIndiv; i++)
	{
		if (subPop.individuals[i]->solutionCost.evaluation >= (distance - 0.01) && subPop.individuals[i]->solutionCost.evaluation <= (distance + 0.01))
			count++;
	}
	if (count <= 1) return false;
	else return true;
}

void Population::diversify()
{
	Individual* randomIndiv;
	double temp = params.penalityCapa;
	double temp2 = params.penalityLength;

	while (feasible.nbIndiv > (int)(0.3 * (double)params.mu))
	{
		delete feasible.individuals[feasible.nbIndiv - 1];
		feasible.individuals.pop_back();
		feasible.nbIndiv--;
	}

	while (unfeasible.nbIndiv > (int)(0.3 * (double)params.mu))
	{
		delete unfeasible.individuals[unfeasible.nbIndiv - 1];
		unfeasible.individuals.pop_back();
		unfeasible.nbIndiv--;
	}

	for (int i = 0; i < params.mu; i++)
	{
		randomIndiv = new Individual(&params, true);
		education(randomIndiv);
		addIndividual(randomIndiv);
		updateValidityCounters(randomIndiv);
		if (!randomIndiv->isValide)
		{
			temp = params.penalityCapa;
			temp2 = params.penalityLength;

			params.penalityCapa *= 50;
			params.penalityLength *= 50;

			trainer->recopieIndividu(trainer, randomIndiv);
			trainer->generalSplit();
			trainer->updateLS();
			trainer->localSearch->runSearchTotal();
			trainer->updateIndiv();
			params.penalityCapa = temp;
			params.penalityLength = temp2;
			trainer->generalSplit();
			trainer->recopieIndividu(randomIndiv, trainer);
			addIndividual(randomIndiv);
		}
		delete randomIndiv;
	}
}

void Population::clear()
{
	while (feasible.nbIndiv > 0)
	{
		delete feasible.individuals[feasible.nbIndiv - 1];
		feasible.individuals.pop_back();
		feasible.nbIndiv--;
	}

	while (unfeasible.nbIndiv > 0)
	{
		delete unfeasible.individuals[unfeasible.nbIndiv - 1];
		unfeasible.individuals.pop_back();
		unfeasible.nbIndiv--;
	}
}

int Population::placeIndividual(SubPopulation& subPop, Individual* indiv)
{
	Individual* monIndiv = new Individual(&params, false);
	monIndiv->recopieIndividu(monIndiv, indiv);

	bool placed = false;
	int i = (int)subPop.individuals.size() - 1;
	subPop.individuals.push_back(monIndiv);
	while (i >= 0 && !placed)
	{
		if (subPop.individuals[i]->solutionCost.evaluation >= indiv->solutionCost.evaluation + 0.001)
		{
			subPop.individuals[i + 1] = subPop.individuals[i];
			i--;
		}
		else
		{
			subPop.individuals[i + 1] = monIndiv;
			placed = true;
			subPop.nbIndiv++;
			updateProximity(subPop, subPop.individuals[i + 1]);
			return i + 1; // success
		}
	}
	if (!placed)
	{
		subPop.individuals[0] = monIndiv;
		placed = true;
		subPop.nbIndiv++;
		updateProximity(subPop, subPop.individuals[0]);
		if (&subPop == &feasible) timeBest = clock();
		return 0; // success
	}
	throw string("erreur placeIndividual");
	return -3;
}

void Population::removeIndividual(SubPopulation& subPop, int p)
{
	Individual* partant = subPop.individuals[p];

	// Placing the individual at the end
	for (int i = p + 1; i < (int)subPop.individuals.size(); i++)
		subPop.individuals[i - 1] = subPop.individuals[i];

	// Removing it from the population
	subPop.individuals.pop_back();
	subPop.nbIndiv--;

	// Removing it from the proximity structures
	for (int i = 0; i < subPop.nbIndiv; i++)
		subPop.individuals[i]->removeProche(partant);

	delete partant;
}

void Population::validatePen(SubPopulation& subPop)
{
	// Updating Individual Evaluations
	for (int i = 0; i < subPop.nbIndiv; i++)
		subPop.individuals[i]->solutionCost.evaluation = subPop.individuals[i]->solutionCost.distance
		+ params.penalityCapa * subPop.individuals[i]->solutionCost.capacityViol
		+ params.penalityLength * subPop.individuals[i]->solutionCost.lengthViol;

	for (int i = 0; i < subPop.nbIndiv; i++)
		for (int j = 0; j < subPop.nbIndiv - i - 1; j++)
			if (subPop.individuals[j]->solutionCost.evaluation >= subPop.individuals[j + 1]->solutionCost.evaluation + 0.01)
			{
				Individual* indiv = subPop.individuals[j];
				subPop.individuals[j] = subPop.individuals[j + 1];
				subPop.individuals[j + 1] = indiv;
			}
}

Individual* Population::getIndividualByBinTourn()
{
	Individual* individu1;
	Individual* individu2;
	int place1, place2;

	// Picking the first individual in the merge of both subpopulations
	place1 = rand() % (feasible.nbIndiv + unfeasible.nbIndiv);
	if (place1 >= feasible.nbIndiv)
		individu1 = unfeasible.individuals[place1 - feasible.nbIndiv];
	else
		individu1 = feasible.individuals[place1];

	// Picking the second individual in the merge of both subpopulations
	place2 = rand() % (feasible.nbIndiv + unfeasible.nbIndiv);
	if (place2 >= feasible.nbIndiv)
		individu2 = unfeasible.individuals[place2 - feasible.nbIndiv];
	else
		individu2 = feasible.individuals[place2];

	evalExtFit(feasible);
	evalExtFit(unfeasible);

	// Keeping the best one
	if (individu1->extendedFitness < individu2->extendedFitness)
		return individu1;
	else
		return individu2;
}

Individual* Population::getIndividualByPercentage(int pourcentage)
{
	int place;
	// Picking the individual in the 25% best of the valide population, if there are individuals in this set
	if ((feasible.nbIndiv * pourcentage) / 100 != 0)
	{
		place = rand() % ((feasible.nbIndiv * pourcentage) / 100);
		return feasible.individuals[place];
	}
	// Picking the individual in the 25% best of the invalide population, if there are individuals in this set
	else if ((unfeasible.nbIndiv * pourcentage) / 100 != 0)
	{
		place = rand() % ((unfeasible.nbIndiv * pourcentage) / 100);
		return unfeasible.individuals[place];
	}
	else // If everything fails
	{
		throw string("ERROR SELECTION POURC");
		return NULL;
	}
}

Individual* Population::getBestFeasibleIndividual()
{
	if (feasible.nbIndiv != 0) return feasible.individuals[0];
	else return NULL;
}

Individual* Population::getBestInfeasibleIndividual()
{
	if (unfeasible.nbIndiv != 0) return unfeasible.individuals[0];
	else return NULL;
}

void Population::ExportBest(const string& nomFichier)
{
	vector<int> rout;
	vector<vector<vector<int>>> allRoutes;
	vector<vector<vector<pair <int, int> > > > allRoutesArcs;
	allRoutes.push_back(vector<vector<int>>());
	allRoutesArcs.push_back(vector<vector<pair<int, int> > >());
	int compteur;
	Node* noeudActuel;
	LocalSearch* loc;
	ofstream myfile;
	double temp, temp2;
	Individual* bestValide = getBestFeasibleIndividual();

	if (bestValide != NULL)
	{
		// we load the local search structure to have the full information on the routes and easily print the solution
		// we set a high penalty, so Split and LS does not have the bad idea to create an infeasible solution from the best known feasible one
		temp = params.penalityCapa;
		temp2 = params.penalityLength;
		params.penalityCapa = 100000;
		params.penalityLength = 100000;
		education(bestValide);
		loc = trainer->localSearch;
		params.penalityCapa = temp;
		params.penalityLength = temp2;

		// Little debugging tests before printing
		trainer->testPatternCorrectness();
		if (!trainer->isValide || trainer->solutionCost.lengthViol > 0.000001 || trainer->solutionCost.capacityViol > 0.000001)
			throw string("ERROR: Last individual became infeasible !!!!");

		// Opening the file to write the solution
		myfile.open(nomFichier.data());
		myfile.precision(10);
		cout.precision(10);

		// Writing the distance
		if (cli.type != Data::MM_kWRPP)
		{
			cout << "Writing the best solution : distance : " << trainer->solutionCost.distance;
			myfile << trainer->solutionCost.distance << endl;
		}
		else
		{
			cout << "Writing the best solution, maximum distance : " << bestValide->maxRoute;
			myfile << bestValide->maxRoute << endl;
		}

		// Writing the number of routes
		if (params.periodique)
		{
			cout << " | nbRoutes : " << params.nbVehiculesPerDep;
			myfile << params.nbVehiculesPerDep << endl;
		}
		else
		{
			cout << " | nbRoutes : " << trainer->nbRoutes;
			myfile << trainer->nbRoutes << endl;
		}

		cout << " | in " << nomFichier.c_str() << endl;

		// Printing the total time of the run
		// (we print the number of clock ticks to help for short runs, the user will do the proper conversion) 
		myfile << (long long)clock() << endl;

		// Printing the time to find the best solution
		// (we print the number of clock ticks to help for short runs, the user will do the proper conversion) 
		myfile << (long long)timeBest << endl;

		// Printing the routes and their content
		for (int k = 1; k <= cli.getNbDays(); k++)
		{
			compteur = 1;
			allRoutes.push_back(vector<vector<int>>());
			allRoutesArcs.push_back(vector<vector<pair<int, int> > >());
			for (int i = 0; i < params.nombreVehicules[k]; i++)
			{
				// Test if the route is empty
				if (!loc->routes[k][i].depot->next->isDepot)
				{
					// The route is not empty
					// First, we pre-process again the data structures on the route with the flag "true", which allow to track back the orientation of the visits
					loc->routes[k][i].updateRouteData(true);
					noeudActuel = loc->routes[k][i].depot->next;
					rout.clear();
					rout.push_back(loc->routes[k][i].depot->cour);
					rout.push_back(noeudActuel->cour);

					while (!noeudActuel->isDepot)
					{
						noeudActuel = noeudActuel->next;
						rout.push_back(noeudActuel->cour);
					}

					allRoutes[k].push_back(rout);
					allRoutesArcs[k].push_back(loc->routes[k][i].depot->pred->seq0_i->bestCostArcs[0][0]);

					if (loc->routes[k][i].depot->pred->seq0_i->bestCostArcs[0][0].size() != rout.size())
						throw string("Issue : mismatch between the route size and the number of arcs reported by the SeqData");

					myfile << " " << loc->routes[k][i].depot->cour; // Printing the depot
					myfile << " " << (k - 1) % params.oldNbDays + 1; // Printing the day
					myfile << " " << compteur; // Printing the index of the route
					myfile << " " << loc->routes[k][i].depot->pred->seq0_i->load; // Printing the total demand
					myfile << " " << loc->routes[k][i].depot->pred->seq0_i->evaluation(loc->routes[k][i].depot->pred->seq0_i, loc->routes[k][i].vehicle) << " "; // Printing the total cost of this route

					myfile << " " << (int)rout.size(); // Printing the number of customers in the route
					for (int j = 0; j < (int)rout.size(); j++) // Printing the visits and their orientation
					{
						if (cli.deadheadingArcs && j > 0)
						{
							// Using the predecessors matrix to reconstruct the shortest path connecting the previous service to the current one
							vector<int> temp;
							int orig = loc->routes[k][i].depot->pred->seq0_i->bestCostArcs[0][0][j - 1].second;
							int dest = loc->routes[k][i].depot->pred->seq0_i->bestCostArcs[0][0][j].first;
							int curr = dest;
							while (curr != orig)
							{
								temp.push_back(curr);
								curr = params.ar_predNodes[orig][curr];
							}

							// Printing the deadheading (travel) arcs in the shortest path, if any
							if (!temp.empty())
							{
								temp.push_back(orig);
								for (int n = (int)temp.size() - 1; n > 0; n--)
								{
									myfile << " (T ";
									myfile << temp[n] << ",";
									myfile << temp[n - 1] << ")";
								}
							}
						}

						if (rout[j] < cli.nbDepots)
							myfile << " (D ";
						else
							myfile << " (S ";
						myfile << rout[j] << ",";
						myfile << loc->routes[k][i].depot->pred->seq0_i->bestCostArcs[0][0][j].first << ",";
						myfile << loc->routes[k][i].depot->pred->seq0_i->bestCostArcs[0][0][j].second << ")";
					}
					myfile << endl;
					compteur++;
				}
			}
		}

		myfile.close();

		// Check the solution
		if (!solutionChecker(allRoutes, allRoutesArcs, trainer->solutionCost.distance, bestValide->maxRoute))
		{
			// If the solution does not pass the checker, then we erase the file (we will detect when running the script that some results are missing)
			for (int i = 0; i < 10; i++)
				cout << "INFEASIBLE SOLUTION IN CHECKER -- ERASING SOLUTION !!!" << endl;
			myfile.open(nomFichier.data(), std::ofstream::trunc);
			myfile << "" << endl;
			myfile.close();
		}

	}
	else
	{
		cout << "Impossible to find a feasible individual" << endl;
		if (cli.softConstraints)
		{
			cout << "Taking the best infeasible individual as solution since the soft constraints flag (-soft) is enabled" << endl;

			Individual* bestInvalide = getBestInfeasibleIndividual();

			education(bestInvalide);
			loc = trainer->localSearch;

			// Little debugging tests before printing
			trainer->testPatternCorrectness();

			// Opening the file to write the solution
			myfile.open(nomFichier.data());
			myfile.precision(10);
			cout.precision(10);

			// Writing the distance
			if (cli.type != Data::MM_kWRPP)
			{
				cout << "Writing the best solution : distance : " << trainer->solutionCost.distance;
				myfile << trainer->solutionCost.distance << endl;
			}
			else
			{
				cout << "Writing the best solution, maximum distance : " << bestInvalide->maxRoute;
				myfile << bestInvalide->maxRoute << endl;
			}

			// Writing the number of routes
			if (params.periodique)
			{
				cout << " | nbRoutes : " << params.nbVehiculesPerDep;
				myfile << params.nbVehiculesPerDep << endl;
			}
			else
			{
				cout << " | nbRoutes : " << trainer->nbRoutes;
				myfile << trainer->nbRoutes << endl;
			}

			cout << " | in " << nomFichier.c_str() << endl;

			// Printing the total time of the run
			// (we print the number of clock ticks to help for short runs, the user will do the proper conversion) 
			myfile << (long long)clock() << endl;

			// Printing the time to find the best solution
			// (we print the number of clock ticks to help for short runs, the user will do the proper conversion) 
			myfile << (long long)timeBest << endl;

			// Printing the routes and their content
			for (int k = 1; k <= cli.getNbDays(); k++)
			{
				compteur = 1;
				allRoutes.push_back(vector<vector<int>>());
				allRoutesArcs.push_back(vector<vector<pair<int, int> > >());
				for (int i = 0; i < params.nombreVehicules[k]; i++)
				{
					// Test if the route is empty
					if (!loc->routes[k][i].depot->next->isDepot)
					{
						// The route is not empty
						// First, we pre-process again the data structures on the route with the flag "true", which allow to track back the orientation of the visits
						loc->routes[k][i].updateRouteData(true);
						noeudActuel = loc->routes[k][i].depot->next;
						rout.clear();
						rout.push_back(loc->routes[k][i].depot->cour);
						rout.push_back(noeudActuel->cour);

						while (!noeudActuel->isDepot)
						{
							noeudActuel = noeudActuel->next;
							rout.push_back(noeudActuel->cour);
						}

						allRoutes[k].push_back(rout);
						allRoutesArcs[k].push_back(loc->routes[k][i].depot->pred->seq0_i->bestCostArcs[0][0]);

						if (loc->routes[k][i].depot->pred->seq0_i->bestCostArcs[0][0].size() != rout.size())
							throw string("Issue : mismatch between the route size and the number of arcs reported by the SeqData");

						myfile << " " << loc->routes[k][i].depot->cour; // Printing the depot
						myfile << " " << (k - 1) % params.oldNbDays + 1; // Printing the day
						myfile << " " << compteur; // Printing the index of the route
						myfile << " " << loc->routes[k][i].depot->pred->seq0_i->load; // Printing the total demand
						myfile << " " << loc->routes[k][i].depot->pred->seq0_i->bestCost00 << " "; // Printing the total cost of this route (without penalities)

						myfile << " " << (int)rout.size(); // Printing the number of customers in the route
						for (int j = 0; j < (int)rout.size(); j++) // Printing the visits and their orientation
						{
							if (cli.deadheadingArcs && j > 0)
							{
								// Using the predecessors matrix to reconstruct the shortest path connecting the previous service to the current one
								vector<int> temp;
								int orig = loc->routes[k][i].depot->pred->seq0_i->bestCostArcs[0][0][j - 1].second;
								int dest = loc->routes[k][i].depot->pred->seq0_i->bestCostArcs[0][0][j].first;
								int curr = dest;
								while (curr != orig)
								{
									temp.push_back(curr);
									curr = params.ar_predNodes[orig][curr];
								}

								// Printing the deadheading (travel) arcs in the shortest path, if any
								if (!temp.empty())
								{
									temp.push_back(orig);
									for (int n = (int)temp.size() - 1; n > 0; n--)
									{
										myfile << " (T ";
										myfile << temp[n] << ",";
										myfile << temp[n - 1] << ")";
									}
								}
							}

							if (rout[j] < cli.nbDepots)
								myfile << " (D ";
							else
								myfile << " (S ";
							myfile << rout[j] << ",";
							myfile << loc->routes[k][i].depot->pred->seq0_i->bestCostArcs[0][0][j].first << ",";
							myfile << loc->routes[k][i].depot->pred->seq0_i->bestCostArcs[0][0][j].second << ")";
						}
						myfile << endl;
						compteur++;
					}
				}
			}

			myfile.close();
		}
	}
}

bool Population::solutionChecker(const vector<vector<vector<int>>>& allRoutes, const vector<vector<vector<pair<int, int>>>>& allRoutesArcs, double expectedCost, double expectedMaxRoute)
{
	double totalCost = 0;
	double routeCost;
	double maxRouteLength = 0;
	double totalLoad;
	int cour;

	// Verify that all customers are serviced
	// For the PCARP, it will also compute the patterns of the deliveries and verify that its correct.
	// This is done in a first step.
	vector<int> currentPatterns = vector<int>(cli.nbDepots + params.nbClients);
	for (int d = 1; d <= cli.getNbDays(); d++)
	{
		for (int r = 0; r < (int)allRoutes[d].size(); r++)
		{
			for (int i = 1; i < (int)allRoutes[d][r].size() - 1; i++)
			{
				currentPatterns[allRoutes[d][r][i]] += (int)pow(2.0, cli.getNbDays() - d);
			}
		}
	}

	bool existsOneFeasiblePattern;
	for (int i = cli.nbDepots; i < cli.nbDepots + params.nbClients; i++)
	{
		existsOneFeasiblePattern = false;
		for (int p = 0; p < (int)params.clients[i].visits.size(); p++)
		{
			if (params.clients[i].visits[p].pat == currentPatterns[i] || (cli.type == Data::MDCARP && currentPatterns[i] > 0))
				existsOneFeasiblePattern = true;
		}
		if (!existsOneFeasiblePattern)
		{
			cout << "SOLUTION CHECKER: Infeasible Pattern" << endl;
			return false;
		}
	}

	// Verification of the load constraints
	for (int d = 1; d <= cli.getNbDays(); d++)
	{
		for (int r = 0; r < (int)allRoutes[d].size(); r++)
		{
			// For each route
			if (allRoutes[d][r][0] >= cli.nbDepots)
			{
				cout << "SOLUTION CHECKER: No depot at beginning of route" << endl;
				return false;
			}

			if (allRoutes[d][r][allRoutes[d][r].size() - 1] >= cli.nbDepots)
			{
				cout << "SOLUTION CHECKER: No depot at end of route" << endl;
				return false;
			}

			totalLoad = 0;
			for (int i = 0; i < (int)allRoutes[d][r].size(); i++)
			{
				cour = allRoutes[d][r][i];
				if (cli.type != Data::MDCARP)
					totalLoad += params.clients[cour].demandPatDay[currentPatterns[cour]][d];
				else
					totalLoad += params.clients[cour].demandPatDay[1][1];
			}
			if (totalLoad < 0)
			{
				cout << "SOLUTION CHECKER: Issue with the load" << endl;
				return false;
			}

			else if (totalLoad > params.ordreVehicules[d][r].vehicleCapacity + 0.0001)
			{
				cout << "SOLUTION CHECKER: Violation of load constraint" << endl;
				return false;
			}
		}
	}

	// Verification of the solution cost
	// If its the MM-kWRPP, we verify the cost of the longest route
	for (int d = 1; d <= cli.getNbDays(); d++)
	{
		for (int r = 0; r < (int)allRoutes[d].size(); r++)
		{
			// For each route, sum the distances (case of the CARP)
			routeCost = 0;

			if (!params.hasTurnPenalties) // Case without turn penalties (using distances between nodes)
			{
				for (int i = 0; i < (int)allRoutesArcs[d][r].size() - 1; i++)
					routeCost += params.ar_distanceNodes[allRoutesArcs[d][r][i].second][allRoutesArcs[d][r][i + 1].first];
			}
			else                          // Case with turn penalties (using distances in the line graph)
			{
				for (int i = 0; i < (int)allRoutesArcs[d][r].size() - 1; i++)
				{
					Arc* arc1 = params.clients[allRoutes[d][r][i]].getArc(allRoutesArcs[d][r][i].first, allRoutesArcs[d][r][i].second);
					Arc* arc2 = params.clients[allRoutes[d][r][i + 1]].getArc(allRoutesArcs[d][r][i + 1].first, allRoutesArcs[d][r][i + 1].second);
					routeCost += params.ar_distanceArcs[arc1->indexArc][arc2->indexArc];
				}
			}
			for (int i = 0; i < (int)allRoutesArcs[d][r].size(); i++)
			{
				if (allRoutesArcs[d][r][i].first == params.clients[allRoutes[d][r][i]].ar_nodesExtr0)
					routeCost += params.clients[allRoutes[d][r][i]].ar_serviceCost01;
				else
					routeCost += params.clients[allRoutes[d][r][i]].ar_serviceCost10;
			}
			if (routeCost > maxRouteLength) // Updating the maximum route cost
				maxRouteLength = routeCost;
			totalCost += routeCost; // Summing the costs
		}
	}

	if ((cli.type != Data::MM_kWRPP && totalCost != expectedCost) || (cli.type == Data::MM_kWRPP && maxRouteLength != expectedMaxRoute))
	{
		cout << "SOLUTION CHECKER: Cost is not correct" << endl;
		return false;
	}

	return true; // Success
}

void Population::ExportBKS(const string& nomFichier)
{
	double fit;
	int secondValue;
	ifstream fichier;

	fichier.open(nomFichier.c_str());
	if (fichier.is_open())
	{
		fichier >> fit;
		fichier >> secondValue;
		fichier.close();

		// Testing if the best solution is better than the BKS
		// If the problem is a classic CVRP, CARP, MDCARP which seeks to optimize the distance 
		if (cli.type != Data::PCARP && cli.type != Data::MM_kWRPP && getBestFeasibleIndividual() != NULL && getBestFeasibleIndividual()->solutionCost.evaluation < fit - 0.001)
		{
			cout << "!!! New BKS !!! : distance = " << getBestFeasibleIndividual()->solutionCost.evaluation << " " << endl;
			ExportBest(nomFichier);
		}
		// If its a PCARP, main objective is fleet size, and then distance counts
		else if (cli.type == Data::PCARP && getBestFeasibleIndividual() != NULL && (getBestFeasibleIndividual()->nbRoutes < secondValue || (getBestFeasibleIndividual()->nbRoutes == secondValue && getBestFeasibleIndividual()->solutionCost.evaluation < fit - 0.001)))
		{
			cout << "!!! New BKS !!! : fleet size = " << getBestFeasibleIndividual()->nbRoutes << " | distance = " << getBestFeasibleIndividual()->solutionCost.evaluation << " " << endl;
			ExportBest(nomFichier);
		}
		else if (cli.type == Data::MM_kWRPP && getBestFeasibleIndividual() != NULL && getBestFeasibleIndividual()->maxRoute < secondValue - 0.001)
		{
			cout << "!!! New BKS !!! : maximum route size = " << getBestFeasibleIndividual()->maxRoute << endl;
			ExportBest(nomFichier);
		}
	}
	else
	{
		cout << " No best known solution (BKS) file has been found, creating a new file " << endl;
		ExportBest(nomFichier);
	}
}

double Population::fractionOfLoadFeasibleIndividuals()
{
	int count = 0;
	for (list<bool>::iterator it = recentLoadFeasibility.begin(); it != recentLoadFeasibility.end(); ++it)
		if (*it == true) count++;

	return double(count) / 50.;
}

double Population::fractionOfTimeFeasibleIndividuals()
{
	int count = 0;
	for (list<bool>::iterator it = recentTimeFeasibility.begin(); it != recentTimeFeasibility.end(); ++it)
		if (*it == true) count++;

	return double(count) / 50.;
}

double Population::getDiversity(const SubPopulation& subPop)
{
	double total = 0;
	int count = 0;
	for (int i = 0; i < min(subPop.nbIndiv, params.mu); i++)
	{
		for (int j = i + 1; j < min(subPop.nbIndiv, params.mu); j++)
		{
			total += subPop.individuals[i]->distance(subPop.individuals[j]);
			count++;
		}
	}
	return total / (double)count;
}

double Population::getAverageCostOfFeasibleIndividuals()
{
	double moyenne = 0;
	for (int i = 0; i < min(feasible.nbIndiv, params.mu); i++)
		moyenne += feasible.individuals[i]->solutionCost.evaluation;
	return  moyenne / min(feasible.nbIndiv, params.mu);
}

double Population::getAverageCostOfInfeasibleIndividuals()
{
	double moyenne = 0;
	for (int i = 0; i < min(unfeasible.nbIndiv, params.mu); i++)
		moyenne += unfeasible.individuals[i]->solutionCost.evaluation;
	return  moyenne / min(unfeasible.nbIndiv, params.mu);
}

double Population::getAverageAgeOfFeasibleIndividuals()
{
	double ageMoyen = 0;
	for (int i = 0; i < min(feasible.nbIndiv, params.mu); i++)
		ageMoyen += feasible.individuals[i]->age;
	return  ageMoyen / min(feasible.nbIndiv, params.mu);
}

int Population::selectRemovalCandidate(const SubPopulation& subPop)
{
	// Selects one individual to be eliminated from the population
	vector<int> classement;
	int temp, sortant;

	updateAge();
	evalExtFit(subPop);

	for (int i = 0; i < subPop.nbIndiv; i++)
		classement.push_back(i);

	// Adding a penalty in case of clone (in the objective space or solution space)
	for (int i = 1; i < subPop.nbIndiv; i++)
	{
		if (subPop.individuals[i]->distPlusProche(1) <= 0.001) // in solution space
			subPop.individuals[i]->extendedFitness += 5;
		if (fitExist(subPop, subPop.individuals[i])) // in objective space
			subPop.individuals[i]->extendedFitness += 5;
	}

	// Ranking the elements per extended fitness and selecting out the worst
	for (int n = 0; n < subPop.nbIndiv; n++)
	{
		for (int i = 0; i < subPop.nbIndiv - n - 1; i++)
		{
			if (subPop.individuals[classement[i]]->extendedFitness > subPop.individuals[classement[i + 1]]->extendedFitness)
			{
				temp = classement[i + 1];
				classement[i + 1] = classement[i];
				classement[i] = temp;
			}
		}
	}

	sortant = classement[subPop.nbIndiv - 1];
	return sortant;
}

void Population::updateAge()
{
	for (int i = 0; i < feasible.nbIndiv; i++)
		feasible.individuals[i]->age++;

	for (int i = 0; i < unfeasible.nbIndiv; i++)
		unfeasible.individuals[i]->age++;
}

void Population::education(Individual* indiv)
{
	indiv->recopieIndividu(trainer, indiv);
	trainer->generalSplit();
	trainer->updateLS();
	trainer->localSearch->runSearchTotal();
	trainer->updateIndiv();
	indiv->recopieIndividu(indiv, trainer);
}

void Population::updateValidityCounters(Individual* indiv)
{
	recentLoadFeasibility.push_back(indiv->solutionCost.capacityViol < 0.001);
	recentLoadFeasibility.pop_front();
	recentTimeFeasibility.push_back(indiv->solutionCost.lengthViol < 0.001);
	recentTimeFeasibility.pop_front();
}

void Population::printStatus(int nbIter)
{
	// Some traces to observe the status of the population
	cout.precision(8);

	cout << "It " << nbIter << " | Sol ";

	if (getBestFeasibleIndividual() != NULL)
		cout << getBestFeasibleIndividual()->solutionCost.distance << " " << getBestFeasibleIndividual()->solutionCost.routes << " ";
	else
		cout << "NO-VALID ";

	if (getBestInfeasibleIndividual() != NULL)
		cout << getBestInfeasibleIndividual()->solutionCost.evaluation;
	else
		cout << "NO-INVALID";

	cout << " | Moy " << getAverageCostOfFeasibleIndividuals() << " " << getAverageCostOfInfeasibleIndividuals()
		<< " | Div " << getDiversity(feasible) << " " << getDiversity(unfeasible) << endl
		<< " | Val " << fractionOfLoadFeasibleIndividuals() << " " << fractionOfTimeFeasibleIndividuals()
		<< " | Pen " << params.penalityCapa << " " << params.penalityLength
		<< " | Pop " << feasible.nbIndiv << " " << unfeasible.nbIndiv;
	if (getBestInfeasibleIndividual() != NULL && getBestFeasibleIndividual() == NULL)
		cout << " | Feas : distance " << getBestInfeasibleIndividual()->solutionCost.distance
		<< " duration " << getBestInfeasibleIndividual()->solutionCost.lengthViol
		<< " load " << getBestInfeasibleIndividual()->solutionCost.capacityViol;
	cout << endl;
	//cout << " | Age Valides : " << getAverageAgeOfFeasibleIndividuals() << endl;
}
