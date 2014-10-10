/*
 * Copyright (c) 2014, German Aerospace Center (DLR)
 * All Rights Reserved.
 *
 * See the file "LICENSE" for the full license governing this code.
 */
// ----------------------------------------------------------------------------

#ifndef COBC_RTOS_POSIX_QUEUE_H
#define COBC_RTOS_POSIX_QUEUE_H

#include <stddef.h>
#include <stdint.h>

#include <cobc/time/duration.h>

namespace cobc
{
namespace rtos
{

/**
 * Atomic Queue.
 *
 * Can be used to exchange data between different threads.
 *
 * \author  Fabian Greif
 * \ingroup rtos
 */
template <typename T>
class Queue
{
public:
    /**
     * Create a Queue.
     *
     * \param numberOfItems
     *      The maximum number of items that the queue can contain.
     */
    Queue(size_t numberOfItems);

    /**
     * Destroy the queue.
     */
    ~Queue();

    /**
     * Send data to the queue.
     *
     * May trigger a thread rescheduling. The calling thread will be preempted
     * if a higher priority thread is unblocked as the result of this operation.
     *
     * \param data
     *      Reference to the item that is to be placed on the queue.
     *
     * \retval true     Value was successfully stored in the queue.
     * \retval false    Timeout occurred. Queue is full and data could not be
     *                  appended in the specified time.
     */
    bool
    send(const T& data);

    /**
     * Receive data from the queue.
     *
     * \param data
     *      Reference to the buffer into which the received item will be copied.
     * \param timeout
     *      Timeout in milliseconds resolution.
     *
     * \retval true     Value was received correctly and put in \p data.
     * \retval false    Timeout occurred, \p data was not changed.
     */
    bool
    receive(T& data, cobc::time::Duration timeout = cobc::time::Duration::infinity());

private:
    // disable copy constructor
    Queue(const Queue& other);

    // disable assignment operator
    Queue&
    operator=(const Queue& other);

    size_t
    increment(size_t index);

    // POSIX handles
    pthread_mutex_t mutex;
    pthread_cond_t signal;

    T* buffer;
    const size_t maximumSize;
    size_t itemsInBuffer;
    size_t head;
    size_t tail;
};

}
}

#include "queue_impl.h"

#endif // COBC_RTOS_POSIX_QUEUE_H