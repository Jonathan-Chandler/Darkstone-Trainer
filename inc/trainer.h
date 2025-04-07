#ifndef __TRAINER_H__
#define __TRAINER_H__
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <stdint.h>
#include "debug.h"
#include "process_info.h"

class DarkstoneTrainer
{
  private:
    enum GameVersion
    {
      GAME_VERSION_GOG,
      GAME_VERSION_STEAM,
      GAME_VERSION_COUNT
    };

    GameVersion mDarkstoneVersion;
    ProcessInfo mDarkstoneProcess;

    int mCurrentPlayer;
    LPVOID mEquipBaseAddress;
    LPVOID mInventoryBaseAddress;
    LPVOID mInventoryMapBaseAddress;
    LPVOID mCharBaseAddress;
    LPVOID mSpellBaseAddress;
    LPVOID mCursorItemBaseAddress;

  public:
    DarkstoneTrainer();

    void showMenu();
    void showPlayerSelectMenu();
    void showCharacterStatsMenu();
    void showCharacterSpellsMenu();
    void showCharacterSkillsMenu();
    void showWeaponStatsMenu();
    void showArmorStatsMenu(UINT_PTR armor_offset);

    int    getCurrentPlayer();
    void   setCurrentPlayer(int player);
    LPVOID getPlayerBaseAddr(LPVOID pBaseAddr);

    int setValue(LPVOID pBaseAddr, LPVOID pAddrOffset, int16_t value);
    int setMemoryBlock(LPVOID pBaseAddr, LPVOID pAddrOffset, char *pValue, SIZE_T nBytes);
};

namespace DarkstoneOffsets
{
  static const LPVOID PLAYER_2_OFFSET = (LPVOID)0x7554;

  enum
  {
    CHAR_STAT_LEVEL,
    CHAR_STAT_STRENGTH,
    CHAR_STAT_MAGIC,
    CHAR_STAT_DEXTERITY,
    CHAR_STAT_VITALITY,
    CHAR_STAT_REMAINING_POINTS,
    CHAR_STAT_AGE,
    CHAR_STAT_COUNT
  };
  static const LPVOID pCharacterStatOffset[] = 
  {
    (LPVOID)0x58,
    (LPVOID)0x0,
    (LPVOID)0x2,
    (LPVOID)0x4,
    (LPVOID)0x6,
    (LPVOID)0x5e,
    (LPVOID)0x74,
  };

  enum 
  {
    SPELL_CONFUSION,
    SPELL_HEALING,
    SPELL_FEAR,
    SPELL_THUNDER,
    SPELL_SLOWNESS,
    SPELL_FIREWALL,
    SPELL_MUTATION,
    SPELL_STONE,
    SPELL_NIGHT_VISION,
    SPELL_HASTE,
    SPELL_MAGIC_MISSILE,
    SPELL_FIREBALL,
    SPELL_ABSORPTION,
    SPELL_ANTIDOTE,
    SPELL_FLAME_THROWER,
    SPELL_STORM,
    SPELL_INVIS,
    SPELL_DEATH_DOME,
    SPELL_INVOCATION,
    SPELL_SPARK,
    SPELL_TELEPORT,
    SPELL_DETECT,
    SPELL_FOOD,
    SPELL_BERSERKER,
    SPELL_FORGETFULNESS,
    SPELL_REFLECTIONS,
    SPELL_MAGIC_BOMB,
    SPELL_MAGIC_DOOR,
    SPELL_POISON_CLOUD,
    SPELL_RESURRECTION,
    SPELL_TELEKINESIS2,
    SPELL_LIGHT,
    SPELL_COUNT,
    SPELL_REMAINING_TIMER,
    SPELL_ENUM_END,
  };
  static const LPVOID pCharacterSpellOffset[] = 
  {
    (LPVOID)0x000,   //  1: confusion
    (LPVOID)0x010,   //  2: healing
    (LPVOID)0x020,   //  3: fear
    (LPVOID)0x030,   //  4: thunder
    (LPVOID)0x040,   //  5: slowness
    (LPVOID)0x050,   //  6: firewall
    (LPVOID)0x060,   //  7: mutation
    (LPVOID)0x070,   //  8: stone
    (LPVOID)0x080,   //  9: night vision
    (LPVOID)0x090,   // 10: haste
    (LPVOID)0x0A0,   // 11: magic missile
    (LPVOID)0x0B0,   // 12: fireball
    (LPVOID)0x0C0,   // 13: absorption
    (LPVOID)0x0D0,   // 14: antidote
    (LPVOID)0x0E0,   // 15: flame thrower
    (LPVOID)0x0F0,   // 16: storm
    (LPVOID)0x100,   // 17: invis
    (LPVOID)0x110,   // 18: death dome
    (LPVOID)0x120,   // 19: invocation
    (LPVOID)0x130,   // 20: spark
    (LPVOID)0x140,   // 21: teleport
    (LPVOID)0x150,   // 22: detect
    (LPVOID)0x160,   // 23: food
    (LPVOID)0x170,   // 24: berserker
    (LPVOID)0x180,   // 25: forgetfulness
    (LPVOID)0x190,   // 26: reflections
    (LPVOID)0x1A0,   // 27: magic bomb
    (LPVOID)0x1B0,   // 28: magic door
    (LPVOID)0x1C0,   // 29: poison cloud
    (LPVOID)0x1D0,   // 30: resurrection
    (LPVOID)0x1E0,   // 31: telekinesis
    (LPVOID)0x1F0,   // 32: light
    (LPVOID)0x000,   // not used
    (LPVOID)0x00C,   // time remaining for spell
  };

