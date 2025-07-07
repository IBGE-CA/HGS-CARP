#pragma once

#include <vector>

#include "Link.h"

namespace carp_core
{
	class Data;

	class BaseProcessedData
	{
	public:

		// Distance matrix. Old ar_distanceNodes.
		std::vector<std::vector<double>> distances;

		// FW predecesors. Old ar_predNodes.
		std::vector<std::vector<int>> pred;

	protected:
		BaseProcessedData();

	private:
		Data& data;

		void buildDistances();
	};
}
