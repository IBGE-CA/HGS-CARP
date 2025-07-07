#pragma once

namespace carp_core
{
	class Service
	{
	public:
		Service(int from, int to, double costFromTo, double costToFrom, int demand = 0) :
			from(from), to(to), costFromTo(costFromTo), costToFrom(costToFrom), demand(demand) {}

		int from = -1;
		int to = -1;
		
		double costFromTo = 0.0;
		double costToFrom = 0.0;

		int demand;

		bool isService() const { return demand > 0; }
	};

	typedef std::reference_wrapper<Service> RService;
}
