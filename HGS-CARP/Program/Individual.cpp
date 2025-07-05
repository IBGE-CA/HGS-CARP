/*  ---------------------------------------------------------------------- //
	Hybrid Genetic Search for Arc Routing Problems -- HGS-CARP
	Copyright (C) 2016 Thibaut VIDAL

	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program.  If not, see <http://www.gnu.org/licenses/>.
//  ---------------------------------------------------------------------- */

#include "Individual.h"

#include <iostream>

#include "Params.h"
#include "SeqData.h"
#include "LocalSearch.h"
#include "Node.h"
#include "Route.h"

using namespace std;

Individual::Individual(Params* params, bool createAllStructures) : params(params)
{
	// Initializing some structures that enable to compute the data on subsequences in the Split algorithm
	myseq = new SeqData(params);
	for (int i = 0; i < params->nbClients + cli.nbDepots + 1; i++)
		seq.push_back(new SeqData(params));

	vector<int> tempVect;
	vector<double> tempVectDbl;
	Pattern p1;
	p1.dep = 0;
	p1.pat = 0;
	localSearch = new LocalSearch();

	// Initializing the chromosome structures
	for (int i = 0; i <= cli.getNbDays(); i++)
	{
		chromT.push_back(tempVect);
		chromR.push_back(tempVect);
		for (int j = 0; j < params->nombreVehicules[i]; j++)
			chromR[i].push_back(-1);
	}

	for (int i = 0; i < params->nbClients + cli.nbDepots; i++)
		chromP.push_back(p1);

	for (int i = 0; i < cli.nbDepots + params->nbClients; i++)
	{
		successors.push_back(vector<int>());
		predecessors.push_back(vector<int>());
		for (int k = 0; k <= cli.getNbDays(); k++)
		{
			successors[i].push_back(-1);
			predecessors[i].push_back(-1);
		}
	}

	// If we wish to also create the individual, the local search and Split structures
	if (createAllStructures)
	{
		/* CREATING THE INITIAL INDIVIDUAL */
		vector<vector<int>> tempVect2;
		int maxDepots = 0;
		int temp, temp2, jj, dayCombinaison, depot;
		age = 0;
		isFitnessComputed = false;
		vector<double> charge;

		for (int i = 0; i <= cli.getNbDays(); i++)
			charge.push_back(0);

		for (int i = cli.nbDepots; i < params->nbClients + cli.nbDepots; i++)
		{
			if (params->clients[i].freq != 0)
			{
				chromP[i] = params->clients[i].visits[rand() % (int)params->clients[i].visits.size()];
				dayCombinaison = chromP[i].pat;
				depot = chromP[i].dep;
				if (chromP[i].dep == -1) cout << "error" << endl;
				for (int k = 0; k < params->oldNbDays; k++)
				{
					temp2 = dayCombinaison % 2;
					dayCombinaison = dayCombinaison / 2;
					if (temp2 == 1) chromT[params->oldNbDays - k + depot * params->oldNbDays].push_back(i);
				}
			}
		}

		// shuffling
		for (int k = 1; k <= cli.getNbDays(); k++)
		{
			for (int i = 0; i <= (int)chromT[k].size() - 1; i++)
			{
				jj = i + rand() % ((int)chromT[k].size() - i);
				temp = chromT[k][i];
				chromT[k][i] = chromT[k][jj];
				chromT[k][jj] = temp;
			}
		}

		/* CREATING THE SPLIT STRUCTURES */

		for (int k = 0; k <= cli.getNbDays(); k++)
		{
			pred.push_back(tempVect2);
			for (int i = 0; i < params->nombreVehicules[k] + 1; i++)
			{
				pred[k].push_back(tempVect);
				pred[k][i].push_back(0);
				for (int j = 0; j < (int)params->nbClients + cli.nbDepots + 1; j++)
					pred[k][i].push_back(0);
			}
		}

		vector<SolutionCost> potTemp;
		SolutionCost csol;
		csol.evaluation = 1.e30;

		for (int k = 0; k <= cli.getNbDays(); k++)
			if (params->nombreVehicules[k] > maxDepots)
				maxDepots = params->nombreVehicules[k];

		for (int i = 0; i < maxDepots + 1; i++)
		{
			potentials.push_back(potTemp);
			for (int j = 0; j < (int)params->nbClients + cli.nbDepots + 1; j++)
				potentials[i].push_back(csol);
		}
		potentials[0][0].evaluation = 0;

		for (int i = 0; i < cli.nbDepots + params->nbClients; i++)
		{
			splitArcsCosts.push_back(vector<SolutionCost>());
			for (int j = 0; j < cli.nbDepots + params->nbClients + 1; j++)
				splitArcsCosts[i].push_back(SolutionCost());
		}
	}
}

