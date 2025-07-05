#include "CommandLineInterface.h"

#include <iostream>

using namespace std;
using namespace carp_core;

CommandLineInterface::CommandLineInterface()
{
	instanceFile = "";
	outputFile = "";
	bksFile = "";

	name = "";
	timeLimit = 300;
	seed = 0;
	type = Data::CARP;

	nbVehicles = -1;
	nbDepots = 1;

	hasTimeLimit = false;
	softConstraints = false;
	deadheadingArcs = false;

	//TODO: [RM] These values are temporary.
	penaltyCapacity = 100.0;
	penaltyTime = 100.0;

	silent = false;

	// using the time to generate a seed when seed = 0 
	srand((unsigned int)time(nullptr));
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
			silent = true;
		else if (param == "-seed")
		{
			seed = atoi(argv[++i]);
			srand(seed);
		}
		else if (param == "-type")
		{
			type = getType(argv[++i]);
			if (type == Data::NoType)
			{
				printHelp("Error: Unrecognized problem type. Valid types are: CARP, NEARP, PCARP, MDCARP, NEARP_TP, MM_kWRPP.");
				return false;
			}
		}
		else if (param == "-veh")
			nbVehicles = atoi(argv[++i]);
		else if (param == "-dep")
			nbDepots = atoi(argv[++i]);
		else if (param == "-tcap")
			hasTimeLimit = true;
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

	if (type == Data::MDCARP && nbDepots <= 0)
	{
		printHelp("Error: For the MDCARP, please specify a positive number of depots.");
		return false;
	}

	return true;
}

Data::Type CommandLineInterface::getType(const string& type)
{
	if (type == "CARP") return Data::CARP;
	else if (type == "MDCARP") return Data::MDCARP;
	else if (type == "NEARP") return Data::NEARP;
	else return Data::NoType;
}

void CommandLineInterface::printHelp(const string& message)
{
	if (!message.empty()) cout << message << endl;
	cout << "Usage: HGS-CARP <instance name> [options]" << endl;
	cout << "Options:" << endl;
	cout << "  -h            Show this help message." << endl;
	cout << "  -s            Silent mode." << endl;
	cout << "  -t <value>    Set CPU time allowed in seconds (default = 300)." << endl;
	cout << "  -seed <value> Set random seed (default = 42)." << endl;
	cout << "  -type <value> Set problem type (CARP, MDCARP, NEARP - default = CARP)." << endl;
	cout << "  -veh <value>  Set number of vehicles (default = by instance)." << endl;
	cout << "  -dep <value>  Set number of depots (default = by instance)." << endl;
	cout << "  -tcap         Enable time-capacitated problem (default = false)." << endl;
	cout << "  -soft         Enable soft constraints (default = false)." << endl;
	cout << "  -dhead        Output deadheading arcs (default = false)." << endl;
}
