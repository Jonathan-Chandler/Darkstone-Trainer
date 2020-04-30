#ifndef TRAINER_H
#define TRAINER_H
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <stdint.h>

//#define DEBUG_MODE

#ifdef DEBUG_MODE
#define DEBUG_PRINT(...) printf("%s:%d: ",__FILE__, __LINE__); printf(__VA_ARGS__)
#else
#define DEBUG_PRINT(...)
#endif

typedef struct
{
  HWND    window_handle;
  DWORD   process_id;
  HANDLE  process_handle;

  LPVOID  equip_address;
  LPVOID  inventory_address;
  LPVOID  inventory_map_address;
  LPVOID  char_address;
  LPVOID  spell_address;
  LPVOID  cursor_item_address;
} trainer_data_t;

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
  EQUIP_AMULET_L,
  EQUIP_AMULET_R,
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
  item_offset_armour = 0x0B4,
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


void trainer_initialize(trainer_data_t *trainer);
void trainer_destroy(trainer_data_t *trainer);
void trainer_show_menu(trainer_data_t *trainer);
void trainer_set_weapon_stats(trainer_data_t *trainer);
void trainer_set_char_stats(trainer_data_t *trainer);
void trainer_set_char_skills(trainer_data_t *trainer);
void trainer_set_char_spells(trainer_data_t *trainer);
void trainer_get_equip_stats(trainer_data_t *trainer);
void trainer_get_inventory(trainer_data_t *trainer);
void trainer_get_inventory_loc(trainer_data_t *trainer, uint8_t x, uint8_t y);
void trainer_get_map(trainer_data_t *trainer);
void trainer_inventory_delete_item_name(trainer_data_t *trainer, char *item_name);

void trainer_inventory_add_gold(trainer_data_t *trainer);

#endif // TRAINER_H
