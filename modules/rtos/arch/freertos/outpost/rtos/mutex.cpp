/*
 * Copyright (c) 2013, German Aerospace Center (DLR)
 *
 * This file is part of outpost 0.6.
 *
 * It is distributed under the terms of the GNU General Public License with a
 * linking exception. See the file "LICENSE" for the full license governing
 * this code.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 */
// ----------------------------------------------------------------------------

#include "mutex.h"

#include <outpost/rtos/failure_handler.h>

// ----------------------------------------------------------------------------
outpost::rtos::Mutex::Mutex()
{
    mHandle = xSemaphoreCreateRecursiveMutex();

    if (mHandle == 0)
    {
        FailureHandler::fatal(FailureCode::resourceAllocationFailed(Resource::mutex));
    }
}

outpost::rtos::Mutex::~Mutex()
{
    vQueueDelete(mHandle);
}

// ----------------------------------------------------------------------------
bool
outpost::rtos::Mutex::acquire()
{
    // wait indefinitely
    return (xSemaphoreTakeRecursive(mHandle, portMAX_DELAY) == pdTRUE);
}

bool
outpost::rtos::Mutex::acquire(time::Duration timeout)
{
    return (xSemaphoreTakeRecursive(mHandle, timeout.milliseconds() * configTICK_RATE_HZ / 1000) == pdTRUE);
}

void
outpost::rtos::Mutex::release()
{
    xSemaphoreGiveRecursive(mHandle);
}