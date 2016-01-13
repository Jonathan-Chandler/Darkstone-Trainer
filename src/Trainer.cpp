#include "Trainer.h"
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
    int selection = -1;
    while (selection != 0)
    {
        cout << "Select what you would like to modify" << endl;
        cout << "1: Character Stats" << endl;
        cout << "2: Weapon Stats" << endl;
        cout << "0: Exit" << endl;
        cin >> selection;
        switch (selection)
        {
            case 0:
                return;
            case 1:
                setCharacterStats();
                break;
            case 2:
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
            (LPVOID)0x6
        };
    
    while (stat != 0)
    {
        cout << "Select character stat: " << endl;
        cout << "1: Character Level" << endl;
        cout << "2: Character Strength" << endl;
        cout << "3: Character Magic" << endl;
        cout << "4: Character Dexterity" << endl;
        cout << "5: Character Vitality" << endl;
        cout << "0: Return to main menu" << endl;
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
                setValue(characterAddress, characterStatOffset[stat-1], value);
                break;
            default:
                return;
        }
    }
}

void Trainer::setValue(LPVOID address, LPVOID offset, short int value)
{
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
            (LPVOID)0xE
        };
    while (stat != 0)
    {
        cout << "Select weapon stat: " << endl;
        cout << "1: Min Damage" << endl;
        cout << "2: Max Damage" << endl;
        cout << "3: Spell Effect" << endl;
        cout << "4: Physical Effect" << endl;
        cout << "5: Percent Hit Increase" << endl;
        cout << "0: Return to main menu" << endl;
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
                setValue(weaponAddress, weaponStatOffset[stat-1], value);
                break;
            default:
                return;
        }
    }
}

