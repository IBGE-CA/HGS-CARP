#pragma once

#include "../carp-core/Singleton.h"
#include "../carp-core/BaseProcessedData.h"

namespace carp_one
{
	class ProcessedData : public carp_core::Singleton<ProcessedData>, public carp_core::BaseProcessedData
	{
		friend class carp_core::Singleton<ProcessedData>;
	};
}