void Individual::recopieIndividu(Individual* destination, Individual* source)
{
	destination->chromT = source->chromT;
	destination->chromP = source->chromP;
	destination->chromR = source->chromR;
	destination->solutionCost.capacityViol = source->solutionCost.capacityViol;
	destination->solutionCost.evaluation = source->solutionCost.evaluation;
	destination->solutionCost.distance = source->solutionCost.distance;
	destination->solutionCost.lengthViol = source->solutionCost.lengthViol;
	destination->solutionCost.routes = source->solutionCost.routes;
	destination->isFitnessComputed = source->isFitnessComputed;
	destination->age = 0;
	destination->isValide = source->isValide;
	destination->successors = source->successors;
	destination->predecessors = source->predecessors;
	destination->nbRoutes = source->nbRoutes;
	destination->maxRoute = source->maxRoute;
	destination->potentials = source->potentials;
	destination->pred = source->pred;
	destination->toPlace.clear();
	destination->toPlace = source->toPlace;
}

void Individual::shakingSwap(int nbShak)
{
	const CommandLineInterface& cli = CommandLineInterface::getBuilt();

	// only used in the ILS
	int itShak = 0;
	while (itShak < nbShak)
	{
		// picking a random day to operate the shaking
		int day = rand() % cli.getNbDays() + 1;

		// picking two random services in this day to be swapped
		int nbCustDay = (int)chromT[day].size();
		int pos1 = rand() % nbCustDay;
		int pos2 = rand() % nbCustDay;

		int temp = chromT[day][pos1];
		chromT[day][pos1] = chromT[day][pos2];
		chromT[day][pos2] = temp;

		itShak++;
	}
}

Individual::~Individual()
{
	for (int i = 0; i < seq.size(); i++)
		delete seq[i];
	seq.clear();
	delete localSearch;
	delete myseq;
}

void Individual::generalSplit()
{
	const CommandLineInterface& cli = CommandLineInterface::getBuilt();

	solutionCost.evaluation = 0;
	solutionCost.capacityViol = 0;
	solutionCost.distance = 0;
	solutionCost.lengthViol = 0;
	solutionCost.routes = 0;

	// performing the Split for each day
	// we first try the simple split, 
	for (int k = 1; k <= cli.getNbDays(); k++)
		if (chromT[k].size() != 0 && splitSimple(k) == 0) splitLF(k); // if its not successful we run the Split with limited fleet

	// Do we have a feasible solution
	if (solutionCost.capacityViol <= 0.00000001 && solutionCost.lengthViol <= 0.00000001)
		isValide = true;
	else
		isValide = false;

	// If split was unsuccessful, we allow more capacity violations
	// Usually, the allowed capacity violation is largely sufficient, and this message would indicate a bug.
	if (solutionCost.evaluation > 1.e20)
	{
		cout << "Increasing the capacity violation limit in Split" << endl;
		params->borne *= 1.1;
		generalSplit();
	}

	// A quick test for safety (to avoid any possibility of infinite loops and printouts)
	if (params->borne >= 100.0)
		throw string("Impossible to Split, most likely a problem of the dataset, aborting the run");

	isFitnessComputed = true;
	measureSol(); // calling a post-processing function which fills all other data structures and verifies the solution cost
	computeSuccessors(); // updating the predecessor and successor structure
}

