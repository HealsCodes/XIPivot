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

#ifndef __ASHITA_ADK_H_INCLUDED__
#define __ASHITA_ADK_H_INCLUDED__

#if defined (_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

 ////////////////////////////////////////////////////////////////////////////////////////////////////
 //
 // ReSharper Specific Disables (Please do not edit these!)
 //
 ////////////////////////////////////////////////////////////////////////////////////////////////////
 //
 // ReSharper disable CppUnusedIncludeDirective
 // ReSharper disable CppPolymorphicClassWithNonVirtualPublicDestructor
 //
 ////////////////////////////////////////////////////////////////////////////////////////////////////

 ////////////////////////////////////////////////////////////////////////////////////////////////////
 //
 // Ashita Interface Version
 //
 // Defines the current interface version that Ashita is compiled against. This version must match
 // when a plugin is loaded for it to work properly. Invalid versions will result in the plugin
 // not loading and printing an error.
 //
 // DO NOT EDIT THIS!
 //
 ////////////////////////////////////////////////////////////////////////////////////////////////////
#define ASHITA_INTERFACE_VERSION 3.2

////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Direct Input SDK Version Definition
//
////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef DIRECTINPUT_VERSION
#define DIRECTINPUT_VERSION 0x0800
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////
//
// General Includes
//
////////////////////////////////////////////////////////////////////////////////////////////////////
#include <Windows.h>
#include <functional>
#include "d3d8/includes/d3d8.h"
#include "d3d8/includes/d3dx8.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Ashita SDK Includes
//
////////////////////////////////////////////////////////////////////////////////////////////////////
#include "newIDirectInputDevice8A.h"
#include "AS_BinaryData.h"
#include "AS_CommandParser.h"
#include "AS_Event.h"
#include "AS_Exception.h"
#include "AS_LockableObject.h"
#include "AS_Memory.h"
#include "AS_Objects.h"
#include "AS_Registry.h"
#include "AS_Thread.h"
#include "ffxi/entity.h"
#include "ffxi/enums.h"
#include "ffxi/inventory.h"
#include "ffxi/party.h"
#include "ffxi/player.h"
#include "ffxi/target.h"
#include "imgui.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Forward Declarations
//
////////////////////////////////////////////////////////////////////////////////////////////////////

struct IEntity;             // IEntity forward-declaration.
struct IInventory;          // IInventory forward-declaration.
struct IParty;              // IParty forward-declaration.
struct IPlayer;             // IPlayer forward-declaration.
struct ITarget;             // ITarget forward-declaration.
struct IFontObject;         // IFontObject forward declaration.
struct IPrimitiveObject;    // IPrimitiveObject forward declaration.
struct IKeyboard;           // IKeyboard forward declaration.
struct IMouse;              // IMouse forward declaration.

////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Ashita Version Return Object
//
// Used with the GetVersion export of the Ashita.dll file, this can be used to read the
// current version of Ashita from the file properly.
//
////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma warning(disable : 4201)
union ashitaversion_t
{
    // The raw version value.
    uint64_t Version;

    // Union based data parts of Version.
    struct {
        uint16_t Major;
        uint16_t Minor;
        uint16_t Build;
        uint16_t Revision;
    };
};
#pragma warning(default : 4201)

////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Ashita Install Parameters
//
// Used with the Install export of Ashita.dll file. These are the parameters Ashita expects
// to be passed to the install call when it is first being prepared for usage.
//
////////////////////////////////////////////////////////////////////////////////////////////////////
struct ashitainstallparams_t
{
    HMODULE     ModuleInstance;         // The module instance of Ashita.dll.
    uint32_t    LanguageId;             // The language id to use.
    char        BootScript[MAX_PATH];   // The boot script to load for the boot configurations.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Forward Declarations
//
////////////////////////////////////////////////////////////////////////////////////////////////////
struct lua_State { };

////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Ashita Enumerations
//
// Enumerations used within Ashita's various interfaces and functions.
//
////////////////////////////////////////////////////////////////////////////////////////////////////
namespace Ashita
{
    /**
     * Command Input Type Enumeration
     *
     * Used with the Ashita ChatManager command functions.
     */
    enum class CommandInputType : int32_t {
        ForceHandle = -3,       // Informs Ashita to handle the command by force.
        Script = -2,            // Informs Ashita to handle the command as an Ashita based script command.
        Parse = -1,             // Informs Ashita to handle the command as an Ashita based command.
        Menu = 0,               // Informs Ashita to handle the command as if it were input via an in-game menu.
        Typed = 1,              // Informs Ashita to handle the command as if it were typed into the chat window by the player.
        Macro = 2               // Informs Ashita to handle the command as if it were invoked from a macro.
    };

    /**
     * Frame Anchor Enumeration
     *
     * Used with Ashita's custom font objects.
     */
    enum class FrameAnchor : uint32_t {
        TopLeft = 0,            // Anchors the font object to the top-left.
        TopRight = 1,           // Anchors the font object to the top-right.
        BottomLeft = 2,         // Anchors the font object to the bottom-left.
        BottomRight = 3,        // Anchors the font object to the bottom-right.
        Right = 1,              // Anchors the font object to the right.
        Bottom = 2              // Anchors the font object to the bottom.
    };

    /**
     * FrameAnchor bitwise AND operator override.
     *
     * @param {FrameAnchor} a - The first frame anchor value.
     * @param {FrameAnchor} b - The second frame anchor value.
     * @returns {uint32_t} The AND value of a and b.
     */
    inline uint32_t operator& (const FrameAnchor& a, const FrameAnchor& b)
    {
        return (uint32_t)((uint32_t)a & (uint32_t)b);
    }

    /**
     * Mouse Input Enumeration
     *
     * Used with Ashita's custom font objects mouse event callback.
     */
    enum class MouseInput : uint32_t {
        LeftClick = 0,          // Left mouse button was clicked.
        RightClick = 1,         // Right mouse button was clicked.
        MiddleClick = 2,        // Middle mouse button was clicked.
        X1Click = 3,            // X1 mouse button was clicked.
        X2Click = 4,            // X2 mouse button was clicked.
        MouseWheelUp = 5,       // Mouse wheel was scrolled up.
        MouseWheelDown = 6,     // Mouse wheel was scrolled down.
        MouseMove = 7           // Mouse was moved over the object.
    };

    /**
     * Log Level Enumeration
     *
     * Used with Ashita's logging manager.
     */
    enum class LogLevel : uint32_t {
        None = 0,
        Information = 1,
        Warning = 2,
        Error = 3,
        Debug = 4
    };
}; // namespace Ashita

////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Input Defines
//
////////////////////////////////////////////////////////////////////////////////////////////////////

typedef HRESULT /**/(__stdcall *getdatacallback_t)(DWORD, LPDIDEVICEOBJECTDATA, DWORD, LPDWORD, DWORD);
typedef HRESULT /**/(__stdcall *getstatecallback_t)(DWORD, LPVOID);
typedef BOOL    /**/(__stdcall *keyboardcallback_t)(WPARAM, LPARAM);
typedef BOOL    /**/(__stdcall *mousecallback_t)(UINT, WPARAM, LPARAM);

///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Font Object Defines
//
///////////////////////////////////////////////////////////////////////////////////////////////////

typedef std::function<void(int32_t, void*, int32_t, int32_t)> MOUSEEVENT;

///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Resource File Definitions
//
// IAbility
//
//      An ability entry parsed from the games data files. Contatins information on how the 
//      ability works, requirements, and other useful information.
//
// ISpell
//
//      A spell entry parsed from the games data files. Contains information on how the spell
//      works, requirements, and other useful information.
//
// IMonstrosityAbility
//
//      A monstrosity ability entry. Contains info on the abilities requirements. (Used for items.)
//
//  IItem
//
//      An item entry parsed from the games data files. Contains information on how the item
//      works, can be used, requirements, and other useful information. Also contains the items
//      bitmap icon used and displayed in-game.
//
///////////////////////////////////////////////////////////////////////////////////////////////////

struct IAbility
{
    uint16_t    Id;                 // The abilities id. (Not the same as its recast id!)
    uint8_t     Type;               // The abilities type. (Type & 7)
    uint8_t     Element;            // The abilities element.
    uint16_t    ListIconId;         // The abilities list icon id.
    uint16_t    ManaCost;           // The abilities mana cost.
    uint16_t    TimerId;            // The abilities timer id.
    uint16_t    ValidTargets;       // The abilities valid targets.
    int16_t     TP;                 // The abilities TP cost.
    uint8_t     Unknown0000;        // Unknown.
    uint8_t     MonsterLevel;       // The abilities monster level.
    int8_t      Range;              // The abilities usage range. (Range % 0x0F)
    uint8_t     Unknown0001[30];    // Unknown.
    uint8_t     EOE;                // EOE (End of entry. 0xFF)