  enum 
  {
    SKILL_IDENTIFICATION,   //  0: Identification
    SKILL_TRADE,            //  1: Trade
    SKILL_REPAIR,           //  2: Repair
    SKILL_PERCEPTION,       //  3: Perception
    SKILL_DEFUSING,         //  4: Defusing
    SKILL_FORESTER,         //  5: Forester
    SKILL_LEARNING,         //  6: Learning
    SKILL_THEFT,            //  7: Theft
    SKILL_SILENCE,          //  8: Silence
    SKILL_MEDITATION,       //  9: Meditation
    SKILL_CONCENTRATION,    // 10: Concentration
    SKILL_ORIENTATION,      // 11: Orientation
    SKILL_MEDICINE,         // 12: Medicine
    SKILL_RECHARGING,       // 13: Recharging
    SKILL_EXORCISM,         // 14: Exorcism
    SKILL_PRAYER,           // 15: Prayer
    SKILL_DETECTION,        // 16: Detection
    SKILL_MASTER,           // 17: Master Of Arms
    SKILL_COMMUNION_MONK,   // 18: Communion (Monk)
    SKILL_COMMUNION_WIZARD, // 19: Communion (Wizard)
    SKILL_LANGUAGE,         // 20: Language
    SKILL_LYCANTHROPY,      // 21: Lycanthropy
    SKILL_COUNT,
    SKILL_TIME_REMAINING,
    SKILL_ENUM_END,
  };
  static const LPVOID pCharacterSkillOffset[] = 
  {
    (LPVOID)0x200,   //  0: identification
    (LPVOID)0x210,   //  1: Trade
    (LPVOID)0x220,   //  2: Repair
    (LPVOID)0x230,   //  3: Perception
    (LPVOID)0x240,   //  4: Defusing
    (LPVOID)0x250,   //  5: Forester
    (LPVOID)0x260,   //  6: Learning
    (LPVOID)0x270,   //  7: Theft
    (LPVOID)0x280,   //  8: Silence
    (LPVOID)0x290,   //  9: Meditation
    (LPVOID)0x2A0,   // 10: Concentration
    (LPVOID)0x2B0,   // 11: Orientation
    (LPVOID)0x2C0,   // 12: Medicine
    (LPVOID)0x2D0,   // 13: Recharging
    (LPVOID)0x2E0,   // 14: Exorcism
    (LPVOID)0x2F0,   // 15: Prayer
    (LPVOID)0x300,   // 16: Detection
    (LPVOID)0x310,   // 17: Master Of Arms
    (LPVOID)0x320,   // 18: Communion (Monk)
    (LPVOID)0x330,   // 19: Communion (Wizard)
    (LPVOID)0x340,   // 20: Language
    (LPVOID)0x350,   // 21: Lycanthropy
    (LPVOID)0x000,   // not used
    (LPVOID)0x00C,   // time remaining for skill
  };

