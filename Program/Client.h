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

#pragma once

#include <vector>

// Used to store coordinates of nodes
struct Couple
{
	double x;
	double y;
};

// Used to store the list of allowable patterns for each customer
struct Pattern
{
	int pat;
	int dep;
	double cost;

	// Tests if pat2 is a possible completion of an incomplete Pattern pat1 (PCARP)
	static bool isSubset(Pattern pat1, Pattern pat2)
	{
		if (pat2.dep != pat1.dep && pat1.dep != -1)
			return false;

		int code1 = pat1.pat;
		int code2 = pat2.pat;

		while (code1 != 0)
		{
			if (code1 % 2 > code2 % 2)
				return false;
			code1 = code1 / 2;
			code2 = code2 / 2;
		}
		return true;
	}
};

// used only for NEARP with turn penalties
struct Arc
{
	int indexArc;
	int nodeBegin;
	int nodeEnd;
	double cost;
	int nb_Turns;
};

// different clients types
enum ClientType { AR_DEPOT, AR_CLIENT_NODE, AR_CLIENT_EDGE, AR_CLIENT_ARC };

class Client
{
public:

	// customer number
	int custNum;

	// coordinates
	Couple coord;

	// duration for a service
	double serviceDuration;

	// demand of a customer
	// (demand per day in a PCARP).
	double demand;

	// how many visits per week
	int freq;

	// temporary variable used during computation of data structures for the patterns (multi-period problems)
	int codeTravail;

	// type of service
	ClientType ar_nodeType;

	// if the clients type is an edge, we store the begin and end node.
	// if its a node, the start and end are the same
	// start node is nodesExtr[0], 
	// end node is nodesExtr[1]
	int ar_nodesExtr0;
	int ar_nodesExtr1;

	// Service cost on the edge may depend on the direction
	double ar_serviceCost01;
	double ar_serviceCost10;

	// For NEARP with turn penalties, the number of modes is defined (1 for a service on an arc, 2 for an edge, more for a node in the presence of turn penalties)
	int ar_nbModes;
	std::vector<Arc*> ar_Modes;
	Arc* getArc(int i, int j); // Returns the pointer towards the arc from the information of its endpoints (brute force), or NULL if failure to find

	// list of all possible visit combinations (multi-period problems)
	// Each visit combination is coded with the decimal equivalent of
	// the corresponding binary bit string. For example, in a 5-day
	// period, the code 10 which is equivalent to the bit string 01010
	// means that a customer is visited on days 2 and 4. (Days are
	// numbered from left to right.)
	std::vector<Pattern> visits;

	// A copy of the previous structure (used to still have the original back when performing problem decompositions)
	std::vector<Pattern> visitsOrigin;

	// visitsDyn[code] returns the index of the Pattern in the list of patterns for the customer, otherwise -1 if this Pattern is not valid
	std::vector<int> visitsDyn;
	void computeVisitsDyn(int nbDays, int oldNbDays);

	// test to know if a visit Pattern is accepted
	bool testPat(int Pattern, int oldNbDays);

	// jourSuiv[code][day] tells in which next day a customer should be placed, according to already existing visits
	std::vector<std::vector<int>> jourSuiv;

	/* FOR THE PCARP */
	// In the PCARP, the quantity to pick on a day may vary depending on the choice of Pattern
	// As it corresponds to the demand accumulated during all previous days and this day itself.
	// demandPatDay[Pattern][day] returns the demand accumulated until "day"
	// It returns -1 in case of infeasibility
	std::vector<std::vector<double>> demandPatDay;

	// functions used for the pre-processing of these data structures
	void computeJourSuiv(int nbDays, int oldNbDays);
	void frec(int y, int z, int n, int oldNbDays);
	void ajoute(int y, int z, int oldNbDays);

	// For a customer, ordered list of depots by increasing distance 
	std::vector<int> ordreProximiteDepots;

	// For a customer i, list of close customers j by proximity
	std::vector<int> sommetsVoisins;

	// For a customer j, list of customers i to which j is considered as "close"
	std::vector<int> sommetsVoisinsAvant;

	Client();

	~Client(void);
};
