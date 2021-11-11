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

#ifndef __ASHITA_AS_OBJECTS_H_INCLUDED__
#define __ASHITA_AS_OBJECTS_H_INCLUDED__

#if defined (_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include <Windows.h>

namespace Ashita
{
    typedef struct tagAS_Rect
    {
        uint32_t x, y, w, h;

        tagAS_Rect(void)
            : x(0), y(0), w(0), h(0)
        { }
        tagAS_Rect(uint32_t nW, uint32_t nH)
            : x(0), y(0), w(nW), h(nH)
        { }
        tagAS_Rect(uint32_t nX, uint32_t nY, uint32_t nW, uint32_t nH)
            : x(nX), y(nY), w(nW), h(nH)
        { }
        explicit tagAS_Rect(const RECT& r)
            : x(r.left), y(r.top), w(r.right - r.left), h(r.bottom - r.top)
        { }

        // Assignment operator overload.
        tagAS_Rect operator= (const tagAS_Rect& r)
        {
            this->x = r.x;
            this->y = r.y;
            this->w = r.w;
            this->h = r.h;

            return *this;
        }
        // Addition operator overload.
        tagAS_Rect operator+ (const tagAS_Rect& r)
        {
            this->x += r.x;
            this->y += r.y;
            this->w += r.w;
            this->h += r.h;

            return *this;
        }
        // Subtraction operator overload.
        tagAS_Rect operator- (const tagAS_Rect& r)
        {
            this->x -= r.x;
            this->y -= r.y;
            this->w -= r.w;
            this->h -= r.h;

            return *this;
        }

        // Equality operator overload.
        bool operator== (const tagAS_Rect& r) const
        {
            return this->x == r.x && this->y == r.y && this->w == r.w && this->h == r.h;
        }

        // Non-Equality opreator overload.
        bool operator!= (const tagAS_Rect& r) const
        {
            return !(*this == r);
        }
    } asrect_t;

    typedef struct tagAS_WindowInfo
    {
        HWND        Hwnd;
        uint32_t    Style;
        uint32_t    StyleEx;
        asrect_t    WindowRect;

        tagAS_WindowInfo(void)
            : Hwnd(nullptr), Style(0), StyleEx(0), WindowRect(0, 0, 0, 0)
        { }
        tagAS_WindowInfo(HWND hWnd, uint32_t dwStyle, uint32_t dwStyleEx, int32_t x, int32_t y, int32_t w, int32_t h)
            : Hwnd(hWnd), Style(dwStyle), StyleEx(dwStyleEx), WindowRect(x, y, w, h)
        { }

        /**
         * Resets the window information class.
         */
        void Reset(void)
        {
            this->Hwnd = nullptr;
            this->Style = 0;
            this->StyleEx = 0;
            this->WindowRect.x = 0;
            this->WindowRect.y = 0;
            this->WindowRect.w = 0;
            this->WindowRect.h = 0;
        }
    } aswindowinfo_t;
}; // namespace Ashita

#endif // __ASHITA_AS_OBJECTS_H_INCLUDED__