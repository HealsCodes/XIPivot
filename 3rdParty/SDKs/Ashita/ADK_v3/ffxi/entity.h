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

#ifndef __ASHITA_AS_FFXI_ENTITY_H_INCLUDED__
#define __ASHITA_AS_FFXI_ENTITY_H_INCLUDED__

#if defined (_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

namespace Ashita
{
    namespace FFXI
    {
        struct position_t
        {
            float X;
            float Y;
            float Z;
            float Unknown0000;
            float Roll;
            float Yaw; // Heading
            float Pitch;

            bool operator==(const position_t &pos) const
            {
                return ((this->X == pos.X) && (this->Y == pos.Y) && (this->Z == pos.Z));
            }

            bool operator!=(const position_t &pos) const
            {
                return !(this == &pos);
            }
        };

        struct move_t
        {
            float X;
            float Y;
            float Z;
            float Unknown0000;

            bool operator==(const move_t &pos) const
            {
                return ((this->X == pos.X) && (this->Y == pos.Y) && (this->Z == pos.Z));
            }

            bool operator!=(const move_t &pos) const
            {
                return !(this == &pos);
            }
        };

        struct movement_t
        {
            position_t  LocalPosition;
            float       Unknown0000;
            position_t  LastPosition;
            uint32_t    Unknown0001;
            move_t      Move;
        };

        struct look_t
        {
            uint16_t Hair;      // Hair Style
            uint16_t Head;      // Head Armor       (Starts at 0x1000)
            uint16_t Body;      // Body Armor       (Starts at 0x2000)
            uint16_t Hands;     // Hands Armor      (Starts at 0x3000)
            uint16_t Legs;      // Legs Armor       (Starts at 0x4000)
            uint16_t Feet;      // Feet Armor       (Starts at 0x5000)
            uint16_t Main;      // Main Weapon      (Starts at 0x6000)
            uint16_t Sub;       // Sub Weapon       (Starts at 0x7000)
            uint16_t Ranged;    // Ranged Weapon    (Starts at 0x8000)
        };

        struct render_t
        {
            uint32_t Flags0;    // Main Render Flags
            uint32_t Flags1;    // Name Flags (Party, Away, Anon)
            uint32_t Flags2;    // Name Flags (Bazaar, GM Icon, etc.)
            uint32_t Flags3;    // Entity Flags (Shadow)
            uint32_t Flags4;    // Name Flags (Name Visibility)
        };

        struct animation_t
        {
            uint8_t Animations[0x28];
        };

#pragma pack(push, 1)
        struct ffxi_entity_t
        {
            uintptr_t       CYyObjectVtable;        // CYyObject
            movement_t      Movement;
            uint8_t         Unknown0000[28];
            uintptr_t       Unknown0001;            // Unknown Vtable pointer.
            uint32_t        TargetIndex;
            uint32_t        ServerId;
            int8_t          Name[28];
            float           Speed;
            float           AnimationSpeed;
            uintptr_t       WarpPointer;            // Pointer to the entities editable movement information.
            uint32_t        Unknown0002[13];
            float           Distance;
            uint32_t        Unknown0003;            // Usually 0x64.
            uint32_t        Unknown0004;            // Usually 0x64.
            float           Heading;                // Yaw
            uintptr_t       PetOwnerId;
            uint8_t         HealthPercent;
            uint8_t         ManaPercent;            // Pet only.
            uint8_t         EntityType;
            uint8_t         Race;
            uint32_t        Unknown0005;
            uint16_t        ModelFade;              // Used to force-refresh the entity model.
            uint8_t         Unknown0006[6];
            look_t          Look;
            uint8_t         Unknown0007[14];
            uint16_t        ActionTimer1;
            uint16_t        ActionTimer2;
            render_t        Render;
            uint32_t        Padding0000;
            float           Unknown0008;            // Fishing related.
            uint32_t        Unknown0009;            // Fade-in effect. (Valid values: 3 / 6)
            uint16_t        Unknown0010;            // Fade-in misc. (-1 gets reset to 0)
            uint32_t        Unknown0011;
            uint16_t        NpcSpeechLoop;
            uint16_t        NpcSpeechFrame;
            uint8_t         Unknown0012[18];
            float           Speed2;                 // Editable movement speed.
            uint16_t        NpcWalkPosition1;
            uint16_t        NpcWalkPosition2;
            uint16_t        NpcWalkMode;
            uint16_t        CostumeId;
            uint8_t         mou4[4];                // Always 'mou4'.
            uint32_t        Status;
            uint32_t        StatusServer;
            uint32_t        StatusNpcChat;          // Used while talking with an npc.
            uint32_t        Unknown0013;
            uint32_t        Unknown0014;
            uint32_t        Unknown0015;
            uint32_t        Unknown0016;
            uint32_t        ClaimServerId;          // The entities server id that has claim (or last claimed) the entity.
            uint32_t        Unknown0017;            // Inventory related.
            animation_t     Animations;             // The entities animation strings. (idl, sil, wlk, etc.)
            uint16_t        AnimationTick;          // Current ticks of the active animation.
            uint16_t        AnimationStep;          // Current step of the active animation.
            uint8_t         AnimationPlay;          // Current animation playing. (6 = Stand>Sit, 12 = Play current emote.)
            uint8_t         Unknown0018;            // Animation related.
            uint16_t        EmoteTargetIndex;       // The target index the emote is being performed on.
            uint16_t        EmoteId;                // The id of the emote.
            uint16_t        Unknown0019;
            uint32_t        EmoteIdString;          // The string id of emote.
            uintptr_t       EmoteTargetWarpPointer; // The emote target entities warp pointer.
            uint32_t        Unknown0020;
            uint32_t        SpawnFlags;             // 0x01 = PC, 0x02 = NPC, 0x10 = Mob, 0x0D = Self
            uint32_t        LinkshellColor;         // ARGB style color code.
            uint16_t        NameColor;              // Numerical code to pre-defined colors.
            uint16_t        CampaignNameFlag;       // Normally 0x4000, low-byte sets the flag.
            uint16_t        FishingTimer;           // Counts down from when you click 'fish' til you either make a catch or reel-in.
            uint16_t        FishingCastTimer;       // Counts down from when you click 'fish' til your bait hits the water.
            uint32_t        FishingUnknown0000;     // Fishing related.
            uint32_t        FishingUnknown0001;     // Fishing related.
            uint16_t        FishingUnknown0002;     // Fishing related.
            uint8_t         Unknown0021[14];
            uint16_t        TargetedIndex;          // The entities targeted index. (Does not always populate. Does not populate for local player.)
            uint16_t        PetTargetIndex;         // The entities pet target index.
            uint16_t        Unknown0022;            // Npc talking countdown timer. (After done talking with npc.)
            uint8_t         Unknown0023;            // Flag set after talking with an npc.
            uint8_t         BallistaScoreFlag;      // Ballista / PvP related.
            uint8_t         PankrationEnabled;
            uint8_t         PankrationFlagFlip;
            uint16_t        Unknown0024;
            float           ModelSize;
            uint8_t         Unknown0025[8];
            uint16_t        MonstrosityFlag;        // The entities monstrosity flags.
            uint16_t        MonstrosityNameId;      // The entities monstrosity name id.
            int8_t          MonstrosityName[36];    // The entities monstrosity name.
        };
#pragma pack(pop)
    }; // namespace FFXI
}; // namespace Ashita

#endif // __ASHITA_AS_FFXI_ENTITY_H_INCLUDED__