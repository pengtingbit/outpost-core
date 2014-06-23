/*
 * Copyright (c) 2013, German Aerospace Center (DLR)
 * All Rights Reserved.
 *
 * See the file "LICENSE" for the full license governing this code.
 */

#ifndef COBC_RTOS_NONE_TIMER_H
#define COBC_RTOS_NONE_TIMER_H

#include <stddef.h>
#include <cobc/time/duration.h>

#include <cobc/rtos/callable.h>

namespace cobc
{
    namespace rtos
    {
        /**
         * Software timer.
         *
         * The timer callback functions are called in the context of the
         * timer server thread.
         *
         * The Timer Server thread is responsible for executing the timer
         * service routines associated with all task-based timers.
         *
         * The Timer Server is designed to remain blocked until a task-based
         * timer fires. This reduces the execution overhead of the Timer Server.
         *
         * \author    Fabian Greif
         * \ingroup    rtos
         */
        class Timer
        {
        public:
            /**
             * Type of the timer handler function.
             *
             * \param timer
             *         Timer which caused the function to be called. Can be used
             *         to restart the timer.
             */
            typedef void (Callable::*Function)(Timer *timer);

            /**
             * Create a timer.
             *
             * \param object
             *         Instance to with the function to be called belongs. Must
             *         be sub-class of cobc::rtos::Callable.
             * \param function
             *         Member function of \p object to call when the timer
             *         expires.
             * \param name
             *         Name of the timer. Maximum length is four characters. Longer
             *         names will be truncated.
             *
             * \see    cobc::Callable
             */
            template <typename T>
            Timer(T *object, void (T::*function)(Timer *timer), const char* name = "TIM-");

            /**
             * Delete the timer.
             *
             * If the timer is running, it is automatically canceled. All it's
             * allocated resources are reclaimed and can be used for another
             * timer.
             */
            ~Timer();

            /**
             * Start the timer.
             *
             * If the timer is running it is automatically reset before being
             * initiated.
             *
             * \param duration
             *         Runtime duration.
             */
            void
            start(time::Duration duration);

            /**
             * Reset the timer interval to it's original value when it is
             * currently running.
             */
            void
            reset();

            /**
             * Abort operation.
             *
             * The timer will not fire until the next invocation of reset() or
             * start().
             */
            void
            cancel();

        private:
            void
            createTimer(const char* name);

            /// Object and member function to call when the timer expires.
            Callable * const object;
            Function const function;
        };
    }
}

// ----------------------------------------------------------------------------
// Implementation of the template constructor
template <typename T>
cobc::rtos::Timer::Timer(T *object, void (T::*function)(Timer *timer), const char* name) :
    object(reinterpret_cast<Callable *>(object)),
    function(reinterpret_cast<Function>(function))
{
    this->createTimer(name);
}

#endif // COBC_RTOS_RTEMS_TIMER_H