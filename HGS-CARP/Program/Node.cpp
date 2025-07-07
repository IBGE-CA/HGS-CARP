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

#include "Node.h"

#include "Params.h"

Node::Node(bool isDepot, int cour, int day, bool isPresent, Node* next, Node* pred, Route* route, Params* params)
	: params(params), isDepot(isDepot), cour(cour), day(day), isPresent(isPresent), next(next), pred(pred), route(route)
{
	int ccour = cour;
	if (isDepot) ccour = 0;

	// Initialization of the coutInsertion structure
	for (int i = 0; i < (int)params->clients[ccour].visits.size(); i++)
	{
		coutInsertion.push_back(1.e30);
		placeInsertion.push_back(NULL);
	}
	place = -1;
}

Node::Node(Node const& copy)
{
	// Copy constructor
	isDepot = copy.isDepot;
	cour = copy.cour;
	place = copy.place;
	day = copy.day;
	isPresent = copy.isPresent;
	next = copy.next;
	pred = copy.pred;
	route = copy.route;
	params = copy.params;
	coutInsertion = copy.coutInsertion;
	placeInsertion = copy.placeInsertion;
	moves = copy.moves;
}

Node& Node::operator=(Node const& copy)
{
	// Copy constructor
	isDepot = copy.isDepot;
	cour = copy.cour;
	place = copy.place;
	day = copy.day;
	isPresent = copy.isPresent;
	next = copy.next;
	pred = copy.pred;
	route = copy.route;
	params = copy.params;
	coutInsertion = copy.coutInsertion;
	placeInsertion = copy.placeInsertion;
	moves = copy.moves;
	return *this;
}

void Node::setRemaining()
{
	// seq1 has exactly the same meaning than seqi_j[0], but its more convenient to use and read
	seq1 = seqi_j[0];
	seq12 = seqi_j[1];
	seq123 = seqi_j[2];
	seq21 = seqj_i[1];
	seq321 = seqj_i[2];
}