    const char* Name[3];            // The abilities name. (0 = Default, 1 = Japanese, 2 = English)
    const char* Description[3];     // The abilities description. (0 = Default, 1 = Japanese, 2 = English)
};

struct ISpell
{
    uint16_t    Index;              // The spells index. (Used with recast.)
    uint16_t    Type;               // The spells magic type.
    uint16_t    Element;            // The spells element type.
    uint16_t    ValidTargets;       // The spells valid targets.
    uint16_t    Skill;              // The spells magic skill type.
    uint16_t    ManaCost;           // The spells mana coast.
    uint8_t     CastTime;           // The spells cast time. (CastTime / 4.0)
    uint8_t     RecastDelay;        // The spells recast delay. (RecastDelay / 4.0)
    int16_t     LevelRequired[24];  // The spells level requirements. (Per-job, if 0xFF, cannot be learned.)
    uint16_t    Id;                 // The spells id.
    uint16_t    ListIcon1;          // The spells icon id (1).
    uint16_t    ListIcon2;          // The spells icon id (2).
    uint8_t     Requirements;       // The spells requirements.
    int8_t      Range;              // The spells casting range. (Range % 0x0F)
    uint8_t     Unknown0000[29];    // Unknown.
    uint8_t     EOE;                // EOE (End of entry. 0xFF)

    const char* Name[3];            // The spells name. (0 = Default, 1 = Japanese, 2 = English)
    const char* Description[3];     // The spells description. (0 = Default, 1 = Japanese, 2 = English)
};

struct IMonstrosityAbility
{
    uint16_t    MoveId;             // The monster move id of the ability.
    int8_t      Level;              // The monster level of the ability.
    uint8_t     Unknown0000;        // Unknown.
};

struct IItem
{
    uint32_t    ItemId;             // The items id.
    uint16_t    Flags;              // The items flags.
    uint16_t    StackSize;          // The items stack size.
    uint16_t    ItemType;           // The items type.
    uint16_t    ResourceId;         // The items resource id. (Mainly used for AH sorting.)
    uint16_t    ValidTargets;       // The items valid targets for use.

    uint16_t    Level;              // The items level requirement to use.
    uint16_t    Slots;              // The items equipment slots where the item can be equipped to.
    uint16_t    Races;              // The items races that can use the item.
    uint32_t    Jobs;               // The items jobs that can use the item.
    uint8_t     SuperiorLevel;      // The items superior level.
    uint16_t    ShieldSize;         // The items shield size.
    uint8_t     MaxCharges;         // The items max charges.
    uint16_t    CastTime;           // The items cast time.
    uint16_t    CastDelay;          // The items cast delay.
    uint32_t    RecastDelay;        // The items recast delay.
    uint16_t    BaseItemId;         // The items base item id used for upgrades.
    uint16_t    ItemLevel;          // The items item level.
    uint16_t    Damage;             // The items damage.
    uint16_t    Delay;              // The items delay.
    uint16_t    DPS;                // The items damae per second.
    uint8_t     Skill;              // The items skill type.
    uint8_t     JugSize;            // The items jug size.

    uint16_t    InstinctCost;       // The items instinct cost.

    uint16_t    MonstrosityId;                      // The items monstrosity id.
    char        MonstrosityName[32];                // The items monstrosity name.
    IMonstrosityAbility MonstrosityAbilities[16];   // The items monstrosity abilities.

    uint16_t    PuppetSlotId;       // The items slot id (for PUP).
    uint32_t    PuppetElements;     // The items elements (for PUP).

    const char* Name[3];            // The items name.
    const char* Description[3];     // The items description.
    const char* LogNameSingular[3]; // The items log name (singular).
    const char* LogNamePlural[3];   // The items log name (plural).

    uint32_t    ImageSize;          // The items image size.
    uint8_t     ImageType;          // The items image type.
    uint8_t     ImageName[0x10];    // The items image name.
    uint8_t     Bitmap[0x980];      // The items bitmap data.
};

///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Chat Manager
//
// Ashita chat manager that interacts with the games chat and command systems.
//
///////////////////////////////////////////////////////////////////////////////////////////////////

interface IChatManager
{
    // Command Functions
    virtual void ParseCommand(const char* command, int32_t type) = 0;
    virtual void QueueCommand(const char* command, int32_t type) = 0;

    // Chat Functions
    virtual void AddChatMessage(int32_t mode, const char* message) = 0;
    virtual int32_t ParseAutoTranslate(const char* message, char* buffer, int32_t bufferSize, bool useBrackets) = 0;

    // Script Functions
    virtual void RunScript(bool useTaskQueue, const char* file) = 0;

    // Input Text Functions
    virtual const char* GetInputText(void) const = 0;
    virtual void SetInputText(const char* message) = 0;
    virtual bool IsInputOpen(void) = 0;

    // Chat Functions (Helpers)
    virtual void Write(const char* msg) = 0;
    virtual void Writef(const char* format, ...) = 0;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Configuration Manager
//
// Ashita configuration manager that interacts with simple XML based configuration files.
//
///////////////////////////////////////////////////////////////////////////////////////////////////

interface IConfigurationManager
{
    // Configuration File Functions
    virtual bool Load(const char* alias, const char* file) = 0;
    virtual bool Save(const char* alias, const char* file) = 0;
    virtual void Remove(const char* alias) = 0;

    // Value Related Functions
    virtual void set_value(const char* alias, const char* name, const char* value) = 0;
    virtual const char* get_string(const char* alias, const char* name) = 0;
    virtual bool get_bool(const char* alias, const char* name, bool defaultValue) = 0;
    virtual uint8_t get_uint8(const char* alias, const char* name, uint8_t defaultValue) = 0;
    virtual uint16_t get_uint16(const char* alias, const char* name, uint16_t defaultValue) = 0;
    virtual uint32_t get_uint32(const char* alias, const char* name, uint32_t defaultValue) = 0;
    virtual uint64_t get_uint64(const char* alias, const char* name, uint64_t defaultValue) = 0;
    virtual int8_t get_int8(const char* alias, const char* name, int8_t defaultValue) = 0;
    virtual int16_t get_int16(const char* alias, const char* name, int16_t defaultValue) = 0;
    virtual int32_t get_int32(const char* alias, const char* name, int32_t defaultValue) = 0;
    virtual int64_t get_int64(const char* alias, const char* name, int64_t defaultValue) = 0;
    virtual float get_float(const char* alias, const char* name, float defaultValue) = 0;
    virtual double get_double(const char* alias, const char* name, double defaultValue) = 0;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Data Manager
//
// Ashita data manager that interacts with various in-game memory structures such as
// inventory, player, party, target, etc.
//
///////////////////////////////////////////////////////////////////////////////////////////////////

interface IDataManager
{
    virtual IEntity* GetEntity(void) = 0;
    virtual IInventory* GetInventory(void) = 0;
    virtual IParty* GetParty(void) = 0;
    virtual IPlayer* GetPlayer(void) = 0;
    virtual ITarget* GetTarget(void) = 0;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Font Manager
//
// Ashita font manager that interacts with Ashitas built-in highly customizable font objects.
//
///////////////////////////////////////////////////////////////////////////////////////////////////

interface IFontManager
{
    virtual IFontObject* Create(const char* alias) = 0;
    virtual IFontObject* Get(const char* alias) = 0;
    virtual void Delete(const char* alias) = 0;
    virtual bool GetHideObjects(void) const = 0;
    virtual void SetHideObjects(bool hide) = 0;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Font Object
//
// Ashita font object interface that allows interaction with the highly customizable font objects.
//
///////////////////////////////////////////////////////////////////////////////////////////////////

interface IFontObject
{
    // Helper Functions
    virtual bool HitTest(int32_t x, int32_t y) = 0;

    // Property Getters
    virtual const char* GetAlias(void) const = 0;
    virtual bool GetVisibility(void) const = 0;
    virtual const char* GetFontFamily(void) const = 0;
    virtual int32_t GetFontHeight(void) const = 0;
    virtual float GetPositionX(void) const = 0;
    virtual float GetPositionY(void) const = 0;
    virtual bool GetLocked(void) const = 0;
    virtual float GetPadding(void) const = 0;
    virtual uint32_t GetAnchor(void) const = 0;
    virtual uint32_t GetAnchorParent(void) const = 0;
    virtual D3DCOLOR GetColor(void) const = 0;
    virtual uint32_t GetCreateFlags(void) const = 0;
    virtual uint32_t GetDrawFlags(void) const = 0;
    virtual bool GetBold(void) const = 0;
    virtual bool GetItalic(void) const = 0;
    virtual bool GetRightJustified(void) const = 0;
    virtual const char* GetText(void) const = 0;
    virtual bool GetDirtyFlag(void) const = 0;
    virtual bool GetAutoResize(void) const = 0;
    virtual float GetWindowWidth(void) const = 0;
    virtual float GetWindowHeight(void) const = 0;
    virtual float GetRealPositionX(void) const = 0;
    virtual float GetRealPositionY(void) const = 0;
    virtual MOUSEEVENT GetMouseEventFunction(void) const = 0;
    virtual void GetTextSize(SIZE* size) const = 0;
    virtual IFontObject* GetParent(void) const = 0;

