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

#ifndef __ASHITA_AS_EXCEPTION_H_INCLUDED__
#define __ASHITA_AS_EXCEPTION_H_INCLUDED__

#if defined (_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include <Windows.h>
#include <eh.h>

// Missing Status Code Defines
#ifndef STATUS_POSSIBLE_DEADLOCK
#define STATUS_POSSIBLE_DEADLOCK 0xC0000194
#endif

// Macro For Handling Exception Cases
#define CASE(Exception) \
    case Exception: \
        this->m_Exception = #Exception; \
        break;

namespace Ashita
{
    namespace Exception
    {
        class AS_Exception
        {
            uint32_t        m_ExceptionId;
            const char*     m_Exception;

        public:
            /**
             * Constructor and Deconstructor
             */
            explicit AS_Exception(uint32_t id)
                : m_ExceptionId(id)
                , m_Exception(nullptr)
            {
                switch (this->m_ExceptionId)
                {
                    CASE(EXCEPTION_ACCESS_VIOLATION);
                    CASE(EXCEPTION_ARRAY_BOUNDS_EXCEEDED);
                    CASE(EXCEPTION_BREAKPOINT);
                    CASE(EXCEPTION_DATATYPE_MISALIGNMENT);
                    CASE(EXCEPTION_FLT_DENORMAL_OPERAND);
                    CASE(EXCEPTION_FLT_DIVIDE_BY_ZERO);
                    CASE(EXCEPTION_FLT_INEXACT_RESULT);
                    CASE(EXCEPTION_FLT_INVALID_OPERATION);
                    CASE(EXCEPTION_FLT_OVERFLOW);
                    CASE(EXCEPTION_FLT_STACK_CHECK);
                    CASE(EXCEPTION_FLT_UNDERFLOW);
                    CASE(EXCEPTION_GUARD_PAGE);
                    CASE(EXCEPTION_ILLEGAL_INSTRUCTION);
                    CASE(EXCEPTION_IN_PAGE_ERROR);
                    CASE(EXCEPTION_INT_DIVIDE_BY_ZERO);
                    CASE(EXCEPTION_INT_OVERFLOW);
                    CASE(EXCEPTION_INVALID_DISPOSITION);
                    CASE(EXCEPTION_INVALID_HANDLE);
                    CASE(EXCEPTION_NONCONTINUABLE_EXCEPTION);
                    CASE(EXCEPTION_POSSIBLE_DEADLOCK);
                    CASE(EXCEPTION_PRIV_INSTRUCTION);
                    CASE(EXCEPTION_SINGLE_STEP);
                    CASE(EXCEPTION_STACK_OVERFLOW);

                default:
                    this->m_Exception = "Unknown exception occurred.";
                    break;
                }
            }
            ~AS_Exception(void) { }

        public:
            /**
             * Returns the exception id of the current wrapped exception.
             *
             * @returns {uint32_t} The exception id.
             */
            uint32_t GetExceptionId(void) const
            {
                return this->m_ExceptionId;
            }
            const char* GetException(void) const
            {
                return this->m_Exception;
            }
        };

        class ScopedTranslator
        {
            _se_translator_function m_Function;

        public:
            /**
             * Constructor and Deconstructor
             */
            ScopedTranslator(void)
            {
                this->m_Function = ::_set_se_translator(&ScopedTranslator::ScopedTranslatorFunc);
            }
            ~ScopedTranslator(void)
            {
                if (this->m_Function != nullptr)
                    ::_set_se_translator(this->m_Function);
                this->m_Function = nullptr;
            }

        private:
            /**
             * Exception translation function used to convert a C structured exception to a C++ typed exception.
             * (Used to wrap the exception into our custom exception object.)
             *
             * @param {uint32_t} id - The id of the exception being wrapped.
             * @param {EXCEPTION_POINTERS*} pointers - The pointer information of the exception being handled.
             */
            static void ScopedTranslatorFunc(uint32_t id, struct _EXCEPTION_POINTERS* pointers)
            {
                UNREFERENCED_PARAMETER(pointers);
                throw AS_Exception(id);
            }
        };
    }; // namespace Exception
}; // namespace Ashita

#endif // __ASHITA_AS_EXCEPTION_H_INCLUDED__