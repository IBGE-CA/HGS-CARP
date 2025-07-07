#include "CommandLineInterface.h"

#include <iostream>
#include <stdexcept>

using namespace std;
using namespace carp_core;

bool CommandLineInterface::load(int argc, char* argv[])
{
	if (argc < mandatory.size() + 1)
	{
		printHelp("Error: Too few arguments!");
		return false;
	}

	for (int i = 0; i < mandatory.size(); i++)
		if (!setOption(mandatory[i], argv[i + 1]))
		{
			printHelp("Error loading mandatory flag " + mandatory[i] + "!");
			return false;
		}

	for (int i = (int)mandatory.size() + 1; i < argc; i++)
	{
		string flag = string(argv[i]);
		if (flag == "-h")
		{
			printHelp();
			return false;
		}
		else if (!(hasArguments(flag) ? setOption(flag, argv[++i]) : setOption(flag)))
		{
			printHelp("Unrecognized flag " + flag + "!");
			return false;
		}
	}

	return true;
}

bool CommandLineInterface::hasArguments(const std::string& flag) const
{
	return (find(intOptions.begin(), intOptions.end(), flag) != intOptions.end() ||
		find(doubleOptions.begin(), doubleOptions.end(), flag) != doubleOptions.end() ||
		find(stringOptions.begin(), stringOptions.end(), flag) != stringOptions.end() ||
		find(choiceOptions.begin(), choiceOptions.end(), flag) != choiceOptions.end());
}

bool CommandLineInterface::setOption(const std::string& flag)
{
	auto it = find(boolOptions.begin(), boolOptions.end(), flag);
	if (it == boolOptions.end()) return false;

	it->value = true;
	return true;
}

bool CommandLineInterface::setOption(const std::string& flag, const string option)
{
	auto iti = find(intOptions.begin(), intOptions.end(), flag);
	if (iti != intOptions.end())
	{
		try
		{
			iti->value = stoi(option);
			return true;
		}
		catch (const invalid_argument&)
		{
			cerr << "Invalid integer value for option " << flag << ": " << option << endl;
			return false;
		}
	}

	auto itd = find(doubleOptions.begin(), doubleOptions.end(), flag);
	if (itd != doubleOptions.end())
	{
		try
		{
			itd->value = stod(option);
			return true;
		}
		catch (const invalid_argument&)
		{
			cerr << "Invalid double value for option " << flag << ": " << option << endl;
			return false;
		}
	}

	auto its = find(stringOptions.begin(), stringOptions.end(), flag);
	if (its != stringOptions.end())
	{
		its->value = string(option);
		return true;
	}

	auto itc = find(choiceOptions.begin(), choiceOptions.end(), flag);
	if (itc != choiceOptions.end())
	{
		try
		{
			auto choiceIt = choices[flag].find(option);
			if (choiceIt == choices[flag].end())
			{
				cerr << "Invalid choice for option " << flag << ": " << option << endl;
				return false;
			}
			itc->value = choiceIt->second;
			return true;
		}
		catch (const invalid_argument&)
		{
			cerr << "Invalid integer value for option " << flag << ": " << option << endl;
			return false;
		}
	}

	return false;
}

void CommandLineInterface::printHelp(const string& message)
{
	if (!message.empty()) cout << message << endl;
	cout << "Usage: carp";
	for (const auto& opt : mandatory) cout << " <" << opt << ">";
	cout << " [optional]" << endl;

	for (const auto& option : mandatory) printOption(option, true);
	for (const auto& option : optional) printOption(option);
}

void CommandLineInterface::printOption(const string& option, bool mandatory)
{
	string tab = "    ";

	auto iti = find(intOptions.begin(), intOptions.end(), option);
	if (iti != intOptions.end())
		cout << tab << (mandatory ? "<" : "") << iti->option.flag << (mandatory ? ">: " : " <int>: ") << iti->option.description << endl;
	else
	{
		auto itd = find(doubleOptions.begin(), doubleOptions.end(), option);
		if (itd != doubleOptions.end())
			cout << tab << (mandatory ? "<" : "") << itd->option.flag << (mandatory ? ">: " : " <double>: ") << itd->option.description << endl;
		else
		{
			auto itb = find(boolOptions.begin(), boolOptions.end(), option);
			if (itb != boolOptions.end())
				cout << tab << itb->option.flag << ": " << itb->option.description << endl;
			else
			{
				auto its = find(stringOptions.begin(), stringOptions.end(), option);
				if (its != stringOptions.end())
					cout << tab << (mandatory ? "<" : "") << its->option.flag << (mandatory ? ">: " : " <string>: ") << its->option.description << endl;
				else
				{
					auto itc = find(choiceOptions.begin(), choiceOptions.end(), option);
					if (itc != choiceOptions.end())
						cout << tab << (mandatory ? "<" : "") << itc->option.flag << (mandatory ? ">: " : " <choice>: ") << itc->option.description << endl;
					else
						cout << option << endl;
				}
			}
		}
	}
}

void CommandLineInterface::addIntegerOption(const CommandLineOption& clo, int& value)
{
	auto it = find(intOptions.begin(), intOptions.end(), clo);
	if (it != intOptions.end())
		throw runtime_error("Option already exists: " + clo.flag);
	intOptions.emplace_back(clo, value);
	if (clo.flag[0] != '-') mandatory.push_back(clo.flag);
	else optional.push_back(clo.flag);
}

void CommandLineInterface::addDoubleOption(const CommandLineOption& clo, double& value)
{
	auto it = find(doubleOptions.begin(), doubleOptions.end(), clo);
	if (it != doubleOptions.end())
		throw runtime_error("Option already exists: " + clo.flag);
	doubleOptions.emplace_back(clo, value);
	if (clo.flag[0] != '-') mandatory.push_back(clo.flag);
	else optional.push_back(clo.flag);
}

void CommandLineInterface::addBooleanOption(const CommandLineOption& clo, bool& value)
{
	auto it = find(boolOptions.begin(), boolOptions.end(), clo);
	if (it != boolOptions.end())
		throw runtime_error("Option already exists: " + clo.flag);
	boolOptions.emplace_back(clo, value);
	if (clo.flag[0] != '-') throw runtime_error("Boolean option must be optional!");
	else optional.push_back(clo.flag);
}

void CommandLineInterface::addStringOption(const CommandLineOption& clo, string& value)
{
	auto it = find(stringOptions.begin(), stringOptions.end(), clo);
	if (it != stringOptions.end())
		throw runtime_error("Option already exists: " + clo.flag);
	stringOptions.emplace_back(clo, value);
	if (clo.flag[0] != '-') mandatory.push_back(clo.flag);
	else optional.push_back(clo.flag);
}

void CommandLineInterface::addChoiceOption(const CommandLineOption& clo, int& value, const unordered_map<string, int>& choices)
{
	auto it = find(choiceOptions.begin(), choiceOptions.end(), clo);
	if (it != choiceOptions.end())
		throw runtime_error("Option already exists: " + clo.flag);
	choiceOptions.emplace_back(clo, value);
	this->choices[clo.flag] = choices;
	if (clo.flag[0] != '-') mandatory.push_back(clo.flag);
	else optional.push_back(clo.flag);
}

void CommandLineInterface::addSeparator(const string& separator)
{
	optional.push_back(separator);
}
