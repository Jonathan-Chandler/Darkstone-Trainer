#include "../include/trainer.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <windows.h>

static void trainer_set_value(trainer_data_t *trainer, LPVOID address, LPVOID offset, short int value);

void trainer_initialize(trainer_data_t *trainer)
{
#if defined(STEAM) && defined(GOG) || (!defined(STEAM) && !defined(GOG))
  // either both targets selected or neither selected
  #error "Select either Steam or GOG as trainer target"
#elif defined(GOG)          // GOG 1.05B
  trainer->game_title = "DarkStone DSI";
  trainer->weapon_address = (LPVOID)0x00AFB8AC;
  trainer->char_address = (LPVOID)0x00AF9F4E;
  trainer->spell_address = (LPVOID)0x00AFA034;
#elif defined(STEAM)        // Steam
  trainer->game_title = "DarkStone";
  trainer->weapon_address = (LPVOID)0x00AFBA5C;
  trainer->char_address = (LPVOID)0x00AFA0FE;
  trainer->spell_address = (LPVOID)0x00AFA1E4;
#endif
  trainer->window_handle = FindWindow(0, trainer->game_title);

  if (trainer->window_handle == 0)
  {
    printf("Darkstone was not found. Make sure the game is running before starting this trainer.\n");
    system("Pause");
    return;
  }
  else
  {
    printf("Successfully connected to Darkstone process\n");
    
  }

  GetWindowThreadProcessId(trainer->window_handle, &trainer->process_id);

  trainer->process_handle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, trainer->process_id);
}

void trainer_show_inventory(trainer_data_t *trainer)
{
}

void trainer_show_menu(trainer_data_t *trainer)
{
  char buff[10];
  char *end_ptr;
  int16_t selection = -1;

  if (trainer->window_handle == 0)
    return;

  while (selection != 0)
  {
    printf("Select what you would like to modify\n");
    printf("1: Character Stats\n");
    printf("2: Character Spells\n");
    printf("3: Weapon Stats\n");
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
//      case 3:
//        setCharacterSkills();
//        break;
      case 3:
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
    printf("Select character stat: \n");
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
    }
    else
    {
      printf("Invalid spell");
      return;
    }
  }
}

static void trainer_set_value(trainer_data_t *trainer, LPVOID address, LPVOID offset, short int value)
{
  if (trainer->window_handle == 0)
    return;

  if (WriteProcessMemory(trainer->process_handle, (LPVOID)((unsigned int)address + (unsigned int)offset), &value, sizeof(value), NULL))
  {
    printf("Value was written successfully.\n");
  }
  else
  {
    printf("Error: Could not modify value.\n");
  }
}

void trainer_set_weapon_stats(trainer_data_t *trainer)
{
  char buff[10];
  char *end_ptr;
  int16_t stat = 1;
  int16_t value = 101;

  LPVOID weaponStatOffset[] = 
  {
    (LPVOID)0x0,
    (LPVOID)0x2,
    (LPVOID)0x6,
    (LPVOID)0x8,
    (LPVOID)0xE,
    (LPVOID)0x40
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

    if (stat >= 0 && stat <=6)
    {
      trainer_set_value(trainer, trainer->weapon_address, weaponStatOffset[stat-1], value);
    }
    else
    {
      printf("Invalid stat");
    }
  }
}