    virtual IPrimitiveObject* GetBackground(void) const = 0;

    // Property Setters
    virtual void SetAlias(const char* alias) = 0;
    virtual void SetVisibility(bool visible) = 0;
    virtual void SetFontFamily(const char* family) = 0;
    virtual void SetFontHeight(int32_t height) = 0;
    virtual void SetPositionX(float x) = 0;
    virtual void SetPositionY(float y) = 0;
    virtual void SetLocked(bool locked) = 0;
    virtual void SetPadding(float size) = 0;
    virtual void SetAnchor(uint32_t anchor) = 0;
    virtual void SetAnchorParent(uint32_t anchor) = 0;
    virtual void SetColor(D3DCOLOR color) = 0;
    virtual void SetCreateFlags(uint32_t flags) = 0;
    virtual void SetDrawFlags(uint32_t flags) = 0;
    virtual void SetBold(bool bold) = 0;
    virtual void SetItalic(bool italic) = 0;
    virtual void SetRightJustified(bool justified) = 0;
    virtual void SetText(const char* text) = 0;
    virtual void SetDirtyFlag(bool dirty) = 0;
    virtual void SetAutoResize(bool resize) = 0;
    virtual void SetWindowWidth(float width) = 0;
    virtual void SetWindowHeight(float height) = 0;
    virtual void SetMouseEventFunction(MOUSEEVENT func) = 0;
    virtual void SetParent(IFontObject* parent) = 0;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Input Manager
//
// Ashita input manager that interacts with the hooked keyboard and mouse devices.
//
///////////////////////////////////////////////////////////////////////////////////////////////////

interface IInputManager
{
    virtual IKeyboard* GetKeyboard(void) const = 0;
    virtual IMouse* GetMouse(void) const = 0;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Log Manager
//
// Ashita log manager that interacts with the per-instance created log file that Ashita uses to
// record various information about its operation. This file can be used to help debug issues
// with addons and plugins.
//
///////////////////////////////////////////////////////////////////////////////////////////////////

interface ILogManager
{
    virtual bool Log(uint32_t level, const char* source, const char* msg) = 0;
    virtual bool Logf(uint32_t level, const char* source, const char* fmt, ...) = 0;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Packet Manager
//
// Ashita packet manager that interacts with the games packet system.
//
///////////////////////////////////////////////////////////////////////////////////////////////////

interface IPacketManager
{
    virtual void AddIncomingPacket(uint16_t id, uint32_t len, void* data) = 0;
    virtual void AddOutgoingPacket(uint16_t id, uint32_t len, void* data) = 0;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Plugin Manager
//
// Ashita plugin manager that interacts with Ashitas internal plugin system.
//
///////////////////////////////////////////////////////////////////////////////////////////////////

interface IPluginManager
{
    virtual bool Load(const char* name) = 0;
    virtual bool Unload(const char* name) = 0;
    virtual void UnloadAll(void) = 0;
    virtual void* GetPlugin(const char* name) = 0;
    virtual void* GetPlugin(uint32_t index) = 0;
    virtual uint32_t GetPluginCount(void) = 0;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Pointer Manager
//
// Ashita pointer manager that interacts with Ashitas internal pointer system.
//
///////////////////////////////////////////////////////////////////////////////////////////////////

interface IPointerManager
{
    virtual uintptr_t GetPointer(const char* name) = 0;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Primitive Object
//
// Ashita primitive object interface that allows interaction with the primitive objects created
// within Ashita via the Font Manager.
//
///////////////////////////////////////////////////////////////////////////////////////////////////

interface IPrimitiveObject
{
    // Helper Functions
    virtual void SetTextureFromFile(const char* file) = 0;
    virtual void SetTextureFromResource(const char* moduleName, const char* resource) = 0;
    virtual bool HitTest(int32_t x, int32_t y) = 0;

    // Property Getters
    virtual bool GetVisibility(void) const = 0;
    virtual float GetPositionX(void) const = 0;
    virtual float GetPositionY(void) const = 0;
    virtual float GetWidth(void) const = 0;
    virtual float GetHeight(void) const = 0;
    virtual D3DCOLOR GetColor(void) const = 0;
    virtual bool GetBorderVisibility(void) const = 0;
    virtual D3DCOLOR GetBorderColor(void) const = 0;
    virtual uint32_t GetBorderFlags(void) const = 0;
    virtual Ashita::asrect_t GetBorderSizes(void) const = 0;

    // Property Setters
    virtual void SetVisibility(bool isVisible) = 0;
    virtual void SetPositionX(float x) = 0;
    virtual void SetPositionY(float y) = 0;
    virtual void SetWidth(float width) = 0;
    virtual void SetHeight(float height) = 0;
    virtual void SetColor(D3DCOLOR color) = 0;
    virtual void SetBorderVisibility(bool isVisible) = 0;
    virtual void SetBorderColor(D3DCOLOR color) = 0;
    virtual void SetBorderFlags(uint32_t flags) = 0;
    virtual void SetBorderSizes(uint32_t top, uint32_t right, uint32_t bottom, uint32_t left) = 0;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Resource Manager
//
// Ashita resource manager interface that interacts with the internal DAT file parser to obtain
// various information about the games content. (Abilities, Spells, Items, Various Strings, etc.)
//
///////////////////////////////////////////////////////////////////////////////////////////////////

interface IResourceManager
{
    // Ability Functions
    virtual IAbility* GetAbilityById(uint32_t id) = 0;
    virtual IAbility* GetAbilityByName(const char* name, uint32_t languageId) = 0;
    virtual IAbility* GetAbilityByTimerId(uint32_t id) = 0;

    // Spell Functions
    virtual ISpell* GetSpellById(uint32_t id) = 0;
    virtual ISpell* GetSpellByName(const char* name, uint32_t languageId) = 0;

    // Item Functions
    virtual IItem* GetItemById(uint32_t id) = 0;
    virtual IItem* GetItemByName(const char* name, uint32_t languageId) = 0;

    // String Functions
    virtual const char* GetString(const char* table, uint32_t index) = 0;
    virtual const char* GetString(const char* table, uint32_t index, uint32_t languageId) = 0;
    virtual int32_t GetString(const char* table, const char* name) = 0;
    virtual int32_t GetString(const char* table, const char* name, uint32_t languageId) = 0;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Data Related Interfaces
//
// IEntity
//
//      Exposes functions to interact with the games various entities. (Players, Monsters, NPCs, etc.)
//
// IInventory
//
//      Exposes functions to interact with the inventory block of the games data. This includes the
//      players inventory and storage, the players equipment, and the treasure pool.
//
// IParty
//
//      Exposes functions to interact with the games party data.
//
// IPlayer
//
//      Exposes functions to interact with the local players information. (Level, job, skills, etc.)
//
// ITarget
//
//      Exposes functions to interact with the current target information.
//
///////////////////////////////////////////////////////////////////////////////////////////////////

interface IEntity
{
    // WARNING: Plugins should not use this function!
    //
    //      Use the Get/Set functions instead to help ensure your plugin does not
    //      break between updates of Ashita!
    virtual Ashita::FFXI::ffxi_entity_t* GetEntity(uint32_t index) const = 0;

