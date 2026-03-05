#ifndef LOADER_H
#define LOADER_H

#include <stdint.h>
#include <stdbool.h>
#include "../static/cart_type_data.h"
#include "ext_ram.h"
#include "mbc.h"

#define CART_TYPE_ADDR 0x0147

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
  mbc_t* mbc;
  ext_ram_t* ext_ram;
} cart_t;

cart_t* cart_create(char* file_name);
void cart_destroy(cart_t* cart);

uint8_t* get_ram_bank(cart_t* cart, uint8_t bank_num);

#endif
