#pragma once

#include <string>

class Loader
{
public:

	static bool load(const std::string& file_name);

private:
	static int depot;

	static bool loadCARP(std::ifstream& in);

	static void simplifyDemands();
	static void readRequireds(std::ifstream& in, int r);
	static void readNonRequired(std::ifstream& in, int m);
	static void closeRead(int d);

	static void loadKnwonUpperBound();
};
