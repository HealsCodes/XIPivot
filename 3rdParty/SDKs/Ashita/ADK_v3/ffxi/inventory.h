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

#ifndef __ASHITA_AS_FFXI_INVENTORY_INCLUDED__
#define __ASHITA_AS_FFXI_INVENTORY_INCLUDED__

#if defined (_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

namespace Ashita {
    namespace FFXI {
        struct item_t
        {
            uint16_t        Id;                         // The item id.
            uint16_t        Index;                      // The item index.
            uint32_t        Count;                      // The item count.
            uint32_t        Flags;                      // The item flags. (5 = Equipped, 25 = Bazaar)
            uint32_t        Price;                      // The item price, if being sold in bazaar.
            uint8_t         Extra[28];                  // The item extra data. (Weaponskill Points, Charges, Augments, etc.)
        };

        struct items_t
        {
            item_t          Item[81];                   // The items within a storage container.
        };

        struct treasureitem_t
        {
            uint32_t        Flags;                      // The item flags.
            uint32_t        ItemId;                     // The item id.
            uint32_t        Count;                      // The item count.
            uint32_t        Unknown0000[9];             // Unknown
            uint32_t        Status;                     // The item status.
            uint16_t        Lot;                        // The local players lot on the item.
            uint16_t        WinningLot;                 // The current winning lot.
            uint32_t        WinningEntityServerId;      // The winning lotters server id.
            uint32_t        WinningEntityTargetIndex;   // The winning lotters target index.
            char            WinningLotterName[16];      // The winning lotters name.
            uint32_t        TimeToLive;                 // The time left to til the item leaves the pool.
            uint32_t        DropTime;                   // The time the item entered the pool.
        };

        struct equipment_t
        {
            uint32_t        Slot;                       // The slot id of the equipment.
            uint32_t        ItemIndex;                  // The item index where the item is located.
        };

        struct ffxi_inventory_t
        {
            uint8_t         Unknown0000[0x9860];
            items_t         Storage[(uint32_t)Enums::Containers::ContainerMax];
            uint8_t         Unknown0001[0x021C];        // Potential future storage space.
            treasureitem_t  TreasurePool[0x000A];
            uint32_t        Unknown0002;                // Unknown (Treasure related. Set to 1 after zoning, 2 when something has dropped to the pool.)
            uint8_t         Unknown0003;                // Unknown (Treasure related.)
            uint8_t         StorageMaxCapacity1[(uint32_t)Enums::Containers::ContainerMax + 1];
            uint16_t        StorageMaxCapacity2[(uint32_t)Enums::Containers::ContainerMax];
            uint32_t        Unknown0004;
            uint8_t         Unknown0005[0x0188];
            equipment_t     Equipment[(uint32_t)Enums::EquipmentSlots::EquipmentSlotMax];
            uint8_t         Unknown0006[0x0240];
            uint8_t         Unknown0007[0x00BC];
            uint32_t        CraftWait;                  // Handles if the player is crafting or able to craft again.
        };
    }; // namespace FFXI
}; // namespace Ashita

#endif // __ASHITA_AS_FFXI_INVENTORY_INCLUDED__