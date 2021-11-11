/**
 * Ashita - Copyright (c) 2014 - 2021 atom0s [atom0s@live.com]
 *
 * This work is licensed under the Creative Commons Attribution-NonCommercial-NoDerivatives 4.0 International License.
 * To view a copy of this license, visit http://creativecommons.org/licenses/by-nc-nd/4.0/ or send a letter to
 * Creative Commons, PO Box 1866, Mountain View, CA 94042, USA.
 *
 * By using Ashita, you agree to the above license and its terms.
 *
 *      Attribution - You must give appropriate credit, provide a link to the license and indicate if changes were
 *                    made. You must do so in any reasonable manner, but not in any way that suggests the licensor
 *                    endorses you or your use.
 *
 *   Non-Commercial - You may not use the material (Ashita) for commercial purposes.
 *
 *   No-Derivatives - If you remix, transform, or build upon the material (Ashita), you may not distribute the
 *                    modified material. You are, however, allowed to submit the modified works back to the original
 *                    Ashita project in attempt to have it added to the original project.
 *
 * You may not apply legal terms or technological measures that legally restrict others
 * from doing anything the license permits.
 *
 * No warranties are given.
 */

#ifndef __ASHITA_AS_BINARYDATA_H_INCLUDED__
#define __ASHITA_AS_BINARYDATA_H_INCLUDED__

#if defined (_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/**
 * Credits to the original ProjectXI team for these functions which is where they have
 * originated from. (See CBasePacket of the ProjectXI source code base.)
 */

namespace Ashita
{
    class BinaryData
    {
    public:
        /**
         * Packs data into the given buffer. (Big Endian)
         */
        static uint32_t PackBitsBE(uint8_t* data, uint64_t value, uint32_t offset, uint8_t len)
        {
            return PackBitsBE(data, value, 0, offset, len);
        }

        /**
         * Packs data into the given buffer. (Big Endian)
         */
        static uint32_t PackBitsBE(uint8_t* data, uint64_t value, uint32_t byteOffset, uint32_t bitOffset, uint8_t len)
        {
            byteOffset += bitOffset >> 3;
            bitOffset %= 8;

            auto bitmask = (uint64_t)0xFFFFFFFFFFFFFFFFLL;
            bitmask >>= 64 - len;
            bitmask <<= bitOffset;
            value <<= bitOffset;
            value &= bitmask;
            bitmask ^= 0xFFFFFFFFFFFFFFFFLL;

            if (len + bitOffset <= 8)
            {
                auto dataPointer = (uint8_t*)&data[byteOffset];
                auto bitmaskUC = (uint8_t)bitmask;
                auto valueUC = (uint8_t)value;
                *dataPointer &= bitmaskUC;
                *dataPointer |= valueUC;
            }
            else if (len + bitOffset <= 16)
            {
                auto dataPointer = (uint16_t*)&data[byteOffset];
                auto bitmaskUC = (uint16_t)bitmask;
                auto valueUC = (uint16_t)value;
                *dataPointer &= bitmaskUC;
                *dataPointer |= valueUC;
            }
            else if (len + bitOffset <= 32)
            {
                auto dataPointer = (uint32_t*)&data[byteOffset];
                auto bitmaskUC = (uint32_t)bitmask;
                auto valueUC = (uint32_t)value;
                *dataPointer &= bitmaskUC;
                *dataPointer |= valueUC;
            }
            else if (len + bitOffset <= 64)
            {
                auto dataPointer = (uint64_t*)&data[byteOffset];
                *dataPointer &= bitmask;
                *dataPointer |= value;
            }
            else
            {
                // We should not get here.. an error occurred..
                // data size > 64bits
            }

            return (byteOffset << 3) + bitOffset + len;
        }

        /**
         * Unpacks data from the given buffer. (Big Endian)
         */
        static uint64_t UnpackBitsBE(uint8_t* data, uint32_t offset, uint8_t len)
        {
            return UnpackBitsBE(data, 0, offset, len);
        }

