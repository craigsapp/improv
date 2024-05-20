//
// Programmer:    Craig Stuart Sapp <craig@ccrma.stanford.edu>
// Thanks to:     Erik Neuenschwander <erikn@leland.stanford.edu>
//                   for Windows 95 assembly code for Pentium clock cycles.
//                Ozgur Izmirli <ozgur@ccrma.stanford.edu>
//                   for concept of periodic timer.
// Creation Date: Mon Oct 13 11:34:57 GMT-0800 1997
// Last Modified: Tue Feb 10 21:05:19 GMT-0800 1998
// Last Modified: Sat Sep 19 15:56:48 PDT 1998
// Last Modified: Mon Feb 22 04:44:25 PST 1999
// Last Modified: Sun Nov 28 12:39:39 PST 1999 (added adjustPeriod())
// Last Modified: Sun Nov 20 02:03:24 PST 2005 (changed to int64bit cpu speed)
// Last Modified: Tue Jun  9 13:43:51 PDT 2009 (added Apple OSX interface)
// Filename:      .../sig/code/control/SigTimer/SigTimer.h
// Web Address:   http://www-ccrma.stanford.edu/~craig/improv/include/SigTimer.h
// Syntax:        C++
//
// Description:   This class can only be used on Motorola Pentinum 75 Mhz
//                chips or better because the timing information is
//                extracted from the clock cycle count from a register
//                on the CPU itself.  This class will estimate the
//                speed of the computer, but it would be better if there
//                was a way of finding out the speed from some function.
//                This class is used primarily for timing of MIDI input
//                and output at a millisecond resolution.
//

#ifndef SIGTIMER_H_INCLUDED
#define SIGTIMER_H_INCLUDED

#include <time.h>

#ifdef VISUAL
   #include <wtypes.h>
   typedef LONGLONG int64bits;
#else
   #if defined(OSXOLD) || defined(OSXPC)
      // typedef Uint64 int64bits;
      // Changed in later versions of OS X (maybe with BSD switch):
      typedef long long unsigned int int64bits;
      // also see SigTimer.cpp for #define for OSXTIMER
   #else
      typedef long long unsigned int int64bits;
      #include <unistd.h>                 /* for millisleep function */
   #endif
#endif

using TimeSpec = struct timespec;

class SigTimer {
   public:
                       SigTimer           (void);
                       SigTimer           (int aSpeed);
                       SigTimer           (SigTimer& aTimer);
                      ~SigTimer           ();

      void             adjustPeriod       (double periodDelta);
      int              expired            (void);
      double           getPeriod          (void);
      double           getPeriodCount     (void);
      double           getTempo           (void);
      int              getTicksPerSecond  (void);
      int              getTime            (void);
      double           getTimeInSeconds   (void);
      int              getTimeInTicks     (void);
      void             reset              (void);
      void             setPeriod          (double aPeriod);
      void             setTempo           (double beatsPerMinute);
      void             setPeriodCount     (double aCount);
      void             setTicksPerSecond  (int aTickRate);
      void             start              (void);
      void             sync               (SigTimer& aTimer);
      void             update             (void);
      void             update             (int periodCount);

      static int64bits getCpuSpeed        (void);
      static int64bits clockCycles        (TimeSpec& tspec);

   protected:
      static int64bits globalOffset;
      static int64bits cpuSpeed;

      int64bits        offset;
      int              ticksPerSecond;
      double           period;

   // protected functions
      double           getFactor          (void);

	public:
		TimeSpec m_tspec;
};


// The following function is mostly for Linux:
void millisleep(int milliseconds);
void millisleep(float milliseconds);


#endif /* SIGTIMER_H_INCLUDED */