// Simple Split, does not necessarily respect the number of vehicles
int Individual::splitSimple(int k)
{
	// We will only use the line "0" of the potential and pred data structures
	myseq->initialisation(params->ordreVehicules[k][0].depotNumber, params, this, k, false);
	double cost, mydist, mytminex, myloadex;
	int j;

	// Main code of Split
	for (int i = 0; i < (int)chromT[k].size(); i++)
	{
		// Compute a route with a single visit
		seq[i]->initialisation(params->ordreVehicules[k][0].depotNumber, params, this, k, false);
		j = i;
		while (j < (int)chromT[k].size() && seq[j]->load <= params->ordreVehicules[k][0].vehicleCapacity * params->borne)
		{
			// Extend this route to the next visit
			seq[j + 1]->concatOneAfter(seq[j], chromT[k][j], this, k);
			cost = seq[0]->evaluation(seq[j + 1], myseq, &params->ordreVehicules[k][0], mydist, mytminex, myloadex); // and evaluate

			// Test if this label is better
			if (potentials[0][i].evaluation + cost < potentials[0][j + 1].evaluation)
			{
				potentials[0][j + 1].evaluation = potentials[0][i].evaluation + cost;
				potentials[0][j + 1].capacityViol = potentials[0][i].capacityViol + myloadex;
				potentials[0][j + 1].distance = potentials[0][i].distance + mydist;
				potentials[0][j + 1].lengthViol = potentials[0][i].lengthViol + mytminex;
				potentials[0][j + 1].routes = potentials[0][i].routes + 1;
				pred[k][0][j + 1] = i;
			}
			j++;

		}
	}

	// Count the number of routes and see if the soltion is OK
	j = (int)chromT[k].size();
	for (int jj = 0; jj < params->nombreVehicules[k]; jj++)
	{
		pred[k][params->nombreVehicules[k] - jj][j] = pred[k][0][j];
		j = pred[k][params->nombreVehicules[k] - jj][j];
	}

	// If we arrived to the beginning
	if (j == 0)
	{
		// We cumulate the costs
		solutionCost.capacityViol += potentials[0][chromT[k].size()].capacityViol;
		solutionCost.evaluation += potentials[0][chromT[k].size()].evaluation;
		solutionCost.distance += potentials[0][chromT[k].size()].distance;
		solutionCost.lengthViol += potentials[0][chromT[k].size()].lengthViol;
		solutionCost.routes += potentials[0][chromT[k].size()].routes;
		initPot(k); // we reinitialize the dynamic programming structures for the next use
		return 1;
	}
	else
	{
		initPot(k); // we reinitialize the dynamic programming structures for the next use
		return 0;
	}
}

