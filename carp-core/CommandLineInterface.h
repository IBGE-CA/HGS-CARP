#pragma once

#include <vector>
#include <unordered_map>

#include "CommandLineOption.h"

namespace carp_core
{
	class CommandLineInterface
	{
	public:

		bool load(int argc, char* argv[]);

		void addIntegerOption(const CommandLineOption& clo, int& value);
		void addDoubleOption(const CommandLineOption& clo, double& value);
		void addBooleanOption(const CommandLineOption& clo, bool& value);
		void addStringOption(const CommandLineOption& clo, std::string& value);
		void addChoiceOption(const CommandLineOption& clo, int& value, const std::unordered_map<std::string, int>& choices);
		void addSeparator(const std::string& separator);

	protected:
		void printHelp(const std::string& message = "");

	private:
		std::vector<CommandLineOptionValue<int>> intOptions;
		std::vector<CommandLineOptionValue<double>> doubleOptions;
		std::vector<CommandLineOptionValue<bool>> boolOptions;
		std::vector<CommandLineOptionValue<std::string>> stringOptions;
		std::vector<CommandLineOptionValue<int>> choiceOptions;
		std::unordered_map<std::string, std::unordered_map<std::string, int>> choices;

		std::vector<std::string> optional;
		std::vector<std::string> mandatory;

		bool hasArguments(const std::string& flag) const;
		bool setOption(const std::string& flag);
		bool setOption(const std::string& flag, const std::string option);

		void printOption(const std::string& option, bool mandatory = false);
	};
}
