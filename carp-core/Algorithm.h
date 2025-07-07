#pragma once

namespace carp_core
{
	class Algorithm
	{
	public:
		virtual ~Algorithm() = default;

		virtual bool run() = 0;

		virtual void doCleanExit() {}
	};
}
