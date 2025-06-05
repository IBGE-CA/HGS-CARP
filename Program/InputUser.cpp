#include "InputUser.h"

#include <iostream>

#include "Data.h"
#include "ProcessedData.h"
#include "CommandLineInterface.h"

using namespace std;

InputUser::InputUser() : data(Data::getInstance()), pd(ProcessedData::getInstance()), cli(CommandLineInterface::getInstance()), out(&null)
{
	setSilent(is_silent = cli.silent);
}

void InputUser::setSilent(bool value)
{
	out.rdbuf((is_silent = value) ? &null : cout.rdbuf());
}