    // Property Getters
    virtual float GetLocalX(uint32_t index) const = 0;
    virtual float GetLocalY(uint32_t index) const = 0;
    virtual float GetLocalZ(uint32_t index) const = 0;
    virtual float GetLocalRoll(uint32_t index) const = 0;
    virtual float GetLocalYaw(uint32_t index) const = 0;
    virtual float GetLocalPitch(uint32_t index) const = 0;
    virtual float GetLastX(uint32_t index) const = 0;
    virtual float GetLastY(uint32_t index) const = 0;
    virtual float GetLastZ(uint32_t index) const = 0;
    virtual float GetLastRoll(uint32_t index) const = 0;
    virtual float GetLastYaw(uint32_t index) const = 0;
    virtual float GetLastPitch(uint32_t index) const = 0;
    virtual float GetMoveX(uint32_t index) const = 0;
    virtual float GetMoveY(uint32_t index) const = 0;
    virtual float GetMoveZ(uint32_t index) const = 0;
    virtual uint32_t GetTargetIndex(uint32_t index) const = 0;
    virtual uint32_t GetServerId(uint32_t index) const = 0;
    virtual const char* GetName(uint32_t index) const = 0;
    virtual float GetSpeed(uint32_t index) const = 0;
    virtual float GetAnimationSpeed(uint32_t index) const = 0;
    virtual uintptr_t GetWarpPointer(uint32_t index) const = 0;
    virtual float GetDistance(uint32_t index) const = 0;
    virtual float GetHeading(uint32_t index) const = 0;
    virtual uintptr_t GetPetOwnerId(uint32_t index) const = 0;
    virtual uint8_t GetHealthPercent(uint32_t index) const = 0;
    virtual uint8_t GetManaPercent(uint32_t index) const = 0;
    virtual uint8_t GetEntityType(uint32_t index) const = 0;
    virtual uint8_t GetRace(uint32_t index) const = 0;
    virtual uint16_t GetModelFade(uint32_t index) const = 0;
    virtual uint16_t GetLookHair(uint32_t index) const = 0;
    virtual uint16_t GetLookHead(uint32_t index) const = 0;
    virtual uint16_t GetLookBody(uint32_t index) const = 0;
    virtual uint16_t GetLookHands(uint32_t index) const = 0;
    virtual uint16_t GetLookLegs(uint32_t index) const = 0;
    virtual uint16_t GetLookFeet(uint32_t index) const = 0;
    virtual uint16_t GetLookMain(uint32_t index) const = 0;
    virtual uint16_t GetLookSub(uint32_t index) const = 0;
    virtual uint16_t GetLookRanged(uint32_t index) const = 0;
    virtual uint16_t GetActionTimer1(uint32_t index) const = 0;
    virtual uint16_t GetActionTimer2(uint32_t index) const = 0;
    virtual uint32_t GetRenderFlags0(uint32_t index) const = 0;
    virtual uint32_t GetRenderFlags1(uint32_t index) const = 0;
    virtual uint32_t GetRenderFlags2(uint32_t index) const = 0;
    virtual uint32_t GetRenderFlags3(uint32_t index) const = 0;
    virtual uint32_t GetRenderFlags4(uint32_t index) const = 0;
    virtual uint16_t GetNpcSpeechLoop(uint32_t index) const = 0;
    virtual uint16_t GetNpcSpeechFrame(uint32_t index) const = 0;
    virtual float GetSpeed2(uint32_t index) const = 0;
    virtual uint16_t GetNpcWalkPosition1(uint32_t index) const = 0;
    virtual uint16_t GetNpcWalkPosition2(uint32_t index) const = 0;
    virtual uint16_t GetNpcWalkMode(uint32_t index) const = 0;
    virtual uint16_t GetCostumeId(uint32_t index) const = 0;
    virtual uint32_t GetStatus(uint32_t index) const = 0;
    virtual uint32_t GetStatusServer(uint32_t index) const = 0;
    virtual uint32_t GetStatusNpcChat(uint32_t index) const = 0;
    virtual uint32_t GetClaimServerId(uint32_t index) const = 0;
    virtual uint8_t* GetAnimations(uint32_t index) const = 0;
    virtual uint16_t GetAnimationTick(uint32_t index) const = 0;
    virtual uint16_t GetAnimationStep(uint32_t index) const = 0;
    virtual uint8_t GetAnimationPlay(uint32_t index) const = 0;
    virtual uint16_t GetEmoteTargetIndex(uint32_t index) const = 0;
    virtual uint16_t GetEmoteId(uint32_t index) const = 0;
    virtual uint32_t GetEmoteIdString(uint32_t index) const = 0;
    virtual uintptr_t GetEmoteTargetWarpPointer(uint32_t index) const = 0;
    virtual uint32_t GetSpawnFlags(uint32_t index) const = 0;
    virtual uint32_t GetLinkshellColor(uint32_t index) const = 0;
    virtual uint16_t GetNameColor(uint32_t index) const = 0;
    virtual uint16_t GetCampaignNameFlag(uint32_t index) const = 0;
    virtual uint16_t GetFishingTimer(uint32_t index) const = 0;
    virtual uint16_t GetFishingCastTimer(uint32_t index) const = 0;
    virtual uint32_t GetFishingUnknown0000(uint32_t index) const = 0;
    virtual uint32_t GetFishingUnknown0001(uint32_t index) const = 0;
    virtual uint16_t GetFishingUnknown0002(uint32_t index) const = 0;
    virtual uint16_t GetTargetedIndex(uint32_t index) const = 0;
    virtual uint16_t GetPetTargetIndex(uint32_t index) const = 0;
    virtual uint8_t GetBallistaScoreFlag(uint32_t index) const = 0;
    virtual uint8_t GetPankrationEnabled(uint32_t index) const = 0;
    virtual uint8_t GetPankrationFlagFlip(uint32_t index) const = 0;
    virtual float GetModelSize(uint32_t index) const = 0;
    virtual uint16_t GetMonstrosityFlag(uint32_t index) const = 0;
    virtual uint16_t GetMonstrosityNameId(uint32_t index) const = 0;
    virtual const char* GetMonstrosityName(uint32_t index) const = 0;

