#include "trainer.h"
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <windows.h>
#include <string.h>

#define ITEM_STRUCT_SIZE  0x19C   // 412
#define MAX_ITEM_COUNT    0x32    //  50

#define INVENTORY_WIDTH   6
#define INVENTORY_HEIGHT  8
#define SPELL_TIMER_OFFSET 12

typedef struct
{
  uint8_t inventory_map[INVENTORY_HEIGHT][INVENTORY_WIDTH];
} inventory_t;

static uint8_t null_item[ITEM_STRUCT_SIZE] = {0};

void trainer_initialize(trainer_data_t *trainer)
{
  if ((trainer->window_handle = FindWindow(0, "DarkStone DSI")) != 0)
  {
    // Good Old Games 1.05B offsets
    trainer->equip_address = (LPVOID)0x00AFAE54;
    trainer->inventory_address = (LPVOID)0x00AFBE6C;
    trainer->inventory_map_address = (LPVOID)0x00B0121C;
    trainer->char_address = (LPVOID)0x00AF9F4E;
    trainer->spell_address = (LPVOID)0x00AFA034;
    trainer->cursor_item_address = (LPVOID)0x009BC2A8;
  }
  else if ((trainer->window_handle = FindWindow(0, "DarkStone")) != 0)
  {
    // Steam offsets
    trainer->equip_address = (LPVOID)0x00AFB004;
    trainer->inventory_address = (LPVOID)0x00AFC01C;
    trainer->inventory_map_address = (LPVOID)0x00B013CC;
    trainer->char_address = (LPVOID)0x00AFA0FE;
    trainer->spell_address = (LPVOID)0x00AFA1E4;
  }
  else
  {
    printf("Darkstone was not found. Make sure the game is running before starting this trainer.\n");
    system("Pause");
    exit(-1);
  }
  
  printf("Successfully connected to Darkstone process\n");
  DEBUG_PRINT("window_handle = %p\n", trainer->window_handle);

  GetWindowThreadProcessId(trainer->window_handle, &trainer->process_id);
  DEBUG_PRINT("process_id = %lu\n", trainer->process_id);

  // kernel32
  trainer->process_handle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, trainer->process_id);
  DEBUG_PRINT("process_handle = %p\n", trainer->process_handle);
  DEBUG_PRINT("all-access=%lX\n",PROCESS_ALL_ACCESS);
  DEBUG_PRINT("all-access-dec=%lu\n",PROCESS_ALL_ACCESS);
}

void trainer_destroy(trainer_data_t *trainer)
{
  DEBUG_PRINT("Destroy trainer\n");

  if (trainer->process_handle)
  {
    DEBUG_PRINT("Closing process handle: %p\n", trainer->process_handle);
    CloseHandle(trainer->process_handle);
  }

  trainer->window_handle = 0;
  trainer->process_handle = 0;
}

static void trainer_set_block(trainer_data_t *trainer, LPVOID address, uint8_t *value, size_t value_size)
{
  if (trainer->process_handle == 0)
  {
    printf("Failed: process_handle not initialized");
    return;
  }

  if (WriteProcessMemory(trainer->process_handle, address, value, value_size, NULL))
  {
    DEBUG_PRINT("Block was written successfully.\n");
  }
  else
  {
    printf("Error: Could not modify block.\n");
  }
}

static void trainer_set_value(trainer_data_t *trainer, LPVOID address, LPVOID offset, short int value)
{
  if (trainer->process_handle == 0)
  {
    printf("Failed: process_handle not initialized");
    return;
  }

  if (WriteProcessMemory(trainer->process_handle, (LPVOID)((UINT_PTR)address + (UINT_PTR)offset), &value, sizeof(value), NULL))
  {
    DEBUG_PRINT("Value was written successfully.\n");
  }
  else
  {
    printf("Error: Could not modify value.\n");
  }
}

