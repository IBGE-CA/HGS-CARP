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

#pragma once

#include <string>

class commandline
{
private:

	// say if the commandline is valid
	bool command_ok;

	// CPU time allowed
	int cpu_time;

	// seed
	int seed;

	// instance type, if given (see definitions in Params.h)
	int type;

	// nbVehicles, if given (for the MM-kWRPP, the fleet size has to be specified)
	int nbVehicles;

	// nbDepots, if given (used to generate the MDCARP instances)
	int nbDepots;

	// time-capacitated problem, if given (problem in which all costs and the vehicle capacity are time-based)
	bool timeCapacitated;

	// soft constraints, if given (solutions violating capacity or duration constraints will be accepted with cost penalties applied)
	bool softConstraints;

	// output deadheading arcs, if given (deadheading arcs will be included in the solution output)
	bool deadheadingArcs;

	// instance path
	std::string instance_name;

	// output path
	std::string output_name;

	// BKS path (used to replace it if a better solution is found)
	std::string BKS_name;

	// simple setters
	void SetDefaultOutput(const std::string& to_parse);

public:

	// constructor
	commandline(int argc, char* argv[]);

	// destructor
	~commandline();

	std::string get_path_to_instance();
	std::string get_path_to_solution();
	std::string get_path_to_BKS();
	int get_cpu_time();
	int get_type();
	int get_nbVeh();
	int get_nbDep();
	int get_seed();
	bool get_timeCapacitated();
	bool get_softConstraints();
	bool get_deadheadingArcs();

	// say if the commandline is valid
	bool is_valid();
};
