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

#ifndef __ASHITA_AS_FFXI_ENUMS_H_INCLUDED__
#define __ASHITA_AS_FFXI_ENUMS_H_INCLUDED__

#if defined (_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/**
 * Name Flag Definitions
 */
#define FLAG_INEVENT                0x00000002
#define FLAG_CHOCOBO                0x00000040
#define FLAG_WALLHACK               0x00000200
#define FLAG_INVITE                 0x00000800
#define FLAG_ANON                   0x00001000
#define FLAG_UNKNOWN                0x00002000
#define FLAG_AWAY                   0x00004000
#define FLAG_PLAYONLINE             0x00010000
#define FLAG_LINKSHELL              0x00020000
#define FLAG_DC                     0x00040000
#define FLAG_GM                     0x04000000
#define FLAG_GM_SUPPORT             0x04000000
#define FLAG_GM_SENIOR              0x05000000
#define FLAG_GM_LEAD                0x06000000
#define FLAG_GM_PRODUCER            0x07000000
#define FLAG_BAZAAR                 0x80000000

namespace Ashita {
    namespace FFXI {
        namespace Enums
        {
            ////////////////////////////////////////////////////////////////////////////////////////////////////
            //
            // Equipment / Inventory Related Enumerations
            //
            ////////////////////////////////////////////////////////////////////////////////////////////////////

            /**
             * Containers Enumeration
             */
            enum class Containers : uint32_t {
                Inventory = 0,
                Safe = 1,
                Storage = 2,
                Temporary = 3,
                Locker = 4,
                Satchel = 5,
                Sack = 6,
                Case = 7,
                Wardrobe = 8,
                Safe2 = 9,
                Wardrobe2 = 10,
                Wardrobe3 = 11,
                Wardrobe4 = 12,
                Wardrobe5 = 13,
                Wardrobe6 = 14,
                Wardrobe7 = 15,
                Wardrobe8 = 16,
                Recycle = 17,

                ContainerMax = 18
            };

            /**
             * Equipment Slots Enumeration
             */
            enum class EquipmentSlots : uint32_t {
                Main = 0,
                Sub,
                Range,
                Ammo,
                Head,
                Body,
                Hands,
                Legs,
                Feet,
				Neck,
                Waist,
                Ear1,
                Ear2,
                Ring1,
                Ring2,
                Back,

                EquipmentSlotMax = 16
            };

            /**
             * Treasure Status Enumeration
             */
            enum class TreasureStatus {
                None = 0,
                Pass,
                Lot
            };

            ////////////////////////////////////////////////////////////////////////////////////////////////////
            //
            // Entity Related Enumerations
            //
            ////////////////////////////////////////////////////////////////////////////////////////////////////

            /**
             * Entity Types Enumeration
             */
            enum class EntityTypes : uint32_t {
                Player = 0,     // Player Entities
                Npc1 = 1,       // NPC Entities (Town Folk, etc.)
                Npc2 = 2,       // NPC Entities (Home Points, Moogles, Coffers, Town Folk, etc.)
                Npc3 = 3,       // NPC Entities (Doors, Lights, Unique Objects, Bridges, etc.)
                Elevator = 4,   // Elevator Entities
                Airship = 5,    // Airship / Boat Entities
            };

            /**
             * Entity Spawn Flags Enumeration
             */
            enum class EntitySpawnFlags : uint32_t {
                Player = 0x0001,            // Entity is a player.
                Npc = 0x0002,               // Entity is an NPC.
                PartyMember = 0x0004,       // Entity is a party member.
                AllianceMember = 0x0008,    // Entity is an alliance member.
                Monster = 0x0010,           // Entity is a monster.
                Object = 0x0020,            // Entity is an object.
                LocalPlayer = 0x0200,       // Entity is the local player.
            };