static void trainer_block_to_item(item_t *item, uint8_t *mem_block)
{
  memcpy(&item->name1, &mem_block[item_offset_name_1], sizeof(item->name1));
  memcpy(&item->name2, &mem_block[item_offset_name_2], sizeof(item->name2));
  memcpy(&item->name3, &mem_block[item_offset_name_3], sizeof(item->name3));
  memcpy(&item->sprite, &mem_block[item_offset_sprite], sizeof(item->sprite));
  memcpy(&item->gold, &mem_block[item_offset_gold], sizeof(item->gold));
  memcpy(&item->min_dmg, &mem_block[item_offset_min_dmg], sizeof(item->min_dmg));
  memcpy(&item->max_dmg, &mem_block[item_offset_max_dmg], sizeof(item->max_dmg));
  memcpy(&item->spell_effect, &mem_block[item_offset_spell_effect], sizeof(item->spell_effect));
  memcpy(&item->phys_effect, &mem_block[item_offset_phys_effect], sizeof(item->phys_effect));
  memcpy(&item->armour, &mem_block[item_offset_armour], sizeof(item->armour));
  memcpy(&item->percent_hit, &mem_block[item_offset_percent_hit], sizeof(item->percent_hit));
  memcpy(&item->percent_armour, &mem_block[item_offset_percent_armour], sizeof(item->percent_armour));
  memcpy(&item->spell_increase, &mem_block[item_offset_spell_increase], sizeof(item->spell_increase));
  memcpy(&item->x_loc, &mem_block[item_offset_x_loc], sizeof(item->x_loc));
  memcpy(&item->y_loc, &mem_block[item_offset_y_loc], sizeof(item->y_loc));
}

static void trainer_get_value(trainer_data_t *trainer, LPVOID address, LPVOID offset, uint8_t *value_addr, size_t value_size)
{
  if (trainer->process_handle == 0)
  {
    printf("Failed: process_handle not initialized");
    return;
  }

//   BOOL ReadProcessMemory(
//   HANDLE  hProcess,
//   LPCVOID lpBaseAddress,
//   LPVOID  lpBuffer,
//   SIZE_T  nSize,
//   SIZE_T  *lpNumberOfBytesRead
// );
  // typedef PVOID HANDLE; -> typedef void *PVOID;
  // HANDLE LPCVOID LPVOID SIZE_T SIZE_T
  // typedef CONST void *LPCVOID;
  // typedef void *LPVOID;
  if (ReadProcessMemory(trainer->process_handle, (LPVOID)((UINT_PTR)address + (UINT_PTR)offset), value_addr, value_size, NULL))
  {
    // DEBUG_PRINT("Value was read successfully.\n");
  }
  else
  {
    printf("Error: Could not read value.\n");
  }
}

void trainer_show_menu(trainer_data_t *trainer)
{
  char buff[10];
  char *end_ptr;
  int16_t selection = -1;

  if (trainer->process_handle == 0)
  {
    printf("Failed: process_handle not initialized");
    return;
  }

  while (selection != 0)
  {
    printf("\nSelect what you would like to modify\n");
    printf("1: Character Stats\n");
    printf("2: Character Spells\n");
    printf("3: Character Skills\n");
    printf("4: Weapon Stats\n");
    printf("0: Exit\n");

    if (fgets(buff, sizeof(buff), stdin) != NULL) 
    {
      selection = (int16_t) strtol(buff, &end_ptr, 10);
    }
    else
    {
      printf("Invalid value\n");
      return;
    }

    switch (selection)
    {
      case 0:
        return;
      case 1:
        trainer_set_char_stats(trainer);
        break;
      case 2:
        trainer_set_char_spells(trainer);
        break;
      case 3:
        trainer_set_char_skills(trainer);
        break;
      case 4:
        trainer_set_weapon_stats(trainer);
        break;
      default:
        break;
    }
  }
}

