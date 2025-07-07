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

#include "SeqData.h"

#include "Params.h"
#include "Individual.h"

using namespace std;

/* --------------------------------------------------------------- */
/* ROUTE EVALUATION PROCEDURES FOR CARP, PCARP, NEARP and MM-kWRPP */
/* --------------------------------------------------------------- */

void SeqData::initialisation(int Ucour, Params* mesParams, Individual* myIndiv, int day, bool isForPathTracking)
{
	params = mesParams;
	load = params->clients[Ucour].demandPatDay[myIndiv->chromP[Ucour].pat][day];
	distance = min(params->clients[Ucour].ar_serviceCost01, params->clients[Ucour].ar_serviceCost10);

	bestCost00 = 1.e20; // Cannot start and finish in the same extremity if we have a single node in the sequence
	bestCost11 = 1.e20; // Cannot start and finish in the same extremity if we have a single node in the sequence
	bestCost01 = params->clients[Ucour].ar_serviceCost01;
	bestCost10 = params->clients[Ucour].ar_serviceCost10;

	// Initializing the structure for tracking the path
	if (isForPathTracking)
	{
		bestCostArcs[0][0].clear();
		bestCostArcs[0][1].clear();
		bestCostArcs[1][0].clear();
		bestCostArcs[1][1].clear();
		bestCostArcs[0][1].push_back(pair<int, int>(params->clients[Ucour].ar_nodesExtr0, params->clients[Ucour].ar_nodesExtr1));
		bestCostArcs[1][0].push_back(pair<int, int>(params->clients[Ucour].ar_nodesExtr1, params->clients[Ucour].ar_nodesExtr0));
	}

	firstNode = Ucour;
	lastNode = Ucour;
}

void SeqData::concatOneAfter(SeqData* seq, int Vcour, Individual* myIndiv, int day)
{
	Client* lastCli = &params->clients[seq->lastNode];
	Client* vCourCli = &params->clients[Vcour];

	vector<double>& distanceNodescli0 = params->ar_distanceNodes[lastCli->ar_nodesExtr0];
	vector<double>& distanceNodescli1 = params->ar_distanceNodes[lastCli->ar_nodesExtr1];

	// All pairs shortest path pre-processing
	bestCost01 = min(seq->bestCost00 + distanceNodescli0[vCourCli->ar_nodesExtr0],
		seq->bestCost01 + distanceNodescli1[vCourCli->ar_nodesExtr0])
		+ params->clients[Vcour].ar_serviceCost01;

	bestCost11 = min(seq->bestCost10 + distanceNodescli0[vCourCli->ar_nodesExtr0],
		seq->bestCost11 + distanceNodescli1[vCourCli->ar_nodesExtr0])
		+ params->clients[Vcour].ar_serviceCost01;

	bestCost00 = min(seq->bestCost00 + distanceNodescli0[vCourCli->ar_nodesExtr1],
		seq->bestCost01 + distanceNodescli1[vCourCli->ar_nodesExtr1])
		+ params->clients[Vcour].ar_serviceCost10;

	bestCost10 = min(seq->bestCost10 + distanceNodescli0[vCourCli->ar_nodesExtr1],
		seq->bestCost11 + distanceNodescli1[vCourCli->ar_nodesExtr1])
		+ params->clients[Vcour].ar_serviceCost10;

	// This part of pre-processing is useful to compute the lower bounds
	distance = min(min(bestCost01, bestCost11), min(bestCost00, bestCost10));

	// Load pre-processing
	load = seq->load + params->clients[Vcour].demandPatDay[myIndiv->chromP[Vcour].pat][day];
	firstNode = seq->firstNode;
	lastNode = Vcour;
}

