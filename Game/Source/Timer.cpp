#include"Timer.h"

Timer::Timer()
{

}

Timer::~Timer()
{

}

void Timer::Initialize()
{
	time = 0;
	offset = 0;
}

void Timer::UpdateTimer()
{
	time += 0.01999f;
}

double Timer::GetTimer()
{
	return (int)time;
}