void trainer_set_char_stats(trainer_data_t *trainer)
{
  char buff[10];
  char *end_ptr;
  int16_t stat = 1;
  int16_t value = -1;
  LPVOID characterStatOffset[] = 
  {
    (LPVOID)0x58,
    (LPVOID)0x0,
    (LPVOID)0x2,
    (LPVOID)0x4,
    (LPVOID)0x6,
    (LPVOID)0x5e
  };

  while (stat != 0)
  {
    printf("\nSelect character stat: \n");
    printf("1: Level\n");
    printf("2: Strength\n");
    printf("3: Magic\n");
    printf("4: Dexterity\n");
    printf("5: Vitality\n");
    printf("6: Remaining Stat Points\n");
    printf("0: Return to main menu\n");

    // get stat id
    if (fgets(buff, sizeof(buff), stdin) != NULL) 
    {
      stat = (int16_t) strtol(buff, &end_ptr, 10);
    }
    else
    {
      printf("Invalid stat\n");
      return;
    }

    // get stat value
    if (stat != 0)
    {
      printf("Enter a value (-32768 through 32767): ");
      if (fgets(buff, sizeof(buff), stdin) != NULL) 
      {
        value = (int16_t) strtol(buff, &end_ptr, 10);
      }
      else
      {
        printf("Invalid value\n");
        return;
      }
    }

    // set if not exiting
    if (stat > 0 && stat <= 6)
    {
      trainer_set_value(trainer, trainer->char_address, characterStatOffset[stat-1], value);
    }
  }
}

void trainer_set_char_spells(trainer_data_t *trainer)
{
  char buff[10];
  char *end_ptr;
  int16_t spell = 1;
  int16_t value = -1;
  LPVOID spellAddressOffset[] = 
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
    (LPVOID)0x1B0,   // 28: telekinesis
    (LPVOID)0x1C0,   // 29: poison cloud
    (LPVOID)0x1D0,   // 30: resurrection
    (LPVOID)0x1E0,   // 31: telekinesis
    (LPVOID)0x1F0,   // 32: light
  };

  while (spell != 0)
  {
    printf("\nSelect spell level to modify: \n");
    printf(" 1: Confusion\n");
    printf(" 2: Healing\n");
    printf(" 3: Fear\n");
    printf(" 4: Thunder\n");
    printf(" 5: Slowness\n");
    printf(" 6: Firewall\n");
    printf(" 7: Mutation\n");
    printf(" 8: Stone\n");
    printf(" 9: Night Vision\n");
    printf("10: Haste\n");
    printf("11: Magic Missile\n");
    printf("12: Fireball\n");
    printf("13: Absorption\n");
    printf("14: Antidote\n");
    printf("15: Flame Thrower\n");
    printf("16: Storm\n");
    printf("17: Invisibility\n");
    printf("18: Death Dome\n");
    printf("19: Invocation\n");
    printf("20: Spark\n");
    printf("21: Teleport\n");
    printf("22: Detection\n");
    printf("23: Food\n");
    printf("24: Berserker\n");
    printf("25: Forgetfulness\n");
    printf("26: Reflections\n");
    printf("27: Magic Bomb\n");
    printf("28: Magic Door\n");
    printf("29: Poison Cloud\n");
    printf("30: Resurrection\n");
    printf("31: Telekinesis\n");
    printf("32: Light\n");
    printf("\n0: Return to main menu\n");

    // get spell #
    if (fgets(buff, sizeof(buff), stdin) != NULL) 
    {
      spell = (int16_t) strtol(buff, &end_ptr, 10);
    }
    else
    {
      printf("Invalid spell\n");
      return;
    }

    // get spell level
    if (spell != 0)
    {
      printf("Enter a value (-32768 through 32767): ");

      if (fgets(buff, sizeof(buff), stdin) != NULL) 
      {
        value = (int16_t) strtol(buff, &end_ptr, 10);
      }
      else
      {
        printf("Invalid value\n");
        return;
      }
    }

    if (spell > 0 && spell <= 32)
    {
      trainer_set_value(trainer, trainer->spell_address, spellAddressOffset[spell-1], value);

      // Also reset the remaining buff timer when changing any skill/spell
      uint8_t timer_remaining[4] = {0};
      trainer_set_block(trainer, (LPVOID)((UINT_PTR)trainer->spell_address + (UINT_PTR)spellAddressOffset[spell-1] + (UINT_PTR)SPELL_TIMER_OFFSET), timer_remaining, sizeof(timer_remaining));
      DEBUG_PRINT("Timer address: %p\n",(LPVOID)((UINT_PTR)trainer->spell_address + (UINT_PTR)spellAddressOffset[spell-1]) + (UINT_PTR)SPELL_TIMER_OFFSET);
    }
    else if (spell != 0)
    {
      printf("Invalid spell\n");
      return;
    }
  }
}

