#pragma once

#include "Singleton.h"

#include <vector>

#include "Link.h"

namespace carp_core
{
	class Data;

	class ProcessedData : public Singleton<ProcessedData>
	{
		friend class Singleton<ProcessedData>;
	public:

		// Distance matrix. Old ar_distanceNodes.
		std::vector<std::vector<double>> distances;

		// FW predecesors. Old ar_predNodes.
		std::vector<std::vector<int>> pred;

	private:
		Data& data;

		void buildDistances();

		ProcessedData();
	};
}
