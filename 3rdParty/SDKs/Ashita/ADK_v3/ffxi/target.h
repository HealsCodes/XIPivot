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

#ifndef __ASHITA_AS_FFXI_TARGET_INCLUDED__
#define __ASHITA_AS_FFXI_TARGET_INCLUDED__

#if defined (_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

namespace Ashita {
    namespace FFXI {
        struct target_t
        {
            uint32_t    TargetIndex;                // The current target index.
            uint32_t    TargetServerId;             // The current target server id.
            uintptr_t   TargetEntityPointer;        // The current target entity pointer.
            uintptr_t   TargetWarpPointer;          // The current target warp pointer.
            uint32_t    Unknown0000[4];             // Unknown
            uint8_t     TargetVisible;              // 0/1 if the target is set and visible.
            uint8_t     Unknown0001[3];             // Unknown (Booleans)
            uint16_t    TargetMask;                 // The current target mask.
            uint16_t    TargetCalculatedId;         // The targets calculated id.
            uint32_t    SubTargetIndex;             // The current sub-target index.
            uint32_t    SubTargetServerId;          // The current sub-target server id.
            uintptr_t   SubTargetEntityPointer;     // The current sub-target entity pointer.
            uintptr_t   SubTargetWarpPointer;       // The current sub-target warp pointer.
            uint32_t    Unknown0002[4];             // Unknown
            uint8_t     SubTargetVisible;           // 0/1 if the sub-target is set and visible.
            uint8_t     Unknown0003[3];             // Unknown (Booleans)
            uint16_t    SubTargetMask;              // The current sub-target mask.
            uint8_t     Unknown0004[4];             // Unknown
            uint8_t     SubTargetActive;            // 1 if the main target is set and the game is making use the sub-target entry.
            uint8_t     TargetDeactivate;           // If set to 1, will undo one step of the targeting windows.
            uint8_t     Unknown0005[8];             // Unknown (Involves graphics related things, such as blinking the targeted entity.)
            uint8_t     IsLockedOn;                 // 0/1 if the target is currently locked on. (Low bit 0x00 and 0x01.)
            uint8_t     Unknown0006[3];             // Unknown
            uint32_t    TargetSelectionMask;        // The selection mask used to determine what can be targeted with the current action.
            uint8_t     Unknown0007[16];            // Unknown
            uint8_t     IsMenuOpen;                 // 0/1 if the menu is open. (ie. selecting a spell on a target.) 74
        };

        struct targetwindow_t
        {
            uintptr_t   TargetWindowVTablePointer;  // The VTable pointer to the target window class.
            uint32_t    Unknown0000;                // Unknown
            uint32_t    TargetWindowPointer;        // Pointer to the target window object.
            uint32_t    Unknown0001;                // Unknown
            uint8_t     Unknown0002;                // Unknown
            uint8_t     Unknown0003[3];             // Unknown
            int8_t      Name[48];                   // The targets name.
            uint32_t    Unknown0004;                // Unknown
            uint32_t    TargetEntityPointer;        // The targets entity pointer.
            uint16_t    FrameOffsetX;               // The target window frame x offset.
            uint16_t    FrameOffsetY;               // The target window frame y offset.
            uint32_t    IconPosition;               // The target window frame icon position.
            uint32_t    Unknown0005;                // Unknown
            uint32_t    Unknown0006;                // Unknown
            uint32_t    Unknown0007;                // Unknown
            uint32_t    TargetServerId;             // The targets server id.                   
            uint8_t     HealthPercent;              // The targets health percent.
            uint8_t     DeathFlag;                  // Dims the health bar when target is dead..
            uint8_t     ReraiseFlag;                // Dims the health bar when target is reraising.. (Death flag must be set!)
            uint8_t     Unknown0008;                // Unknown
            uint16_t    Unknown0009;                // Unknown
            uint8_t     Unknown0010;                // Toggles 0 -> 1 when first targeting..
        };
    }; // namespace FFXI
}; // namespace Ashita

#endif // __ASHITA_AS_FFXI_TARGET_INCLUDED__