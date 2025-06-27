#include "InputUser.h"

#include <iostream>

#include "Data.h"
#include "ProcessedData.h"
#include "CommandLineInterface.h"

using namespace std;

InputUser::InputUser() : data(Data::getInstance()), pd(ProcessedData::getInstance()), cli(CommandLineInterface::getInstance()), out(&null)
{
	setSilent(isSilent = cli.silent);
}

InputUser::InputUser(const InputUser& other) : data(Data::getInstance()), pd(ProcessedData::getInstance()), cli(CommandLineInterface::getInstance()), out(&null)
{
	setSilent(isSilent = other.isSilent);
}

void InputUser::setSilent(bool value)
{
	out.rdbuf((isSilent = value) ? &null : cout.rdbuf());
}
