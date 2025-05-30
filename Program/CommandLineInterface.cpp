#include "CommandLineInterface.h"

#include <iostream>

using namespace std;

CommandLineInterface::CommandLineInterface()
{
	instanceFile = "";
	outputFile = "";
	bksFile = "";

	name = "";
	timeLimit = 300;
	seed = 0;
	type = CARP;
	nbVeh = -1;
	nbDep = -1;
	timeCapacitated = false;
	softConstraints = false;
	deadheadingArcs = false;
}

bool CommandLineInterface::load(int argc, char* argv[])
{
	if (argc < 2)
	{
		printHelp("Error: Instance name is required.");
		return false;
	}

	instanceFile = argv[1];
	name = instanceFile.substr(instanceFile.find_last_of('/') + 1, instanceFile.find_last_of('.') - instanceFile.find_last_of('/') - 1);

	outputFile = "out/" + name + "-sol.txt";
	bksFile = "out/" + name + "-bks.txt";

	for (int i = 2; i < argc; ++i)
	{
		string param = string(argv[i]);

		if (param == "-t")
			timeLimit = atoi(argv[++i]);
		else if (param == "-s")
			seed = atoi(argv[++i]);
		else if (param == "-type")
		{
			type = getType(argv[++i]);
			if (type == NoType)
			{
				printHelp("Error: Unrecognized problem type. Valid types are: CARP, MCGRP, PCARP, MDCARP, MCGRP_TP, MM_kWRPP.");
				return false;
			}
		}
		else if (param == "-veh")
			nbVeh = atoi(argv[++i]);
		else if (param == "-dep")
			nbDep = atoi(argv[++i]);
		else if (param == "-tcap")
			timeCapacitated = true;
		else if (param == "-soft")
			softConstraints = true;
		else if (param == "-dhead")
			deadheadingArcs = true;
		else if (param == "-h")
		{
			printHelp();
			return false;
		}
		else
		{
			printHelp("Error: Unrecognized option '" + param + "'.");
			return false;
		}
	}

	if (type == PCARP && nbVeh <= 0)
	{
		printHelp("Error: For the PCARP, please specify the starting value (an upper bound) for the number of vehicles." +
			string("Setting -veh 10 should be a reasonable choice for the instances enclosed in the archive."));
		return false;
	}

	if (type == MDCARP && nbDep <= 0)
	{
		printHelp("Error: For the MDCARP, please specify a positive number of depots.");
		return false;
	}

    return true;
}

int CommandLineInterface::getType(const string& type)
{
	if (type == "CARP") return CARP;
	else if (type == "MCGRP") return MCGRP;
	else if (type == "PCARP") return PCARP;
	else if (type == "MDCARP") return MDCARP;
	else if (type == "MCGRP_TP") return MCGRP_TP;
	else if (type == "MM_kWRPP") return MM_kWRPP;
	else return NoType;
}

void CommandLineInterface::printHelp(const string& message)
{
	if (!message.empty()) cout << message << endl;
	cout << "Usage: HGS-CARP <instance name> [options]" << endl;
	cout << "Options:" << endl;
	cout << "  -h            Show this help message." << endl;
	cout << "  -t <value>    Set CPU time allowed in seconds (default = 300)." << endl;
	cout << "  -s <value>    Set random seed (default = 42)." << endl;
	cout << "  -type <value> Set problem type (CARP, MCGRP, PCARP, MDCARP, MCGRP_TP, MM_kWRPP - default = CARP)." << endl;
	cout << "  -veh <value>  Set number of vehicles (default = by instance)." << endl;
	cout << "  -dep <value>  Set number of depots (default = by instance)." << endl;
	cout << "  -tcap         Enable time-capacitated problem (default = false)." << endl;
	cout << "  -soft         Enable soft constraints (default = false)." << endl;
	cout << "  -dhead        Output deadheading arcs (default = false)." << endl;
}