void trainer_set_char_skills(trainer_data_t *trainer)
{
  char buff[10];
  char *end_ptr;
  int16_t skill = 1;
  int16_t value = -1;

  LPVOID skillAddressOffset[] = 
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
  };

  while (skill != 0)
  {
    printf("\nSelect skill level to modify: \n");
    printf(" 1: Identification\n");
    printf(" 2: Trade\n");
    printf(" 3: Repair\n");
    printf(" 4: Perception\n");
    printf(" 5: Defusing\n");
    printf(" 6: Forester\n");
    printf(" 7: Learning\n");
    printf(" 8: Theft\n");
    printf(" 9: Silence\n");
    printf("10: Meditation\n");
    printf("11: Concentration\n");
    printf("12: Orientation\n");
    printf("13: Medicine\n");
    printf("14: Recharging\n");
    printf("15: Exorcism\n");
    printf("16: Prayer\n");
    printf("17: Detection\n");
    printf("18: Master Of Arms\n");
    printf("19: Communion (Monk)\n");
    printf("20: Communion (Wizard)\n");
    printf("21: Language\n");
    printf("22: Lycanthropy\n");
    printf("\n0: Return to main menu\n");

    // get skill #
    if (fgets(buff, sizeof(buff), stdin) != NULL) 
    {
      skill = (int16_t) strtol(buff, &end_ptr, 10);
    }
    else
    {
      printf("Invalid skill\n");
      return;
    }

    // get skill level
    if (skill != 0)
    {
      printf("Enter a value (0 through 65535): ");

      if (fgets(buff, sizeof(buff), stdin) != NULL) 
      {
        value = (int16_t) strtol(buff, &end_ptr, 10);
      }
      else
      {
        printf("Invalid value\n");
        return;
      }
    }

    if (skill > 0 && skill <= 22)
    {
      trainer_set_value(trainer, trainer->spell_address, skillAddressOffset[skill-1], value);

      // Also reset the remaining buff timer when changing any skill/spell
      uint8_t timer_remaining[4] = {0};
      trainer_set_block(trainer, (LPVOID)((UINT_PTR)trainer->spell_address + (UINT_PTR)skillAddressOffset[skill-1] + (UINT_PTR)SPELL_TIMER_OFFSET), timer_remaining, sizeof(timer_remaining));
      DEBUG_PRINT("Timer address: %p\n",(LPVOID)((UINT_PTR)trainer->spell_address + (UINT_PTR)skillAddressOffset[skill-1]) + SPELL_TIMER_OFFSET);
    }
    else if (skill != 0)
    {
      printf("Invalid skill");
    }
  }
}

void trainer_inventory_delete_item_name(trainer_data_t *trainer, char *item_name)
{
  LPVOID item_offset = 0;
  item_t temp_item = {0};

  for (UINT_PTR i = 0; i < MAX_ITEM_COUNT; i++)
  {
    item_offset = (LPVOID)(i * ITEM_STRUCT_SIZE);
    LPVOID item_address = (LPVOID)((UINT_PTR)trainer->inventory_address + (UINT_PTR)item_offset);
    trainer_get_value(trainer, trainer->inventory_address, item_offset, (uint8_t*)&temp_item, sizeof(temp_item));

    if (strcmp(temp_item.name1, item_name) == 0)
    {
      trainer_set_block(trainer, item_address, null_item, ITEM_STRUCT_SIZE);
      printf("Deleted %s at item index %" PRId64 "\n", item_name, i);
    }
  }
}

void trainer_get_map(trainer_data_t *trainer)
{
  inventory_t bag_map;

  trainer_get_value(trainer, trainer->inventory_map_address, 0, (uint8_t*)&bag_map, sizeof(bag_map));
  DEBUG_PRINT("\nbag map: \n");
// item_data_addr = inventory_base_addr + ((item_map_index - 0xA) * sizeof(item_t));

  for (int y = 0; y < INVENTORY_HEIGHT; y++)
  {
    for (int x = 0; x < INVENTORY_WIDTH; x++)
    {
      printf("%02X ", bag_map.inventory_map[y][x]);
    }
    printf("\n");
  }
}