            /**
             * Entity Hair Enumeration
             */
            enum class EntityHair {
                Hair1A = 0,
                Hair1B,
                Hair2A,
                Hair2B,
                Hair3A,
                Hair3B,
                Hair4A,
                Hair4B,
                Hair5A,
                Hair5B,
                Hair6A,
                Hair6B,
                Hair7A,
                Hair7B,
                Hair8A,
                Hair8B,

                // Non-Player Hair Styles
                Fomar = 29,
                Mannequin = 30,
            };

            /**
             * Entity Race Enumeration
             */
            enum class EntityRace {
                Invalid = 0,
                HumeMale,
                HumeFemale,
                ElvaanMale,
                ElvaanFemale,
                TarutaruMale,
                TarutaruFemale,
                Mithra,
                Galka,

                // Non-Player Races
                MithraChild = 29,
                HumeChildFemale = 30,
                HumeChildMale = 31,
                GoldChocobo = 32,
                BlackChocobo = 33,
                BlueChocobo = 34,
                RedChocobo = 35,
                GreenChocobo = 36
            };

            ////////////////////////////////////////////////////////////////////////////////////////////////////
            //
            // Player Related Enumerations
            //
            ////////////////////////////////////////////////////////////////////////////////////////////////////

            /**
             * Jobs Enumeration
             */
            enum class Jobs : uint32_t {
                None = 0,
                Warrior = 1,
                Monk = 2,
                WhiteMage = 3,
                BlackMage = 4,
                RedMage = 5,
                Thief = 6,
                Paladin = 7,
                DarkKnight = 8,
                Beastmaster = 9,
                Bard = 10,
                Ranger = 11,
                Samurai = 12,
                Ninja = 13,
                Dragoon = 14,
                Summoner = 15,
                BlueMage = 16,
                Corsair = 17,
                Puppetmaster = 18,
                Dancer = 19,
                Scholar = 20,
                Geomancer = 21,
                RuneFencer = 22
            };

            /**
             * Login Status Enumeration
             */
            enum class LoginStatus {
                LoginScreen = 0,
                Loading = 1,
                LoggedIn = 2
            };

            /**
             * Craft Rank Enumeration
             */
            enum class CraftRank {
                Amateur = 0,
                Recruit,
                Initiate,
                Novice,
                Apprentice,
                Journeyman,
                Craftsman,
                Artisan,
                Adept,
                Veteran
            };

            /**
             * Skill Types Enumeration
             */
            enum class SkillTypes : uint32_t {
                // Weapon Skills
                HandToHand = 1,
                Dagger = 2,
                Sword = 3,
                GreatSword = 4,
                Axe = 5,
                GreatAxe = 6,
                Scythe = 7,
                Polarm = 8,
                Katana = 9,
                GreatKatana = 10,
                Club = 11,
                Staff = 12,

                // Combat Skills
                Archery = 25,
                Marksmanship = 26,
                Throwing = 27,
                Guard = 28,
                Evasion = 29,
                Shield = 30,
                Parry = 31,
                Divine = 32,
                Healing = 33,
                Enhancing = 34,
                Enfeebling = 35,
                Elemental = 36,
                Dark = 37,
                Summoning = 38,
                Ninjutsu = 39,
                Singing = 40,
                String = 41,
                Wind = 42,
                BlueMagic = 43,

                // Crafting Skills
                Fishing = 48,
                Woodworking = 49,
                Smithing = 50,
                Goldsmithing = 51,
                Clothcraft = 52,
                Leathercraft = 53,
                Bonecraft = 54,
                Alchemy = 55,
                Cooking = 56,
                Synergy = 57,
                ChocoboDigging = 58,
            };

            /**
             * Player Nation Id
             */
            enum class Nation {
                SandOria,
                Bastok,
                Windurst
            };

            ////////////////////////////////////////////////////////////////////////////////////////////////////
            //
            // Resources Related Enumerations
            //
            ////////////////////////////////////////////////////////////////////////////////////////////////////

