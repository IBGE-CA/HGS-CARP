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

#include "commandline.h"

#include <iostream>

using namespace std;

void commandline::SetDefaultOutput(const string& to_parse)
{
	char caractere1 = '/';
	char caractere2 = '\\';

	int position = (int)to_parse.find_last_of(caractere1);
	int position2 = (int)to_parse.find_last_of(caractere2);
	if (position2 > position) position = position2;

	if (position != -1)
	{
		output_name = "sol-" + to_parse.substr(position + 1, to_parse.length() - 1);
		BKS_name = "bks-" + to_parse.substr(position + 1, to_parse.length() - 1);
	}
	else
	{
		output_name = "sol-" + to_parse;
		BKS_name = "bks-" + to_parse;
	}
}

commandline::commandline(int argc, char* argv[])
{
	if (argc % 2 != 0 || argc > 20 || argc < 2)
	{
		cout << "incorrect command line" << endl;
		command_ok = false;
		return;
	}

	// default values
	instance_name = string(argv[1]);
	SetDefaultOutput(string(argv[1]));
	cpu_time = 300; // Five minutes is default CPU time
	seed = 0;
	type = -1;
	nbVehicles = -1;
	nbDepots = -1;
	hasTimeLimit = false;
	softConstraints = false;
	deadheadingArcs = false;

	// reading the commandline parameters
	for (int i = 2; i < argc; i += 2)
	{
		if (string(argv[i]) == "-t")
			cpu_time = atoi(argv[i + 1]);
		else if (string(argv[i]) == "-sol")
			output_name = string(argv[i + 1]);
		else if (string(argv[i]) == "-bks")
			BKS_name = string(argv[i + 1]);
		else if (string(argv[i]) == "-seed")
			seed = atoi(argv[i + 1]);
		else if (string(argv[i]) == "-type")
			type = atoi(argv[i + 1]);
		else if (string(argv[i]) == "-veh")
			nbVehicles = atoi(argv[i + 1]);
		else if (string(argv[i]) == "-dep")
			nbDepots = atoi(argv[i + 1]);
		else if (string(argv[i]) == "-tcap")
			hasTimeLimit = atoi(argv[i + 1]) != 0;
		else if (string(argv[i]) == "-soft")
			softConstraints = atoi(argv[i + 1]) != 0;
		else if (string(argv[i]) == "-dhead")
			deadheadingArcs = atoi(argv[i + 1]) != 0;
		else
		{
			cout << "Non-recognized command : " << string(argv[i]) << endl;
			command_ok = false;
		}
	}

	if (type == -1)
	{
		cout << "Please specify a problem type : " << endl;
		cout << "-type 30 = CARP" << endl;
		cout << "-type 31 = NEARP" << endl;
		cout << "-type 32 = PCARP" << endl;
		cout << "-type 33 = MDCARP" << endl;
		cout << "-type 34 = NEARP-TP" << endl;
		cout << "-type 35 = MM-kWRPP" << endl;
		command_ok = false;
		return;
	}

	if (type == 32 && nbVehicles == -1)
	{
		cout << "For the PCARP, please specify the starting value (an upper bound) for the number of vehicles" << endl;
		cout << "Setting -veh 10 should be a reasonable choice for the instances enclosed in the archive" << endl;
		command_ok = false;
		return;
	}

	if (type == 33 && nbDepots == -1)
	{
		cout << "For the MDCARP, please also specify the number of depots" << endl;
		command_ok = false;
		return;
	}

	command_ok = true;
}

commandline::~commandline() {}

string commandline::get_path_to_instance()
{
	return instance_name;
}

string commandline::get_path_to_solution()
{
	return output_name;
}

string commandline::get_path_to_BKS()
{
	return BKS_name;
}

int commandline::get_type()
{
	return type;
}

int commandline::get_nbVeh()
{
	return nbVehicles;
}

int commandline::get_nbDep()
{
	return nbDepots;
}

int commandline::get_cpu_time()
{
	return cpu_time;
}

int commandline::get_seed()
{
	return seed;
}

bool commandline::get_timeCapacitated()
{
	return hasTimeLimit;
}

bool commandline::get_softConstraints()
{
	return softConstraints;
}

bool commandline::get_deadheadingArcs()
{
	return deadheadingArcs;
}

bool commandline::is_valid()
{
	return command_ok;
}
