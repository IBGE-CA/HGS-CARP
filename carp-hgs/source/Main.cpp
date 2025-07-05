using namespace std;

#include <iostream>

#include "CommandLineInterface.h"
#include "Loader.h"

using namespace std;
using namespace carp_core;

int main(int argc, char* argv[])
{
	try
	{
		CommandLineInterface cli = CommandLineInterface::getInstance();
		if (!cli.load(argc, argv)) return -1;

		Loader loader;
		if (!loader.load(cli.instanceFile))
		{
			cout << "Error while loading the instance file : " << cli.instanceFile << endl;
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
