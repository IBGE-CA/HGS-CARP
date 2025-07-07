#pragma once

#include "Constraint.h"

#include <string>

class Cut : public Constraint
{
public:
	enum Type
	{
		CapacityCut,
		OddCutsetCut,
	};

	Cut(Type type) : type(type) {}

	Type type;

	std::string getCutName() const;
};