void trainer_get_inventory(trainer_data_t *trainer)
{
  LPVOID item_offset = 0;
  item_t temp_item = {0};
  uint8_t item_block[ITEM_STRUCT_SIZE] = {0};

  for (UINT_PTR i = 0; i < MAX_ITEM_COUNT; i++)
  {
    item_offset = (LPVOID)(i * ITEM_STRUCT_SIZE);
    trainer_get_value(trainer, trainer->inventory_address, item_offset, item_block, sizeof(item_block));
    trainer_block_to_item(&temp_item, item_block);
    
    if (temp_item.name1[0])
    {
      printf("id: %" PRId64 "\n", i);
      printf("Item name: %s\n", temp_item.name1);
      printf("Item name2: %s\n", temp_item.name2);
      printf("Item name3: %s\n", temp_item.name3);
      printf("Sprite_name: %s\n", temp_item.sprite);
      printf("Gold count: %i\n", temp_item.gold);
      printf("Min dmg: %i\n", temp_item.min_dmg);
      printf("Max dmg: %i\n", temp_item.max_dmg);
      printf("Spell effect: %i\n", temp_item.spell_effect);
      printf("Physical effect: %i\n", temp_item.phys_effect);
      printf("Armour: %i\n", temp_item.armour);
      printf("Percent hit: %i\n", temp_item.percent_hit);
      printf("Percent armour: %i\n", temp_item.percent_armour);
      printf("Spell ID: %i\n", temp_item.spell_increase);
      printf("X: %i\n", temp_item.x_loc);
      printf("Y: %i\n", temp_item.y_loc);
    }
  }
}

void trainer_set_weapon_stats(trainer_data_t *trainer)
{
  char buff[10];
  char *end_ptr;
  int16_t stat = 1;
  int16_t value = 101;

  LPVOID weapon_offset = (LPVOID)((UINT_PTR)trainer->equip_address + (UINT_PTR)(EQUIP_HAND_L * ITEM_STRUCT_SIZE));

  LPVOID weaponStatOffset[] = 
  {
    (LPVOID) item_offset_min_dmg,
    (LPVOID) item_offset_max_dmg,
    (LPVOID) item_offset_spell_effect,
    (LPVOID) item_offset_phys_effect,
    (LPVOID) item_offset_percent_hit,
    (LPVOID) item_offset_max_durability,
  };

  while (stat != 0)
  {
    printf("\nSelect weapon stat: \n");
    printf("1: Min Damage\n");
    printf("2: Max Damage\n");
    printf("3: Spell Effect\n");
    printf("4: Physical Effect\n");
    printf("5: Percent Hit Increase\n");
    printf("6: Max Durability (use negative values for indestructible)\n");
    printf("\n0: Return to main menu\n");
 
    if (fgets(buff, sizeof(buff), stdin) != NULL) 
    {
      stat = (int16_t) strtol(buff, &end_ptr, 10);
    }

    if (stat != 0)
    {
      printf("Enter a value (-32768 through 32767): ");

      if (fgets(buff, sizeof(buff), stdin) != NULL) 
      {
        value = (int16_t) strtol(buff, &end_ptr, 10);
      }
    }

    if (stat > 0 && stat <=6)
    {
      trainer_set_value(trainer, weapon_offset, weaponStatOffset[stat-1], value);
    }
    else if (stat != 0)
    {
      printf("Invalid stat");
    }
  }
}

void trainer_get_inventory_loc(trainer_data_t *trainer, uint8_t x, uint8_t y)
{
  LPVOID item_offset = 0;
  item_t temp_item = {0};
  uint8_t item_block[ITEM_STRUCT_SIZE] = {0};

  for (UINT_PTR i = 0; i < MAX_ITEM_COUNT; i++)
  {
    item_offset = (LPVOID)(i * ITEM_STRUCT_SIZE);
    trainer_get_value(trainer, trainer->inventory_address, item_offset, item_block, sizeof(item_block));
    trainer_block_to_item(&temp_item, item_block);
    
    if (temp_item.name1[0])
    {
      printf("Item name: %s\n", temp_item.name1);
      printf("Item name2: %s\n", temp_item.name2);
      printf("Item name3: %s\n", temp_item.name3);
      printf("Sprite_name: %s\n", temp_item.sprite);
      printf("Gold count: %i\n", temp_item.gold);
      printf("Min dmg: %i\n", temp_item.min_dmg);
      printf("Max dmg: %i\n", temp_item.max_dmg);
      printf("Armour: %i\n", temp_item.armour);
      printf("Percent hit: %i\n", temp_item.percent_hit);
      printf("Percent armour: %i\n", temp_item.percent_armour);
      printf("Spell ID: %i\n", temp_item.spell_increase);
      printf("X: %i\n", temp_item.x_loc);
      printf("Y: %i\n", temp_item.y_loc);
    }
  }
}