// Split for problems with limited fleet
// To avoid evaluating several time ("m" times, where m is the number of vehicles) the costs of the arcs 
// (this could be time consuming for some complex VRP variants, for example problems with HOS regulations)
// We already pre-compute and store them in "splitArcsCosts"
void Individual::splitLF(int k)
{
	double cost, mydist, mytminex, myloadex;
	myseq->initialisation(params->ordreVehicules[k][0].depotNumber, params, this, k, false);

	// preprocessing arc costs
	for (int i = 0; i < (int)chromT[k].size(); i++)
	{
		seq[i]->initialisation(params->ordreVehicules[k][0].depotNumber, params, this, k, false);
		splitArcsCosts[i][i].evaluation = seq[i]->evaluation(seq[i], myseq, &params->ordreVehicules[k][0], mydist, mytminex, myloadex);
		splitArcsCosts[i][i].capacityViol = myloadex;
		splitArcsCosts[i][i].distance = mydist;
		splitArcsCosts[i][i].lengthViol = mytminex;
		for (int j = i; j < (int)chromT[k].size() && seq[j]->load <= params->ordreVehicules[k][0].vehicleCapacity * params->borne; j++)
		{
			seq[j + 1]->concatOneAfter(seq[j], chromT[k][j], this, k);
			splitArcsCosts[i][j + 1].evaluation = seq[j + 1]->evaluation(seq[j + 1], myseq, &params->ordreVehicules[k][0], mydist, mytminex, myloadex);
			splitArcsCosts[i][j + 1].capacityViol = myloadex;
			splitArcsCosts[i][j + 1].distance = mydist;
			splitArcsCosts[i][j + 1].lengthViol = mytminex;
		}
	}


	// for each vehicle
	for (int cam = 0; cam < params->nombreVehicules[k]; cam++)
	{
		int i = 0;
		// propagate all labels
		while (i < (int)chromT[k].size() && potentials[cam][i].evaluation < 1.e29)
		{
			cost = splitArcsCosts[i][i].evaluation;
			myloadex = splitArcsCosts[i][i].capacityViol;
			mydist = splitArcsCosts[i][i].distance;
			mytminex = splitArcsCosts[i][i].lengthViol;
			if (potentials[cam][i].evaluation + cost < potentials[cam + 1][i].evaluation)
			{
				potentials[cam + 1][i].evaluation = potentials[cam][i].evaluation + cost;
				potentials[cam + 1][i].capacityViol = potentials[cam][i].capacityViol + myloadex;
				potentials[cam + 1][i].distance = potentials[cam][i].distance + mydist;
				potentials[cam + 1][i].lengthViol = potentials[cam][i].lengthViol + mytminex;
				potentials[cam + 1][i].routes = potentials[cam][i].routes;
				pred[k][cam + 1][i] = i;
			}
			int j = i;
			while (j < (int)chromT[k].size() && myloadex <= params->ordreVehicules[k][cam].vehicleCapacity * (params->borne - 1.0))
			{
				cost = splitArcsCosts[i][j + 1].evaluation;
				myloadex = splitArcsCosts[i][j + 1].capacityViol;
				mydist = splitArcsCosts[i][j + 1].distance;
				mytminex = splitArcsCosts[i][j + 1].lengthViol;
				if (potentials[cam][i].evaluation + cost < potentials[cam + 1][j + 1].evaluation)
				{
					potentials[cam + 1][j + 1].evaluation = potentials[cam][i].evaluation + cost;
					potentials[cam + 1][j + 1].capacityViol = potentials[cam][i].capacityViol + myloadex;
					potentials[cam + 1][j + 1].distance = potentials[cam][i].distance + mydist;
					potentials[cam + 1][j + 1].lengthViol = potentials[cam][i].lengthViol + mytminex;
					potentials[cam + 1][j + 1].routes = potentials[cam][i].routes + 1;
					pred[k][cam + 1][j + 1] = i;
				}
				j++;
			}
			i++;
		}
	}

	// we cumulate the costs
	solutionCost.capacityViol += potentials[params->nombreVehicules[k]][chromT[k].size()].capacityViol;
	solutionCost.evaluation += potentials[params->nombreVehicules[k]][chromT[k].size()].evaluation;
	solutionCost.distance += potentials[params->nombreVehicules[k]][chromT[k].size()].distance;
	solutionCost.lengthViol += potentials[params->nombreVehicules[k]][chromT[k].size()].lengthViol;
	solutionCost.routes += potentials[params->nombreVehicules[k]][chromT[k].size()].routes;

	// and clean the dynamic programming structures
	initPot(k);
}

