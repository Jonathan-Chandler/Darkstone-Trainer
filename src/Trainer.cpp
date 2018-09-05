#include "../include/Trainer.h"
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <windows.h>

using namespace std;

Trainer::Trainer()
{
  gameTitle = "DarkStone DSI";
  hwnd = FindWindow(0, gameTitle);

  if (hwnd == 0)
  {
    cout << "Darkstone was not found. Make sure the game is running before starting this trainer." << endl;
    system("Pause");
    return;
  }
  else
  {
    cout << "Successfully connected to Darkstone process" << endl;
  }

  GetWindowThreadProcessId(hwnd, &process_ID);
  hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, process_ID);
}

void Trainer::showTrainerMenu()
{
  if (hwnd == 0)
    return;

  int selection = -1;
  while (selection != 0)
  {
    cout << endl << "Select what you would like to modify" << endl;
    cout << "1: Character Stats" << endl;
    cout << "2: Character Spells" << endl;
    cout << "3: Weapon Stats" << endl;
    cout << endl << "0: Exit" << endl;
    cin >> selection;
    switch (selection)
    {
      case 0:
        return;
      case 1:
        setCharacterStats();
        break;
      case 2:
        setCharacterSpells();
        break;
//      case 3:
//        setCharacterSkills();
//        break;
      case 3:
        setWeaponStats();
        break;
      default:
        break;
    }
  }
  return;

}

void Trainer::setCharacterStats()
{
  unsigned int stat = 1;
  short int value = -1;
  LPVOID characterAddress = (LPVOID)0x00AF9F4E;
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
    cout << endl << "Select character stat: " << endl;
    cout << "1: Level" << endl;
    cout << "2: Strength" << endl;
    cout << "3: Magic" << endl;
    cout << "4: Dexterity" << endl;
    cout << "5: Vitality" << endl;
    cout << "6: Remaining Stat Points" << endl;
    cout << endl << "0: Return to main menu" << endl;
    cin >> stat;
    if (stat != 0)
    {
      cout << "Enter a value (-32768 through 32767): ";
      cin >> value;
    }
    switch (stat)
    {
      case 1:
      case 2:
      case 3:
      case 4:
      case 5:
      case 6:
        setValue(characterAddress, characterStatOffset[stat-1], value);
        break;
      default:
        return;
    }
  }
}

void Trainer::setCharacterSpells()
{
  unsigned int stat = 1;
  short int value = -1;
  LPVOID spellAddress = (LPVOID)0x00AFA034;
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

  while (stat != 0)
  {
    cout << endl << "Select spell level to modify: " << endl;
    cout << " 1: Confusion" << endl;
    cout << " 2: Healing" << endl;
    cout << " 3: Fear" << endl;
    cout << " 4: Thunder" << endl;
    cout << " 5: Slowness" << endl;
    cout << " 6: Firewall" << endl;
    cout << " 7: Mutation" << endl;
    cout << " 8: Stone" << endl;
    cout << " 9: Night Vision" << endl;
    cout << "10: Haste" << endl;
    cout << "11: Magic Missile" << endl;
    cout << "12: Fireball" << endl;
    cout << "13: Absorption" << endl;
    cout << "14: Antidote" << endl;
    cout << "15: Flame Thrower" << endl;
    cout << "16: Storm" << endl;
    cout << "17: Invisibility" << endl;
    cout << "18: Death Dome" << endl;
    cout << "19: Invocation" << endl;
    cout << "20: Spark" << endl;
    cout << "21: Teleport" << endl;
    cout << "22: Detection" << endl;
    cout << "23: Food" << endl;
    cout << "24: Berserker" << endl;
    cout << "25: Forgetfulness" << endl;
    cout << "26: Reflections" << endl;
    cout << "27: Magic Bomb" << endl;
    cout << "28: Magic Door" << endl;
    cout << "29: Poison Cloud" << endl;
    cout << "30: Resurrection" << endl;
    cout << "31: Telekinesis" << endl;
    cout << "32: Light" << endl;
    cout << endl << "0: Return to main menu" << endl;
    cin >> stat;
    if (stat != 0)
    {
      cout << "Enter a value (-32768 through 32767): ";
      cin >> value;
    }
    switch (stat)
    {
      case 1:
      case 2:
      case 3:
      case 4:
      case 5:
      case 6:
      case 7:
      case 8:
      case 9:
      case 10:
      case 11:
      case 12:
      case 13:
      case 14:
      case 15:
      case 16:
      case 17:
      case 18:
      case 19:
      case 20:
      case 21:
      case 22:
      case 23:
      case 24:
      case 25:
      case 26:
      case 27:
      case 28:
      case 29:
      case 30:
      case 31:
      case 32:
        setValue(spellAddress, spellAddressOffset[stat-1], value);
        break;
      default:
        return;
    }
  }
}

void Trainer::setCharacterSkills()
{
//  unsigned int stat = 1;
//  short int value = -1;
//  LPVOID characterAddress = (LPVOID)0x00AF9F4E;
//  LPVOID characterStatOffset[] = 
//  {
//    (LPVOID)0x58,
//    (LPVOID)0x0,
//    (LPVOID)0x2,
//    (LPVOID)0x4,
//    (LPVOID)0x6,
//    (LPVOID)0x5e
//  };
//
//  while (stat != 0)
//  {
//    cout << "Select character stat: " << endl;
//    cout << "1: Character Level" << endl;
//    cout << "2: Character Strength" << endl;
//    cout << "3: Character Magic" << endl;
//    cout << "4: Character Dexterity" << endl;
//    cout << "5: Character Vitality" << endl;
//    cout << "6: Remaining Points" << endl;
//    cout << "0: Return to main menu" << endl;
//    cin >> stat;
//    if (stat != 0)
//    {
//      cout << "Enter a value (-32768 through 32767): ";
//      cin >> value;
//    }
//    switch (stat)
//    {
//      case 1:
//      case 2:
//      case 3:
//      case 4:
//      case 5:
//        setValue(characterAddress, characterStatOffset[stat-1], value);
//        break;
//      default:
//        return;
//    }
//  }
}

void Trainer::setValue(LPVOID address, LPVOID offset, short int value)
{
  if (hwnd == 0)
    return;

  if (WriteProcessMemory(hProcess, (LPVOID)((unsigned int)address + (unsigned int)offset), &value, sizeof(value), NULL))
  {
    cout << "Value was written successfully." << endl;
  }
  else
  {
    cout << "Error: Could not modify value." << endl;
  }
}

void Trainer::setWeaponStats()
{
  unsigned int stat = 1;
  short int value = 101;
  LPVOID weaponAddress = (LPVOID)0x00AFB8AC;
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
    cout << endl << "Select weapon stat: " << endl;
    cout << "1: Min Damage" << endl;
    cout << "2: Max Damage" << endl;
    cout << "3: Spell Effect" << endl;
    cout << "4: Physical Effect" << endl;
    cout << "5: Percent Hit Increase" << endl;
    cout << "6: Max Durability (use negative values for indestructible)" << endl;
    cout << endl << "0: Return to main menu" << endl;
    cin >> stat;
    if (stat != 0)
    {
      cout << "Enter a value (-32768 through 32767): ";
      cin >> value;
    }
    switch (stat)
    {
      case 1:
      case 2:
      case 3:
      case 4:
      case 5:
      case 6:
        setValue(weaponAddress, weaponStatOffset[stat-1], value);
        break;
      default:
        return;
    }
  }
}