void trainer_get_equip_stats(trainer_data_t *trainer)
{
  LPVOID item_offset = 0;
  item_t temp_item = {0};
  uint8_t item_block[ITEM_STRUCT_SIZE] = {0};

  for (UINT_PTR i = 0; i < EQUIP_COUNT; i++)
  {
    item_offset = (LPVOID)(i * ITEM_STRUCT_SIZE);
    trainer_get_value(trainer, trainer->equip_address, item_offset, (uint8_t*)item_block, sizeof(item_block));
    trainer_block_to_item(&temp_item, item_block);
    
    if (temp_item.name1[0])
    {
      printf("id: %" PRId64 "\n", i);
      printf("Item name: %s\n", temp_item.name1);
      printf("Item name2: %s\n", temp_item.name2);
      printf("Item name3: %s\n", temp_item.name3);
      printf("Sprite_name: %s\n", temp_item.sprite);
      printf("Gold count (sale price): %i\n", temp_item.gold);
      printf("Min dmg: %i\n", temp_item.min_dmg);
      printf("Max dmg: %i\n", temp_item.max_dmg);
      printf("Spell effect: %i\n", temp_item.spell_effect);
      printf("Physical effect: %i\n", temp_item.phys_effect);
      printf("Armour: %i\n", temp_item.armour);
      printf("Percent hit: %i\n", temp_item.percent_hit);
      printf("Percent armour: %i\n", temp_item.percent_armour);
      printf("Spell ID: %i\n", temp_item.spell_increase);
      printf("X: %i\n", temp_item.x_loc);
      printf("Y: %i\n", temp_item.y_loc);
    }
  }
}

//static void trainer_block_to_inventory_map(inventory_t *map, uint8_t *mem_block)
//{
//  for (int y = 0; y < INVENTORY_HEIGHT; y++)
//  {
//    for (int x = 0; x < INVENTORY_WIDTH; x++)
//    {
//      map->inventory_map[x][y] = mem_block[x+y*INVENTORY_HEIGHT];
//    }
//  }
//}
//
//void trainer_get_equip_stats(trainer_data_t *trainer, char *item_name)
//{
//  LPVOID item_offset = 0;
//  item_t temp_item = {0};
//
//  for (unsigned i = 0; i < MAX_ITEM_COUNT; i++)
//  {
//    item_offset = (LPVOID)(i * ITEM_STRUCT_SIZE);
//    LPVOID item_address = (uint32_t)trainer->inventory_address + item_offset;
//    trainer_get_value(trainer, trainer->inventory_address, item_offset, (uint8_t*)&temp_item, sizeof(temp_item));
//
//    if (strcmp(temp_item.name1, item_name) == 0)
//    {
//      trainer_set_block(trainer, item_address, null_item, ITEM_STRUCT_SIZE);
//      printf("Deleted %s at item index %d\n", item_name, i);
//    }
//  }
//}
//
//void trainer_set_equip_stats(trainer_data_t *trainer)
//{
//  LPVOID item_offset = 0;
//  item_t temp_item = {0};
//
//  for (UINT_PTR i = 0; i < MAX_ITEM_COUNT; i++)
//  {
//    item_offset = (LPVOID)(i * ITEM_STRUCT_SIZE);
//    LPVOID item_address = (uint32_t)trainer->inventory_address + item_offset;
//    trainer_get_value(trainer, trainer->inventory_address, item_offset, (uint8_t*)&temp_item, sizeof(temp_item));
//
//    if (strcmp(temp_item.name1, item_name) == 0)
//    {
//      trainer_set_block(trainer, item_address, null_item, ITEM_STRUCT_SIZE);
//      printf("Deleted %s at item index %d\n", item_name, i);
//    }
//  }
//}
