#include "CapacityCut.h"

#include "ProcessedData.h"

using namespace carp_one;

CapacityCut::CapacityCut(int ks, const std::vector<int>& requireds, const boost::dynamic_bitset<>& in_cut) :
	Cut(Cut::CapacityCut), ks(ks), requireds(requireds), in_cut(in_cut)
{
	const auto& pd = ProcessedData::getBuilt();
	sense = Constraint::GreaterEqual;
	rhs = 2 * ks;
	//for (const auto& edge : pd.req_edges)
	//	if (in_cut[edge.from] != in_cut[edge.to])
	//		lhs.emplace_back(VarKey(edge.id, VarKey::ReqEdge), 1.0);
}
