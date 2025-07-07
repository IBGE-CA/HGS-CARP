#pragma once

#include "../carp-core/BaseInputUser.h"

namespace carp_one
{
	class Parameters;
	class ProcessedData;

	class InputUser : public carp_core::BaseInputUser
	{
	public:
		InputUser();
		InputUser(const InputUser& other);
		virtual ~InputUser() = default;
		
	protected:
		const Parameters& params;
		const ProcessedData& pd;
	};
}
