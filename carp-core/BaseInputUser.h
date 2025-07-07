#pragma once

#include <ostream>

namespace carp_core
{
	class Data;

	class BaseInputUser
	{
	public:
		BaseInputUser();
		BaseInputUser(const BaseInputUser& other);
		virtual ~BaseInputUser() = default;

		void setSilent(bool value);

	protected:
		const Data& data;

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
