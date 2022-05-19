#include "trainer.h"
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <windows.h>
#include <string.h>
#include <iostream>

#define MAX_ITEM_COUNT    0x32    //  50

#define INVENTORY_WIDTH   6
#define INVENTORY_HEIGHT  8

DarkstoneTrainer::DarkstoneTrainer()
  : mDarkstoneProcess()
  , mDarkstoneVersion(-1)
{
  if ((mDarkstoneProcess.getIdFromWindow("DarkStone DSI")) == 0)
  {
    // Good Old Games 1.05B offsets
    mDarkstoneVersion = GAME_VERSION_GOG;
    mEquipBaseAddress = (LPVOID)0x00AFAE54;
    mInventoryBaseAddress = (LPVOID)0x00AFBE6C;
    mInventoryMapBaseAddress = (LPVOID)0x00B0121C;
    mCharBaseAddress = (LPVOID)0x00AF9F4E;
    mSpellBaseAddress = (LPVOID)0x00AFA034;
    mCursorItemBaseAddress = (LPVOID)0x009BC2A8;
    printf("Found DarkStone DSI window\n");
  }
  else if ((mDarkstoneProcess.getIdFromWindow("DarkStone")) == 0)
  {
    // Steam offsets
    mDarkstoneVersion = GAME_VERSION_STEAM;
    mEquipBaseAddress = (LPVOID)0x00AFB004;
    mInventoryBaseAddress = (LPVOID)0x00AFC01C;
    mInventoryMapBaseAddress = (LPVOID)0x00B013CC;
    mCharBaseAddress = (LPVOID)0x00AFA0FE;
    mSpellBaseAddress = (LPVOID)0x00AFA1E4;
    mCursorItemBaseAddress = (LPVOID)0x0;
    printf("Found DarkStone window\n");
  }
  else
  {
    debug_error("Darkstone was not found. Make sure the game is running before starting this trainer.");
    system("Pause");
    exit(-1);
  }

  if (mDarkstoneProcess.openProcessHandle() < 0)
  {
    debug_error("Failed to open Darkstone process handle");
    system("Pause");
    exit(-1);
  }
}

void DarkstoneTrainer::showMenu()
{
  int selection = -1;

  while (selection != 0)
  {
    printf("\nSelect a stat to modify\n");
    printf("1: Character Stats\n");
    printf("2: Character Spells\n");
    printf("3: Character Skills\n");
    printf("4: Weapon Stats\n");
    printf("0: Exit\n");

    std::cin >> selection;
    switch (selection)
    {
      case 0:
        return;
      case 1:
        showCharacterStatsMenu();
        break;
      case 2:
        showCharacterSpellsMenu();
        break;
      case 3:
        showCharacterSkillsMenu();
        break;
      case 4:
        showWeaponStatsMenu();
        break;
      default:
        break;
    }
  }
}

void DarkstoneTrainer::showCharacterStatsMenu()
{
  int16_t stat = 1;
  int16_t value = -1;

  while (stat >= 0)
  {
    std::cout << "\nSelect character stat: \n";
    std::cout << "1: Level\n";
    std::cout << "2: Strength\n";
    std::cout << "3: Magic\n";
    std::cout << "4: Dexterity\n";
    std::cout << "5: Vitality\n";
    std::cout << "6: Remaining Stat Points\n";
    std::cout << "0: Return to main menu\n";

    // stat id
    std::cin >> stat;

    // get stat value
    stat -= 1;
    if (stat < 0 || stat >= DarkstoneOffsets::CHAR_STAT_COUNT)
      return;

    std::cout << "Enter a value (-32768 through 32767): ";
    std::cin >> value;

    setValue(mCharBaseAddress, DarkstoneOffsets::pCharacterStatOffset[stat], value);
  }
}

void DarkstoneTrainer::showCharacterSpellsMenu()
{
  int16_t spell = 1;
  int16_t value = -1;

  while (spell >= 0)
  {
    std::cout << "\nSelect spell: \n";
    std::cout << " 1: Confusion\n";
    std::cout << " 2: Healing\n";
    std::cout << " 3: Fear\n";
    std::cout << " 4: Thunder\n";
    std::cout << " 5: Slowness\n";
    std::cout << " 6: Firewall\n";
    std::cout << " 7: Mutation\n";
    std::cout << " 8: Stone\n";
    std::cout << " 9: Night Vision\n";
    std::cout << "10: Haste\n";
    std::cout << "11: Magic Missile\n";
    std::cout << "12: Fireball\n";
    std::cout << "13: Absorption\n";
    std::cout << "14: Antidote\n";
    std::cout << "15: Flame Thrower\n";
    std::cout << "16: Storm\n";
    std::cout << "17: Invisibility\n";
    std::cout << "18: Death Dome\n";
    std::cout << "19: Invocation\n";
    std::cout << "20: Spark\n";
    std::cout << "21: Teleport\n";
    std::cout << "22: Detection\n";
    std::cout << "23: Food\n";
    std::cout << "24: Berserker\n";
    std::cout << "25: Forgetfulness\n";
    std::cout << "26: Reflections\n";
    std::cout << "27: Magic Bomb\n";
    std::cout << "28: Magic Door\n";
    std::cout << "29: Poison Cloud\n";
    std::cout << "30: Resurrection\n";
    std::cout << "31: Telekinesis\n";
    std::cout << "32: Light\n";
    std::cout << "0: Return to main menu\n";

    // spell id
    std::cin >> spell;
    spell -= 1;
    if (spell < 0 || spell >= DarkstoneOffsets::SPELL_COUNT)
      return;

    // spell level
    std::cout << "Enter a value (-32768 through 32767): ";
    std::cin >> value;

    // set level
    if (setValue(mSpellBaseAddress, DarkstoneOffsets::pCharacterSpellOffset[spell], value) < 0)
    {
      debug_error("Fail to set spell value");
    }

    // reset the remaining buff timer when changing any skill/spell
    char timer_remaining[4] = {0};
    if (setMemoryBlock(mSpellBaseAddress, (LPVOID)((UINT_PTR)DarkstoneOffsets::pCharacterSpellOffset[spell] + (UINT_PTR)DarkstoneOffsets::pCharacterSpellOffset[DarkstoneOffsets::SPELL_REMAINING_TIMER]), timer_remaining, sizeof(timer_remaining)) < 0)
    {
      debug_error("Fail to set remaining spell timer");
    }
  }
}