void SeqData::concatOneAfterWithPathTracking(SeqData* seq, int Vcour, Individual* myIndiv, int day)
{
	Client* lastCli = &params->clients[seq->lastNode];
	Client* vCourCli = &params->clients[Vcour];

	vector<double>& distanceNodescli0 = params->ar_distanceNodes[lastCli->ar_nodesExtr0];
	vector<double>& distanceNodescli1 = params->ar_distanceNodes[lastCli->ar_nodesExtr1];

	pair <int, int> myPair;

	// All pairs shortest path pre-processing 
	// This is hard coded to remain close to the original code (as in the original CARP this function was hard coded and only designed to report the best cost, and the path itself was computed in a little post-processing)
	// See the NEARP-TP for a more concise version
	double bestCost01a = seq->bestCost00 + distanceNodescli0[vCourCli->ar_nodesExtr0] + params->clients[Vcour].ar_serviceCost01;
	double bestCost01b = seq->bestCost01 + distanceNodescli1[vCourCli->ar_nodesExtr0] + params->clients[Vcour].ar_serviceCost01;
	if (bestCost01a < bestCost01b) { bestCost01 = bestCost01a; bestCostArcs[0][1] = seq->bestCostArcs[0][0]; }
	else { bestCost01 = bestCost01b; bestCostArcs[0][1] = seq->bestCostArcs[0][1]; }
	bestCostArcs[0][1].push_back(pair<int, int>(params->clients[Vcour].ar_nodesExtr0, params->clients[Vcour].ar_nodesExtr1)); // Tracking the best path

	double bestCost11a = seq->bestCost10 + distanceNodescli0[vCourCli->ar_nodesExtr0] + params->clients[Vcour].ar_serviceCost01;
	double bestCost11b = seq->bestCost11 + distanceNodescli1[vCourCli->ar_nodesExtr0] + params->clients[Vcour].ar_serviceCost01;
	if (bestCost11a < bestCost11b) { bestCost11 = bestCost11a; bestCostArcs[1][1] = seq->bestCostArcs[1][0]; }
	else { bestCost11 = bestCost11b; bestCostArcs[1][1] = seq->bestCostArcs[1][1]; }
	bestCostArcs[1][1].push_back(pair<int, int>(params->clients[Vcour].ar_nodesExtr0, params->clients[Vcour].ar_nodesExtr1)); // Tracking the best path

	double bestCost00a = seq->bestCost00 + distanceNodescli0[vCourCli->ar_nodesExtr1] + params->clients[Vcour].ar_serviceCost10;
	double bestCost00b = seq->bestCost01 + distanceNodescli1[vCourCli->ar_nodesExtr1] + params->clients[Vcour].ar_serviceCost10;
	if (bestCost00a < bestCost00b) { bestCost00 = bestCost00a; bestCostArcs[0][0] = seq->bestCostArcs[0][0]; }
	else { bestCost00 = bestCost00b; bestCostArcs[0][0] = seq->bestCostArcs[0][1]; }
	bestCostArcs[0][0].push_back(pair<int, int>(params->clients[Vcour].ar_nodesExtr1, params->clients[Vcour].ar_nodesExtr0)); // Tracking the best path

	double bestCost10a = seq->bestCost10 + distanceNodescli0[vCourCli->ar_nodesExtr1] + params->clients[Vcour].ar_serviceCost10;
	double bestCost10b = seq->bestCost11 + distanceNodescli1[vCourCli->ar_nodesExtr1] + params->clients[Vcour].ar_serviceCost10;
	if (bestCost10a < bestCost10b) { bestCost10 = bestCost10a; bestCostArcs[1][0] = seq->bestCostArcs[1][0]; }
	else { bestCost10 = bestCost10b; bestCostArcs[1][0] = seq->bestCostArcs[1][1]; }
	bestCostArcs[1][0].push_back(pair<int, int>(params->clients[Vcour].ar_nodesExtr1, params->clients[Vcour].ar_nodesExtr0)); // Tracking the best path

	// This part of pre-processing is useful to compute the lower bounds
	distance = min(min(bestCost01, bestCost11), min(bestCost00, bestCost10));

	// Load pre-processing
	load = seq->load + params->clients[Vcour].demandPatDay[myIndiv->chromP[Vcour].pat][day];
	firstNode = seq->firstNode;
	lastNode = Vcour;
}


