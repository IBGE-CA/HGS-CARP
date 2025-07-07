#pragma once

#include <string>

namespace carp_core
{
	class CommandLineOption
	{
	public:
		CommandLineOption(const std::string& flag, const std::string& description = "")
			: flag(flag), description(description) {}

		std::string flag;
		std::string description;

		bool operator<(const CommandLineOption& other) const { return flag < other.flag; }
		bool operator==(const CommandLineOption& other) const { return flag == other.flag; }
	};

	template<typename T>
	class CommandLineOptionValue
	{
	public:
		CommandLineOptionValue(const CommandLineOption& option, T& value) : option(option), value(value) {}

		CommandLineOption option;
		T& value;

		bool operator==(const std::string& flag) const { return option.flag == flag; }
		bool operator==(const CommandLineOption& other) const { return option.flag == other.flag; }
	};
}
