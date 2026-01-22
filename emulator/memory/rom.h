#ifndef LOADER_H
#define LOADER_H

#include <stdint.h>
#include <stdbool.h>
#include "../static/cart_type_data.h"

const uint16_t CART_TYPE_ADDR = 0x0147;

typedef struct {
  uint8_t* data;
  long size;
  cart_type_enum cart_type;
  char* program_title;
  bool is_ram;
  bool is_batt;
  bool is_timer;
  bool is_rumble;
  bool is_sensor;
} rom_t;

rom_t* rom_create(char* file_name);
void rom_destroy(rom_t* rom);

#endif
