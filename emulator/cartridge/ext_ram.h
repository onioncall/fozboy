#ifndef RAM_H
#define RAM_H

#include <stdint.h>
#include "../static/cart_type_data.h"

// Ext Ram is the cartridge ram
typedef struct {
  uint8_t** banks;
  uint8_t num_banks;
} ext_ram_t;

ext_ram_t* ext_ram_create(cart_type_enum cart_type);
void ext_ram_destroy(ext_ram_t* ram);

#endif
