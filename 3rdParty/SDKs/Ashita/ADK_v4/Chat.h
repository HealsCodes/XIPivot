/**
 * Ashita SDK - Copyright (c) 2019 Ashita Development Team
 * Contact: https://www.ashitaxi.com/
 * Contact: https://discord.gg/Ashita
 *
 * This file is part of Ashita.
 *
 * Ashita is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Ashita is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with Ashita.  If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef __ASHITA_SDK_CHAT_H_INCLUDED__
#define __ASHITA_SDK_CHAT_H_INCLUDED__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

// ReSharper disable CppInconsistentNaming
// ReSharper disable CppUnusedIncludeDirective

#include <iomanip>
#include <ostream>
#include <string>

namespace Ashita::Chat
{
    /**
     * Warning:
     * 
     * There is a bug with the retail client that can break inline coloring when the client needs to 
     * wrap long lines of text into multiple lines. If the players client is configured where the chat 
     * window is too small to display a single chat line, the client will look for an optimal position 
     * in the text to break it into parts. When this happens it only looks for and stores color codes 
     * that start with 0x1E. Codes that start with 0x1F (color table 2) are ignored and incorrectly 
     * recolored when the split lines begins.
     *
     * It is recommended to avoid 0x1F color codes as much as possible and only use 0x1E codes instead!
     */

    namespace Helpers
    {
        constexpr auto HeaderOpen  = u8"\x1E\x51[\x1E\x06";
        constexpr auto HeaderClose = u8"\x1E\x51]\x1E\x01 ";

    }; // namespace Helpers

    namespace Colors
    {
        constexpr auto Reset                = u8"\x1E\x01";
        constexpr auto Normal               = u8"\x1E\x01";
        constexpr auto White                = u8"\x1E\x01";
        constexpr auto LawnGreen            = u8"\x1E\x02";
        constexpr auto MediumSlateBlue      = u8"\x1E\x03";
        constexpr auto Magenta              = u8"\x1E\x05";
        constexpr auto Cyan                 = u8"\x1E\x06";
        constexpr auto Moccasin             = u8"\x1E\x07";
        constexpr auto Coral                = u8"\x1E\x08";
        constexpr auto DimGrey              = u8"\x1E\x41";
        constexpr auto Grey                 = u8"\x1E\x43";
        constexpr auto Salmon               = u8"\x1E\x44";
        constexpr auto Yellow               = u8"\x1E\x45";
        constexpr auto RoyalBlue            = u8"\x1E\x47";
        constexpr auto DarkMagenta          = u8"\x1E\x48";
        constexpr auto Violet               = u8"\x1E\x49";
        constexpr auto Tomato               = u8"\x1E\x4C";
        constexpr auto MistyRose            = u8"\x1E\x4D";
        constexpr auto PaleGoldenRod        = u8"\x1E\x4E";
        constexpr auto Lime                 = u8"\x1E\x4F";
        constexpr auto PaleGreen            = u8"\x1E\x50";
        constexpr auto DarkOrchid           = u8"\x1E\x51";
        constexpr auto Aqua                 = u8"\x1E\x52";
        constexpr auto SpringGreen          = u8"\x1E\x53";
        constexpr auto DarkSalmon           = u8"\x1E\x55";
        constexpr auto MediumSpringGreen    = u8"\x1E\x58";
        constexpr auto MediumPurple         = u8"\x1E\x59";
        constexpr auto Azure                = u8"\x1E\x5A";
        constexpr auto LightCyan            = u8"\x1E\x5C";
        constexpr auto LightGoldenRodYellow = u8"\x1E\x60";
        constexpr auto Plum                 = u8"\x1E\x69";

    }; // namespace Colors

    namespace FFXiSymbols
    {
        constexpr auto FireIcon       = u8"\xEF\x1F";
        constexpr auto IceIcon        = u8"\xEF\x20";
        constexpr auto WindIcon       = u8"\xEF\x21";
        constexpr auto EarthIcon      = u8"\xEF\x22";
        constexpr auto LightningIcon  = u8"\xEF\x23";
        constexpr auto WaterIcon      = u8"\xEF\x24";
        constexpr auto LightIcon      = u8"\xEF\x25";
        constexpr auto DarknessIcon   = u8"\xEF\x26";
        constexpr auto TranslateOpen  = u8"\xEF\x27";
        constexpr auto TranslateClose = u8"\xEF\x28";
        constexpr auto On             = u8"\xEF\x29";
        constexpr auto Off            = u8"\xEF\x2A";
        constexpr auto OnFrench       = u8"\xEF\x2B";
        constexpr auto OffFrench      = u8"\xEF\x2C";
        constexpr auto OnGerman       = u8"\xEF\x2D";
        constexpr auto OffGerman      = u8"\xEF\x2E";

    }; // namespace FFXiSymbols

    namespace UnicodeSymbols
    {
        /**
         * The names of these symbols are taken from the following sources:
         * 
         * https://en.wikipedia.org/wiki/Hiragana
         * https://en.wikipedia.org/wiki/Kanji
         * https://en.wikipedia.org/wiki/Katakana
         * https://en.wikipedia.org/wiki/List_of_Japanese_typographic_symbols
         * https://www.ssec.wisc.edu/~tomw/java/unicode.html
         * https://www.key-shortcut.com/en/writing-systems/%E3%81%B2%E3%82%89%E3%81%8C%E3%81%AA-japanese
         * https://en.wikipedia.org/wiki/List_of_logic_symbols
         */

        constexpr auto IdeographicComma            = u8"\x81\x41";
        constexpr auto IdeographicFullStop         = u8"\x81\x42";
        constexpr auto Comma                       = u8"\x81\x43";
        constexpr auto FullStop                    = u8"\x81\x44";
        constexpr auto MiddleDot                   = u8"\x81\x45";
        constexpr auto Colon                       = u8"\x81\x46";
        constexpr auto SemiColon                   = u8"\x81\x47";
        constexpr auto QuestionMark                = u8"\x81\x48";
        constexpr auto ExclamationMark             = u8"\x81\x49";
        constexpr auto VoicedSoundMark             = u8"\x81\x4A";
        constexpr auto SemiVoicedSoundMark         = u8"\x81\x4B";
        constexpr auto Apostrophe                  = u8"\x81\x4C";
        constexpr auto Accent                      = u8"\x81\x4D";
        constexpr auto CircumflexAccent            = u8"\x81\x4F";
        constexpr auto Macron                      = u8"\x81\x50";
        constexpr auto LowLine                     = u8"\x81\x51";
        constexpr auto KatakanaIterationMark       = u8"\x81\x52";
        constexpr auto KatakanaVoicedIterationMark = u8"\x81\x53";
        constexpr auto HiraganaIterationMark       = u8"\x81\x54";
        constexpr auto HiraganaVoicedIterationMark = u8"\x81\x55";
        constexpr auto Ditto                       = u8"\x81\x56";
        constexpr auto Repetition                  = u8"\x81\x57";
        constexpr auto Unknown8158                 = u8"\x81\x58";
        constexpr auto Unknown8159                 = u8"\x81\x59";
        constexpr auto Maru                        = u8"\x81\x5A";
        constexpr auto Unknown815B                 = u8"\x81\x5B";
        constexpr auto Unknown815C                 = u8"\x81\x5C";
        constexpr auto Unknown815D                 = u8"\x81\x5D";
        constexpr auto Solidus                     = u8"\x81\x5E";
        constexpr auto ReverseSolidus              = u8"\x81\x5F";
        constexpr auto Tilde                       = u8"\x81\x60";
        constexpr auto Wave                        = u8"\x81\x60";
        constexpr auto ParallelVerticalLines       = u8"\x81\x61";
        constexpr auto VerticalLine                = u8"\x81\x62";
        constexpr auto Pipe                        = u8"\x81\x62";
        constexpr auto HorizontalEllipsis          = u8"\x81\x63";
        constexpr auto TwoDotLeader                = u8"\x81\x64";
        constexpr auto Unknown8165                 = u8"\x81\x65";
        constexpr auto Unknown8166                 = u8"\x81\x66";
        constexpr auto Unknown8167                 = u8"\x81\x67";
        constexpr auto Unknown8168                 = u8"\x81\x68";
        constexpr auto LeftParenthesis             = u8"\x81\x69";
        constexpr auto RightParenthesis            = u8"\x81\x6A";
        constexpr auto LeftTortoiseBracket         = u8"\x81\x6B";
        constexpr auto RightTortoiseBracket        = u8"\x81\x6C";
        constexpr auto LeftSquareBracket           = u8"\x81\x6D";
        constexpr auto RightSquareBracket          = u8"\x81\x6E";
        constexpr auto LeftCurlyBracket            = u8"\x81\x6F";
        constexpr auto RightCurlyBracket           = u8"\x81\x70";
        constexpr auto LeftAngleBracket            = u8"\x81\x71";
        constexpr auto RightAngleBracket           = u8"\x81\x72";
        constexpr auto LeftDoubleAngleBracket      = u8"\x81\x73";
        constexpr auto RightDoubleAngleBracket     = u8"\x81\x74";
        constexpr auto LeftCornerBracket           = u8"\x81\x75";
        constexpr auto RightCornerBracket          = u8"\x81\x76";
        constexpr auto LeftWhiteCornerBracket      = u8"\x81\x77";
        constexpr auto RightWhiteCornerBracket     = u8"\x81\x78";
        constexpr auto LeftBlackLenticularBracket  = u8"\x81\x79";
        constexpr auto RightBlackLenticularBracket = u8"\x81\x7A";
        constexpr auto Plus                        = u8"\x81\x7B";
        constexpr auto Minus                       = u8"\x81\x7C";
        constexpr auto PlusMinus                   = u8"\x81\x7D";
        constexpr auto Multiply                    = u8"\x81\x7E";
        constexpr auto Divide                      = u8"\x81\x7F";
        constexpr auto Unknown8180                 = u8"\x81\x80"; // Copy of 817F
        constexpr auto Equals                      = u8"\x81\x81";
        constexpr auto NotEquals                   = u8"\x81\x82";
        constexpr auto LessThan                    = u8"\x81\x83";
        constexpr auto GreaterThan                 = u8"\x81\x84";
        constexpr auto LessThanOrEquals            = u8"\x81\x85";
        constexpr auto GreaterThanOrEquals         = u8"\x81\x86";
        constexpr auto Infinite                    = u8"\x81\x87";
        constexpr auto Therefore                   = u8"\x81\x88";
        constexpr auto Male                        = u8"\x81\x89";
        constexpr auto Female                      = u8"\x81\x8A";
        constexpr auto Degree                      = u8"\x81\x8B";
        constexpr auto Arcminute                   = u8"\x81\x8C";
        constexpr auto Arcsecond                   = u8"\x81\x8D";
        constexpr auto DegreeCelsius               = u8"\x81\x8E";
        constexpr auto Unknown818F                 = u8"\x81\x8F"; // Copy of 815F
        constexpr auto Dollar                      = u8"\x81\x90";
        constexpr auto Cent                        = u8"\x81\x91";
        constexpr auto Pounds                      = u8"\x81\x92";
        constexpr auto Percent                     = u8"\x81\x93";
        constexpr auto Pound                       = u8"\x81\x94";
        constexpr auto Hashtag                     = u8"\x81\x94";
        constexpr auto Ampersand                   = u8"\x81\x95";
        constexpr auto Asterisk                    = u8"\x81\x96";
        constexpr auto At                          = u8"\x81\x97";
        constexpr auto Section                     = u8"\x81\x98";
        constexpr auto WhiteStar                   = u8"\x81\x99"; // Colors inversed, but proper naming based on look.
        constexpr auto BlackStar                   = u8"\x81\x9A"; // Colors inversed, but proper naming based on look.
        constexpr auto WhiteCircle                 = u8"\x81\x9B"; // Colors inversed, but proper naming based on look.
        constexpr auto BlackCircle                 = u8"\x81\x9C"; // Colors inversed, but proper naming based on look.
        constexpr auto Bullseye                    = u8"\x81\x9D";
        constexpr auto CircleJot                   = u8"\x81\x9D";
        constexpr auto WhiteDiamond                = u8"\x81\x9E"; // Colors inversed, but proper naming based on look.
        constexpr auto BlackDiamond                = u8"\x81\x9F"; // Colors inversed, but proper naming based on look.
        constexpr auto WhiteSquare                 = u8"\x81\xA0";
        constexpr auto BlackSquare                 = u8"\x81\xA1";
        constexpr auto WhiteTriangleUp             = u8"\x81\xA2";
        constexpr auto BlackTriangleUp             = u8"\x81\xA3";
        constexpr auto WhiteTriangleDown           = u8"\x81\xA4";
        constexpr auto BlackTriangleDown           = u8"\x81\xA5";
        constexpr auto ReferenceMark               = u8"\x81\xA6";
        constexpr auto PostalMark                  = u8"\x81\xA7";
        constexpr auto RightArrow                  = u8"\x81\xA8";
        constexpr auto LeftArrow                   = u8"\x81\xA9";
        constexpr auto UpArrow                     = u8"\x81\xAA";
        constexpr auto DownArrow                   = u8"\x81\xAB";
        constexpr auto Geta                        = u8"\x81\xAC";
        constexpr auto ElementOf                   = u8"\x81\xAD";
        constexpr auto ContainsAsMember            = u8"\x81\xAE";
        constexpr auto SubsetOrEquals              = u8"\x81\xAF";
        constexpr auto SupersetOrEquals            = u8"\x81\xB0";
        constexpr auto Subset                      = u8"\x81\xB1";
        constexpr auto Superset                    = u8"\x81\xB2";
        constexpr auto Union                       = u8"\x81\xB3";
        constexpr auto Intersection                = u8"\x81\xB4";
        constexpr auto LogicalAnd                  = u8"\x81\xB5";
        constexpr auto LogicalOr                   = u8"\x81\xB6";
        constexpr auto Not                         = u8"\x81\xB7";
        constexpr auto Unknown81B8                 = u8"\x81\xB8"; // Copy of 81AD
        constexpr auto Unknown81B9                 = u8"\x81\xB9"; // Copy of 81AE
        constexpr auto Unknown81BA                 = u8"\x81\xBA"; // Copy of 81AF
        constexpr auto Unknown81BB                 = u8"\x81\xBB"; // Copy of 81B0
        constexpr auto Unknown81BC                 = u8"\x81\xBC"; // Copy of 81B1
        constexpr auto Unknown81BD                 = u8"\x81\xBD"; // Copy of 81B2
        constexpr auto Unknown81BE                 = u8"\x81\xBE"; // Copy of 81B3
        constexpr auto Unknown81BF                 = u8"\x81\xBF"; // Copy of 81B4
        constexpr auto Unknown81C0                 = u8"\x81\xC0"; // Copy of 81B5
        constexpr auto Unknown81C1                 = u8"\x81\xC1"; // Copy of 81B6
        constexpr auto Unknown81C2                 = u8"\x81\xC2"; // Copy of 81B7
        constexpr auto Implies                     = u8"\x81\xC3";
        constexpr auto Iif                         = u8"\x81\xC4";
        constexpr auto ForAll                      = u8"\x81\xC5";
        constexpr auto ForEach                     = u8"\x81\xC5";
        constexpr auto Exists                      = u8"\x81\xC6";
        constexpr auto Angle                       = u8"\x81\xC7";
        constexpr auto Unknown81C8                 = u8"\x81\xC8"; // Copy of 81B5
        constexpr auto Unknown81C9                 = u8"\x81\xC9"; // Copy of 81B6
        constexpr auto Unknown81CA                 = u8"\x81\xCA"; // Copy of 81B7
        constexpr auto Unknown81CB                 = u8"\x81\xCB"; // Copy of 81C3
        constexpr auto Unknown81CC                 = u8"\x81\xCC"; // Copy of 81C4
        constexpr auto Unknown81CD                 = u8"\x81\xCD"; // Copy of 81C5
        constexpr auto Unknown81CE                 = u8"\x81\xCE"; // Copy of 81C6
        constexpr auto Unknown81CF                 = u8"\x81\xCF"; // Copy of 81C7
        constexpr auto Bot                         = u8"\x81\xD0";
        constexpr auto Falsum                      = u8"\x81\xD0";
        constexpr auto UpTack                      = u8"\x81\xD0";
        constexpr auto Tie                         = u8"\x81\xD1";
        constexpr auto Partial                     = u8"\x81\xD2";
        constexpr auto Nabla                       = u8"\x81\xD3";
        constexpr auto IdenticalTo                 = u8"\x81\xD4";
        constexpr auto ApproximatelyEqual          = u8"\x81\xD5";
        constexpr auto MuchLessThan                = u8"\x81\xD6";
        constexpr auto MuchGreaterThan             = u8"\x81\xD7";
        constexpr auto SquareRoot                  = u8"\x81\xD8";
        constexpr auto InvertedLazyS               = u8"\x81\xD9";
        constexpr auto Unknown81DA                 = u8"\x81\xDA"; // Copy of 81CF
        constexpr auto Unknown81DB                 = u8"\x81\xDB"; // Copy of 81D0
        constexpr auto Unknown81DC                 = u8"\x81\xDC"; // Copy of 81D1
        constexpr auto Unknown81DD                 = u8"\x81\xDD"; // Copy of 81D2
        constexpr auto Unknown81DE                 = u8"\x81\xDE"; // Copy of 81D3
        constexpr auto Unknown81DF                 = u8"\x81\xDF"; // Copy of 81D4
        constexpr auto Unknown81E0                 = u8"\x81\xE0"; // Copy of 81D5
        constexpr auto Unknown81E1                 = u8"\x81\xE1"; // Copy of 81D6
        constexpr auto Unknown81E2                 = u8"\x81\xE2"; // Copy of 81D7
        constexpr auto Unknown81E3                 = u8"\x81\xE3"; // Copy of 81D8
        constexpr auto Unknown81E4                 = u8"\x81\xE4"; // Copy of 81D9
        constexpr auto Proportional                = u8"\x81\xE5";
        constexpr auto Because                     = u8"\x81\xE6";
        constexpr auto Integral                    = u8"\x81\xE7";
        constexpr auto DoubleIntegral              = u8"\x81\xE8";
        constexpr auto AWithOverRing               = u8"\x81\xE9";
        constexpr auto PerMil                      = u8"\x81\xEA";
        constexpr auto PerMile                     = u8"\x81\xEA";
        constexpr auto MusicSharp                  = u8"\x81\xEB";
        constexpr auto MusicFlat                   = u8"\x81\xEC";
        constexpr auto MusicEighthNote             = u8"\x81\xED";
        constexpr auto Dagger                      = u8"\x81\xEE";
        constexpr auto DoubleDagger                = u8"\x81\xEF";

    }; // namespace UnicodeSymbols

    /**
     * Displays the given text colored using the 0x1E color table.
     * 
     * @param {uint8_t} n - The number of the color to use.
     * @param {std::string} str - The string to display.
     * @return {std::string} The colorized string.
     */
    inline std::string Color1(const uint8_t n, const std::string& str)
    {
        std::string res;

        res += "\x1E";
        res += (char)n;
        res += str;
        res += "\x1E\x01";

        return res;
    }

    /**
     * Displays the given text colored using the 0x1F color table.
     * 
     * @param {uint8_t} n - The number of the color to use.
     * @param {std::string} str - The string to display.
     * @return {std::string} The colorized string.
     */
    inline std::string Color2(const uint8_t n, const std::string& str)
    {
        std::string res;

        res += "\x1F";
        res += (char)n;
        res += str;
        res += "\x1E\x01";

        return res;
    }

    /**
     * Displays the given text surrounded by colored brackets.
     * 
     * @param {std::string} str - The string to display.
     * @return {std::string} The colorized string.
     */
    inline std::string Header(const std::string& str)
    {
        std::string res;

        res += Helpers::HeaderOpen;
        res += str;
        res += Helpers::HeaderClose;

        return res;
    }

    /**
     * Displays the given text in a dark red color.
     * 
     * @param {std::string} str - The string to display.
     * @return {std::string} The colorized string.
     */
    inline std::string Critical(const std::string& str)
    {
        return Color1(76, str);
    }

    /**
     * Displays the given text in a red color.
     * 
     * @param {std::string} str - The string to display.
     * @return {std::string} The colorized string.
     */
    inline std::string Error(const std::string& str)
    {
        return Color1(68, str);
    }

    /**
     * Displays the given text in a plain color.
     * 
     * @param {std::string} str - The string to display.
     * @return {std::string} The colorized string.
     */
    inline std::string Message(const std::string& str)
    {
        return Color1(106, str);
    }

    /**
     * Displays the given text in a green color.
     * 
     * @param {std::string} str - The string to display.
     * @return {std::string} The colorized string.
     */
    inline std::string Success(const std::string& str)
    {
        return Color1(2, str);
    }

    /**
     * Displays the given text in a yellow color.
     * 
     * @param {std::string} str - The string to display.
     * @return {std::string} The colorized string.
     */
    inline std::string Warning(const std::string& str)
    {
        return Color1(104, str);
    }

}; // namespace Ashita::Chat

#endif // __ASHITA_SDK_CHAT_H_INCLUDED__