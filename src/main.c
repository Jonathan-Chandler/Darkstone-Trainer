#include "trainer.h"
#include <stdio.h>
#include <stdint.h>

int main()
{
  trainer_data_t ds_trainer;
  trainer_initialize(&ds_trainer);
  trainer_show_menu(&ds_trainer);
  trainer_destroy(&ds_trainer);

  //trainer_get_inventory(&ds_trainer);
//  trainer_get_equip_stats(&ds_trainer);
//  trainer_get_inventory_loc(&ds_trainer, 0, 7);
//  trainer_inventory_delete_item_name(&ds_trainer, "ITEM_FOOD2");
  //trainer_get_map(&ds_trainer);
//  trainer_show_inventory(&ds_trainer);

  return 0;
}
