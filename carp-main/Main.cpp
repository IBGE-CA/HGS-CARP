using namespace std;

#include <iostream>

#include "../carp-core/Loader.h"

#include "Parameters.h"

using namespace std;
using namespace carp_main;
using namespace carp_core;

int main(int argc, char* argv[])
{
	try
	{
		Parameters params = Parameters();
		if (!params.load(argc, argv)) return -1;

		Loader loader;
		if (!loader.load(params.instanceFile))
		{
			cout << "Error while loading the instance file : " << params.instanceFile << endl;
			return -1;
		}
	}
	catch (const string& e)
	{
		cout << e << endl;
		cout << endl;
		return -1;
	}

	return 0;
}
