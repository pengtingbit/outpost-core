/*
 * Copyright (c) 2013-2018, German Aerospace Center (DLR)
 *
 * This file is part of the development version of OUTPOST.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * Authors:
 * - 2019, Jan Malburg (DLR RY-AVS)
 */

#ifndef OUTPOST_HAL_PROTOCOL_DISPATCHER_H_
#define OUTPOST_HAL_PROTOCOL_DISPATCHER_H_

#include "protocol_dispatcher_interface.h"
#include "receiver_interface.h"

#include <outpost/base/slice.h>
#include <outpost/rtos.h>
#include <outpost/support/heartbeat.h>
#include <outpost/time/duration.h>
#include <outpost/utils/container/reference_queue.h>
#include <outpost/utils/container/shared_object_pool.h>

#include <stdint.h>

#include <array>

namespace outpost
{
namespace hal
{
template <typename protocolType,   // pod and must support operator=, operator==, and default
                                   // constructor
          uint32_t numberOfQueues  // how many queues can be included
          >
class ProtocolDispatcher : public virtual ProtocolDispatcherInterface<protocolType>
{
public:
    /**
     * @param offset	      number of bytes before the protocol identifier
     */
    ProtocolDispatcher(uint32_t offSet) :
        mNumberOfListeners(0),
        mNumberOfDroppedPackages(0),
        mNumberOfUnmatchedPackages(0),
        mNumberOfPartialPackages(0),
        mNumberOfOverflowedBytes(0),
        mOffset(offSet)
    {
    }

    virtual ~ProtocolDispatcher()
    {
    }

    /**
     * This function is more for future debugging purposes,
     * sets a queue that will get all that no-one else matched
     *
     * @param[in] queue	The queue that will get all packages that are not matched by any regular
     * queue
     * @param[in] pool the pool to get the storage from, the provided memories shall be large enough
     * to fit a package
     * @param[in] dropPartial   if true only complete packages will be put into the queue
     *
     * @return	True 	if successfull
     * 			false 	if nullptr oder already set
     */
    bool
    setDefaultQueue(outpost::utils::SharedBufferPoolBase* pool,
                    outpost::utils::SharedBufferQueueBase* queue,
                    bool dropPartial = false) override;

    /**
     * Adds a queue for a specific protocol id.
     * This call can only succeed numberOfQueues times per dispatcher
     *
     * @param[in] id	The id value to listen to
     * @param[in] pool	The pool to allocate memory from, the provided memories shall be large
     * enough to fit a package of the specific protocol
     * @param[in] queue	The queue to write the values to
     * @param[in] dropPartial   if true only complete packages will be put into the queue
     *
     * @return	true if successful
     * 			false	if nullpointer or all queue places filled up
     */
    bool
    addQueue(protocolType id,
             outpost::utils::SharedBufferPoolBase* pool,
             outpost::utils::SharedBufferQueueBase* queue,
             bool dropPartial = false) override;

    /**
     * Return the number of packages that were dropped for a given queue.
     * If a queue is listening to different protocol the sum is returned
     *
     * @param queue the queue
     * @return	sum of all packages dropped for the given queue
     */
    inline uint32_t
    getNumberOfDroppedPackages(outpost::utils::SharedBufferQueueBase* queue) override;

    /**
     * @return	Returns the number of packages the were totally dropped,
     * 			i.e. don't got any queue or all queues where full
     */
    inline uint32_t
    getNumberOfDroppedPackages() override;

    /**
     * @return Return the number of received packages that were cut because they exceeded
     * maxPacketSize
     */
    inline uint32_t
    getNumberOfPartialPackages() override;

    /**
     * Return the number of packages that were cut for a given queue.
     * If a queue is listening to different protocol the sum is returned
     *
     * @param queue the queue
     * @return	sum of all packages cut for the given queue
     */
    inline uint32_t
    getNumberOfPartialPackages(outpost::utils::SharedBufferQueueBase* queue) override;

    /**
     * @return Return the number of bytes that were cut because packages exceeded maxPacketSize
     */
    inline uint32_t
    getNumberOfOverflowedBytes() override;

    /**
     * Return the number of bytes that were cut for a given queue.
     * If a queue is listening to different protocol the sum is returned
     *
     * @param queue the queue
     * @return	sum of all bytes cut for the given queue
     */
    inline uint32_t
    getNumberOfOverflowedBytes(outpost::utils::SharedBufferQueueBase* queue) override;

    /**
     * @return Returns number of packages that could not be match to any queue,
     *         note will not increase once setRemainingQueue was executed successfully
     */
    inline uint32_t
    getNumberOfUnmatchedPackages() override;

    /**
     * Resets all error counters back to 0
     */
    inline void
    resetErrorCounters() override;

    /**
     * Handles a package
     * @param package	The buffer containing the package
     * @param readBytes	The number of bytes in the packages may be larger then the buffer, in that
     * case the package has been cut
     */
    void
    handlePackage(const outpost::Slice<const uint8_t>& package, uint32_t readBytes) override;

private:
    struct Listener
    {
        Listener() :
            mQueue(nullptr),
            mPool(0),
            mId(0),
            mNumberOfDroppedPackages(0),
            mNumberOfPartialPackages(0),
            mNumberOfOverflowedBytes(0),
            mDropPartial(false){};
        outpost::utils::SharedBufferQueueBase* mQueue;
        outpost::utils::SharedBufferPoolBase* mPool;
        protocolType mId;
        uint32_t mNumberOfDroppedPackages;
        uint32_t mNumberOfPartialPackages;
        uint32_t mNumberOfOverflowedBytes;
        bool mDropPartial;
    };

    bool
    insertIntoQueue(Listener& listener,
                    const outpost::Slice<const uint8_t>& package,
                    uint32_t readBytes);

    // one additional for the match rest one
    std::array<Listener, numberOfQueues> mListeners;
    Listener mDefaultListener;
    uint32_t mNumberOfListeners;
    uint32_t mNumberOfDroppedPackages;
    uint32_t mNumberOfUnmatchedPackages;
    uint32_t mNumberOfPartialPackages;
    uint32_t mNumberOfOverflowedBytes;
    outpost::rtos::Mutex mMutex;

    const uint32_t mOffset;
};

}  // namespace hal
}  // namespace outpost

#include "protocol_dispatcher_impl.h"

#endif /* OUTPOST_HAL_PROTOCOL_DISPATCHER_H_ */
