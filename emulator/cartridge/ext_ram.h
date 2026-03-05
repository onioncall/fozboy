#ifndef RAM_H
#define RAM_H

#include <stdint.h>
#include "../static/cart_type_data.h"

// Ext Ram is the cartridge ram
typedef struct {
  uint8_t** banks;
  uint8_t num_banks;
  char* snapshot_filename;
} ext_ram_t;

ext_ram_t* ext_ram_create(cart_type_enum cart_type, char* rom_file_name);
void ext_ram_destroy(ext_ram_t *ram);
int snapshot_ram(ext_ram_t *ext_ram);
int shapshot_ram_throttled(ext_ram_t *ext_ram);
int load_snapshot(ext_ram_t *ext_ram);

#endif
