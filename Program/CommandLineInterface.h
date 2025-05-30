#pragma once

#include <string>

class CommandLineInterface
{
public:
	CommandLineInterface();

	bool load(int argc, char* argv[]);

	enum Type
	{
		CARP,
		MCGRP,
		PCARP,
		MDCARP,
		MCGRP_TP,
		MM_kWRPP,
		NoType,
	};

	std::string instanceFile;
	std::string outputFile;
	std::string bksFile;

	std::string name;
	int timeLimit;
	int seed;
	int type;
	int nbVeh;
	int nbDep;
	bool timeCapacitated;
	bool softConstraints;
	bool deadheadingArcs;

private:
	int getType(const std::string& type);

	void printHelp(const std::string& message = "");
};
