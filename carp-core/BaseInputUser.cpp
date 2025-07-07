#include "BaseInputUser.h"

#include <iostream>

#include "Data.h"

using namespace std;
using namespace carp_core;

BaseInputUser::BaseInputUser() : data(Data::getInstance()), out(&null)
{
	setSilent(isSilent = false);
}

BaseInputUser::BaseInputUser(const BaseInputUser& other) : data(Data::getInstance()), out(&null)
{
	setSilent(isSilent = other.isSilent);
}

void BaseInputUser::setSilent(bool value)
{
	out.rdbuf((isSilent = value) ? &null : cout.rdbuf());
}
