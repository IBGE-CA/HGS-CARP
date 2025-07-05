#pragma once

#include <ostream>

namespace carp_core
{
	class Data;
	class ProcessedData;
	class CommandLineInterface;

	class InputUser
	{
	public:
		InputUser();
		InputUser(const InputUser& other);
		virtual ~InputUser() = default;

		void setSilent(bool value);

	protected:
		const Data& data;
		const ProcessedData& pd;
		const CommandLineInterface& cli;

		bool isSilent;
		std::ostream out;

	private:
		class NullBuffer : public std::streambuf
		{
		public:
			int overflow(int c) override { return c; }
		};
		NullBuffer null;
	};
}
