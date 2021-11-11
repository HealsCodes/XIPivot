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

#ifndef __ASHITA_AS_MEMORY_H_INCLUDED__
#define __ASHITA_AS_MEMORY_H_INCLUDED__

#if defined (_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include <Windows.h>
#include <algorithm>
#include <sstream>
#include <vector>

namespace Ashita
{
    /**
     * Iteratable object to reduce the overhead of using a vector.
     */
    template<typename T>
    struct scannableiterator_t
    {
        uintptr_t m_BaseAddress;
        uintptr_t m_BaseSize;

        scannableiterator_t(uintptr_t base, uintptr_t size)
            : m_BaseAddress(base), m_BaseSize(size)
        { }
        scannableiterator_t(const scannableiterator_t&) = delete;

        T* begin(void) { return (T*)this->m_BaseAddress; }
        T* end(void) { return (T*)(this->m_BaseAddress + this->m_BaseSize); }
    };

    class Memory
    {
    public:
        /**
         * Finds a pattern within the given range of data.
         *
         * @param {uintptr_t} baseAddress - The address of the data to begin searching within.
         * @param {uintptr_t} baseSize - The size of the data to search within.
         * @param {const char*} pattern - The pattern to search for.
         * @param {uintptr_t} offset - The offset from the found location to add to the pointer.
         * @param {uintptr_t} count - The result count to use if the pattern is found more than once.
         * @returns {uintptr_t} The address where the pattern was located, 0 otherwise.
         */
        static uintptr_t FindPattern(uintptr_t baseAddress, uintptr_t baseSize, const char* pattern, uintptr_t offset, uintptr_t count)
        {
            // Ensure the incoming pattern is properly aligned..
            if (strlen(pattern) % 2 > 0)
                return 0;

            // Convert the pattern to a vector of data..
            std::vector<std::pair<uint8_t, bool>> vpattern;
            for (size_t x = 0, y = strlen(pattern) / 2; x < y; x++)
            {
                // Obtain the current byte..
                std::stringstream stream(std::string(pattern + (x * 2), 2));

                // Check if this is a wildcard..
                if (stream.str() == "??")
                    vpattern.push_back(std::make_pair(00, false));
                else
                {
                    auto byte = strtol(stream.str().c_str(), nullptr, 16);
                    vpattern.push_back(std::make_pair((uint8_t)byte, true));
                }
            }

            // Create a scanner object to use with the STL functions..
            scannableiterator_t<uint8_t> data(baseAddress, baseSize);
            auto scanStart = data.begin();
            auto result = (uintptr_t)0;

            while (true)
            {
                // Search for the pattern..
                auto ret = std::search(scanStart, data.end(), vpattern.begin(), vpattern.end(),
                    [&](uint8_t curr, std::pair<uint8_t, bool> currPattern)
                {
                    return (!currPattern.second) || curr == currPattern.first;
                });

                // Did we find a match..
                if (ret != data.end())
                {
                    // If we hit the usage count, return the result..
                    if (result == count || count == 0)
                        return (std::distance(data.begin(), ret) + baseAddress) + offset;

                    // Increment the found count and scan again..
                    ++result;
                    scanStart = ++ret;
                }
                else
                    break;
            }

            return 0;
        }

        /**
         * Obtains the calling module handle for the given address.
         *
         * @param {uintptr_t} returnAddress - The address to locate the module owner of.
         * @returns {HMODULE} The module handle if found, nullptr otherwise.
         */
        static HMODULE __stdcall GetCallingModule(uintptr_t returnAddress)
        {
            if (returnAddress == 0)
                return nullptr;

            MEMORY_BASIC_INFORMATION mbi = { 0 };
            if (::VirtualQuery((LPCVOID)returnAddress, &mbi, sizeof(MEMORY_BASIC_INFORMATION)) == sizeof(MEMORY_BASIC_INFORMATION))
                return (HMODULE)mbi.AllocationBase;

            return nullptr;
        }
    };
}; // namespace Ashita

#endif // __ASHITA_AS_MEMORY_H_INCLUDED__