    // Property Setters
    virtual void SetLocalX(uint32_t index, float x) const = 0;
    virtual void SetLocalY(uint32_t index, float y) const = 0;
    virtual void SetLocalZ(uint32_t index, float z) const = 0;
    virtual void SetLocalRoll(uint32_t index, float roll) const = 0;
    virtual void SetLocalYaw(uint32_t index, float yaw) const = 0;
    virtual void SetLocalPitch(uint32_t index, float pitch) const = 0;
    virtual void SetLastX(uint32_t index, float x) const = 0;
    virtual void SetLastY(uint32_t index, float y) const = 0;
    virtual void SetLastZ(uint32_t index, float z) const = 0;
    virtual void SetLastRoll(uint32_t index, float roll) const = 0;
    virtual void SetLastYaw(uint32_t index, float yaw) const = 0;
    virtual void SetLastPitch(uint32_t index, float pitch) const = 0;
    virtual void SetMoveX(uint32_t index, float x) const = 0;
    virtual void SetMoveY(uint32_t index, float y) const = 0;
    virtual void SetMoveZ(uint32_t index, float z) const = 0;
    virtual void SetTargetIndex(uint32_t index, uint32_t targetIndex) const = 0;
    virtual void SetServerId(uint32_t index, uint32_t serverId) const = 0;
    virtual void SetName(uint32_t index, const char* name) const = 0;
    virtual void SetSpeed(uint32_t index, float speed) const = 0;
    virtual void SetAnimationSpeed(uint32_t index, float speed) const = 0;
    virtual void SetWarpPointer(uint32_t index, uintptr_t ptr) const = 0;
    virtual void SetDistance(uint32_t index, float distance) const = 0;
    virtual void SetHeading(uint32_t index, float heading) const = 0;
    virtual void SetPetOwnerId(uint32_t index, uintptr_t petOwnerId) const = 0;
    virtual void SetHealthPercent(uint32_t index, uint8_t hpp) const = 0;
    virtual void SetManaPercent(uint32_t index, uint8_t mpp) const = 0;
    virtual void SetEntityType(uint32_t index, uint8_t type) const = 0;
    virtual void SetRace(uint32_t index, uint8_t race) const = 0;
    virtual void SetModelFade(uint32_t index, uint16_t fade) const = 0;
    virtual void SetLookHair(uint32_t index, uint16_t hair) const = 0;
    virtual void SetLookHead(uint32_t index, uint16_t head) const = 0;
    virtual void SetLookBody(uint32_t index, uint16_t body) const = 0;
    virtual void SetLookHands(uint32_t index, uint16_t hands) const = 0;
    virtual void SetLookLegs(uint32_t index, uint16_t legs) const = 0;
    virtual void SetLookFeet(uint32_t index, uint16_t feet) const = 0;
    virtual void SetLookMain(uint32_t index, uint16_t main) const = 0;
    virtual void SetLookSub(uint32_t index, uint16_t sub) const = 0;
    virtual void SetLookRanged(uint32_t index, uint16_t ranged) const = 0;
    virtual void SetActionTimer1(uint32_t index, uint16_t time) const = 0;
    virtual void SetActionTimer2(uint32_t index, uint16_t time) const = 0;
    virtual void SetRenderFlags0(uint32_t index, uint32_t flags) const = 0;
    virtual void SetRenderFlags1(uint32_t index, uint32_t flags) const = 0;
    virtual void SetRenderFlags2(uint32_t index, uint32_t flags) const = 0;
    virtual void SetRenderFlags3(uint32_t index, uint32_t flags) const = 0;
    virtual void SetRenderFlags4(uint32_t index, uint32_t flags) const = 0;
    virtual void SetNpcSpeechLoop(uint32_t index, uint16_t loop) const = 0;
    virtual void SetNpcSpeechFrame(uint32_t index, uint16_t frame) const = 0;
    virtual void SetSpeed2(uint32_t index, float speed) const = 0;
    virtual void SetNpcWalkPosition1(uint32_t index, uint16_t pos) const = 0;
    virtual void SetNpcWalkPosition2(uint32_t index, uint16_t pos) const = 0;
    virtual void SetNpcWalkMode(uint32_t index, uint16_t mode) const = 0;
    virtual void SetCostumeId(uint32_t index, uint16_t costume) const = 0;
    virtual void SetStatus(uint32_t index, uint32_t status) const = 0;
    virtual void SetStatusServer(uint32_t index, uint32_t status) const = 0;
    virtual void SetStatusNpcChat(uint32_t index, uint32_t status) const = 0;
    virtual void SetClaimServerId(uint32_t index, uint32_t claimid) const = 0;
    virtual void SetAnimations(uint32_t index, uint8_t* data) const = 0;
    virtual void SetAnimationTick(uint32_t index, uint16_t tick) const = 0;
    virtual void SetAnimationStep(uint32_t index, uint16_t step) const = 0;
    virtual void SetAnimationPlay(uint32_t index, uint8_t play) const = 0;
    virtual void SetEmoteTargetIndex(uint32_t index, uint16_t targetIndex) const = 0;
    virtual void SetEmoteId(uint32_t index, uint16_t emoteid) const = 0;
    virtual void SetEmoteIdString(uint32_t index, uint32_t emoteid) const = 0;
    virtual void SetEmoteTargetWarpPointer(uint32_t index, uintptr_t warpPointer) const = 0;
    virtual void SetSpawnFlags(uint32_t index, uint32_t flags) const = 0;
    virtual void SetLinkshellColor(uint32_t index, uint32_t color) const = 0;
    virtual void SetNameColor(uint32_t index, uint16_t color) const = 0;
    virtual void SetCampaignNameFlag(uint32_t index, uint16_t flags) const = 0;
    virtual void SetFishingTimer(uint32_t index, uint16_t timer) const = 0;
    virtual void SetFishingCastTimer(uint32_t index, uint16_t timer) const = 0;
    virtual void SetFishingUnknown0000(uint32_t index, uint32_t unk) const = 0;
    virtual void SetFishingUnknown0001(uint32_t index, uint32_t unk) const = 0;
    virtual void SetFishingUnknown0002(uint32_t index, uint16_t unk) const = 0;
    virtual void SetTargetedIndex(uint32_t index, uint16_t targetedIndex) const = 0;
    virtual void SetPetTargetIndex(uint32_t index, uint16_t petIndex) const = 0;
    virtual void SetBallistaScoreFlag(uint32_t index, uint8_t flag) const = 0;
    virtual void SetPankrationEnabled(uint32_t index, uint8_t enabled) const = 0;
    virtual void SetPankrationFlagFlip(uint32_t index, uint8_t flagflip) const = 0;
    virtual void SetModelSize(uint32_t index, float size) const = 0;
    virtual void SetMonstrosityFlag(uint32_t index, uint16_t flag) const = 0;
    virtual void SetMonstrosityNameId(uint32_t index, uint16_t nameid) const = 0;
    virtual void SetMonstrosityName(uint32_t index, const char* name) const = 0;
};

interface IInventory
{
    virtual Ashita::FFXI::item_t* GetItem(int32_t containerId, int32_t slotId) const = 0;
    virtual uint16_t GetContainerMax(int32_t containerId) const = 0;
    virtual Ashita::FFXI::treasureitem_t* GetTreasureItem(int32_t slotId) const = 0;
    virtual Ashita::FFXI::equipment_t* GetEquippedItem(int32_t equipSlotId) const = 0;
    virtual uint32_t GetCraftWait(void) const = 0;
};

interface IParty
{
    // Alliance Property Getters
    virtual uint32_t GetAllianceLeaderServerId(void) const = 0;
    virtual uint32_t GetAllianceParty0LeaderServerId(void) const = 0;
    virtual uint32_t GetAllianceParty1LeaderServerId(void) const = 0;
    virtual uint32_t GetAllianceParty2LeaderServerId(void) const = 0;
    virtual int8_t GetAllianceParty0Visible(void) const = 0;
    virtual int8_t GetAllianceParty1Visible(void) const = 0;
    virtual int8_t GetAllianceParty2Visible(void) const = 0;
    virtual int8_t GetAllianceParty0MemberCount(void) const = 0;
    virtual int8_t GetAllianceParty1MemberCount(void) const = 0;
    virtual int8_t GetAllianceParty2MemberCount(void) const = 0;
    virtual int8_t GetAllianceInvited(void) const = 0;

    // Party Member Property Getters
    virtual uint8_t GetMemberIndex(uint32_t index) const = 0;
    virtual uint8_t GetMemberNumber(uint32_t index) const = 0;
    virtual const char* GetMemberName(uint32_t index) const = 0;
    virtual uint32_t GetMemberServerId(uint32_t index) const = 0;
    virtual uint32_t GetMemberTargetIndex(uint32_t index) const = 0;
    virtual uint32_t GetMemberCurrentHP(uint32_t index) const = 0;
    virtual uint32_t GetMemberCurrentMP(uint32_t index) const = 0;
    virtual uint32_t GetMemberCurrentTP(uint32_t index) const = 0;
    virtual uint8_t GetMemberCurrentHPP(uint32_t index) const = 0;
    virtual uint8_t GetMemberCurrentMPP(uint32_t index) const = 0;
    virtual uint16_t GetMemberZone(uint32_t index) const = 0;
    virtual uint32_t GetMemberFlagMask(uint32_t index) const = 0;
    virtual uint8_t GetMemberMainJob(uint32_t index) const = 0;
    virtual uint8_t GetMemberMainJobLevel(uint32_t index) const = 0;
    virtual uint8_t GetMemberSubJob(uint32_t index) const = 0;
    virtual uint8_t GetMemberSubJobLvl(uint32_t index) const = 0;
    virtual uint32_t GetMemberServerId2(uint32_t index) const = 0;
    virtual uint8_t GetMemberCurrentHPP2(uint32_t index) const = 0;
    virtual uint8_t GetMemberCurrentMPP2(uint32_t index) const = 0;
    virtual uint8_t GetMemberActive(uint32_t index) const = 0;
};

interface IPlayer
{
    // Property Getters
    virtual uint32_t GetHealthMax(void) const = 0;
    virtual uint32_t GetManaMax(void) const = 0;
    virtual uint8_t GetMainJob(void) const = 0;
    virtual uint8_t GetMainJobLevel(void) const = 0;
    virtual uint8_t GetSubJob(void) const = 0;
    virtual uint8_t GetSubJobLevel(void) const = 0;
    virtual uint16_t GetExpCurrent(void) const = 0;
    virtual uint16_t GetExpNeeded(void) const = 0;
    virtual int16_t GetStat(uint32_t stat) const = 0;
    virtual int16_t GetStatsModifiers(uint32_t stat) const = 0;
    virtual int16_t GetAttack(void) const = 0;
    virtual int16_t GetDefense(void) const = 0;
    virtual int16_t GetResist(uint32_t stat) const = 0;
    virtual uint16_t GetTitle(void) const = 0;
    virtual uint16_t GetRank(void) const = 0;
    virtual uint16_t GetRankPoints(void) const = 0;
    virtual uint16_t GetHomepoint(void) const = 0;
    virtual uint8_t GetNation(void) const = 0;
    virtual uint8_t GetResidence(void) const = 0;
    virtual uint16_t GetSuLevel(void) const = 0;
    virtual uint8_t GetHighestItemLevel(void) const = 0;
    virtual uint8_t GetItemLevel(void) const = 0;
    virtual uint8_t GetMainHandItemLevel(void) const = 0;
    virtual uint32_t GetUnityFaction(void) const = 0;
    virtual uint32_t GetUnityPoints(void) const = 0;
    virtual uint16_t GetUnityPartialPersonalEvalutionPoints(void) const = 0;
    virtual uint16_t GetUnityPersonalEvaluationPoints(void) const = 0;
    virtual uint32_t GetUnityChatColorFlag(void) const = 0;
    virtual uint8_t GetMasteryJob(void) const = 0;
    virtual uint8_t GetMasteryJobLevel(void) const = 0;
    virtual uint8_t GetMasteryFlags(void) const = 0;
    virtual uint8_t GetMasteryUnknown0000(void) const = 0;
    virtual uint32_t GetMasteryExp(void) const = 0;
    virtual uint32_t GetMasteryExpNeeded(void) const = 0;
    virtual Ashita::FFXI::combatskill_t GetCombatSkill(uint32_t skill) const = 0;
    virtual Ashita::FFXI::craftskill_t GetCraftSkill(uint32_t skill) const = 0;
    virtual uint16_t GetAbilityRecast(uint32_t index) const = 0;
    virtual uint16_t GetAbilityRecastTimerId(uint32_t index) const = 0;
    virtual uint32_t GetMountRecast(void) const = 0;
    virtual uint32_t GetMountRecastTimerId(void) const = 0;
    virtual uint8_t GetDataLoadedFlags(void) const = 0;
    virtual uint16_t GetLimitPoints(void) const = 0;
    virtual uint8_t GetMeritPoints(void) const = 0;
    virtual uint8_t GetLimitMode(void) const = 0;
    virtual uint32_t GetMeritPointsMax(void) const = 0;
    virtual uint8_t* GetHomepointMasks(void) const = 0;
    virtual uint32_t GetIsZoning(void) const = 0;

