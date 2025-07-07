#pragma once

#include "../carp-core/Algorithm.h"

namespace carp_hgs
{
	class HybridGeneticSearch : public carp_core::Algorithm
	{
	public:
		bool run();
	};
}