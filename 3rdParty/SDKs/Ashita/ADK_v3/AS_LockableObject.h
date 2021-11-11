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

#ifndef __ASHITA_AS_LOCKABLEOBJECT_H_INCLUDED__
#define __ASHITA_AS_LOCKABLEOBJECT_H_INCLUDED__

#if defined (_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include <Windows.h>

namespace Ashita
{
    namespace Threading
    {
        class AS_LockableObject
        {
            CRITICAL_SECTION m_CriticalSection;

            // Delete the copy constructor.
            AS_LockableObject(const AS_LockableObject& obj) = delete;

        public:
            /**
             * Constructor and Deconstructor
             */
            AS_LockableObject(void)
            {
                ::InitializeCriticalSection(&this->m_CriticalSection);
            }
            virtual AS_LockableObject::~AS_LockableObject(void)
            {
                ::DeleteCriticalSection(&this->m_CriticalSection);
            }

        public:
            /**
             * Locks the object for exclusive usage.
             */
            void Lock(void)
            {
                ::EnterCriticalSection(&this->m_CriticalSection);
            }

            /**
             * Unlocks the object from exclusive usage.
             */
            void Unlock(void)
            {
                ::LeaveCriticalSection(&this->m_CriticalSection);
            }
        };
    }; // namespace Threading
}; // namespace Ashita

#endif // __ASHITA_AS_LOCKABLEOBJECT_H_INCLUDED__