void DarkstoneTrainer::showCharacterSkillsMenu()
{
  int16_t skill = 1;
  int16_t value = -1;

  while (skill >= 0)
  {
    std::cout << "\nSelect skill level to modify: \n";
    std::cout << " 1: Identification\n";
    std::cout << " 2: Trade\n";
    std::cout << " 3: Repair\n";
    std::cout << " 4: Perception\n";
    std::cout << " 5: Defusing\n";
    std::cout << " 6: Forester\n";
    std::cout << " 7: Learning\n";
    std::cout << " 8: Theft\n";
    std::cout << " 9: Silence\n";
    std::cout << "10: Meditation\n";
    std::cout << "11: Concentration\n";
    std::cout << "12: Orientation\n";
    std::cout << "13: Medicine\n";
    std::cout << "14: Recharging\n";
    std::cout << "15: Exorcism\n";
    std::cout << "16: Prayer\n";
    std::cout << "17: Detection\n";
    std::cout << "18: Master Of Arms\n";
    std::cout << "19: Communion (Monk)\n";
    std::cout << "20: Communion (Wizard)\n";
    std::cout << "21: Language\n";
    std::cout << "22: Lycanthropy\n";
    std::cout << "0: Return to main menu\n";

    // get skill #
    std::cin >> skill;

    skill -= 1;
    if (skill < 0 || skill >= DarkstoneOffsets::SKILL_COUNT)
      return;

    // get skill level
    std::cout << "Enter a value (0 through 65535): ";
    std::cin >> value;

    // set level
    if (setValue(mSpellBaseAddress, DarkstoneOffsets::pCharacterSkillOffset[skill], value) < 0)
    {
      debug_error("Fail to set skill value");
    }

    // reset the remaining buff timer when changing any skill/spell
    char timer_remaining[4] = {0};
    if (setMemoryBlock(mSpellBaseAddress, (LPVOID)((UINT_PTR)DarkstoneOffsets::pCharacterSkillOffset[skill] + (UINT_PTR)DarkstoneOffsets::pCharacterSkillOffset[DarkstoneOffsets::SKILL_TIME_REMAINING]), timer_remaining, sizeof(timer_remaining)) < 0)
    {
      debug_error("Fail to set remaining skill timer");
    }
  }
}

void DarkstoneTrainer::showWeaponStatsMenu()
{
  int16_t stat = 1;
  int16_t value = 101;
  LPVOID pMainWeaponOffset = (LPVOID)((UINT_PTR)mEquipBaseAddress + (UINT_PTR)(DarkstoneOffsets::EQUIP_HAND_L * DarkstoneOffsets::ITEM_STRUCT_SIZE));

  while (stat >= 0)
  {
    std::cout << "\nSelect weapon stat: \n";
    std::cout << "1: Min Damage\n";
    std::cout << "2: Max Damage\n";
    std::cout << "3: Spell Effect\n";
    std::cout << "4: Physical Effect\n";
    std::cout << "5: Percent Hit Increase\n";
    std::cout << "6: Max Durability (use negative values for indestructible)\n";
    std::cout << "0: Return to main menu\n";
 
    // stat id
    std::cin >> stat;
    stat -= 1;
    if (stat < 0 || stat >= sizeof(DarkstoneOffsets::pSetWeaponStatMenuOffset)/sizeof(DarkstoneOffsets::pSetWeaponStatMenuOffset[0]))
      return;

    // value
    std::cout << "Enter a value (-32768 through 32767): ";
    std::cin >> value;

    // write
    if (setValue(pMainWeaponOffset, DarkstoneOffsets::pSetWeaponStatMenuOffset[stat], value) < 0)
      debug_error("Fail to set weapon stat");
  }
}

int DarkstoneTrainer::setValue(LPVOID pBaseAddr, LPVOID pAddrOffset, int16_t value)
{
  return mDarkstoneProcess.writeMemory((LPVOID)((UINT_PTR)pBaseAddr + (UINT_PTR)pAddrOffset), &value);
}

int DarkstoneTrainer::setMemoryBlock(LPVOID pBaseAddr, LPVOID pAddrOffset, char *pValue, SIZE_T nBytes)
{
  return mDarkstoneProcess.writeMemoryBlock((LPVOID)((UINT_PTR)pBaseAddr + (UINT_PTR)pAddrOffset), pValue, nBytes);
}
