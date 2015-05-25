#ifndef TIMER_H
#define TIMER_H

class Timer
{
private:
	double time;
	double offset;
public:
	Timer();
	~Timer();

	void Initialize();

	void UpdateTimer(double _time);
	double GetTimer();
};
#endif