void Individual::measureSol()
{
	const CommandLineInterface& cli = CommandLineInterface::getBuilt();

	int j;
	nbRoutes = 0;
	maxRoute = 0;
	double myCost;
	double totalCost = 0;

	for (int kk = 1; kk <= cli.getNbDays(); kk++)
	{
		// we use the result of Split to fill the other data structures
		j = (int)chromT[kk].size();
		for (int jj = 0; jj < params->nombreVehicules[kk]; jj++)
		{
			if (j != (int)pred[kk][params->nombreVehicules[kk] - jj][j])
			{
				// Beginning and end of this route in the chromosome
				int deb = (int)pred[kk][params->nombreVehicules[kk] - jj][j];
				int end = j - 1;

				// Counting the number of routes
				nbRoutes++;

				// Constructing the data to evaluate the route
				seq[deb]->initialisation(params->ordreVehicules[kk][0].depotNumber, params, this, kk, false);
				for (int i = deb; i <= end; i++)
					seq[i + 1]->concatOneAfter(seq[i], chromT[kk][i], this, kk);
				myCost = seq[deb]->evaluation(seq[end + 1], seq[deb], &params->ordreVehicules[kk][0]);

				// Measuring this route to see if its longer than the maxRoute
				if (myCost > maxRoute)
					maxRoute = myCost;

				// And a quick verification of the solution by summing again the cost (for debugging)
				totalCost += myCost;
			}

			j = (int)pred[kk][params->nombreVehicules[kk] - jj][j];
			chromR[kk][params->nombreVehicules[kk] - jj - 1] = j;
		}
	}

	// To avoid any problem of numerical imprecision
	solutionCost.evaluation = solutionCost.distance + params->penalityCapa * solutionCost.capacityViol
		+ params->penalityLength * solutionCost.lengthViol;
}

void Individual::initPot(int day)
{
	for (int i = 0; i < params->nombreVehicules[day] + 1; i++)
		for (size_t j = 0; j <= chromT[day].size() + 1; j++)
			potentials[i][j].evaluation = 1.e30;

	potentials[0][0].evaluation = 0;
	potentials[0][0].capacityViol = 0;
	potentials[0][0].distance = 0;
	potentials[0][0].lengthViol = 0;
	potentials[0][0].routes = 0;
}

void Individual::updateLS()
{
	const CommandLineInterface& cli = CommandLineInterface::getBuilt();

	// Loading the local search structures
	// Warning, Split must have been computed before
	int i, j;
	Node* myDepot;
	Node* myDepotFin;
	Node* myClient;
	Route* myRoute;

	for (int kk = 1; kk <= cli.getNbDays(); kk++)
	{
		// we reinitialize the "ordreParcours" vector
		localSearch->ordreParcours[kk].clear();

		// we reset the "isPresent" values to false
		for (i = cli.nbDepots; i < params->nbClients + cli.nbDepots; i++)
			localSearch->clients[kk][i].isPresent = false;

		// using Split results to load the solution in the LS structure
		j = (int)chromT[kk].size();

		for (int jj = 0; jj < params->nombreVehicules[kk]; jj++)
		{
			i = (int)pred[kk][params->nombreVehicules[kk] - jj][j];

			myDepot = &localSearch->depots[kk][params->nombreVehicules[kk] - jj - 1];
			myDepotFin = &localSearch->depotsFin[kk][params->nombreVehicules[kk] - jj - 1];
			myRoute = &localSearch->routes[kk][params->nombreVehicules[kk] - jj - 1];

			myDepot->next = myDepotFin;
			myDepot->pred = myDepotFin;
			myDepotFin->next = myDepot;
			myDepotFin->pred = myDepot;

			// a single visit
			if (j == i + 1)
			{
				myClient = &localSearch->clients[kk][chromT[kk][i]];
				myClient->pred = myDepot;
				myClient->next = myDepotFin;
				myClient->route = myRoute;
				myClient->isPresent = true;
				localSearch->nodeTestedForEachRoute(myClient->cour, kk);
				myDepot->next = myClient;
				myDepotFin->pred = myClient;
				localSearch->ordreParcours[kk].push_back(myClient->cour);
			}
			else if (j > i + 1)
			{
				// at least two visits
				myClient = &localSearch->clients[kk][chromT[kk][i]];
				myClient->pred = myDepot;
				myClient->next = &localSearch->clients[kk][chromT[kk][i + 1]];
				myClient->route = myRoute;
				myClient->isPresent = true;
				localSearch->nodeTestedForEachRoute(myClient->cour, kk);
				myDepot->next = myClient;
				localSearch->ordreParcours[kk].push_back(myClient->cour);

				// information for the end of the route
				myClient = &localSearch->clients[kk][chromT[kk][j - 1]];
				myClient->pred = &localSearch->clients[kk][chromT[kk][j - 2]];
				myClient->next = myDepotFin;
				myClient->route = myRoute;
				myClient->isPresent = true;
				localSearch->nodeTestedForEachRoute(myClient->cour, kk);
				myDepotFin->pred = myClient;
				localSearch->ordreParcours[kk].push_back(myClient->cour);

				// and the middle
				for (int k = (int)i + 1; k <= j - 2; k++)
				{
					myClient = &localSearch->clients[kk][chromT[kk][k]];
					myClient->pred = &localSearch->clients[kk][chromT[kk][k - 1]];
					myClient->next = &localSearch->clients[kk][chromT[kk][k + 1]];
					myClient->route = myRoute;
					myClient->isPresent = true;
					localSearch->nodeTestedForEachRoute(myClient->cour, kk);
					localSearch->ordreParcours[kk].push_back(myClient->cour);
				}
			}
			j = i;
		}

		for (i = 0; i < params->nombreVehicules[kk]; i++)
			localSearch->routes[kk][i].updateRouteData(false);
	}

	// and we preprocess the route vide structure
	for (int day = 1; day <= cli.getNbDays(); day++)
		localSearch->setRouteVide(day);
}


