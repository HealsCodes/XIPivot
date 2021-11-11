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

#ifndef __ASHITA_AS_REGISTRY_H_INCLUDED__
#define __ASHITA_AS_REGISTRY_H_INCLUDED__

#if defined (_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include <Windows.h>

namespace Ashita
{
    /**
     * Language Id Enumeration
     */
    enum class LanguageId : uint32_t {
        Default = 0,
        Japanese = 1,
        English = 2,
        European = 3
    };

    /**
     * Install Path Id Enumeration
     */
    enum class SquareEnixEntity : uint32_t {
        PlayOnline = 0,
        FinalFantasyXI = 1,
        FinalFantasyXI_TestClient = 2,
        TetraMaster = 3
    };

    class Registry
    {
    public:
        /**
         * Obtains the install path for the given Square Enix entity.
         *
         * @param {LanguageId} langId - The language id to use for looking up the proper registry key parent.
         * @param {SquareEnixEntity} entity - The Square Enix entity id to lookup.
         * @param {LPSTR} buffer - The output buffer to hold the path.
         * @param {uint32_t} - The maximum size of the buffer.
         * @returns {bool} True on success, false otherwise.
         */
        static bool GetInstallPath(LanguageId langId, SquareEnixEntity entity, LPSTR buffer, uint32_t bufferSize)
        {
            // Ensure the language id is within a valid range..
            if ((uint32_t)langId < 0 || (uint32_t)langId > 4)
                return false;

            const char languageTags[4][255] = { "US", "", "US", "EU" };
            const char installFolder[4][255] = { "1000", "0001", "0015", "0002" };

            // Build the registry key to read from..
            char registryPath[MAX_PATH] = { 0 };
            sprintf_s(registryPath, MAX_PATH, "SOFTWARE\\PlayOnline%s\\InstallFolder", languageTags[(uint32_t)langId]);

            // Open the key for reading..
            HKEY key = nullptr;
            if (!(::RegOpenKeyExA(HKEY_LOCAL_MACHINE, registryPath, 0, KEY_QUERY_VALUE | KEY_WOW64_32KEY, &key) == ERROR_SUCCESS))
                return false;

            // Read the install path from the registry..
            char installPath[MAX_PATH] = { 0 };
            DWORD size = MAX_PATH;
            DWORD type = REG_DWORD;
            if (!(::RegQueryValueExA(key, installFolder[(uint32_t)entity], nullptr, &type, (LPBYTE)installPath, &size) == ERROR_SUCCESS))
            {
                ::RegCloseKey(key);
                return false;
            }
            ::RegCloseKey(key);

            // Ensure our buffer is large enough..
            if (strlen(installPath) > bufferSize)
                return false;

            memcpy_s(buffer, bufferSize, installPath, size);
            return true;
        }

        /**
         * Sets a value in the registry.
         *
         * @param {LanguageId} langId - The language id to use for writing to the proper registry key parent.
         * @param {const char*} parent - The inner parent that holds the key we want to write to.
         * @param {const char*} keyName - The key name to write the value to.
         * @param {uint32_t} value - The value to write to the key.
         * @returns {bool} True on success, false otherwise.
         */
        static bool SetValue(LanguageId langId, const char* parent, const char* keyName, uint32_t value)
        {
            // Ensure the language id is within a valid range..
            if ((uint32_t)langId < 0 || (uint32_t)langId > 4)
                return false;

            const char languageTags[4][255] = { "US", "", "US", "EU" };

            // Build the registry key to read from..
            char registryPath[MAX_PATH] = { 0 };
            sprintf_s(registryPath, MAX_PATH, "SOFTWARE\\PlayOnline%s\\%s", languageTags[(uint32_t)langId], parent);

            // Open the key for writing..
            HKEY key = nullptr;
            if (!(::RegOpenKeyExA(HKEY_LOCAL_MACHINE, registryPath, 0, KEY_WRITE | KEY_QUERY_VALUE | KEY_WOW64_32KEY, &key) == ERROR_SUCCESS))
                return false;

            // Write the value..
            auto ret = ::RegSetValueExA(key, keyName, NULL, REG_DWORD, (LPBYTE)&value, sizeof(DWORD));
            ::RegCloseKey(key);

            return ret == ERROR_SUCCESS;
        }
    };
}; // namespace Ashita

#endif // __ASHITA_AS_REGISTRY_H_INCLUDED__