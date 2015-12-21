#ifndef TIMER_H_INCLUDED
#define TIMER_H_INCLUDED

//*************************************************************************//

#include <chrono>

//*************************************************************************//
//
// timer class
//
//*************************************************************************//

template<typename T>
class Timer {

private:

	// timing variables
	std::chrono::time_point<std::chrono::system_clock> t_start, t_stop;
	
	std::chrono::duration<T> t_duration;

public:

	// basic constructor
	Timer();

	// move and copy constructors
    Timer(Timer&&) = default;

    Timer(const Timer&) = default;

    // move and copy assigners
    Timer& operator = (Timer&&) = default;

    Timer& operator = (const Timer&) = default;

    // destructor
    ~Timer() = default;

	// start timer
	void start();

	// stop timer
	void stop();

	// get duration
	T duration() const;
};

//*************************************************************************//

// constructor
template<typename T>
Timer<T>::Timer()

	: t_start(std::chrono::system_clock::now())
	, t_stop(std::chrono::system_clock::now()) {

	t_duration = t_stop - t_start;
}

// start timer
template<typename T>
void Timer<T>::start() { t_start = std::chrono::system_clock::now(); }

// stop timer
template<typename T>
void Timer<T>::stop() {

	t_stop = std::chrono::system_clock::now();
	t_duration = t_stop - t_start;
}

// get duration
template<typename T>
T Timer<T>::duration() const { return t_duration.count(); }

//*************************************************************************//

#endif // TIMER_H_INCLUDED
