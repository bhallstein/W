/*
-----------------------------------------------------------------------------
This source file is part of OGRE
    (Object-oriented Graphics Rendering Engine)
For the latest info, see http://www.ogre3d.org/

Copyright (c) 2000-2011 Torus Knot Software Ltd

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
-----------------------------------------------------------------------------
*/

/*
  Timer implementation from the OGRE source code, combined into standalone
  windows/mac-compatible file 14.8.12 - Ben Hallstein
*/

#ifndef Timer_H
#define Timer_H

#ifdef __APPLE__
	#include <sys/time.h>

#elif defined _WIN32 || _WIN64
	#include <time.h>

	#ifndef WIN32_LEAN_AND_MEAN
		#define WIN32_LEAN_AND_MEAN
	#endif

	#if !defined(NOMINMAX) && defined(_MSC_VER)
		#define NOMINMAX // required to stop windows.h messing up std::min
	#endif

	#include "windows.h"

#endif

namespace W
{
	
	class Timer {
    public:
		Timer();
		~Timer();
		void reset();		// Reset timer
		unsigned long getMilliseconds();	// ms since init/reset
		unsigned long getMicroseconds();	// µs since init/reset
		unsigned long getMillisecondsCPU();	// "only cpu time measured"
		unsigned long getMicrosecondsCPU();	// ~
	private:
		#ifdef __APPLE__
			struct timeval start;
			clock_t zeroClock;
		#elif defined _WIN32 || _WIN64
			clock_t mZeroClock;
			DWORD mStartTick;
			LONGLONG mLastTime;
			LARGE_INTEGER mStartTime;
			LARGE_INTEGER mFrequency;
			DWORD_PTR mTimerMask;
		#endif
    };
	
}

#endif
