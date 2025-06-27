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

#include "Client.h"

#include <iostream>

using namespace std;

void Client::computeVisitsDyn (int nbDays, int oldNbDays) 
{
	visitsDyn.clear();

	// structure initialisations
	for (int i=0 ; i <= (int)pow((float)2,oldNbDays) ; i++)
		visitsDyn.push_back(-1);

	for (int i=0 ; i < (int)visits.size()*oldNbDays/nbDays ; i++)
		visitsDyn[visits[i].pat] = i ;
}

void Client::computeJourSuiv (int nbDays, int oldNbDays) 
{
	int day,day2,j ;
	vector<int> temp ;
	vector<int> temp2 ;
	vector<double> tempDbl = vector<double> (nbDays+1) ;
	for (int i=0 ; i < nbDays+1 ; i++) 
		tempDbl[i] = 0.;

	jourSuiv.clear();

	// initialisation  of computeJourSuiv
	temp.push_back(-1);
	for (int i=0 ; i < oldNbDays ; i++)
	{temp.push_back(0);}

	for (int i=0 ; i <= (int)pow((float)2,oldNbDays) ; i++)
	{
		jourSuiv.push_back(temp);
		demandPatDay.push_back(tempDbl);
	}

	// filling the structure with 0 or 1
	// giving computeJourSuiv[codeT][dayT] : can we add the task associated to code T on dayT 
	for (int i=0 ; i < (int)visits.size() ; i++)
	{
		codeTravail = visits[i].pat ;
		frec ((int)codeTravail,(int)0,(int)pow((float)2,oldNbDays),oldNbDays) ;
	}

	// we replace the 0 and 1 by the data on which is the next day where we can add this task
	for (int k=0 ; k <= (int)pow((float)2,oldNbDays) ; k++)
	{
		if (jourSuiv[k][0] != -1)
		{
			// we go from right to left to find the first non-0 element
			day = 0 ;
			j = oldNbDays ;
			while ( day == 0 )
			{
				if (jourSuiv[k][j] == 1) 
				{ 
					day = j ;
					jourSuiv[k][j] = day ;
				}
				j-- ;
			}

			day2 = day ;
			// day2 targets the last acceptable day
			while ( j>= 1 )
			{
				if (jourSuiv[k][j] == 1) 
				{ 
					day = j ;
				}
				jourSuiv[k][j] = day ;
				j-- ;
			}

			for ( j = day2 + 1 ; j <= oldNbDays ; j++ )
			{
				jourSuiv[k][j] = day ;
			}
		}
	}

	// Here we fill the structure "DemandDayPat"
	// If it was a single day problem, the demand per day is simply the demand of the customer
	if (oldNbDays == 1) 
	{
		for (int k=1 ; k < (int)pow((float)2,oldNbDays) ; k++)
			for (int kk = 1 ; kk <= nbDays ; kk++)
				demandPatDay[k][kk] = demand ;
	}
	else // Case of the PCARP, the demand per day depends on the choice of Pattern 
	{
		int firstNonZero ;
		int tempPat ;
		int tempIndex ;
		double tempLoad ;
		vector<int> dayVisits = vector<int> (oldNbDays+1) ;
		if (this->custNum != 0)
		{
			for (int k=1 ; k < (int)pow((float)2,oldNbDays) ; k++)
			{
				// For this Pattern, compute the visits over the week.
				tempPat = k ;
				for (int kk = 0 ; kk < oldNbDays ; kk++)
				{
					dayVisits[oldNbDays - kk] = tempPat%2 ;
					tempPat = tempPat/2 ;
				}

				// Find the first non-0 element
				firstNonZero = 1 ;
				while (dayVisits[firstNonZero] == 0)
					firstNonZero ++ ;

				// And then we will loop in a circular way, starting from the next day
				tempLoad = 0. ;
				for (int kk = 0 ; kk < oldNbDays ; kk++)
				{
					tempIndex = 1 + (firstNonZero + kk)%oldNbDays ;
					tempLoad += demand ; // We cumulate the daily demand
					if (dayVisits[tempIndex] == 1)
					{
						demandPatDay[k][tempIndex] = tempLoad ;
						tempLoad = 0. ;
					}
				}
				// Just a quick check to verify that the final load is reset correctly to 0
				if (tempLoad != 0.)
					cout << "ERROR demandPatDay" << endl ;

				// Finally, the first visit of the week needs to take care of the load of the week-end (two additional days)
				// Only to be added to the PCARP
				if (oldNbDays > 1)
					demandPatDay[k][firstNonZero] += 2*demand ;
			}
		}
	}
}

bool Client::testPat (int Pattern, int oldNbDays)
{
	int calcul1, calcul2 ;
	bool concorde ;
	for (int i=0 ; i < (int)visits.size() ; i++)
	{
		calcul1 = Pattern ;
		calcul2 = visits[i].pat ;
		concorde = true ;
		for (int k = 0 ; k < oldNbDays ; k++)
		{
			if ((calcul1 % 2) > (calcul2 % 2)) 
				concorde = false ;
			calcul1 = calcul1/2 ;
			calcul2 = calcul2/2 ;
		}
		if (concorde) return true ;
	}
	return false ;
}

void Client::frec (int y, int z,int n, int oldNbDays) 
{
	if (n > y) 
	{ 
		frec (y,z,n/2,oldNbDays) ;
	}
	else 
	{
		if (y!=0)
		{
			frec(y-n,z,n/2,oldNbDays);
			frec(y-n,z+n,n/2,oldNbDays);
		}
		else
		{
			ajoute(z,(int)codeTravail-z,oldNbDays);
		}
	}
}

void Client::ajoute (int y, int z, int oldNbDays)
{
	int temp = z ;
	if (temp !=0)
	{
		jourSuiv[y][0] = 0 ;
		for (int i=0 ; i < oldNbDays ; i++)
		{
			if (jourSuiv[y][oldNbDays-i] == 0) 
			{ 
				jourSuiv[y][oldNbDays-i] = temp%2 ; 
			}
			temp = temp/2 ;
		}
	}
}

Arc * Client::getArc (int i, int j)
{
	for (int k=0 ; k < ar_nbModes ; k++)
		if (ar_Modes[k]->nodeBegin == i && ar_Modes[k]->nodeEnd == j) return ar_Modes[k] ;
	return NULL ;
}

Client::Client()
{
}

Client::~Client(void){}
