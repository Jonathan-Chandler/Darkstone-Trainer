#ifndef TRAINER_H
#define TRAINER_H
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

typedef struct
{
  LPCSTR  game_title;
  HWND    window_handle;
  DWORD   process_id;
  HANDLE  process_handle;

  LPVOID  weapon_address;
  LPVOID  char_address;
  LPVOID  spell_address;
} trainer_data_t;

void trainer_initialize(trainer_data_t *trainer);
void trainer_show_menu(trainer_data_t *trainer);
void trainer_set_weapon_stats(trainer_data_t *trainer);
void trainer_set_char_stats(trainer_data_t *trainer);
void trainer_set_char_skills(trainer_data_t *trainer);
void trainer_set_char_spells(trainer_data_t *trainer);
void trainer_show_inventory(trainer_data_t *trainer);

#endif // TRAINER_H
