// Load rom file 

#include <stdio.h>
#include <stdlib.h>
#include "rom.h"
#include "../static/cart_type_data.h"


uint8_t load_rom(rom_t* rom, char* file_name) {

  FILE* file = fopen(file_name, "rb");
  if (!file) { return -1; }

  fseek(file, 0, SEEK_END);
  rom->size = ftell(file);
  fseek(file, 0, SEEK_SET);

  rom->data = malloc(rom->size);
  fread(rom->data, sizeof(uint8_t), rom->size, file);

  fclose(file);
  return 0;
}

bool is_in_list(uint8_t code, const uint8_t codes[], uint8_t codes_len) {
  for (int i = 0; i < codes_len; i++) {
    if (codes[i] == code) {
      return true;
    }
  }
  return false;
}


// Assigns the appropriate cart_type to `rom` based on rom->data[CART_TYPE_ADDR]
// Returns -1 if no supported cart_type is found
int load_type(rom_t* rom) {
  uint8_t code = rom->data[CART_TYPE_ADDR];

  for (int i = 0; i < CART_TYPE_MAP_LEN; i++) {
    cart_type_data_item item = CART_TYPE_MAP[i];

      if (is_in_list(code, item.codes, item.codes_len)) {
        rom->cart_type = item.cart_type;
        return 0;
      }
  }

  return -1;
}

// Sets flags like is_ram and is_batt if cart type supports those features
void load_meta_type(rom_t* rom) {
  uint8_t code = rom->data[CART_TYPE_ADDR];

  if (is_in_list(code, CODES_RAM, CODES_RAM_LEN)) {
    rom->is_ram = true;
  }

  if (is_in_list(code, CODES_BATT, CODES_BATT_LEN)) {
    rom->is_batt = true;
  }

  if (is_in_list(code, CODES_TIMER, CODES_TIMER_LEN)) {
    rom->is_timer = true;
  }

  if (is_in_list(code, CODES_SENSOR, CODES_SENSOR_LEN)) {
    rom->is_sensor = true;
  }
}

void rom_destroy(rom_t* rom) {
  free(rom->data);
}

rom_t* rom_create(char* file_name) {
  rom_t* rom = malloc(sizeof(rom_t));
  if (load_rom(rom, file_name) < 0) {
    free(rom);
    return NULL;
  }

  if (load_type(rom) < 0) {
    free(rom);
    return NULL;
  }

  load_meta_type(rom);

  return rom;
}
