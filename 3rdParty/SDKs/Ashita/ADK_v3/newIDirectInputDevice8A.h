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

#ifndef __ASHITA_NEWIDIRECTINPUTDEVICE8A_H_INCLUDED__
#define __ASHITA_NEWIDIRECTINPUTDEVICE8A_H_INCLUDED__

#if defined (_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include <Windows.h>
#include <dinput.h>

interface newIDirectInputDevice8A : IDirectInputDevice8A
{
    virtual __declspec(nothrow) HRESULT __stdcall QueryInterface(REFIID riid, LPVOID * ppvObj) override;
    virtual __declspec(nothrow) ULONG   __stdcall AddRef(void) override;
    virtual __declspec(nothrow) ULONG   __stdcall Release(void) override;
    virtual __declspec(nothrow) HRESULT __stdcall GetCapabilities(LPDIDEVCAPS lpDIDevCaps) override;
    virtual __declspec(nothrow) HRESULT __stdcall EnumObjects(LPDIENUMDEVICEOBJECTSCALLBACKA lpCallback, LPVOID pvRef, DWORD dwFlags) override;
    virtual __declspec(nothrow) HRESULT __stdcall GetProperty(REFGUID rguidProp, LPDIPROPHEADER pdiph) override;
    virtual __declspec(nothrow) HRESULT __stdcall SetProperty(REFGUID rguidProp, LPCDIPROPHEADER pdiph) override;
    virtual __declspec(nothrow) HRESULT __stdcall Acquire(void) override;
    virtual __declspec(nothrow) HRESULT __stdcall Unacquire(void) override;
    virtual __declspec(nothrow) HRESULT __stdcall GetDeviceState(DWORD cbData, LPVOID lpvData) override;
    virtual __declspec(nothrow) HRESULT __stdcall GetDeviceData(DWORD cbObjectData, LPDIDEVICEOBJECTDATA rgdod, LPDWORD pdwInOut, DWORD dwFlags) override;
    virtual __declspec(nothrow) HRESULT __stdcall SetDataFormat(LPCDIDATAFORMAT lpdf) override;
    virtual __declspec(nothrow) HRESULT __stdcall SetEventNotification(HANDLE hEvent) override;
    virtual __declspec(nothrow) HRESULT __stdcall SetCooperativeLevel(HWND hwnd, DWORD dwFlags) override;
    virtual __declspec(nothrow) HRESULT __stdcall GetObjectInfo(LPDIDEVICEOBJECTINSTANCEA pdidoi, DWORD dwObj, DWORD dwHow) override;
    virtual __declspec(nothrow) HRESULT __stdcall GetDeviceInfo(LPDIDEVICEINSTANCEA pdidi) override;
    virtual __declspec(nothrow) HRESULT __stdcall RunControlPanel(HWND hwndOwner, DWORD dwFlags) override;
    virtual __declspec(nothrow) HRESULT __stdcall Initialize(HINSTANCE hinst, DWORD dwVersion, REFGUID rguid) override;
    virtual __declspec(nothrow) HRESULT __stdcall CreateEffect(REFGUID rguid, LPCDIEFFECT lpeff, LPDIRECTINPUTEFFECT *ppdeff, LPUNKNOWN punkOuter) override;
    virtual __declspec(nothrow) HRESULT __stdcall EnumEffects(LPDIENUMEFFECTSCALLBACKA lpCallback, LPVOID pvRef, DWORD dwEffType) override;
    virtual __declspec(nothrow) HRESULT __stdcall GetEffectInfo(LPDIEFFECTINFOA pdei, REFGUID rguid) override;
    virtual __declspec(nothrow) HRESULT __stdcall GetForceFeedbackState(LPDWORD pdwOut) override;
    virtual __declspec(nothrow) HRESULT __stdcall SendForceFeedbackCommand(DWORD dwFlags) override;
    virtual __declspec(nothrow) HRESULT __stdcall EnumCreatedEffectObjects(LPDIENUMCREATEDEFFECTOBJECTSCALLBACK lpCallback, LPVOID pvRef, DWORD fl) override;
    virtual __declspec(nothrow) HRESULT __stdcall Escape(LPDIEFFESCAPE pesc) override;
    virtual __declspec(nothrow) HRESULT __stdcall Poll(void) override;
    virtual __declspec(nothrow) HRESULT __stdcall SendDeviceData(DWORD cbObjectData, LPCDIDEVICEOBJECTDATA rgdod, LPDWORD pdwInOut, DWORD fl) override;
    virtual __declspec(nothrow) HRESULT __stdcall EnumEffectsInFile(LPCSTR lpszFileName, LPDIENUMEFFECTSINFILECALLBACK pec, LPVOID pvRef, DWORD dwFlags) override;
    virtual __declspec(nothrow) HRESULT __stdcall WriteEffectToFile(LPCSTR lpszFileName, DWORD dwEntries, LPDIFILEEFFECT rgDiFileEft, DWORD dwFlags) override;
    virtual __declspec(nothrow) HRESULT __stdcall BuildActionMap(LPDIACTIONFORMATA lpdiaf, LPCSTR lpszUserName, DWORD dwFlags) override;
    virtual __declspec(nothrow) HRESULT __stdcall SetActionMap(LPDIACTIONFORMATA lpdiActionFormat, LPCSTR lptszUserName, DWORD dwFlags) override;
    virtual __declspec(nothrow) HRESULT __stdcall GetImageInfo(LPDIDEVICEIMAGEINFOHEADERA lpdiDevImageInfoHeader) override;

    newIDirectInputDevice8A();
    newIDirectInputDevice8A(IDirectInputDevice8A** ppOriginalInterface);
    virtual ~newIDirectInputDevice8A(void);

protected:
    IDirectInputDevice8A*   m_DirectInputDevice;
    UINT                    m_ReferenceCount;
    HWND                    m_ParentHwnd;
};

#endif // __ASHITA_NEWIDIRECTINPUTDEVICE8A_H_INCLUDED__