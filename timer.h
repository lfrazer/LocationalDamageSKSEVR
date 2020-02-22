/*
	VRCustomQuickslots - VR Custom Quickslots SKSE extension for SkyrimVR
	Copyright (C) 2018 L Frazer
	https://github.com/lfrazer

	This file is part of VRCustomQuickslots.

	VRCustomQuickslots is free software: you can redistribute it and/or modify
	it under the terms of the GNU Lesser General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	VRCustomQuickslots is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU Lesser General Public License for more details.

	You should have received a copy of the GNU Lesser General Public License
	along with VRCustomQuickslots.  If not, see <https://www.gnu.org/licenses/>.
*/

#ifndef __TIMER_H
#define __TIMER_H


#ifndef WIN32
#include <inttypes.h>
#define UINT64 uint64_t

#else
#include <windows.h>
#endif

#include <ctime>

class CTimer
{
	
public:
	CTimer(void);		// constructor
	void Init(void);	// init func
	double GetTime(void); // Get current CPU clock accurate time
	double GetTimeSlice(void); // time since last timer update
	double GetLastTime(void);  // last time calculated either by GetTimeSlice

	double GetAbsoluteTimeSinceStart(); // get the time since the program started, ignoring pause subtraction time
	void TimerUpdate(void); // update the timer once per loop to use time slice
	void Pause(void); // pause timer
	void Unpause(void); // unpause timer
	UINT64 GetLastRawTime();

	static time_t GetUnixTimestamp();
	static time_t ConvertWebTimeToTimestamp(const char* webtime);

private:
	double mStartTime = 0.0;
	double mLastTime = 0.0;
	double mTimeSlice = 0.0;
	double mPauseTime = 0.0;
	double mPauseTotalSubtraction = 0.0;
	UINT64 mTicksPerSecond = 0;
	UINT64 mStartTicks = 0;
	int mInitComplete = 0;
	UINT64 mLastRawTime = 0;


};


#endif


