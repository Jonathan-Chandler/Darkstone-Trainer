#include "include/trainer.h"
#include <stdio.h>
#include <stdint.h>

int main()
{
  trainer_data_t ds_trainer;
  trainer_initialize(&ds_trainer);
  trainer_show_menu(&ds_trainer);

  return 0;
}