  enum 
  {
    ITEM_STRUCT_SIZE = 0x19C,
  };
  enum 
  {
    EQUIP_HEAD,
    EQUIP_RING_L1,
    EQUIP_RING_L2,
    EQUIP_RING_R1,
    EQUIP_RING_R2,
    EQUIP_HAND_R,
    EQUIP_HAND_L,
    EQUIP_CHEST,
    EQUIP_AMULET_R,
    EQUIP_AMULET_L,
    EQUIP_COUNT,
  };

  enum 
  {
    item_offset_name_1 = 0x000,
    item_offset_name_2 = 0x020,
    item_offset_name_3 = 0x040,
    item_offset_sprite = 0x066,
    item_offset_gold = 0x0AC,
    item_offset_min_dmg = 0x0B0,
    item_offset_max_dmg = 0x0B2,
    item_offset_spell_effect = 0x0B6, // byte
    item_offset_phys_effect = 0x0B8, // byte
    item_offset_armour = 0x0B4, // int16
    item_offset_percent_damage = 0x0BC,
    item_offset_percent_hit = 0x0BE,
    item_offset_percent_armour = 0x0C0,
    item_offset_x_loc = 0x17C,
    item_offset_y_loc = 0x180,
    item_offset_spell_increase = 0x0C4, 
    item_offset_spell_id = 196,       // level increase / book type
    item_offset_str = 198,            // int16
    item_offset_mag = 200,            // 
    item_offset_dex = 202,            // 
    item_offset_vit = 204,            // 
    item_offset_fire_resist = 206,    // 
    item_offset_poison_resist = 208,  // 
    item_offset_magic_resist = 210,
    item_offset_mana = 212,
    item_offset_health = 214,
    item_offset_light_radius = 216,
    item_offset_min_add_dmg = 218,
    item_offset_max_add_dmg = 220,
    item_offset_req_str = 226,
    item_offset_req_mag = 228,
    item_offset_req_dex = 230,
    item_offset_req_vit = 232,
    item_offset_max_durability = 240,    // int16
    item_offset_item_id = 266,    // int32
  };
  static const LPVOID pSetWeaponStatMenuOffset[] = 
  {
    (LPVOID) item_offset_min_dmg,
    (LPVOID) item_offset_max_dmg,
    (LPVOID) item_offset_spell_effect,
    (LPVOID) item_offset_phys_effect,
    (LPVOID) item_offset_percent_hit,
    (LPVOID) item_offset_max_durability,
    (LPVOID) item_offset_gold,
  };
  static const LPVOID pSetArmorStatMenuOffset[] = 
  {
    (LPVOID) item_offset_gold,
    (LPVOID) item_offset_armour,
    (LPVOID) item_offset_spell_effect,
    (LPVOID) item_offset_phys_effect,
    (LPVOID) item_offset_percent_hit,
    (LPVOID) item_offset_max_durability,
  };

#define ITEM_NAME_LEN 0x20
#define ITEM_SPRITE_LEN 0x20

  typedef struct 
  {
    char      name1[ITEM_NAME_LEN];               // 0x000
    char      name2[ITEM_NAME_LEN];               // 0x020
    char      name3[ITEM_NAME_LEN];               // 0x040
    char      sprite[ITEM_SPRITE_LEN];            // 0x066
    int32_t   gold;                               // 0x0AC
    int16_t   min_dmg;                            // 0x0B0
    int16_t   max_dmg;                            // 0x0B2
    int16_t   spell_effect;                       // 0..8
    int16_t   phys_effect;                        // 0..0x1F
    int16_t   armour;                             // 0x0B4
    int16_t   percent_hit;                        // 0x0BE
    int16_t   percent_armour;                     // 0x0C0
    uint8_t   spell_increase;                     // 0x0C4
    uint8_t   x_loc;                              // 0x17C
    uint8_t   y_loc;                              // 0x180
  } item_t;
}

#endif // TRAINER_H
