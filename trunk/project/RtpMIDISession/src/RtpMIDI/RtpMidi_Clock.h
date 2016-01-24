#pragma once

#include <limits.h>

#ifndef ANDROID
#include "mach/clock.h"
#include "mach/mach_init.h"
#include "mach/mach_host.h"
#include "mach/mach_port.h"
#endif

#define MSEC_PER_SEC 1000
#define USEC_PER_SEC 1000000
#define NSEC_PER_SEC 1000000000


typedef struct RtpMidi_Clock {

	uint32_t clockRate_;

    unsigned long startTime_;
    uint32_t timestamp_;

	void Init(uint32_t initialTimeStamp, uint32_t clockRate)
	{
		timestamp_ = initialTimeStamp;
		clockRate_ = clockRate;

		if( clockRate_ == 0 )
			clockRate_ = (USEC_PER_SEC / 1);

        startTime_ = Ticks();
	}

    static unsigned long millis()
    {
        unsigned long ret=0;
#ifdef ANDROID
        struct timespec monotime;
        clock_gettime(CLOCK_MONOTONIC, &monotime);
        ret=monotime.tv_nsec;
#else
        clock_serv_t cclock;
        mach_timespec_t mts;

        host_get_clock_service(mach_host_self(), SYSTEM_CLOCK, &cclock);
        clock_get_time(cclock, &mts);
        ret=mts.tv_nsec;
        mach_port_deallocate(mach_task_self(), cclock);
#endif
        ret/=MSEC_PER_SEC;
        return ret;
    }

    /// <summary>
    ///     Returns an timestamp value suitable for inclusion in a RTP packet header.
    /// </summary>
    uint32_t _RelativeNow()
    {
        return CalculateCurrentTimeStamp();
    }
    uint32_t Now()
    {
        return CalculateCurrentTimeStamp()+startTime_;
    }

    /// <summary>
    ///     Returns the time spent since the initial clock timestamp value.
    ///     The returned value is expressed in units of "clock pulsations",
    ///     that are equivalent to seconds, scaled by the clock rate.
    ///     i.e: 1 second difference will result in a delta value equals to the clock rate.
    /// </summary>
    uint32_t Delta()
    {
        return CalculateTimeSpent();
    }

	uint32_t CalculateCurrentTimeStamp()
	{
		uint32_t lapse = CalculateTimeSpent();

		// check for potential overflow
		if (timestamp_ + lapse < UINT_MAX )
			return timestamp_ + lapse;

		uint32_t remainder = UINT_MAX  - timestamp_;
		return lapse - remainder;
	}

	uint32_t CalculateTimeSpent()
	{
	    unsigned long millis_=millis();
		unsigned long ticks =  millis_- startTime_;
		unsigned long seconds = ticks / TicksPerSecond();

		uint32_t lapse = (uint32_t)((double) seconds * clockRate_);
		return lapse;
	}

	unsigned long Ticks()
	{
		return millis();
	}

	unsigned long TicksPerSecond()
	{
		return 1000;
	}

} RtpMidiClock_t;

