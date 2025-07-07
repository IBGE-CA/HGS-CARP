#pragma once

#include "../carp-core/BaseInputUser.h"

#include <array>
#include <vector>

namespace carp_hgs
{
	class Individual;
	class Vehicle;

	class Sequence : public carp_core::BaseInputUser
	{
	public:

		int first = -1;
		int last = -1;

		int load = 0;
		double cost = 0.0;

		enum Mode
		{
			FromFrom = 0,
			FromTo = 1,
			ToFrom = 2,
			ToTo = 3,
			ModesSize = 4,
		};

		std::array<double, ModesSize> bestCost;

		// We use this data structure to be able to track the best combination of edge orientations
		// To report the complete solution (with edge orientations) at the end of the algorithm
		// Only used when printing the solution
		// bestCostArcs[i] contains the complete solution (all the path of arcs) when starting with the depot 0 and finishing with mode i
		std::array<std::vector<std::pair<int, int>>, ModesSize> bestCostArcs;

		void init(int current, Individual& individual, bool isForPathTracking);

		void concatOneAfter(const Sequence& sequence, int current);
		// used to track the path when printing the final solution
		void concatOneAfterWithPathTracking(const Sequence& sequence, int current);
		void concatOneBefore(const Sequence& sequence, int current);

		double evaluate(const Sequence& sequence) const;
		double evaluate(const Sequence& sequence1, const Sequence& sequence2) const;
		double evaluate(const Sequence& sequence1, const Sequence& sequence2, double& mydist, double& mytminex, double& myloadex) const;
		double evaluate(const Sequence& sequence1, const Sequence& sequence2, const Sequence& sequence3) const;
		double evaluate(const Sequence& sequence1, const Sequence& sequence2, const Sequence& sequence3, const Sequence& sequence4) const;
		double evaluate(const std::vector<Sequence&> sequences) const;

		double evaluateLB(const Sequence& sequence1) const;
		double evaluateLB(const Sequence& sequence1, const Sequence& sequence2) const;
		double evaluateLB(const Sequence& sequence1, const Sequence& sequence2, const Sequence& sequence3) const;
		double evaluateLB(const Sequence& sequence1, const Sequence& sequence2, const  Sequence& sequence3, const Sequence& sequence4) const;
		double evaluateLB(const std::vector<Sequence&> sequences) const;
	};
}