    // Get Properties (Job Points)
    virtual uint16_t GetCapacityPoints(uint32_t jobid) const = 0;
    virtual uint16_t GetJobPoints(uint32_t jobid) const = 0;
    virtual uint16_t GetJobPointsSpent(uint32_t jobid) const = 0;

    // Get Properties (Status Icons / Buffs)
    virtual int16_t* GetStatusIcons(void) const = 0;
    virtual int32_t* GetStatusTimers(void) const = 0;
    virtual int16_t* GetBuffs(void) const = 0;

    // Helper Functions
    virtual bool HasAbility(uint32_t id) const = 0;
    virtual bool HasKeyItem(uint32_t id) const = 0;
    virtual bool HasPetCommand(uint32_t id) const = 0;
    virtual bool HasSpell(uint32_t id) const = 0;
    virtual bool HasTrait(uint32_t id) const = 0;
    virtual bool HasWeaponSkill(uint32_t id) const = 0;

    // Pet Functions
    virtual uint32_t GetPetTP(void) const = 0;
    virtual uint32_t GetPetMP(void) const = 0;
};

interface ITarget
{
    // Property Getters
    virtual const char* GetTargetName(void) const = 0;
    virtual uint32_t GetTargetHealthPercent(void) const = 0;
    virtual uint32_t GetTargetIndex(void) const = 0;
    virtual uint32_t GetTargetServerId(void) const = 0;
    virtual uintptr_t GetTargetEntityPointer(void) const = 0;
    virtual uintptr_t GetTargetWarpPointer(void) const = 0;
    virtual uint8_t GetTargetVisible(void) const = 0;
    virtual uint16_t GetTargetMask(void) const = 0;
    virtual uint16_t GetTargetCalculatedId(void) const = 0;
    virtual uint32_t GetSubTargetIndex(void) const = 0;
    virtual uint32_t GetSubTargetServerId(void) const = 0;
    virtual uintptr_t GetSubTargetEntityPointer(void) const = 0;
    virtual uintptr_t GetSubTargetWarpPointer(void) const = 0;
    virtual uint8_t GetSubTargetVisible(void) const = 0;
    virtual uint16_t GetSubTargetMask(void) const = 0;
    virtual uint8_t GetSubTargetActive(void) const = 0;
    virtual uint8_t GetTargetDeactivate(void) const = 0;
    virtual uint8_t GetIsLockedOn(void) const = 0;
    virtual uint32_t GetTargetSelectionMask(void) const = 0;
    virtual uint8_t GetIsMenuOpen(void) const = 0;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Input Related Interfaces
//
// IKeyboard
//
//      Exposes functions to interact with the hooked and wrapped keyboard device.
//
// IMouse
//
//      Exposes functions to interact with the hooked and wrapped mouse device.
//
///////////////////////////////////////////////////////////////////////////////////////////////////

interface IKeyboard : newIDirectInputDevice8A
{
    // Keybind Related Functions
    virtual void BindKey(uint32_t key, bool down, bool alt, bool ctrl, bool win, bool apps, bool shift, const char* command) = 0;
    virtual void UnbindKey(uint32_t key, bool down, bool alt, bool ctrl, bool win, bool apps, bool shift) = 0;
    virtual void UnbindAll(void) = 0;
    virtual bool IsKeyBound(uint32_t key, bool alt, bool ctrl, bool win, bool apps, bool shift) = 0;
    virtual void ListBinds(void) = 0;

    // Callback Related Functions
    virtual void AddCallback(const char* alias, LPVOID lpGetDataCallback, LPVOID lpGetStateCallback, LPVOID lpKeyboardCallack, LPVOID lpMouseCallback) = 0;
    virtual void RemoveCallback(const char* alias) = 0;

    // Key Related Helper Functions
    virtual uint32_t V2D(uint32_t key) const = 0;
    virtual uint32_t D2V(uint32_t key) const = 0;
    virtual uint32_t S2D(const char* key) const = 0;
    virtual const char* D2S(uint32_t key) const = 0;

    virtual HWND GetParentWindow(void) const = 0;
    virtual bool GetBlocked(void) const = 0;
    virtual void SetBlocked(bool blocked) = 0;
};

interface IMouse : newIDirectInputDevice8A
{
    virtual HWND GetParentWindow(void) const = 0;
    virtual bool GetBlocked(void) const = 0;
    virtual void SetBlocked(bool blocked) = 0;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Ashita Core
//
// The main Ashita interface passed to plugins when they are loaded that includes the ability
// to obtain and interact with the other various objects defined in this header.
//
///////////////////////////////////////////////////////////////////////////////////////////////////

interface IAshitaCore
{
    // Manager Exposure Functions
    virtual IChatManager* GetChatManager(void) const = 0;
    virtual IConfigurationManager* GetConfigurationManager(void) const = 0;
    virtual IDataManager* GetDataManager(void) const = 0;
    virtual IFontManager* GetFontManager(void) const = 0;
    virtual IGuiManager* GetGuiManager(void) const = 0;
    virtual IInputManager* GetInputManager(void) const = 0;
    virtual IPacketManager* GetPacketManager(void) const = 0;
    virtual IPluginManager* GetPluginManager(void) const = 0;
    virtual IPointerManager* GetPointerManager(void) const = 0;
    virtual IResourceManager* GetResourceManager(void) const = 0;

    // Ashita Property Functions
    virtual HMODULE GetHandle(void) const = 0;
    virtual const char* GetAshitaInstallPathA(void) const = 0;
    virtual const wchar_t* GetAshitaInstallPathW(void) const = 0;
    virtual Ashita::aswindowinfo_t* GetPlayOnlineWindowInfo(void) const = 0;
    virtual Ashita::aswindowinfo_t* GetMaskWindowInfo(void) const = 0;
    virtual Ashita::aswindowinfo_t* GetFFXiWindowInfo(void) const = 0;
    virtual bool GetMouseUnhooked(void) const = 0;
    virtual void SetMouseUnhooked(bool hooked) = 0;

    // Direct3D Property Functions
    virtual uint32_t GetD3DFillMode(void) const = 0;
    virtual bool GetD3DAmbientEnabled(void) const = 0;
    virtual D3DCOLOR GetD3DAmbientColor(void) const = 0;
    virtual void SetD3DFillMode(uint32_t fillmode) = 0;
    virtual void SetD3DAmbientEnabled(bool enabled) = 0;
    virtual void SetD3DAmbientColor(D3DCOLOR color) = 0;

    // DirectInput Property Functions
    virtual bool GetAllowGamepadInBackground(void) const = 0;
    virtual void SetAllowGamepadInBackground(bool enabled) = 0;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Plugin Information
// 
// Plugin information that is shared back to the Ashita plugin manager while a plugin is being
// loaded. Plugins must populate this data in order to be considered valid.
//
///////////////////////////////////////////////////////////////////////////////////////////////////

struct plugininfo_t
{
    char        Name[512];          // The plugins name.
    char        Author[512];        // The plugins author.
    double      InterfaceVersion;   // The plugins ADK interface version that it was compiled with.
    double      PluginVersion;      // The plugins version.
    int32_t     Priority;           // The plugins execution priority.

    /**
     * Default Constructor
     */
    plugininfo_t(void)
    {
        strcpy_s(this->Name, 512, "Unknown");
        strcpy_s(this->Author, 512, "Ashita Development Team");

        this->InterfaceVersion = ASHITA_INTERFACE_VERSION;
        this->PluginVersion = 1.0f;
        this->Priority = 0;
    }

    /**
     * Default Constructor
     */
    plugininfo_t(const char* name, const char* author, double interfaceVersion, double pluginVersion, int32_t priority)
    {
        strcpy_s(this->Name, 512, name);
        strcpy_s(this->Author, 512, author);

        this->InterfaceVersion = interfaceVersion;
        this->PluginVersion = pluginVersion;
        this->Priority = priority;
    }
};

///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Plugin Base
//
// The main plugin interface that exposes the proper implementation of a plugins base class.
//
///////////////////////////////////////////////////////////////////////////////////////////////////

interface IPluginBase
{
    /**
     * Returns the plugins information structure.
     *
     * @returns {plugininfo_t} The plugin information structure of the plugin.
     */
    virtual plugininfo_t GetPluginInfo(void) = 0;