        /**
         * Unpacks data from the given buffer. (Big Endian)
         */
        static uint64_t UnpackBitsBE(uint8_t* data, uint32_t byteOffset, uint32_t bitOffset, uint8_t len)
        {
            byteOffset += (bitOffset >> 3);
            bitOffset %= 8;

            uint64_t retVal;
            auto bitmask = (uint64_t)0xFFFFFFFFFFFFFFFFLL;
            bitmask >>= (64 - len);
            bitmask <<= bitOffset;

            if (len + bitOffset <= 8)
            {
                auto dataPointer = (uint8_t*)&data[byteOffset];
                retVal = (*dataPointer&(uint8_t)bitmask) >> bitOffset;
            }
            else if (len + bitOffset <= 16)
            {
                auto dataPointer = (uint16_t*)&data[byteOffset];
                retVal = (*dataPointer&(uint16_t)bitmask) >> bitOffset;
            }
            else if (len + bitOffset <= 32)
            {
                auto dataPointer = (uint32_t*)&data[byteOffset];
                retVal = (*dataPointer&(uint32_t)bitmask) >> bitOffset;
            }
            else if (len + bitOffset <= 64)
            {
                auto dataPointer = (uint64_t*)&data[byteOffset];
                retVal = (*dataPointer&(uint64_t)bitmask) >> bitOffset;
            }
            else
            {
                // We should not get here.. an error occurred..
                // data size > 64bits
                return 0;
            }

            return retVal;
        }

        /**
         * Packs data into the given buffer. (Little Endian)
         */
        static uint32_t PackBitsLE(uint8_t* data, uint64_t value, uint32_t offset, uint8_t len)
        {
            return PackBitsLE(data, value, 0, offset, len);
        }

        /**
         * Packs data into the given buffer. (Little Endian)
         */
        static uint32_t PackBitsLE(uint8_t* data, uint64_t value, uint32_t byteOffset, uint32_t bitOffset, uint8_t len)
        {
            byteOffset += bitOffset >> 3;
            bitOffset %= 8;

            uint8_t bytesNeeded;
            if (bitOffset + len <= 8)
                bytesNeeded = 1;
            else if (bitOffset + len <= 16)
                bytesNeeded = 2;
            else if (bitOffset + len <= 32)
                bytesNeeded = 4;
            else if (bitOffset + len <= 64)
                bytesNeeded = 8;
            else
            {
                // We should not get here.. an error occurred..
                // data size > 64bits
                return 0;
            }

            auto modifieddata = new uint8_t[bytesNeeded];
            for (uint8_t curByte = 0; curByte < bytesNeeded; ++curByte)
            {
                modifieddata[curByte] = data[byteOffset + (bytesNeeded - 1) - curByte];
            }

            int newBitOffset = (bytesNeeded << 3) - (bitOffset + len);
            PackBitsBE(&modifieddata[0], value, 0, newBitOffset, len);

            for (uint8_t curByte = 0; curByte < bytesNeeded; ++curByte)
            {
                data[byteOffset + (bytesNeeded - 1) - curByte] = modifieddata[curByte];
            }

            if (modifieddata)
                delete[] modifieddata;

            return (byteOffset << 3) + bitOffset + len;
        }

        /**
         * Unpacks data from the given buffer. (Little Endian)
         */
        static uint64_t UnpackBitsLE(uint8_t* data, uint32_t offset, uint8_t len)
        {
            return UnpackBitsLE(data, 0, offset, len);
        }

        /**
         * Unpacks data from the given buffer. (Little Endian)
         */
        static uint64_t UnpackBitsLE(uint8_t* data, uint32_t byteOffset, uint32_t bitOffset, uint8_t len)
        {
            byteOffset += bitOffset >> 3;
            bitOffset %= 8;

            uint8_t bytesNeeded;
            if (bitOffset + len <= 8)
                bytesNeeded = 1;
            else if (bitOffset + len <= 16)
                bytesNeeded = 2;
            else if (bitOffset + len <= 32)
                bytesNeeded = 4;
            else if (bitOffset + len <= 64)
                bytesNeeded = 8;
            else
            {
                // We should not get here.. an error occurred..
                // data size > 64bits
                return 0;
            }

            uint64_t retVal;

            auto modifieddata = new uint8_t[bytesNeeded];
            for (uint8_t curByte = 0; curByte < bytesNeeded; ++curByte)
            {
                modifieddata[curByte] = data[byteOffset + (bytesNeeded - 1) - curByte];
            }

            if (bytesNeeded == 1)
            {
                uint8_t bitmask = 0xFF >> bitOffset;
                retVal = (modifieddata[0] & bitmask) >> (8 - (len + bitOffset));
            }
            else
            {
                int newBitOffset = (bytesNeeded * 8) - (bitOffset + len);
                retVal = UnpackBitsBE(&modifieddata[0], 0, newBitOffset, len);
            }

            if (modifieddata)
                delete[] modifieddata;

            return retVal;
        }
    };
}; // Ashita

#endif // __ASHITA_AS_BINARYDATA_H_INCLUDED__