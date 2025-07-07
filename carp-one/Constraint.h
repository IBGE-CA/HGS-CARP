#pragma once

#include <vector>
#include <ostream>
#include <istream>

#include "../carp-core/Data.h"

class Constraint
{
public:
	enum Sense
	{
		Equal,
		GreaterEqual,
		LessEqual,
	};

	Constraint(Sense sense = Equal, double rhs = 0.0) : id(-1), sense(sense), rhs(rhs) {}

	class Element
	{
	public:
		Element(int key, double coeff) : key(key), coeff(coeff) {}

		int key;
		double coeff;

		bool operator<(const Element& right) const
		{
			return (key == right.key ? coeff - right.coeff < -carp_core::Data::EPS : key < right.key);
		}
	};

	int id = -1;
	std::vector<Element> lhs;
	Sense sense;
	double rhs;

	bool operator<(const Constraint& right) const;

	friend std::ostream& operator<<(std::ostream& out, const Constraint& right);
	friend std::istream& operator>>(std::istream& in, Constraint& right);
};
