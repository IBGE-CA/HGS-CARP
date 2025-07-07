#pragma once

#include "Cut.h"

#include <vector>
#include <boost/dynamic_bitset/dynamic_bitset.hpp>

namespace carp_one
{
	class CapacityCut : public Cut
	{
	public:
		CapacityCut(int ks, const std::vector<int>& requireds, const boost::dynamic_bitset<>& in_cut);

		int ks;
		std::vector<int> requireds;
		boost::dynamic_bitset<> in_cut;
	};
}
