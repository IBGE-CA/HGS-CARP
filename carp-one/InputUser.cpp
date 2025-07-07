#include "InputUser.h"

#include "Parameters.h"
#include "ProcessedData.h"

using namespace carp_one;

InputUser::InputUser() : BaseInputUser(), params(Parameters::getInstance()), pd(ProcessedData::getInstance())
{
}

InputUser::InputUser(const InputUser& other) : BaseInputUser(other), params(Parameters::getInstance()), pd(ProcessedData::getInstance())
{
	setSilent(isSilent = other.isSilent);
}