void SeqData::concatOneBefore(SeqData* seq, int Vcour, Individual* myIndiv, int day)
{
	Client* firstCli = &params->clients[seq->firstNode];
	Client* vCourCli = &params->clients[Vcour];

	vector<double>& distanceNodescli0 = params->ar_distanceNodes[vCourCli->ar_nodesExtr0];
	vector<double>& distanceNodescli1 = params->ar_distanceNodes[vCourCli->ar_nodesExtr1];

	// All pairs shortest path pre-processing
	bestCost00 = params->clients[Vcour].ar_serviceCost01 +
		min(distanceNodescli1[firstCli->ar_nodesExtr0] + seq->bestCost00,
			distanceNodescli1[firstCli->ar_nodesExtr1] + seq->bestCost10);

	bestCost01 = params->clients[Vcour].ar_serviceCost01 +
		min(distanceNodescli1[firstCli->ar_nodesExtr0] + seq->bestCost01,
			distanceNodescli1[firstCli->ar_nodesExtr1] + seq->bestCost11);

	bestCost10 = params->clients[Vcour].ar_serviceCost10 +
		min(distanceNodescli0[firstCli->ar_nodesExtr0] + seq->bestCost00,
			distanceNodescli0[firstCli->ar_nodesExtr1] + seq->bestCost10);

	bestCost11 = params->clients[Vcour].ar_serviceCost10 +
		min(distanceNodescli0[firstCli->ar_nodesExtr0] + seq->bestCost01,
			distanceNodescli0[firstCli->ar_nodesExtr1] + seq->bestCost11);

	// This part of pre-processing is useful to compute the lower bounds
	distance = min(min(bestCost01, bestCost11), min(bestCost00, bestCost10));

	// Load pre-processing
	load = seq->load + params->clients[Vcour].demandPatDay[myIndiv->chromP[Vcour].pat][day];
	firstNode = Vcour;
	lastNode = seq->lastNode;
}

double SeqData::evaluation(SeqData* seq1, Vehicle* vehicle)
{
	return seq1->bestCost00
		+ max(seq1->load - vehicle->vehicleCapacity, 0.0) * params->penalityCapa
		+ max(seq1->bestCost00 - vehicle->maxRouteTime, 0.0) * params->penalityLength;
}

double SeqData::evaluation(SeqData* seq1, SeqData* seq2, Vehicle* vehicle)
{
	Client* cli1 = &params->clients[seq1->lastNode];
	Client* cli2 = &params->clients[seq2->firstNode];

	double totDistance = min(
		min(seq1->bestCost00 + params->ar_distanceNodes[cli1->ar_nodesExtr0][cli2->ar_nodesExtr0] + seq2->bestCost00,
			seq1->bestCost00 + params->ar_distanceNodes[cli1->ar_nodesExtr0][cli2->ar_nodesExtr1] + seq2->bestCost10),
		min(seq1->bestCost01 + params->ar_distanceNodes[cli1->ar_nodesExtr1][cli2->ar_nodesExtr0] + seq2->bestCost00,
			seq1->bestCost01 + params->ar_distanceNodes[cli1->ar_nodesExtr1][cli2->ar_nodesExtr1] + seq2->bestCost10));

	return totDistance
		+ max(seq1->load + seq2->load - vehicle->vehicleCapacity, 0.0) * params->penalityCapa
		+ max(totDistance - vehicle->maxRouteTime, 0.0) * params->penalityLength;
}

double SeqData::evaluation(SeqData* seq1, SeqData* seq2, Vehicle* vehicle, double& mydist, double& mytminex, double& myloadex)
{
	Client* cli1 = &params->clients[seq1->lastNode];
	Client* cli2 = &params->clients[seq2->firstNode];

	mydist = min(
		min(seq1->bestCost00 + params->ar_distanceNodes[cli1->ar_nodesExtr0][cli2->ar_nodesExtr0] + seq2->bestCost00,
			seq1->bestCost00 + params->ar_distanceNodes[cli1->ar_nodesExtr0][cli2->ar_nodesExtr1] + seq2->bestCost10),
		min(seq1->bestCost01 + params->ar_distanceNodes[cli1->ar_nodesExtr1][cli2->ar_nodesExtr0] + seq2->bestCost00,
			seq1->bestCost01 + params->ar_distanceNodes[cli1->ar_nodesExtr1][cli2->ar_nodesExtr1] + seq2->bestCost10));

	myloadex = max(seq1->load + seq2->load - vehicle->vehicleCapacity, 0.0);
	mytminex = max(mydist - vehicle->maxRouteTime, 0.0);

	return mydist + myloadex * params->penalityCapa + mytminex * params->penalityLength;
}

