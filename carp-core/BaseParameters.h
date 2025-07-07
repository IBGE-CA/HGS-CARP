#pragma once

#include "Singleton.h"
#include "CommandLineInterface.h"

#include <string>

#include "Data.h"

namespace carp_core
{
	class BaseParameters : public CommandLineInterface
	{
	public:

		bool load(int argc, char* argv[]);

		std::string instanceFile = "";
		Data::Type type = Data::CARP;

		int nbVehicles = -1;
		int nbDepots = 1;

		bool hasTimeLimit = false;

		bool silent = false;
	};
}
