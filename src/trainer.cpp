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
  : mDarkstoneVersion(GAME_VERSION_COUNT)
  , mDarkstoneProcess()
  , mCurrentPlayer(0)
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
    std::cout << "Found DarkStone DSI window\n";
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
    std::cout << "Found DarkStone window\n";
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
    std::cout << "\nSelect a stat to modify for player " << getCurrentPlayer() << " (or use option 1 to select which player to edit)" << std::endl;
    std::cout << "1: Select player\n";
    std::cout << "2: Character Stats\n";
    std::cout << "3: Character Spells\n";
    std::cout << "4: Character Skills\n";
    std::cout << "5: Weapon Stats\n";
    std::cout << "0: Exit\n";

    std::cin >> selection;
    switch (selection)
    {
      case 0:
        return;
      case 1:
        showPlayerSelectMenu();
        break;
      case 2:
        showCharacterStatsMenu();
        break;
      case 3:
        showCharacterSpellsMenu();
        break;
      case 4:
        showCharacterSkillsMenu();
        break;
      case 5:
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
    std::cout << "\nSelect player " << getCurrentPlayer() << " character stat: \n";
    std::cout << "1: Level\n";
    std::cout << "2: Strength\n";
    std::cout << "3: Magic\n";
    std::cout << "4: Dexterity\n";
    std::cout << "5: Vitality\n";
    std::cout << "6: Remaining Stat Points\n";
    std::cout << "7: Age\n";
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

    // match second value to age
    if (stat == DarkstoneOffsets::CHAR_STAT_AGE)
      setValue(mCharBaseAddress, (LPVOID)((uint32_t)DarkstoneOffsets::pCharacterStatOffset[stat]+4), value);
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
    std::cout << " 0: Return to main menu\n";

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
    std::cout << "\nSelect player " << getCurrentPlayer() << " skill level to modify: \n";
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
    std::cout << " 0: Return to main menu\n";

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
  int32_t value32;
  LPVOID pMainWeaponOffset = (LPVOID)((UINT_PTR)mEquipBaseAddress + (UINT_PTR)(DarkstoneOffsets::EQUIP_HAND_L * DarkstoneOffsets::ITEM_STRUCT_SIZE));

  while (stat >= 0)
  {
begin_loop:
    std::cout << "\nSelect player " << getCurrentPlayer() << " weapon stat: \n";
    std::cout << "1: Min Damage\n";
    std::cout << "2: Max Damage\n";
    std::cout << "3: Spell Effect\n";
    std::cout << "4: Physical Effect\n";
    std::cout << "5: Percent Hit Increase\n";
    std::cout << "6: Max Durability (use negative values for indestructible)\n";
    std::cout << "7: Gold Value\n";
    std::cout << "0: Return to main menu\n";

    // stat id
    std::cin >> stat;
    stat -= 1;
    if (stat < 0 || stat >= sizeof(DarkstoneOffsets::pSetWeaponStatMenuOffset)/sizeof(DarkstoneOffsets::pSetWeaponStatMenuOffset[0]))
      return;

    switch (stat)
    {
      case 2:
        // List known spell effects
        std::cout << "Choose A Spell Effect:\n";
        std::cout << "0: None\n";
        std::cout << "1: Poison\n";
        std::cout << "2: Fire Element\n";
        std::cout << "3: Vampire: Steals 20% Life Points\n";
        std::cout << "4: Magic Missiles\n";
        std::cout << "5: Storm\n";
        std::cout << "6: Stone Curse\n";
        std::cout << "7: Touch of Confusion\n";
        std::cout << "Enter a value (-2147483648 through 2147483647): ";
        std::cin >> value;
        break;

      case 3:
        // uses a bitfield:
        // 00 0001 - push target back
        // 00 0010 - fast attack (overrides faster and fastest attack)
        // 00 0100 - faster attack (overrides fastest attack)
        // 00 1000 - fastest attack
        // 01 0000 - recovery time improved (overrides quick recovery)
        // 10 0000 - quick recovery
        std::cout << "Choose A Physical Effect:\n";
        std::cout << "0: None\n";
        std::cout << "1: Push Target Back\n";                                               // 000 0001
        std::cout << "2: Fast Attack\n";                                                    // 000 0010
        std::cout << "3: Push Target Back + Fast Attack\n";                                 // 000 0011
        std::cout << "4: Faster Attack\n";                                                  // 000 0100
        std::cout << "5: Push Target Back + Faster Attack\n";                               // 000 0101
        //std::cout << "6: Fast Attack\n";                                                  // 000 0110 - fast attack overrides faster attack
        //std::cout << "7: Push Target Back + Fast Attack\n";                               // 000 0111 - fast attack overrides faster attack
        std::cout << "8: Fastest Attack\n";                                                 // 000 1000
        std::cout << "9: Push Target Back + Fastest Attack\n";                              // 000 1001
        //std::cout << "10: Fast Attack\n";                                                 // 000 1010
        //std::cout << "11: Push Target Back + Fast Attack\n";                              // 000 1011
        //std::cout << "12: Faster Attack\n";                                               // 000 1100 - faster attack overrides fastest attack
        //std::cout << "13: Push Target Back + Faster Attack\n";                            // 000 1101 - faster attack overrides fastest attack
        //std::cout << "14: Fast Attack\n";                                                 // 000 1110 - fast attack overrides fastest and faster attack
        std::cout << "16: Recovery Time Improved\n";                                        // 001 0000
        std::cout << "25: Push Target Back + Fastest Attack + Recovery Time Improved\n";    // 001 1001
        std::cout << "32: Quick Recovery\n";                                                // 010 0000
        std::cout << "33: Push Target Back + Quick Recovery\n";                             // 010 0001
        std::cout << "40: Fastest Attack + Quick Recovery\n";                               // 010 1000
        std::cout << "41: Push Target Back + Fastest Attack + Quick Recovery\n";            // 010 1001
        // std::cout << "48: Recovery Time Improved\n";                                     // 011 0000 - recovery time improved overrides quick recovery time
        // std::cout << "64: None \n";                                                      // 100 0000 - values >= 64 do nothing
        std::cout << "Enter a value (-2147483648 through 2147483647): ";
        std::cin >> value;
        break;

      case 6:
        std::cout << "Enter a value (-2147483648 through 2147483647): ";
        std::cin >> value32;

        // set 32-bit value
        if (setMemoryBlock(pMainWeaponOffset, DarkstoneOffsets::pSetWeaponStatMenuOffset[stat], (char*)&value32, sizeof(value32)) < 0)
          debug_error("Fail to set weapon gold stat");
        goto begin_loop;

      default:
        std::cout << "Enter a value (-32768 through 32767): ";
        std::cin >> value;
        break;
    }

    // set 16-bit value
    if (setValue(pMainWeaponOffset, DarkstoneOffsets::pSetWeaponStatMenuOffset[stat], value) < 0)
      debug_error("Fail to set weapon stat");
  }
}