void Individual::updateIndiv()
{
	const CommandLineInterface& cli = CommandLineInterface::getBuilt();

	// Now, we go through the LS structure to update the individual (its chromosomes)
	int pos;
	vector<Route*> ordreRoutes;
	Node* node;

	for (int kk = 1; kk <= cli.getNbDays(); kk++)
	{
		ordreRoutes.clear();
		for (int ii = 0; ii < params->nombreVehicules[kk]; ii++)
			ordreRoutes.push_back(&localSearch->routes[kk][ii]);

		pos = 0;
		for (int r = 0; r < (int)ordreRoutes.size(); r++)
		{
			node = ordreRoutes[r]->depot->next;
			while (!node->isDepot)
			{
				chromT[kk][pos] = node->cour;
				node = node->next;
				pos++;
			}
		}
	}

	// And we apply Split to derive all other structures
	// testPatternCorrectness(); // can be used for debugging
	generalSplit();
}

void Individual::testPatternCorrectness()
{
	const CommandLineInterface& cli = CommandLineInterface::getBuilt();

	// Little test for debugging (test that all visits are correct for each customer)
	vector<int> frequencies;
	for (int i = 0; i < params->nbClients + cli.nbDepots; i++)
		frequencies.push_back(0);

	for (int k = 1; k <= cli.getNbDays(); k++)
		for (int i = 0; i < (int)chromT[k].size(); i++)
			frequencies[chromT[k][i]]++;

	// Here a little test that verifies that all patterns in the chromP correspond to the values in the chromT
	for (int k = 1; k <= cli.getNbDays(); k++)
	{
		int realDay = (k - 1) % params->oldNbDays + 1;
		for (int i = 0; i < (int)chromT[k].size(); i++)
		{
			int calcul = chromP[chromT[k][i]].pat;
			for (int kk = 0; kk < params->oldNbDays - realDay; kk++)
				calcul = calcul / 2;
			if (calcul % 2 != 1)
				cout << "Issue, some customers are not placed in accordance to their Pattern" << endl;
		}
	}

	for (int i = cli.nbDepots; i < params->nbClients + cli.nbDepots; i++)
		if (frequencies[i] != params->clients[i].freq) throw string("Incorrect solution with missing visits");

	for (int i = cli.nbDepots; i < params->nbClients + cli.nbDepots; i++)
		if (params->clients[i].visitsDyn[chromP[i].pat] == -1) throw string("Incorrect solution with missing visits");

	for (int i = cli.nbDepots; i < params->nbClients + cli.nbDepots; i++)
		if (chromP[i].dep < 0 || chromP[i].dep >= cli.nbDepots) throw string("Incorrect solution with missing visits");
}