            /**
             * Equipment Slot Mask Enumeration
             */
            enum class EquipmentSlotMask {
                None = 0x0000,
                Main = 0x0001,
                Sub = 0x0002,
                Range = 0x0004,
                Ammo = 0x0008,
                Head = 0x0010,
                Body = 0x0020,
                Hands = 0x0040,
                Legs = 0x0080,
                Feet = 0x0100,
                Neck = 0x0200,
                Waist = 0x0400,
                LEar = 0x0800,
                REar = 0x1000,
                LRing = 0x2000,
                RRing = 0x4000,
                Back = 0x8000,

                // Slot Groups
                Ears = LEar | REar,
                Rings = LRing | RRing,

                // All Slots
                All = 0xFFFF
            };

            /**
             * Item Flags Enumeration
             */
            enum class ItemFlags : uint32_t {
                None = 0x0000,
                WallHanging = 0x0001,
                Flag1 = 0x0002,
                Flag2 = 0x0004,
                Flag3 = 0x0008,
                DeliveryInner = 0x0010,
                Inscribable = 0x0020,
                NoAuction = 0x0040,
                Scroll = 0x0080,
                Linkshell = 0x0100,
                CanUse = 0x0200,
                CanTradeNpc = 0x0400,
                CanEquip = 0x0800,
                NoSale = 0x1000,
                NoDelivery = 0x2000,
                NoTrade = 0x4000,
                Rare = 0x8000,

                Exclusive = NoAuction | NoDelivery | NoTrade,
                Nothing = Linkshell | NoSale | Exclusive | Rare
            };

            /**
             * Item Type Enumeration
             */
            enum class ItemType : uint32_t {
                None = 0x0000,
                Item = 0x0001,
                QuestItem = 0x0002,
                Fish = 0x0003,
                Weapon = 0x0004,
                Armor = 0x0005,
                Linkshell = 0x0006,
                UsableItem = 0x0007,
                Crystal = 0x0008,
                Currency = 0x0009,
                Furnishing = 0x000A,
                Plant = 0x000B,
                Flowerpot = 0x000C,
                PuppetItem = 0x000D,
                Mannequin = 0x000E,
                Book = 0x000F,
                RacingForm = 0x0010,
                BettingSlip = 0x0011,
                SoulPlate = 0x0012,
                Reflector = 0x0013,
                Logs = 0x0014,
                LotteryTicket = 0x0015,
                TabulaM = 0x0016,
                TabulaR = 0x0017,
                Voucher = 0x0018,
                Rune = 0x0019,
                Evolith = 0x001A,
                StorageSlip = 0x001B,
                Type1 = 0x001C
            };

            /**
             * Job Mask Enumeration
             */
            enum class JobMask : uint32_t {
                None = 0x00000000,
                WAR = 0x00000002,
                MNK = 0x00000004,
                WHM = 0x00000008,
                BLM = 0x00000010,
                RDM = 0x00000020,
                THF = 0x00000040,
                PLD = 0x00000080,
                DRK = 0x00000100,
                BST = 0x00000200,
                BRD = 0x00000400,
                RNG = 0x00000800,
                SAM = 0x00001000,
                NIN = 0x00002000,
                DRG = 0x00004000,
                SMN = 0x00008000,
                BLU = 0x00010000,
                COR = 0x00020000,
                PUP = 0x00040000,
                DNC = 0x00080000,
                SCH = 0x00100000,
                GEO = 0x00200000,
                RUN = 0x00400000,
                MON = 0x00800000,
                JOB24 = 0x01000000,
                JOB25 = 0x02000000,
                JOB26 = 0x04000000,
                JOB27 = 0x08000000,
                JOB28 = 0x10000000,
                JOB29 = 0x20000000,
                JOB30 = 0x40000000,
                JOB31 = 0x80000000,

                AllJobs = 0x007FFFFE,
            };

            /**
             * Combat Type Enumeration
             */
            enum class CombatType {
                Magic = 0x1000,
                Combat = 0x2000
            };

