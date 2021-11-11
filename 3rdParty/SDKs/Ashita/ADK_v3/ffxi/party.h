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

#ifndef __ASHITA_AS_FFXI_PARTY_INCLUDED__
#define __ASHITA_AS_FFXI_PARTY_INCLUDED__

#if defined (_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

namespace Ashita {
    namespace FFXI {
        struct alliance_t
        {
            uint32_t    AllianceLeaderServerId;
            uint32_t    Party0LeaderServerId;
            uint32_t    Party1LeaderServerId;
            uint32_t    Party2LeaderServerId;
            int8_t      Party0Visible;
            int8_t      Party1Visible;
            int8_t      Party2Visible;
            int8_t      Party0MemberCount;
            int8_t      Party1MemberCount;
            int8_t      Party2MemberCount;
            int8_t      Invited;
            int8_t      Unknown0000;
        };

        struct partymember_t
        {
            alliance_t* AllianceInfo;   // Information regarding the party alliance.
            uint8_t     Index;          // The party members index. (Index of their current party.)
            uint8_t     MemberNumber;   // The party members number. (Member number of their current party.)
            int8_t      Name[18];       // The party members name.
            uint32_t    ServerId;       // The party members server id.
            uint32_t    TargetIndex;    // The party members target index.
            uint32_t    Unknown0000;    // Unknown (Some kind of timestamp.)
            uint32_t    CurrentHP;      // The party members current health.
            uint32_t    CurrentMP;      // The party members current mana.
            uint32_t    CurrentTP;      // The party members current TP.
            uint8_t     CurrentHPP;     // The party members current health percent.
            uint8_t     CurrentMPP;     // The party members current mana percent.
            uint16_t    ZoneId;         // The party members current zone id.
            uint16_t    Unknown0001;    // Unknown (Zone id copy when not in the same zone as the member.)
            uint16_t    Unknown0002;    // Unknown (Alaways 0.)
            uint32_t    FlagMask;       // The party members flag mask.
            uint8_t     Unknown0003[49];// Unknown
            uint8_t     MainJob;        // The party members main job id.
            uint8_t     MainJobLevel;   // The party members main job level.
            uint8_t     SubJob;         // The party members sub job id.
            uint8_t     SubJobLvl;      // The party members sub job level.
            uint8_t     Unknown0004[3]; // Unknown
            uint32_t    ServerId2;      // The party members server id. (Duplicate.)
            uint8_t     CurrentHPP2;    // The party members current health percent. (Duplicate)
            uint8_t     CurrentMPP2;    // The party members current mana percent. (Duplicate)
            uint8_t     Active;         // The party members active state. (1 is active, 0 not active.)
            uint8_t     Unknown0005;    // Unknown (Alignment padding.)
        };
    }; // namespace FFXI
}; // namespace Ashita

#endif // __ASHITA_AS_FFXI_PARTY_INCLUDED__