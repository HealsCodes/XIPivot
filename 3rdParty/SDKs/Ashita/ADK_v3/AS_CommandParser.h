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

#ifndef __ASHITA_AS_COMMANDPARSER_H_INCLUDED__
#define __ASHITA_AS_COMMANDPARSER_H_INCLUDED__

#if defined (_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include <Windows.h>
#include <string>
#include <vector>

// Helper Macros
#define HANDLECOMMAND_GA(arg1, arg2, arg3, arg4, arg5, arg6, ...) arg6
#define HANDLECOMMAND_01(a)             Ashita::CommandParser::__command_parse(args[0].c_str(), a)
#define HANDLECOMMAND_02(a, b)          Ashita::CommandParser::__command_parse(args[0].c_str(), a, b)
#define HANDLECOMMAND_03(a, b, c)       Ashita::CommandParser::__command_parse(args[0].c_str(), a, b, c)
#define HANDLECOMMAND_04(a, b, c, d)    Ashita::CommandParser::__command_parse(args[0].c_str(), a, b, c, d)
#define HANDLECOMMAND_05(a, b, c, d, e) Ashita::CommandParser::__command_parse(args[0].c_str(), a, b, c, d, e)
#define HANDLECOMMAND_CC(...)           HANDLECOMMAND_GA(__VA_ARGS__, HANDLECOMMAND_05, HANDLECOMMAND_04, HANDLECOMMAND_03, HANDLECOMMAND_02, HANDLECOMMAND_01,)
#define HANDLECOMMAND(...)              if(args.size() > 0 && HANDLECOMMAND_CC(__VA_ARGS__)(__VA_ARGS__))

namespace Ashita
{
    namespace CommandParser
    {
        /**
         * Compares a command to a subarg to determine if they match.
         *
         * @param {const char*} cmd - The command to compare against
         * @param {const char*} arg1 - The argument to compare to.
         * @returns {bool} True if matches, false otherwise.
         */
        static bool __forceinline __command_cmp(const char* cmd, const char* arg1)
        {
            if (cmd == nullptr || arg1 == nullptr)
                return false;
            return (_stricmp(cmd, arg1) == 0);
        }
        
        /**
         * Compares a command to a subarg to determine if they match.
         *
         * @param {const char*} cmd - The command to compare against
         * @param {const char*} arg1 - The argument to compare to.
         * @returns {bool} True if matches, false otherwise.
         */
        template<typename T>
        static bool __command_parse(T cmd, T arg1)
        {
            return __command_cmp(cmd, arg1);
        }
        
        /**
         * Compares a command to subargs to determine if either match.
         *
         * @param {const char*} cmd - The command to compare against
         * @param {const char*} arg1 - The argument to compare to.
         * @param {const char*} arg2 - The argument to compare to.
         * @returns {bool} True if matches, false otherwise.
         */
        template<typename T>
        static bool __command_parse(T cmd, T arg1, T arg2)
        {
            return __command_cmp(cmd, arg1) || __command_cmp(cmd, arg2);
        }
        
        /**
         * Compares a command to subargs to determine if either match.
         *
         * @param {const char*} cmd - The command to compare against
         * @param {const char*} arg1 - The argument to compare to.
         * @param {const char*} arg2 - The argument to compare to.
         * @param {const char*} arg3 - The argument to compare to.
         * @returns {bool} True if matches, false otherwise.
         */
        template<typename T>
        static bool __command_parse(T cmd, T arg1, T arg2, T arg3)
        {
            return __command_cmp(cmd, arg1) || __command_cmp(cmd, arg2) || __command_cmp(cmd, arg3);
        }
        
        /**
         * Compares a command to subargs to determine if either match.
         *
         * @param {const char*} cmd - The command to compare against
         * @param {const char*} arg1 - The argument to compare to.
         * @param {const char*} arg2 - The argument to compare to.
         * @param {const char*} arg3 - The argument to compare to.
         * @param {const char*} arg4 - The argument to compare to.
         * @returns {bool} True if matches, false otherwise.
         */
        template<typename T>
        static bool __command_parse(T cmd, T arg1, T arg2, T arg3, T arg4)
        {
            return __command_cmp(cmd, arg1) || __command_cmp(cmd, arg2) || __command_cmp(cmd, arg3) || __command_cmp(cmd, arg4);
        }
        
        /**
         * Compares a command to subargs to determine if either match.
         *
         * @param {const char*} cmd - The command to compare against
         * @param {const char*} arg1 - The argument to compare to.
         * @param {const char*} arg2 - The argument to compare to.
         * @param {const char*} arg3 - The argument to compare to.
         * @param {const char*} arg4 - The argument to compare to.
         * @param {const char*} arg5 - The argument to compare to.
         * @returns {bool} True if matches, false otherwise.
         */
        template<typename T>
        static bool __command_parse(T cmd, T arg1, T arg2, T arg3, T arg4, T arg5)
        {
            return __command_cmp(cmd, arg1) || __command_cmp(cmd, arg2) || __command_cmp(cmd, arg3) || __command_cmp(cmd, arg4) || __command_cmp(cmd, arg5);
        }
    }; // namespace CommandParser

    namespace Commands
    {
        /**
         * Determines if the given character is a space character.
         * (Used to avoid CRT asserts.)
         *
         * @param {char} c - The character to check.
         * @returns {bool} True if a space char, false otherwise.
         */
        static __forceinline bool _isspace(char c)
        {
            auto num = (int32_t)c;

            // Checks for the following: ' ', \t \n \v \f \r
            if (num == 0x20 || num == 0x09 || num == 0x0A || num == 0x0B || num == 0x0C || num == 0x0D)
                return true;
            return false;
        }

        /**
         * Obtains the command arguments from the given raw command.
         *
         * @param {const char*} command - The command to parse for arguments.
         * @param {std::vector} args - The return vector to hold the found arguments.
         * @returns {uint32_t} The number of arguments parsed from the command.
         */
        static uint32_t GetCommandArgs(const char* command, std::vector<std::string>* args)
        {
            // The current parsing state we are in..
            enum { NONE, IN_WORD, IN_STRING } state = NONE;

            char currentArgument[255] = { 0 };
            auto p = command;
            char *pStart = nullptr;

            // Walk the string to locate arguments..
            for (; *p != 0; ++p)
            {
                // Obtain the current character.. 
                auto currChar = (char)*p;

                // Handle the current state..
                switch (state)
                {
                case NONE:
                    if (Ashita::Commands::_isspace(currChar))
                        continue;
                    if (currChar == '"')
                    {
                        state = IN_STRING;
                        pStart = (char*)p + 1;
                        continue;
                    }
                    state = IN_WORD;
                    pStart = (char*)p;
                    continue;

                case IN_STRING:
                    if (currChar == '"')
                    {
                        strncpy_s(currentArgument, pStart, p - pStart);
                        args->push_back(std::string(currentArgument));
                        state = NONE;
                        pStart = nullptr;
                    }
                    continue;

                case IN_WORD:
                    if (Ashita::Commands::_isspace(currChar))
                    {
                        strncpy_s(currentArgument, pStart, p - pStart);
                        args->push_back(std::string(currentArgument));
                        state = NONE;
                        pStart = nullptr;
                    }
                    continue;
                }
            }

            // Add any left-over words..
            if (pStart != nullptr)
            {
                strncpy_s(currentArgument, pStart, p - pStart);
                args->push_back(std::string(currentArgument));
            }

            // Return the number of found arguments..
            return args->size();
        }
    }; // namespace Commands    
}; // namespace Ashita

#endif // __ASHITA_AS_COMMANDPARSER_H_INCLUDED__