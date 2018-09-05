#ifndef TRAINER_H
#define TRAINER_H
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <windows.h>
#include <map>
#include <utility>

class Trainer
{
    public:
        Trainer();
        void showTrainerMenu();
    protected:
    private:
        LPCSTR gameTitle = "DarkStone DSI";
        HWND hwnd = FindWindow(0, gameTitle);
        DWORD process_ID;
        HANDLE hProcess;

        void setValue(LPVOID address, LPVOID offset, short int value);
        void setCharacterStats();
        void setWeaponStats();
        void setCharacterSkills();
        void setCharacterSpells();
};

#endif // TRAINER_H