    /**
     * Invoked when the plugin is loaded, allowing it to prepare for usage.
     *
     * @param {IAshitaCore*} core - The Ashita core object to interact with the various Ashita managers.
     * @param {ILogManager*} log - The log manager used to interact with the current log file.
     * @param {uint32_t} id - The plugins id, or its module base, that identifies it other than its name.
     * @returns {bool} True on success, false otherwise. (If false, the plugin will not be loaded.)
     */
    virtual bool Initialize(IAshitaCore* core, ILogManager* log, uint32_t id) = 0;

    /**
     * Invoked when the plugin is being unloaded, allowing it to cleanup its resources.
     */
    virtual void Release(void) = 0;

    ///////////////////////////////////////////////////////////////////////////////////////////////////
    // Chat Manager Callbacks
    ///////////////////////////////////////////////////////////////////////////////////////////////////

    /**
     * Invoked when a command is being processed by the game client.
     *
     * Note:
     *      Please note, this handles all things done via the game in terms of commands
     *      and chat. All / commands as well as normal chat you type, macros, etc. will
     *      be processed through here. You should only use this to handle / commands.
     *
     *      If you wish to handle other bits of outgoing text before the client sees it,
     *      then use the HandleOutgoingText callback instead.
     *
     * @param {const char*} command         - The raw command string being processed.
     * @param {uint32_t} type               - The type of the command being processed. (See Ashita::CommandInputType enumeration.)
     * @returns {bool} True if handled and should be blocked, false otherwise.
     */
    virtual bool HandleCommand(const char* command, int32_t type) = 0;

    /**
     * Invoked when incoming text being sent to the chat log is being processed.
     *
     * @param {int16_t} mode                - The mode of the message being added to the chatlog.
     * @param {const char*} message         - The raw message being added to the chat log.
     * @param {int16_t*} modifiedMode       - The modified mode, if any, that has been altered by other plugins/addons.
     * @param {char*} modifiedMessage       - The modified message, if any, that has been altered by other plugins/addons.
     * @param {bool} blocked                - Flag if this message has been blocked already by another plugin. (Once blocked, other plugins cannot restore it.)
     * @returns {bool} True if handled and should be blocked, false otherwise.
     */
    virtual bool HandleIncomingText(int16_t mode, const char* message, int16_t* modifiedMode, char* modifiedMessage, bool blocked) = 0;

    /**
     * Invoked when outgoing text has not been handled by other plugins/addons.
     * Invoked after HandleCommand if nothing else processed the data.
     *
     * @param {int16_t} mode                - The type of the text that is being sent. (See Ashita::CommandInputType enumeration.)
     * @param {const char*} message         - The raw message being sent.
     * @param {int16_t*} modifiedMode       - The modified mode, if any, that has been altered by other plugins/addons.
     * @param {char*} modifiedMessage       - The modified message, if any, that has been altered by other plugins/addons.
     * @param {bool} blocked                - Flag if this message has been blocked already by another plugin. (Once blocked, other plugins cannot restore it.)
     * @returns {bool} True if handled and should be blocked, false otherwise.
     */
    virtual bool HandleOutgoingText(int32_t type, const char* message, int32_t* modifiedType, char* modifiedMessage, bool blocked) = 0;

    ///////////////////////////////////////////////////////////////////////////////////////////////////
    // Packet Manager Callbacks
    ///////////////////////////////////////////////////////////////////////////////////////////////////

    /**
     * Invoked when an incoming packet is being handled.
     *
     * @param {uint16_t} id                 - The id of the packet.
     * @param {uint32_t} size               - The size of the packet data.
     * @param {void*} data                  - The raw data of the packet.
     * @param {void*} modified              - The modified data, if any, that has been altered by other plugins/addons.
     * @param {bool} blocked                - Flag if this message has been blocked already by another plugin. (Once blocked, other plugins cannot restore it.)
     * @returns {bool} True if handled and should be blocked, false otherwise.
     */
    virtual bool HandleIncomingPacket(uint16_t id, uint32_t size, void* data, void* modified, bool blocked) = 0;

    /**
     * Invoked when an outgoing packet is being handled.
     *
     * @param {uint16_t} id                 - The id of the packet.
     * @param {uint32_t} size               - The size of the packet data.
     * @param {void*} data                  - The raw data of the packet.
     * @param {void*} modified              - The modified data, if any, that has been altered by other plugins/addons.
     * @param {bool} blocked                - Flag if this message has been blocked already by another plugin. (Once blocked, other plugins cannot restore it.)
     * @returns {bool} True if handled and should be blocked, false otherwise.
     */
    virtual bool HandleOutgoingPacket(uint16_t id, uint32_t size, void* data, void* modified, bool blocked) = 0;

    ///////////////////////////////////////////////////////////////////////////////////////////////////
    // Direct3D Callbacks
    ///////////////////////////////////////////////////////////////////////////////////////////////////

    /**
     * Invoked when the plugin is being initialized for Direct3D rendering.
     *
     * Note:
     *      Plugins must return true with this function in order to have the other Direct3D
     *      functions invoked. Returning false is ideal here if you do not need to use the
     *      Direct3D functions within your plugin. This can help with overall performance.
     *
     * @param {IDirect3DDevice8*} device    - The Direct3D device pointer currently being used by the game.
     * @return {bool} True if the plugin should handle the other Direct3D messages, false otherwise.
     */
    virtual bool Direct3DInitialize(IDirect3DDevice8* device) = 0;

    /**
     * Invoked when the plugin is being unloaded and is able to cleanup its Direct3D related resources.
     */
    virtual void Direct3DRelease(void) = 0;

    /**
     * Invoked when the Direct3D device is beginning to render. (BeginScene)
     */
    virtual void Direct3DPreRender(void) = 0;

    /**
     * Invoked when the Direct3D device is ending its rendering. (EndScene)
     */
    virtual void Direct3DRender(void) = 0;

    /**
     * Invoked when the Direct3D device is presenting the scene. (Present)
     *
     * @param {RECT*} pSourceRect               - The source rect being rendered into.
     * @param {RECT*} pDestRect                 - The destination rect being rendered from.
     * @param {HWND} hDestWindowOverride        - The window handle, if any, to override the rendering into.
     * @param {RGNDATA*} pDirtyRegion           - The dirty region data.
     * @returns {bool} True if the call should be blocked, false otherwise.
     */
    virtual bool Direct3DPresent(const RECT* pSourceRect, const RECT* pDestRect, HWND hDestWindowOverride, const RGNDATA* pDirtyRegion) = 0;

    /**
     * Invoked when the Direct3D device is drawing a primitive to the scene. (DrawPrimitive)
     *
     * @param {D3DPRIMITIVETYPE} PrimitiveType  - The type of primitive being rendered.
     * @param {UINT} StartVertex                - Index of the first vertex to load.
     * @param {UINT} PrimitiveCount             - Number of primitives to render.
     * @returns {bool} True if the call should be blocked, false otherwise.
     */
    virtual bool Direct3DDrawPrimitive(D3DPRIMITIVETYPE PrimitiveType, UINT StartVertex, UINT PrimitiveCount) = 0;

    /**
     * Invoked when the Direct3D device is drawing a primitive to the scene. (DrawIndexedPrimitive)
     *
     * @param {D3DPRIMITIVETYPE} PrimitiveType  - The type of primitive being rendered.
     * @param {UINT} minIndex                   - Minimum vertex index for vertices used during this call.
     * @param {UINT} numVertices                - Number of vertices used during this call
     * @param {UINT} startIndex                 - Index of the first index to use when accesssing the vertex buffer.
     * @param {UINT} primCount                  - Number of primitives to render.
     * @returns {bool} True if the call should be blocked, false otherwise.
     */
    virtual bool Direct3DDrawIndexedPrimitive(D3DPRIMITIVETYPE PrimitiveType, UINT minIndex, UINT NumVertices, UINT startIndex, UINT primCount) = 0;

    /**
     * Invoked when the Direct3D device is drawing a primitive to the scene. (DrawPrimitiveUP)
     *
     * @param {D3DPRIMITIVETYPE} PrimitiveType  - The type of primitive being rendered.
     * @param {UINT} PrimitiveCount             - Number of primitives to render.
     * @param {void*} pVertexStreamZeroData     - User memory pointer to the vertex data.
     * @param {UINT} VertexStreamZeroStride     - The number of bytes of data for each vertex.
     * @returns {bool} True if the call should be blocked, false otherwise.
     */
    virtual bool Direct3DDrawPrimitiveUP(D3DPRIMITIVETYPE PrimitiveType, UINT PrimitiveCount, CONST void* pVertexStreamZeroData, UINT VertexStreamZeroStride) = 0;

