#pragma once

#include <fstream>
#include <string>

namespace carp_core
{
	class Loader
	{
	public:
		static bool load(const std::string& file_name);

	private:
		static bool loadCARP(std::ifstream& in);

		static void readRequireds(std::ifstream& in, int r);
		static void readNonRequired(std::ifstream& in, int m);
		static void createEdge(int from, int to, int cost, int demand = 0);

		static void closeRead(int d);

		static void simplifyDemands();
		static void loadKnwonUpperBound();
	};
}
