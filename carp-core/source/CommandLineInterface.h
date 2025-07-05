#pragma once

#include "Singleton.h"

#include <string>

#include "Data.h"

namespace carp_core
{
	class CommandLineInterface : public Singleton<CommandLineInterface>
	{
		friend class Singleton<CommandLineInterface>;
	public:

		bool load(int argc, char* argv[]);

		std::string instanceFile;
		std::string outputFile;
		std::string bksFile;

		std::string name;
		int timeLimit;
		int seed;
		Data::Type type;

		int nbVehicles;
		int nbDepots;

		bool hasTimeLimit;

		//TODO: [RM] Check if we need these.
		bool softConstraints;
		bool deadheadingArcs;

		double penaltyCapacity;
		double penaltyTime;

		bool silent;

	private:
		CommandLineInterface();

		Data::Type getType(const std::string& type);

		void printHelp(const std::string& message = "");
	};
}