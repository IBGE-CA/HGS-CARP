#pragma once

#include "../carp-core/BaseParameters.h"

namespace carp_main
{
	class Parameters : public carp_core::BaseParameters
	{
	public:
		enum Algorithm
		{
			OneIndex,
			HGS,
			None,
		};

		bool load(int argc, char* argv[]);

		Algorithm algorithm = None;

	private:
		carp_core::BaseParameters& getParameters(Algorithm algorithm);
	};
}
