//
// Programmer:    Craig Stuart Sapp <craig@ccrma.stanford.edu>
// Thanks to:     Erik Neuenschwander <erikn@leland.stanford.edu>
//                   for finding Win95 assembly code for Pentium clock cycles.
//                Ozgur Izmirli <ozgur@ccrma.stanford.edu>
//                   for concept of periodic timer.
// Creation Date: Mon Oct 13 11:34:57 GMT-0800 1997
// Last Modified: Sun Nov 28 12:39:39 PST 1999 added adjustPeriod())
// Last Modified: Sun Nov 20 01:19:24 PST 2005 new cpu speed measurement)
// Last Modified: Tue Jun  9 14:17:28 PDT 2009 added Apple OSX capability)
// Last Modified: Sun May 19 13:58:03 PDT 2024 use clock_gettime() for portable high-resolution timer.
// Filename:      .../sig/code/control/SigTimer/SigTimer.cpp
// Web Address:   http://improv.sapp.org/src/SigTimer.cpp
// Syntax:        C++
//
// Description:   Measures time from the CPU cycles.
//

#include "SigTimer.h"

#include <cstdlib>
#include <iostream>

using namespace std;

// declare static variables
int64bits SigTimer::globalOffset = 0;
int64bits SigTimer::cpuSpeed     = 1000000000; // in cycles per second

// get Sleep or usleep function definition for measureCpu function:

#ifdef VISUAL
	#define WIN32_LEAN_AND_MEAN
	#include <windows.h>
	#undef WIN32_LEAN_AND_MEAN
#else
	#include <unistd.h>
	#include <time.h>
#endif


//////////////////////////////
//
// SigTimer::SigTimer -- Initialization of SigTimer objects
//

SigTimer::SigTimer(void) {
	if (globalOffset == 0) {          // coordinate offset between timers
		globalOffset = clockCycles(m_tspec);
	}

	if (cpuSpeed <= 0) {              // initialize CPU speed value
		cpuSpeed = 1000000000;         // nano seconds in one second
	}

	offset = globalOffset;            // initialize the start time of timer
	ticksPerSecond = 1000;            // default of 1000 ticks per second
	period = 1000.0;                  // default period of once per second
}


SigTimer::SigTimer(int aSpeed) {
	if (globalOffset == 0) {
		globalOffset = clockCycles(m_tspec);
	}
	cpuSpeed = aSpeed;

	offset = globalOffset;
	ticksPerSecond = 1000;
	period = 1000.0;                     // default period of once per second
}


SigTimer::SigTimer(SigTimer& aTimer) {
	offset = aTimer.offset;
	ticksPerSecond = aTimer.ticksPerSecond;
	period = aTimer.period;
}



//////////////////////////////
//
// SigTimer::~SigTimer -- Destructor for a SigTimer object.
//

SigTimer::~SigTimer() {
	// do nothing
}



//////////////////////////////
//
// SigTimer::adjustPeriod -- Adjust the period of the timer.
//

void SigTimer::adjustPeriod(double periodDelta) {
	offset -= (int64bits)(getCpuSpeed() * getPeriod() * periodDelta /
			getTicksPerSecond());
}



//////////////////////////////
//
// SigTimer::clockCycles -- Returns the number of clock cycles since last reboot
//	   HARDWARE DEPENDENT -- currently for Pentiums only.
//     static function.
//

int64bits SigTimer::clockCycles(TimeSpec& tspec) {
	clock_gettime(CLOCK_MONOTONIC, &tspec);
	int64bits output = (int64bits)tspec.tv_sec * 1000 + (tspec.tv_nsec + 0.5) / 1000000;
	return output;
}



//////////////////////////////
//
// SigTimer::expired -- returns the integral number of periods
//    That have passed since the last update or reset.
//    See getPeriodCount which returns a floating point
//    count of the period position.
//

int SigTimer::expired(void) {
	return (int)(getTime()/period);
}



//////////////////////////////
//
// SigTimer::getCpuSpeed -- returns the CPU speed of the computer.
//   (static function)
//

int64bits SigTimer::getCpuSpeed(void) {
	return cpuSpeed;
}



//////////////////////////////
//
// SigTimer::getPeriod -- returns the timing period of the timer,
//    if the timer is being used as a periodic timer.
//

double SigTimer::getPeriod(void) {
	return period;
}



//////////////////////////////
//
// SigTimer::getPeriodCount -- returns the current period count
//    of the timer as a double.  Similar in behavior to expired
//    function but lists the current fraction of a period.
//

double SigTimer::getPeriodCount(void) {
	return (double)getTime()/period;
}



