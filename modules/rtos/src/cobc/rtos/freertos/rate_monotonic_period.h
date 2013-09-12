/*
 * Copyright (c) 2013, German Aerospace Center (DLR)
 * All Rights Reserved.
 *
 * See the file "LICENSE" for the full license governing this code.
 */

#ifndef COBC_RTOS_FREERTOS_RATE_MONOTONIC_PERIOD_H
#define COBC_RTOS_FREERTOS_RATE_MONOTONIC_PERIOD_H

#include <cobc/time/duration.h>

#include <freertos/FreeRTOS.h>

#include "../failure_handler.h"

namespace cobc
{
	namespace rtos
	{
		/**
		 * Helper class for Rate-Monotonic Scheduling (RMS).
		 *
		 * Required the FreeRTOS function vTaskDelayUntil().
		 *
		 * @author	Fabian Greif
		 * @ingroup	rtos
		 */
		class RateMonotonicPeriod
		{
		public:
			enum Status
			{
				/// Period has not been started
				IDLE,

				/// Period is currently running
				RUNNING,

				/// Period has expired
				TIMEOUT
			};

			RateMonotonicPeriod();

			~RateMonotonicPeriod();

			/**
			 * Start next period.
			 *
			 * If the rate monotonic period is running, the calling thread will
			 * be blocked for the remainder of the outstanding period and,
			 * upon completion of that period, the period will be reinitialized
			 * with the specified period.
			 *
			 * If the rate monotonic period is not currently running and has
			 * not expired, it is initiated with a length of period ticks and
			 * the calling task returns immediately.
			 *
			 * If the rate monotonic period has expired before the thread invokes
			 * the `nextPeriod` method, the period will be initiated with a
			 * length of *period* and the calling task returns immediately with
			 * a timeout error status.
			 *
			 * @param  period
			 *     Length of the next period. Can be different from the
			 *     previous one.
			 *
			 * @retval	RUNNING
			 *     Period is currently running.
			 * @retval  TIMEOUT
			 *     Last period was missed, this may require some different
			 *     handling from the user.
			 */
			Status
			nextPeriod(time::Duration period);

			/**
			 * Check the status of the current period.
			 *
			 * @retval  IDLE
			 *     Period has not been started.
			 * @retval	RUNNING
			 *     Period is currently running.
			 * @retval  TIMEOUT
			 *     Last period was missed, this may require some different
			 *     handling from the user.
			 */
			Status
			status();

			/**
			 * Period measurement is stopped.
			 *
			 * Can be restarted with the invocation of `nextPeriod`.
			 */
			void
			cancel();

		private:
			bool running;
			portTickType lastWakeTime;
			portTickType currentPeriod;
		};
	}
}

#endif // COBC_RTOS_FREERTOS_RATE_MONOTONIC_PERIOD_H