    /**
     * Invoked when the Direct3D device is drawing a primitive to the scene. (DrawIndexedPrimitiveUP)
     *
     * @param {D3DPRIMITIVETYPE} PrimitiveType  - The type of primitive being rendered.
     * @param {UINT} MinVertexIndex             - Minimum vertex index.
     * @param {UINT} NumVertexIndices           - Number of vertices used during this call.
     * @param {UINT} PrimitiveCount             - Number of primitives to render.
     * @param {void*} pIndexData                - User memory pointer to the index data.
     * @param {D3DFORMAT} IndexDataFormat       - The format of the index data.
     * @param {void*} pVertexStreamZeroData     - User memory pointer to the vertex data.
     * @param {UINT} VertexStreamZeroStride     - The number of bytes of data for each vertex.
     * @returns {bool} True if the call should be blocked, false otherwise.
     */
    virtual bool Direct3DDrawIndexedPrimitiveUP(D3DPRIMITIVETYPE PrimitiveType, UINT MinVertexIndex, UINT NumVertexIndices, UINT PrimitiveCount, CONST void* pIndexData, D3DFORMAT IndexDataFormat, CONST void* pVertexStreamZeroData, UINT VertexStreamZeroStride) = 0;

    /**
     * Invoked when the Direct3D device is setting a render state. (SetRenderState)
     *
     * @param {D3DRENDERSTATETYPE} state        - The render state to alter.
     * @param {DWORD} value                     - The new value for the render state.
     * @returns {bool} True if the call should be blocked, false otherwise.
     */
    virtual bool Direct3DSetRenderState(D3DRENDERSTATETYPE State, DWORD Value) = 0;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Plugin
//
// The main base class that plugins should inherit from. Defaults the function handlers that are
// not overriden to ensure that the plugin does not interfere with calls that it does not use.
//
// Note:
//      Plugins should inherit from this base class to ensure they meet the standards of being
//      a valid Ashita plugin. Failure to export a class that inherits from this base can lead
//      to crashes and potential loss of data / settings.
//
///////////////////////////////////////////////////////////////////////////////////////////////////

class IPlugin : public IPluginBase
{
protected:
    IAshitaCore*        m_AshitaCore;
    ILogManager*        m_LogManager;
    uint32_t            m_PluginId;
    IDirect3DDevice8*   m_Direct3DDevice;

public:
    IPlugin(void) { }
    virtual ~IPlugin(void) { }

public:
    plugininfo_t GetPluginInfo(void) override
    {
        return plugininfo_t("IPlugin", "Ashita Development Team", ASHITA_INTERFACE_VERSION, 1.0f, 0);
    }

public:
    bool Initialize(IAshitaCore* core, ILogManager* log, uint32_t id) override
    {
        this->m_AshitaCore = core;
        this->m_LogManager = log;
        this->m_Direct3DDevice = nullptr;
        this->m_PluginId = id;
        return false;
    }
    void Release(void) override
    { }
    bool HandleCommand(const char* command, int32_t type) override
    {
        UNREFERENCED_PARAMETER(command);
        UNREFERENCED_PARAMETER(type);
        return false;
    }
    bool HandleIncomingText(int16_t mode, const char* message, int16_t* modifiedMode, char* modifiedMessage, bool blocked) override
    {
        UNREFERENCED_PARAMETER(mode);
        UNREFERENCED_PARAMETER(message);
        UNREFERENCED_PARAMETER(modifiedMode);
        UNREFERENCED_PARAMETER(modifiedMessage);
        UNREFERENCED_PARAMETER(blocked);
        return false;
    }
    bool HandleOutgoingText(int32_t type, const char* message, int32_t* modifiedType, char* modifiedMessage, bool blocked) override
    {
        UNREFERENCED_PARAMETER(type);
        UNREFERENCED_PARAMETER(message);
        UNREFERENCED_PARAMETER(modifiedType);
        UNREFERENCED_PARAMETER(modifiedMessage);
        UNREFERENCED_PARAMETER(blocked);
        return false;
    }
    bool HandleIncomingPacket(uint16_t id, uint32_t size, void* data, void* modified, bool blocked) override
    {
        UNREFERENCED_PARAMETER(id);
        UNREFERENCED_PARAMETER(size);
        UNREFERENCED_PARAMETER(data);
        UNREFERENCED_PARAMETER(modified);
        UNREFERENCED_PARAMETER(blocked);
        return false;
    }
    bool HandleOutgoingPacket(uint16_t id, uint32_t size, void* data, void* modified, bool blocked) override
    {
        UNREFERENCED_PARAMETER(id);
        UNREFERENCED_PARAMETER(size);
        UNREFERENCED_PARAMETER(data);
        UNREFERENCED_PARAMETER(modified);
        UNREFERENCED_PARAMETER(blocked);
        return false;
    }
    bool Direct3DInitialize(IDirect3DDevice8* device) override
    {
        this->m_Direct3DDevice = device;
        return false;
    }
    void Direct3DRelease(void) override
    { }
    void Direct3DPreRender(void) override
    { }
    void Direct3DRender(void) override
    { }
    bool Direct3DPresent(const RECT* pSourceRect, const RECT* pDestRect, HWND hDestWindowOverride, const RGNDATA* pDirtyRegion) override
    {
        UNREFERENCED_PARAMETER(pSourceRect);
        UNREFERENCED_PARAMETER(pDestRect);
        UNREFERENCED_PARAMETER(hDestWindowOverride);
        UNREFERENCED_PARAMETER(pDirtyRegion);
        return false;
    }
    bool Direct3DDrawPrimitive(D3DPRIMITIVETYPE PrimitiveType, UINT StartVertex, UINT PrimitiveCount) override
    {
        UNREFERENCED_PARAMETER(PrimitiveType);
        UNREFERENCED_PARAMETER(StartVertex);
        UNREFERENCED_PARAMETER(PrimitiveCount);
        return false;
    }
    bool Direct3DDrawIndexedPrimitive(D3DPRIMITIVETYPE PrimitiveType, UINT minIndex, UINT NumVertices, UINT startIndex, UINT primCount) override
    {
        UNREFERENCED_PARAMETER(PrimitiveType);
        UNREFERENCED_PARAMETER(minIndex);
        UNREFERENCED_PARAMETER(NumVertices);
        UNREFERENCED_PARAMETER(startIndex);
        UNREFERENCED_PARAMETER(primCount);
        return false;
    }
    bool Direct3DDrawPrimitiveUP(D3DPRIMITIVETYPE PrimitiveType, UINT PrimitiveCount, CONST void* pVertexStreamZeroData, UINT VertexStreamZeroStride) override
    {
        UNREFERENCED_PARAMETER(PrimitiveType);
        UNREFERENCED_PARAMETER(PrimitiveCount);
        UNREFERENCED_PARAMETER(pVertexStreamZeroData);
        UNREFERENCED_PARAMETER(VertexStreamZeroStride);
        return false;
    }
    bool Direct3DDrawIndexedPrimitiveUP(D3DPRIMITIVETYPE PrimitiveType, UINT MinVertexIndex, UINT NumVertexIndices, UINT PrimitiveCount, CONST void* pIndexData, D3DFORMAT IndexDataFormat, CONST void* pVertexStreamZeroData, UINT VertexStreamZeroStride) override
    {
        UNREFERENCED_PARAMETER(PrimitiveType);
        UNREFERENCED_PARAMETER(MinVertexIndex);
        UNREFERENCED_PARAMETER(NumVertexIndices);
        UNREFERENCED_PARAMETER(PrimitiveCount);
        UNREFERENCED_PARAMETER(pIndexData);
        UNREFERENCED_PARAMETER(IndexDataFormat);
        UNREFERENCED_PARAMETER(pVertexStreamZeroData);
        UNREFERENCED_PARAMETER(VertexStreamZeroStride);
        return false;
    }
    bool Direct3DSetRenderState(D3DRENDERSTATETYPE State, DWORD Value) override
    {
        UNREFERENCED_PARAMETER(State);
        UNREFERENCED_PARAMETER(Value);
        return false;
    }
};

///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Ashita Plugin Exports
//
//      These functions are required to be exported by all plugins in order to be validated
//      for loading. Failure to export these functions will result in your plugin not being
//      loaded. Plugins must return proper data for the plugin information and plugin creation
//      otherwise it can lead to crashes!
//
//  getinterfaceversion_f
//          Exported function to return the plugins interface version it was compiled with.
//
//  createplugininfo_f
//          Exported function to return information about the given plugin back to Ashita.
//          This info is used to validate the plugin as well as used when the plugin list is printed.
//
//  createplugin_f
//          Exported function to return a new instance of the plugins base class. This class must
//          inherit from IPlugin as the returned value will be the inherited base object.
//
///////////////////////////////////////////////////////////////////////////////////////////////////

typedef double      /**/(__stdcall *getinterfaceversion_f)(void);
typedef void        /**/(__stdcall *createplugininfo_f)(plugininfo_t* info);
typedef IPlugin*    /**/(__stdcall *createplugin_f)(void);

#endif // __ASHITA_ADK_H_INCLUDED__