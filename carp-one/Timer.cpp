#include "Timer.h"

using namespace std;
using namespace std::chrono;

Timer::Timer()
{
	total = 0;
	running = false;
}

void Timer::start()
{
	if (running) return;
	start_point = system_clock::now();
	running = true;
}

double Timer::lap()
{
	if (!running) return numeric_limits<double>::infinity();
	auto x = system_clock::now();
	return (total = duration_cast<milliseconds>(x - start_point).count() / 1000.0);
}

double Timer::stop()
{
	double temp = lap();
	running = false;
	return temp;
}
