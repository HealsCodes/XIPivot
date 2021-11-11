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

#ifndef __ASHITA_AS_THREAD_H_INCLUDED__
#define __ASHITA_AS_THREAD_H_INCLUDED__

#if defined (_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include <Windows.h>
#include "AS_Event.h"

namespace Ashita
{
    namespace Threading
    {
        /**
         * Thread Priority Enumeration
         */
        enum class ThreadPriority : int32_t {
            Lowest = -2,
            BelowNormal = -1,
            Normal = 0,
            AboveNormal = 1,
            Highest = 2
        };

        class AS_Thread
        {
            HANDLE      m_ThreadHandle;
            uint32_t    m_ThreadId;
            AS_Event    m_StartEvent;
            AS_Event    m_EndEvent;

        public:
            AS_Thread(void)
                : m_ThreadHandle(nullptr)
                , m_ThreadId(0)
                , m_StartEvent(true)
                , m_EndEvent(true)
            { }
            virtual ~AS_Thread(void)
            {
                if (this->m_ThreadHandle != nullptr)
                    this->Stop();
            }

        public:
            /**
             * Thread entry function the inheriting class must implement.
             *
             * @returns {uint32_t} The thread functions return value.
             */
            virtual uint32_t ThreadEntry(void) = 0;

        public:
            /**
             * Thread entry function used to signal the thread and run the inheriting
             * classes thread entry function.
             *
             * @returns {uint32_t} The thread functions return value.
             */
            uint32_t InternalEntry(void)
            {
                if (this->IsTerminated())
                    return 0;

                this->m_EndEvent.Reset();
                ::Sleep(10);
                this->m_StartEvent.Raise();

                return this->ThreadEntry();
            }

            /**
             * Starts the thread.
             */
            void Start()
            {
                this->m_StartEvent.Reset();
                this->m_ThreadHandle = ::CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)ThreadCallback, (LPVOID)this, 0, (LPDWORD)&this->m_ThreadId);
            }

            /**
             * Stops the thread.
             */
            void Stop(void)
            {
                this->RaiseEnd();

                if (this->WaitForFinish(INFINITE))
                {
                    ::CloseHandle(this->m_ThreadHandle);
                    this->m_ThreadHandle = nullptr;
                    this->m_ThreadId = 0;
                }
            }

            /**
             * Waits the given amount of milliseconds for the thread handle to signal.
             *
             * @param {uint32_t} milliseconds - The amount of time, in milliseconds, to wait.
             * @returns {uint32_t} True on success, false otherwise.
             */
            bool WaitForFinish(uint32_t milliseconds = INFINITE) const
            {
                if (this->m_ThreadHandle == nullptr)
                    return false;

                return ::WaitForSingleObject(this->m_ThreadHandle, milliseconds) != WAIT_TIMEOUT;
            }

            /**
             * Sets the threads priority.
             *
             * @param {ThreadPriority} p - The thread priority to set the thread to.
             */
            void SetPriority(ThreadPriority p) const
            {
                if (this->m_ThreadHandle != nullptr)
                    ::SetThreadPriority(this->m_ThreadHandle, (int32_t)p);
            }

            /**
             * Gets the threads priority.
             *
             * @returns {ThreadPriority} The threads priority.
             */
            ThreadPriority GetPriority(void) const
            {
                if (this->m_ThreadHandle == nullptr)
                    return (ThreadPriority)0;

                return (ThreadPriority)::GetThreadPriority(this->m_ThreadHandle);
            }

            /**
             * Signals the end event telling the thread it should stop.
             */
            void RaiseEnd(void)
            {
                this->m_EndEvent.Raise();
            }

            /**
             * Resets the end event signal.
             */
            void ResetEnd(void)
            {
                this->m_EndEvent.Reset();
            }

            /**
             * Returns if the thread has been terminated or not.
             *
             * @returns {bool} True if the thread is terminated, false otherwise.
             */
            bool IsTerminated(void) const
            {
                return this->m_EndEvent.IsSignaled();
            }

        public:
            /**
             * Returns the threads handle.
             *
             * @returns {HANDLE} This threads handle.
             */
            HANDLE GetHandle(void) const { return this->m_ThreadHandle; }

            /**
             * Returns the threads id.
             *
             * @returns {uint32_t} This threads id.
             */
            uint32_t GetId(void) const { return this->m_ThreadId; }

            /**
             * Returns the threads exit code.
             *
             * @returns {uint32_t} This threads exit code.
             */
            uint32_t GetExitCode(void) const
            {
                if (this->m_ThreadHandle == nullptr)
                    return 0;

                uint32_t exitCode = 0;
                ::GetExitCodeThread(this->m_ThreadHandle, (LPDWORD)&exitCode);

                return exitCode;
            }

        private:
            /**
             * Internal thread callback to invoke the inheriting objects thread handler.
             *
             * @param {LPVOID} param - The AS_Thread object passed to this callback.
             * @returns {uint32_t} The internal threads return value, 0 otherwise.
             */
            static uint32_t __stdcall ThreadCallback(LPVOID param)
            {
                auto thread = (AS_Thread*)param;
                if (thread != nullptr)
                    return thread->InternalEntry();

                return 0;
            }
        };
    }; // namespace Threading
}; // namespace Ashita

#endif // __ASHITA_AS_THREAD_H_INCLUDED__