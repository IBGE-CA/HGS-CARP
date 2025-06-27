#pragma once

#include "Singleton.h"

#include <string>

#include "Data.h"

class CommandLineInterface : public Singleton<CommandLineInterface>
{
	friend class Singleton<CommandLineInterface>;
public:
	CommandLineInterface();

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

	bool timeCapacitated;
	bool softConstraints;
	bool deadheadingArcs;

	bool silent;

	// We need to minimize fleet size as first objective, then minimize distance as second objective.
	bool isMinFleetSize() const { return type == Data::PCARP; }

	// We need to minimize the length of the maximum route.
	bool isMinMaxTour()  const { return type == Data::MM_kWRPP; }

	// PCARP instances are defined on 5 days.
	int getNbDays() const { return (type == Data::PCARP ? 5 : 1); }

private:
	Data::Type getType(const std::string& type);

	void printHelp(const std::string& message = "");
};
