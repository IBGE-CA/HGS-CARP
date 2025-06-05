#pragma once

#include <ostream>

class Data;
class ProcessedData;
class CommandLineInterface;

class InputUser
{
public:
	InputUser();
	virtual ~InputUser() = default;

	void setSilent(bool value);

protected:
	const Data& data;
	const ProcessedData& pd;
	const CommandLineInterface& cli;

	bool is_silent;
	std::ostream out;

private:
	class NullBuffer : public std::streambuf
	{
	public:
		int overflow(int c) override { return c; }
	};
	NullBuffer null;
};