void DarkstoneTrainer::showPlayerSelectMenu()
{
  int value;

  std::cout << "\nSelect which player to edit (1 or 2): ";
  std::cin >> value;

  setCurrentPlayer(value);
}

void DarkstoneTrainer::setCurrentPlayer(int player)
{
  if (player <= 1)
    mCurrentPlayer = 0;
  else
    mCurrentPlayer = 1;
}

int DarkstoneTrainer::getCurrentPlayer()
{
  return mCurrentPlayer+1;
}

LPVOID DarkstoneTrainer::getPlayerBaseAddr(LPVOID pBaseAddr)
{
  uint32_t currentBase = (uint32_t)pBaseAddr + ((uint32_t)DarkstoneOffsets::PLAYER_2_OFFSET * mCurrentPlayer);
  return (LPVOID)currentBase;
}

int DarkstoneTrainer::setValue(LPVOID pBaseAddr, LPVOID pAddrOffset, int16_t value)
{
  return mDarkstoneProcess.writeMemory((LPVOID)((UINT_PTR)getPlayerBaseAddr(pBaseAddr) + (UINT_PTR)pAddrOffset), &value);
}

int DarkstoneTrainer::setMemoryBlock(LPVOID pBaseAddr, LPVOID pAddrOffset, char *pValue, SIZE_T nBytes)
{
  return mDarkstoneProcess.writeMemoryBlock((LPVOID)((UINT_PTR)getPlayerBaseAddr(pBaseAddr) + (UINT_PTR)pAddrOffset), pValue, nBytes);
}