double SeqData::evaluation(SeqData* seq1, SeqData* seq2, SeqData* seq3, Vehicle* vehicle)
{
	Client* cli1 = &params->clients[seq1->lastNode];
	Client* cli2 = &params->clients[seq2->firstNode];
	Client* cli3 = &params->clients[seq2->lastNode];
	Client* cli4 = &params->clients[seq3->firstNode];

	vector<double>& distanceNodescli10 = params->ar_distanceNodes[cli1->ar_nodesExtr0];
	vector<double>& distanceNodescli11 = params->ar_distanceNodes[cli1->ar_nodesExtr1];

	// joining each sequence in turn
	double bestFinishWithTemp0 =
		min(min(seq1->bestCost00 + distanceNodescli10[cli2->ar_nodesExtr0] + seq2->bestCost00,
			seq1->bestCost00 + distanceNodescli10[cli2->ar_nodesExtr1] + seq2->bestCost10),
			min(seq1->bestCost01 + distanceNodescli11[cli2->ar_nodesExtr0] + seq2->bestCost00,
				seq1->bestCost01 + distanceNodescli11[cli2->ar_nodesExtr1] + seq2->bestCost10));

	double bestFinishWithTemp1 =
		min(min(seq1->bestCost00 + distanceNodescli10[cli2->ar_nodesExtr0] + seq2->bestCost01,
			seq1->bestCost00 + distanceNodescli10[cli2->ar_nodesExtr1] + seq2->bestCost11),
			min(seq1->bestCost01 + distanceNodescli11[cli2->ar_nodesExtr0] + seq2->bestCost01,
				seq1->bestCost01 + distanceNodescli11[cli2->ar_nodesExtr1] + seq2->bestCost11));

	double totDistance = min(min(bestFinishWithTemp0 + params->ar_distanceNodes[cli3->ar_nodesExtr0][cli4->ar_nodesExtr0] + seq3->bestCost00,
		bestFinishWithTemp0 + params->ar_distanceNodes[cli3->ar_nodesExtr0][cli4->ar_nodesExtr1] + seq3->bestCost10),
		min(bestFinishWithTemp1 + params->ar_distanceNodes[cli3->ar_nodesExtr1][cli4->ar_nodesExtr0] + seq3->bestCost00,
			bestFinishWithTemp1 + params->ar_distanceNodes[cli3->ar_nodesExtr1][cli4->ar_nodesExtr1] + seq3->bestCost10));

	// joining the last two sequences (it finishes with 0 necessarily).
	return totDistance
		+ max(seq1->load + seq2->load + seq3->load - vehicle->vehicleCapacity, 0.0) * params->penalityCapa
		+ max(totDistance - vehicle->maxRouteTime, 0.0) * params->penalityLength;
}

double SeqData::evaluation(SeqData* seq1, SeqData* seq2, SeqData* seq3, SeqData* seq4, Vehicle* vehicle)
{
	Client* cli1 = &params->clients[seq1->lastNode];
	Client* cli2 = &params->clients[seq2->firstNode];
	Client* cli3 = &params->clients[seq2->lastNode];
	Client* cli4 = &params->clients[seq3->firstNode];
	Client* cli5 = &params->clients[seq3->lastNode];
	Client* cli6 = &params->clients[seq4->firstNode];

	vector<double>& distanceNodescli10 = params->ar_distanceNodes[cli1->ar_nodesExtr0];
	vector<double>& distanceNodescli11 = params->ar_distanceNodes[cli1->ar_nodesExtr1];
	vector<double>& distanceNodescli30 = params->ar_distanceNodes[cli3->ar_nodesExtr0];
	vector<double>& distanceNodescli31 = params->ar_distanceNodes[cli3->ar_nodesExtr1];

	// joining each sequence in turn
	double bestFinishWithTemp0 =
		min(min(seq1->bestCost00 + distanceNodescli10[cli2->ar_nodesExtr0] + seq2->bestCost00,
			seq1->bestCost00 + distanceNodescli10[cli2->ar_nodesExtr1] + seq2->bestCost10),
			min(seq1->bestCost01 + distanceNodescli11[cli2->ar_nodesExtr0] + seq2->bestCost00,
				seq1->bestCost01 + distanceNodescli11[cli2->ar_nodesExtr1] + seq2->bestCost10));

	double bestFinishWithTemp1 =
		min(min(seq1->bestCost00 + distanceNodescli10[cli2->ar_nodesExtr0] + seq2->bestCost01,
			seq1->bestCost00 + distanceNodescli10[cli2->ar_nodesExtr1] + seq2->bestCost11),
			min(seq1->bestCost01 + distanceNodescli11[cli2->ar_nodesExtr0] + seq2->bestCost01,
				seq1->bestCost01 + distanceNodescli11[cli2->ar_nodesExtr1] + seq2->bestCost11));

	// joining each sequence in turn
	double bestFinishWithTempTemp0 =
		min(min(bestFinishWithTemp0 + distanceNodescli30[cli4->ar_nodesExtr0] + seq3->bestCost00,
			bestFinishWithTemp0 + distanceNodescli30[cli4->ar_nodesExtr1] + seq3->bestCost10),
			min(bestFinishWithTemp1 + distanceNodescli31[cli4->ar_nodesExtr0] + seq3->bestCost00,
				bestFinishWithTemp1 + distanceNodescli31[cli4->ar_nodesExtr1] + seq3->bestCost10));

	double bestFinishWithTempTemp1 =
		min(min(bestFinishWithTemp0 + distanceNodescli30[cli4->ar_nodesExtr0] + seq3->bestCost01,
			bestFinishWithTemp0 + distanceNodescli30[cli4->ar_nodesExtr1] + seq3->bestCost11),
			min(bestFinishWithTemp1 + distanceNodescli31[cli4->ar_nodesExtr0] + seq3->bestCost01,
				bestFinishWithTemp1 + distanceNodescli31[cli4->ar_nodesExtr1] + seq3->bestCost11));

	double totDistance = min(min(bestFinishWithTempTemp0 + params->ar_distanceNodes[cli5->ar_nodesExtr0][cli6->ar_nodesExtr0] + seq4->bestCost00,
		bestFinishWithTempTemp0 + params->ar_distanceNodes[cli5->ar_nodesExtr0][cli6->ar_nodesExtr1] + seq4->bestCost10),
		min(bestFinishWithTempTemp1 + params->ar_distanceNodes[cli5->ar_nodesExtr1][cli6->ar_nodesExtr0] + seq4->bestCost00,
			bestFinishWithTempTemp1 + params->ar_distanceNodes[cli5->ar_nodesExtr1][cli6->ar_nodesExtr1] + seq4->bestCost10));

	// joining the last two sequences (it finishes with 0 necessarily).
	return totDistance
		+ max(seq1->load + seq2->load + seq3->load + seq4->load - vehicle->vehicleCapacity, 0.0) * params->penalityCapa
		+ max(totDistance - vehicle->maxRouteTime, 0.0) * params->penalityLength;
}