double Individual::distance(Individual* indiv2)
{
	const CommandLineInterface& cli = CommandLineInterface::getBuilt();

	// Hamming distance
	bool isIdentical;
	double note = 0.0;

	for (int j = cli.nbDepots; j < params->nbClients + cli.nbDepots; j++)
	{
		// For PVRP (Hamming distance based on the patterns)
		isIdentical = (chromP[j].pat == indiv2->chromP[j].pat && chromP[j].dep == indiv2->chromP[j].dep);

		// For CVRP (Hamming distance based on the predecessors/successors)
		if (!params->periodique)
		{
			for (int s = 1; s <= cli.getNbDays(); s++)
			{
				if ((successors[j][s] != indiv2->successors[j][s] || predecessors[j][s] != indiv2->predecessors[j][s])
					&& (predecessors[j][s] != indiv2->successors[j][s] || successors[j][s] != indiv2->predecessors[j][s]))
					isIdentical = false;
			}
		}

		if (!isIdentical)
			note += 1.0;
	}

	return ((double)note / (double)(2 * params->nbClients));
}

void Individual::computeSuccessors()
{
	const CommandLineInterface& cli = CommandLineInterface::getBuilt();

	int jj;
	for (int i = 0; i < cli.nbDepots + params->nbClients; i++)
		for (int k = 1; k <= cli.getNbDays(); k++)
		{
			successors[i][k] = -1;
			predecessors[i][k] = -1;
		}

	for (int k = 1; k <= cli.getNbDays(); k++)
		if (chromT[k].size() != 0)
		{
			for (int i = 0; i < (int)chromT[k].size() - 1; i++)
				successors[chromT[k][i]][k] = chromT[k][i + 1];

			for (int i = 1; i < (int)chromT[k].size(); i++)
				predecessors[chromT[k][i]][k] = chromT[k][i - 1];

			successors[chromT[k][chromT[k].size() - 1]][k] = params->ordreVehicules[k][0].depotNumber;
			predecessors[chromT[k][0]][k] = params->ordreVehicules[k][0].depotNumber;

			// arranging those which are located at the beginning or end of a route
			for (int i = 0; i < params->nombreVehicules[k]; i++)
			{
				jj = chromR[k][i];
				predecessors[chromT[k][jj]][k] = params->ordreVehicules[k][0].depotNumber;
				if (jj != 0)
					successors[chromT[k][jj - 1]][k] = params->ordreVehicules[k][0].depotNumber;
			}
		}
}

void Individual::addProche(Individual* indiv)
{
	// Adding an individual in the structure of proximity (diversity management procedures)
	list<ProxData>::iterator it;
	ProxData data;
	data.indiv = indiv;
	data.dist = distance(indiv);

	if (plusProches.empty()) plusProches.push_back(data);
	else
	{
		it = plusProches.begin();
		while (it != plusProches.end() && it->dist < data.dist)
			++it;
		plusProches.insert(it, data);
	}
}

void Individual::removeProche(Individual* indiv)
{
	// Removing an individual in the structure of proximity (diversity management procedures)
	list<ProxData>::iterator last = plusProches.end();
	for (list<ProxData>::iterator first = plusProches.begin(); first != last; )
		if (first->indiv == indiv)
			first = plusProches.erase(first);
		else
			++first;
}

double Individual::distPlusProche(int n)
{
	// Computing the average distance with the close elements (diversity management)
	double result = 0;
	double compte = 0;
	list<ProxData>::iterator it = plusProches.begin();

	for (int i = 0; i < n && it != plusProches.end(); i++)
	{
		result += it->dist;
		compte += 1.0;
		++it;
	}
	return result / compte;
}
