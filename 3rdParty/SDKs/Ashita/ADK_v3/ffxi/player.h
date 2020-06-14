/**
 * Ashita - Copyright (c) 2014 - 2017 atom0s [atom0s@live.com]
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

#ifndef __ASHITA_AS_FFXI_PLAYER_INCLUDED__
#define __ASHITA_AS_FFXI_PLAYER_INCLUDED__

#if defined (_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

namespace Ashita {
    namespace FFXI {
        struct playerstats_t
        {
            int16_t Strength;
            int16_t Dexterity;
            int16_t Vitality;
            int16_t Agility;
            int16_t Intelligence;
            int16_t Mind;
            int16_t Charisma;
        };

        struct playerresists_t
        {
            int16_t Fire;
            int16_t Ice;
            int16_t Wind;
            int16_t Earth;
            int16_t Lightning;
            int16_t Water;
            int16_t Light;
            int16_t Dark;
        };

        struct combatskill_t
        {
            uint16_t Raw;

            uint16_t GetSkill(void) const { return (uint16_t)(this->Raw & 0x7FFF); }
            bool IsCapped(void)     const { return (this->Raw & 0x8000) == 0 ? false : true; };
        };

        struct craftskill_t
        {
            uint16_t Raw;

            uint16_t GetSkill(void) const { return (this->Raw & 0x1FE0) >> 5; }
            uint16_t GetRank(void)  const { return (uint16_t)(this->Raw & 0x1F); }
            bool IsCapped(void)     const { return (this->Raw & 0x8000) >> 15 == 0 ? false : true; };
        };

        struct combatskills_t
        {
            combatskill_t Unknown;
            combatskill_t HandToHand;
            combatskill_t Dagger;
            combatskill_t Sword;
            combatskill_t GreatSword;
            combatskill_t Axe;
            combatskill_t GreatAxe;
            combatskill_t Scythe;
            combatskill_t Polearm;
            combatskill_t Katana;
            combatskill_t GreatKatana;
            combatskill_t Club;
            combatskill_t Staff;
            combatskill_t Unused0000;
            combatskill_t Unused0001;
            combatskill_t Unused0002;
            combatskill_t Unused0003;
            combatskill_t Unused0004;
            combatskill_t Unused0005;
            combatskill_t Unused0006;
            combatskill_t Unused0007;
            combatskill_t Unused0008;
            combatskill_t Unused0009;
            combatskill_t Unused0010;
            combatskill_t Unused0011;
            combatskill_t Archery;
            combatskill_t Marksmanship;
            combatskill_t Throwing;
            combatskill_t Guarding;
            combatskill_t Evasion;
            combatskill_t Shield;
            combatskill_t Parrying;
            combatskill_t Divine;
            combatskill_t Healing;
            combatskill_t Enhancing;
            combatskill_t Enfeebling;
            combatskill_t Elemental;
            combatskill_t Dark;
            combatskill_t Summon;
            combatskill_t Ninjitsu;
            combatskill_t Singing;
            combatskill_t String;
            combatskill_t Wind;
            combatskill_t BlueMagic;
            combatskill_t Unused0012;
            combatskill_t Unused0013;
            combatskill_t Unused0014;
            combatskill_t Unused0015;
        };

        struct craftskills_t
        {
            craftskill_t Fishing;
            craftskill_t Woodworking;
            craftskill_t Smithing;
            craftskill_t Goldsmithing;
            craftskill_t Clothcraft;
            craftskill_t Leathercraft;
            craftskill_t Bonecraft;
            craftskill_t Alchemy;
            craftskill_t Cooking;
            craftskill_t Synergy;
            craftskill_t Riding;
            craftskill_t Unused0000;
            craftskill_t Unused0001;
            craftskill_t Unused0002;
            craftskill_t Unused0003;
            craftskill_t Unused0004;
        };

        struct abilityrecast_t
        {
            uint16_t    Recast;         // The abilities recast timer at the time of use.
            uint8_t     Unknown0000;    // Unknown
            uint8_t     RecastTimerId;  // The abilities recast timer id.
            uint32_t    Unknown0001;    // Unknown
        };

#pragma pack(push, 1)
        struct playerinfo_t
        {
            uint32_t        HealthMax;              // The players max health.
            uint32_t        ManaMax;                // The players max mana.
            uint8_t         MainJob;                // The players main job id.
            uint8_t         MainJobLevel;           // The players main job level.
            uint8_t         SubJob;                 // The players sub job id.
            uint8_t         SubJobLevel;            // The players sub job level.
            uint16_t        ExpCurrent;             // The players current experience points.
            uint16_t        ExpNeeded;              // The players current experience points needed to level.
            playerstats_t   Stats;                  // The players base stats.
            playerstats_t   StatsModifiers;         // The players stat modifiers.
            int16_t         Attack;                 // The players attack.
            int16_t         Defense;                // The players defense.
            playerresists_t Resists;                // The players elemental resists.
            uint16_t        Title;                  // The players title id.
            uint16_t        Rank;                   // The players rank number.
            uint16_t        RankPoints;             // The players rank points.
            uint8_t         Nation;                 // The players nation id.
            uint8_t         Residence;              // The players residence id.
            uint32_t        Homepoint;              // The players homepoint. (Homepoint & 0x0000FFFF)
            combatskills_t  CombatSkills;           // The players combat skills.
            craftskills_t   CraftSkills;            // The players crafting skills.
            abilityrecast_t AbilityInfo[34];        // The players ability recast information.
            uint8_t         Unknown0000[6];         // Unknown - Potentially a 'start' delay time to offset the ability recasts from.
            uint16_t        LimitPoints;            // The players current limit points.
            uint8_t         MeritPoints;            // The players current merit points.
            uint8_t         LimitMode;              // The players current limit mode.
            uint32_t        MeritPointsMax;         // The players max merits.
            uint8_t         Unknown0001[214];       // Unknown
            int16_t         StatusIcons[32];        // The players status icons used for status timers.
            int32_t         StatusTimers[32];       // The players status timers.
            uint8_t         Unknown0002[104];       // Unknown
            int16_t         Buffs[32];              // The players current status effect icon ids.
        };
#pragma pack(pop)
    }; // namespace FFXI
}; // namespace Ashita

#endif // __ASHITA_AS_FFXI_PLAYER_INCLUDED__