double SeqData::evaluation(vector<SeqData*> seqs, Vehicle* vehicle)
{
	SeqData* seqbPred = seqs[0];
	SeqData* seqb;

	double loadTemp = seqbPred->load;
	double bestFinishWith0 = seqbPred->bestCost00;
	double bestFinishWith1 = seqbPred->bestCost01;
	double bestFinishWithTemp0;
	double bestFinishWithTemp1;

	int nbSeqs = (int)seqs.size();

	for (int s = 1; s < nbSeqs - 1; s++)
	{
		seqbPred = seqs[s - 1];
		seqb = seqs[s];
		Client* cli1 = &params->clients[seqbPred->lastNode];
		Client* cli2 = &params->clients[seqb->firstNode];
		loadTemp += seqb->load;

		vector<double>& distanceNodescli0 = params->ar_distanceNodes[cli1->ar_nodesExtr0];
		vector<double>& distanceNodescli1 = params->ar_distanceNodes[cli1->ar_nodesExtr1];

		// joining each sequence in turn
		bestFinishWithTemp0 =
			min(min(bestFinishWith0 + distanceNodescli0[cli2->ar_nodesExtr0] + seqb->bestCost00,
				bestFinishWith0 + distanceNodescli0[cli2->ar_nodesExtr1] + seqb->bestCost10),
				min(bestFinishWith1 + distanceNodescli1[cli2->ar_nodesExtr0] + seqb->bestCost00,
					bestFinishWith1 + distanceNodescli1[cli2->ar_nodesExtr1] + seqb->bestCost10));

		bestFinishWithTemp1 =
			min(min(bestFinishWith0 + distanceNodescli0[cli2->ar_nodesExtr0] + seqb->bestCost01,
				bestFinishWith0 + distanceNodescli0[cli2->ar_nodesExtr1] + seqb->bestCost11),
				min(bestFinishWith1 + distanceNodescli1[cli2->ar_nodesExtr0] + seqb->bestCost01,
					bestFinishWith1 + distanceNodescli1[cli2->ar_nodesExtr1] + seqb->bestCost11));

		bestFinishWith0 = bestFinishWithTemp0;
		bestFinishWith1 = bestFinishWithTemp1;
	}

	seqbPred = seqs[nbSeqs - 2];
	seqb = seqs[nbSeqs - 1];
	Client* cli1 = &params->clients[seqbPred->lastNode];
	Client* cli2 = &params->clients[seqb->firstNode];
	loadTemp += seqb->load;

	double totDistance = min(min(bestFinishWith0 + params->ar_distanceNodes[cli1->ar_nodesExtr0][cli2->ar_nodesExtr0] + seqb->bestCost00,
		bestFinishWith0 + params->ar_distanceNodes[cli1->ar_nodesExtr0][cli2->ar_nodesExtr1] + seqb->bestCost10),
		min(bestFinishWith1 + params->ar_distanceNodes[cli1->ar_nodesExtr1][cli2->ar_nodesExtr0] + seqb->bestCost00,
			bestFinishWith1 + params->ar_distanceNodes[cli1->ar_nodesExtr1][cli2->ar_nodesExtr1] + seqb->bestCost10));

	// joining the last two sequences (it finishes with 0 necessarily).
	return totDistance
		+ max(loadTemp - vehicle->vehicleCapacity, 0.0) * params->penalityCapa
		+ max(totDistance - vehicle->maxRouteTime, 0.0) * params->penalityLength;
}

