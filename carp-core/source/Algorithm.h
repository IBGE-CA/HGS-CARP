#pragma once

#include "InputUser.h"

namespace carp_core
{
	class Algorithm : public InputUser
	{
	public:
		virtual ~Algorithm() = default;

		virtual bool run() = 0;

		virtual void doCleanExit() {}
	};
}