//////////////////////////////
//
// SigTimer::getTempo -- returns the current tempo in terms
//     of beats (ticks) per minute.
//

double SigTimer::getTempo(void) {
	return getTicksPerSecond() * 60.0 / getPeriod();
}



//////////////////////////////
//
// SigTimer::getTicksPerSecond -- return the number of ticks per
//     second.
//

int SigTimer::getTicksPerSecond(void) {
	return ticksPerSecond;
}



//////////////////////////////
//
// SigTimer::getTime -- returns time in milliseconds by default.
// 	time can be altered to be in some other unit of time
//	by using the setTicksPerSecond function.
//	(Default is 1000 ticks per second.)
//

int SigTimer::getTime(void) {
	return (int)((clockCycles(m_tspec)-offset)/getFactor());
}



//////////////////////////////
//
// SigTimer::getTimeInSeconds
//

double SigTimer::getTimeInSeconds(void) {
	return ((clockCycles(m_tspec)-offset)/(double)cpuSpeed);
}



//////////////////////////////
//
// SigTimer::getTimeInTicks
//

int SigTimer::getTimeInTicks(void) {
	return (int)((clockCycles(m_tspec)-offset)/getFactor());
}



//////////////////////////////
//
// SigTimer::reset -- set the timer to 0.
//

void SigTimer::reset(void) {
	offset = clockCycles(m_tspec);
}



//////////////////////////////
//
// SigTimer::setPeriod -- sets the period length of the timer.
//    input value cannot be less than 1.0.
//

void SigTimer::setPeriod(double aPeriod) {
	if (aPeriod < 1.0) {
		cerr << "Error: period too small: " << aPeriod << endl;
		exit(1);
	}
	period = aPeriod;
}



//////////////////////////////
//
// SigTimer::setPeriodCount -- adjusts the offset time according
//     to the current period to match the specified period count.
//

void SigTimer::setPeriodCount(double aCount) {
	offset = (int64bits)(clockCycles(m_tspec) - aCount * getPeriod() *
			getCpuSpeed() / getTicksPerSecond());
}



//////////////////////////////
//
// SigTimer::setTempo -- sets the period length in terms of
//	beats per minute.
//

void SigTimer::setTempo(double beatsPerMinute) {
	if (beatsPerMinute < 1.0) {
		cerr << "Error: tempo is too slow: " << beatsPerMinute << endl;
		exit(1);
	}
	double count = getPeriodCount();
	period = getTicksPerSecond() * 60.0 / beatsPerMinute;
	setPeriodCount(count);
}



//////////////////////////////
//
// SigTimer::setTicksPerSecond --
//

void SigTimer::setTicksPerSecond(int aTickRate) {
	if (aTickRate <= 0) {
		cerr << "Error: Cannot set the tick rate to be negative: "
			  << aTickRate << endl;
		exit(1);
	}
	ticksPerSecond = aTickRate;
}



//////////////////////////////
//
// SigTimer::start --
//

void SigTimer::start(void) {
	reset();
}



//////////////////////////////
//
// SigTimer::sync --
//

void SigTimer::sync(SigTimer& aTimer) {
	offset = aTimer.offset;
}



//////////////////////////////
//
// SigTimer::update -- set the timer start to the next period.
//

void SigTimer::update(void) {
	if (getTime() >= getPeriod()) {
		offset += (int64bits)(getPeriod() * getFactor());
	}
}

// update(int) will do nothing if the periodCount is greater than
// than the expired period count.

void SigTimer::update(int periodCount) {
	if (periodCount > expired()) {
		return;
	} else {
		offset += (int64bits)(getPeriod() * getFactor() * periodCount);
	}
}


///////////////////////////////////////////////////////////////////////////
//
// protected functions:
//


//////////////////////////////
//
// SigTimer::getFactor --
//

double SigTimer::getFactor(void) {
#ifdef OSXTIMER
	return (double)getCpuSpeed()/1000.0;
#else
	return (double)((double)getCpuSpeed()/(double)getTicksPerSecond());
#endif
}



///////////////////////////////////////////////////////////////////////////
//
// Miscellaneous global timing functions are located here (used in the
//  Improv environments):
//

void millisleep(int milliseconds) {
	#ifdef VISUAL
		Sleep(milliseconds);
	#else
		usleep(milliseconds * 1000);
	#endif
}


void millisleep(float milliseconds) {
	#ifdef VISUAL
		// cannot convert to microseconds in Visual C++ yet.
		// Tell me how and I'll fix the following line
		Sleep((unsigned long)milliseconds);
	#else
		usleep((int)(milliseconds * 1000.0));
	#endif
}



