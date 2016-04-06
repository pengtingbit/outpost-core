/*
 * Copyright (c) 2013, German Aerospace Center (DLR)
 *
 * This file is part of libCOBC 0.5.
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

#ifndef COBC_HAL_SPACEWIRE_H
#define COBC_HAL_SPACEWIRE_H

#include <stdint.h>
#include <cstddef>

#include <cobc/time/duration.h>

namespace cobc
{
namespace hal
{

/**
 * SpaceWire Interface
 *
 * \author    Fabian Greif
 */
class SpaceWire
{
public:
    enum EndMarker
    {
        partial = 0,    ///< Leave packet open to add additional data later
        eop = 1,        ///< End of packet
        eep = 2         ///< Error end of packet
    };

    struct Result
    {
        enum Type
        {
            success,
            failure,
            timeout
        };
    };

    /**
     * Send buffer.
     */
    struct TransmitBuffer
    {
        TransmitBuffer(uint8_t* d) :
            data(d),
            length(),
            end(eop)
        {
        }

        /**
         * Points to preallocated memory section.
         * Maximum size is implementation specific.
         */
        uint8_t* const data;
        size_t length;
        EndMarker end;
    };

    struct ReceiveBuffer
    {
        ReceiveBuffer() :
            data(0), length(0), end(eop)
        {
        }

        const ReceiveBuffer&
        operator=(const ReceiveBuffer& other)
        {
            data = other.data;
            length = other.length;
            end = other.end;

            return *this;
        }

        const uint8_t* data;
        size_t length;
        EndMarker end;
    };

    virtual
    ~SpaceWire();

    /**
     * Get the maximum length of a SpaceWire packet.
     */
    virtual size_t
    getMaximumPacketLength() const = 0;

    /**
     * Open a configured SpaceWire device.
     *
     * \retval  true    If the device could be opened.
     * \retval  false   otherwise.
     */
    virtual bool
    open() = 0;

    virtual void
    close() = 0;

    virtual bool
    up(cobc::time::Duration timeout) = 0;

    virtual void
    down(cobc::time::Duration timeout) = 0;

    virtual bool
    isUp() = 0;


    /**
     * Request a send buffer.
     *
     * The SpaceWire link is blocked until the buffer is returned
     * by sending the buffer via send()!
     *
     * \param[out]  buffer
     *      Pointer to a send buffer. 0 in a case of a failure.
     * \param[in]   timeout
     *      Time to wait for a free transmit buffer.
     */
    virtual Result::Type
    requestBuffer(TransmitBuffer*& buffer,
                  cobc::time::Duration timeout) = 0;

    /**
     * Send a configured buffer.
     *
     * This releases the buffer.
     * The buffer must contain a complete message.
     *
     * \param[out]  buffer
     *      Pointer to a send buffer. Must be the same pointer which
     *      was requested via requestBuffer() earlier.
     */
    virtual Result::Type
    send(TransmitBuffer* buffer) = 0;

    /**
     * Receive data.
     *
     * \param[out]  buffer
     *      Pointer to a receive buffer. 0 in a case of a failure.
     * \param[in]   timeout
     *      Time to wait for a SpaceWire message to arrive.
     */
    virtual Result::Type
    receive(ReceiveBuffer& buffer,
            cobc::time::Duration timeout) = 0;

    /**
     * Release receive buffer.
     *
     * \param[in]   buffer
     *      Send buffer. Must be the same pointer which was acquired
     *      by a receive() earlier.
     */
    virtual void
    releaseBuffer(const ReceiveBuffer& buffer) = 0;

    /**
     * Discard all messages currently waiting in the receive buffers.
     */
    virtual void
    flushReceiveBuffer() = 0;
};

}
}

#endif // COBC_HAL_SPACEWIRE_H