            /**
             * Ability Type Enumeration
             */
            enum class AbilityType {
                General = 0,
                Job,
                Pet,
                Weapon,
                Trait,
                BloodPactRage,
                Corsair,
                CorsairShot,
                BloodPactWard,
                Samba,
                Waltz,
                Step,
                Florish1,
                Scholar,
                Jig,
                Flourish2,
                Monster,
                Flourish3,
                Weaponskill,
                Rune,
                Ward,
                Effusion,
            };

            /**
             * Magic Type Enumeration
             */
            enum class MagicType : uint32_t {
                None = 0,
                WhiteMagic = 1,
                BlackMagic = 2,
                Summon = 3,
                Ninjutsu = 4,
                Song = 5,
                BlueMagic = 6,
                Geomancy = 7,
                Trust = 8
            };

            /**
             * Element Color Enumeration
             */
            enum class ElementColor {
                Red,
                Clear,
                Green,
                Yellow,
                Purple,
                Blue,
                White,
                Black
            };

            /**
             * Element Type Enumeration
             */
            enum class ElementType {
                Fire,
                Ice,
                Air,
                Earth,
                Thunder,
                Water,
                Light,
                Dark,

                Special = 0x0F,
                Unknown = 0xFF
            };

            /**
             * Puppet Slot Enumeration
             */
            enum class PuppetSlot {
                None,
                Head,
                Body,
                Attachment
            };

            /**
             * Race Mask Enumeration
             */
            enum class RaceMask {
                None = 0x0000,
                HumeMale = 0x0002,
                HumeFemale = 0x0004,
                ElvaanMale = 0x0008,
                ElvaanFemale = 0x0010,
                TarutaruMale = 0x0020,
                TarutaruFemale = 0x0040,
                Mithra = 0x0080,
                Galka = 0x0100,
                Hume = 0x0006,
                Elvaan = 0x0018,
                Tarutaru = 0x0060,

                Male = 0x012A,
                Female = 0x00D4,

                All = 0x01FE,
            };

            /**
             * Target Type Enumeration
             */
            enum class TargetType {
                None = 0x00,
                Self = 0x01,
                Player = 0x02,
                PartyMember = 0x04,
                AllianceMember = 0x08,
                Npc = 0x10,
                Enemy = 0x20,
                Unknown = 0x40,
                CorpseOnly = 0x80,
                Corpse = 0x9D
            };

            ////////////////////////////////////////////////////////////////////////////////////////////////////
            //
            // Vana'diel Related Enumerations
            //
            ////////////////////////////////////////////////////////////////////////////////////////////////////

            /**
             * Moon Phase Enumeration
             */
            enum class MoonPhase {
                New,
                WaxingCrescent,
                WaxingCrescent2,
                FirstQuarter,
                WaxingGibbous,
                WaxingGibbous2,
                Full,
                WaningGibbous,
                WaningGibbous2,
                LastQuarter,
                WaningCrescent,
                WaningCrescent2,

                Unknown
            };

            /**
             * Weekday Enumeration
             */
            enum class Weekday {
                Firesday,
                Earthsday,
                Watersday,
                Windsday,
                Iceday,
                Lightningday,
                Lightsday,
                Darksday,

                Unknown
            };

            /**
             * Weather Enumeration
             */
            enum class Weather : uint32_t {
                Clear = 0,
                Sunny,
                Cloudy,
                Fog,
                Fire,
                FireTwo,
                Water,
                WaterTwo,
                Earth,
                EarthTwo,
                Wind,
                WindTwo,
                Ice,
                IceTwo,
                Lightning,
                LightningTwo,
                Light,
                LightTwo,
                Dark,
                DarkTwo
            };
        }; // namespace Enums
    }; // namespace FFXI
}; // namespace Ashita

#endif // __ASHITA_AS_FFXI_ENUMS_H_INCLUDED__