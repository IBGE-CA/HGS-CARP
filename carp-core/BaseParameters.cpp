#include "BaseParameters.h"

#include <unordered_map>

using namespace std;
using namespace carp_core;

bool BaseParameters::load(int argc, char* argv[])
{
	unordered_map<string, int> types = {
		{ "CARP", Data::CARP },
		{ "MDCARP", Data::MDCARP },
		{ "NEARP", Data::NEARP }
	};

	addSeparator("General Options:");
	addStringOption({ "file", "The name of the instance file to load." }, instanceFile);
	addChoiceOption({ "-type", "Problem type (CARP, MDCARP, NEARP - default = CARP)." }, (int&)type, types);
	addIntegerOption({ "-veh", "The number of vehicles (default = by instance)." }, nbVehicles);
	addIntegerOption({ "-dep", "Set number of depots (default = by instance)." }, nbDepots);
	addBooleanOption({ "-tcap", "Enable time limit for the problem (default = false)." }, hasTimeLimit);
	addBooleanOption({ "-s", "Enable silent mode (default = false)." }, silent);

	if (!CommandLineInterface::load(argc, argv))
		return false;

	if (type == Data::MDCARP && nbDepots <= 0)
	{
		printHelp("Error: For the MDCARP, please specify a positive number of depots.");
		return false;
	}

	return true;
}