// The same evaluators, but to produce lower bounds
double SeqData::evaluationLB(SeqData* seq1, Vehicle* vehicle)
{
	return seq1->distance
		+ max(seq1->load - vehicle->vehicleCapacity, 0.0) * params->penalityCapa
		+ max(seq1->distance - vehicle->maxRouteTime, 0.0) * params->penalityLength;
}

double SeqData::evaluationLB(SeqData* seq1, SeqData* seq2, Vehicle* vehicle)
{
	double totDistance = seq1->distance + seq2->distance + params->timeCost[seq1->lastNode][seq2->firstNode];

	return totDistance
		+ max(seq1->load + seq2->load - vehicle->vehicleCapacity, 0.0) * params->penalityCapa
		+ max(totDistance - vehicle->maxRouteTime, 0.0) * params->penalityLength;
}

double SeqData::evaluationLB(SeqData* seq1, SeqData* seq2, SeqData* seq3, Vehicle* vehicle)
{
	double totDistance = seq1->distance + seq2->distance + seq3->distance
		+ params->timeCost[seq1->lastNode][seq2->firstNode] + params->timeCost[seq2->lastNode][seq3->firstNode];

	return totDistance
		+ max(seq1->load + seq2->load + seq3->load - vehicle->vehicleCapacity, 0.0) * params->penalityCapa
		+ max(totDistance - vehicle->maxRouteTime, 0.0) * params->penalityLength;
}

double SeqData::evaluationLB(SeqData* seq1, SeqData* seq2, SeqData* seq3, SeqData* seq4, Vehicle* vehicle)
{
	double totDistance = seq1->distance + seq2->distance + seq3->distance + seq4->distance + params->timeCost[seq1->lastNode][seq2->firstNode]
		+ params->timeCost[seq2->lastNode][seq3->firstNode]
		+ params->timeCost[seq3->lastNode][seq4->firstNode];

	return totDistance
		+ max(seq1->load + seq2->load + seq3->load + seq4->load - vehicle->vehicleCapacity, 0.0) * params->penalityCapa
		+ max(totDistance - vehicle->maxRouteTime, 0.0) * params->penalityLength;
}

double SeqData::evaluationLB(vector<SeqData*> seqs, Vehicle* vehicle)
{
	SeqData* seqbPred = seqs[0];
	SeqData* seqb = seqs[1];
	double loadTemp = seqbPred->load;
	double distanceTemp = seqbPred->distance;

	for (int i = 1; i < (int)seqs.size(); i++)
	{
		seqbPred = seqs[i - 1];
		seqb = seqs[i];
		distanceTemp += params->timeCost[seqbPred->lastNode][seqb->firstNode] + seqb->distance;
		loadTemp += seqb->load;
	}
	return distanceTemp
		+ max(loadTemp - vehicle->vehicleCapacity, 0.0) * params->penalityCapa
		+ max(distanceTemp - vehicle->maxRouteTime, 0.0) * params->penalityLength;
}

SeqData::SeqData(Params* params)
{
	bestCostArcs = vector<vector<vector<pair<int, int> > > >(2);
	bestCostArcs[0] = vector<vector<pair<int, int> > >(2);
	bestCostArcs[1] = vector<vector<pair<int, int> > >(2);
	this->params = params;
	firstNode = -1;
	lastNode = -1;
}

SeqData::SeqData()
{
	bestCostArcs = vector<vector<vector<pair<int, int> > > >(2);
	bestCostArcs[0] = vector<vector<pair<int, int> > >(2);
	bestCostArcs[1] = vector<vector<pair<int, int> > >(2);
	firstNode = -1;
	lastNode = -1;
}
