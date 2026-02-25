// Load rom data file as virtual cartridge

#include <stdio.h>
#include <stdlib.h>
#include "cart.h"
#include "../static/cart_type_data.h"
#include "ext_ram.h"
#include "mbc.h"


uint8_t load_data(cart_t* cart, char* file_name) {

  FILE* file = fopen(file_name, "rb");
  if (!file) { return -1; }

  fseek(file, 0, SEEK_END);
  cart->size = ftell(file);
  fseek(file, 0, SEEK_SET);

  cart->data = malloc(cart->size);
  fread(cart->data, sizeof(uint8_t), cart->size, file);

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


// Assigns the appropriate cart_type to `cart` based on cart->data[CART_TYPE_ADDR]
// Returns -1 if no supported cart_type is found
int load_type(cart_t* cart) {
  uint8_t code = cart->data[CART_TYPE_ADDR];

  for (int i = 0; i < CART_TYPE_MAP_LEN; i++) {
    cart_type_data_item item = CART_TYPE_MAP[i];

      if (is_in_list(code, item.codes, item.codes_len)) {
        cart->cart_type = item.cart_type;
        return 0;
      }
  }

  return -1;
}

// Sets flags like is_ram and is_batt if cart type supports those features
void load_meta_type(cart_t* cart) {
  uint8_t code = cart->data[CART_TYPE_ADDR];

  if (is_in_list(code, CODES_RAM, CODES_RAM_LEN)) {
    cart->is_ram = true;
  }

  if (is_in_list(code, CODES_BATT, CODES_BATT_LEN)) {
    cart->is_batt = true;
  }

  if (is_in_list(code, CODES_TIMER, CODES_TIMER_LEN)) {
    cart->is_timer = true;
  }

  if (is_in_list(code, CODES_SENSOR, CODES_SENSOR_LEN)) {
    cart->is_sensor = true;
  }
}

void cart_destroy(cart_t* cart) {
  if (cart->data != NULL) {
    free(cart->data);
  }

  if (cart->mbc != NULL) {
    free(cart->mbc);
  }

  if (cart->ext_ram != NULL) {
    ext_ram_destroy(cart->ext_ram);
  }
  free(cart);
}

cart_t* cart_create(char* file_name) {
  cart_t* cart = malloc(sizeof(cart_t));
  if (load_data(cart, file_name) < 0) {
    cart_destroy(cart);
    return NULL;
  }

  if (load_type(cart) < 0) {
    cart_destroy(cart);
    return NULL;
  }

  load_meta_type(cart);

  cart->mbc = mbc_create(cart->cart_type);
  if (!cart->mbc) {
    cart_destroy(cart);
    return NULL;
  } 

  cart->ext_ram = ext_ram_create(cart->cart_type, file_name);
  if (!cart->ext_ram) {
    cart_destroy(cart);
    return NULL;
  }

  return cart;
}

uint8_t* get_ram_bank(cart_t* cart, uint8_t bank_num) {
  // "In most MBCs, if an unmapped RAM bank is selected 
  // (which would be translate to an out of bounds RAM address 
  // by the MBC controller), the MBC will simply wrap around 
  // the internal ram address and would access a valid RAM address."
  // - https://gbdev.io/pandocs/MBCs.html#mbc-unmapped-ram-bank-access
  return cart->ext_ram->banks[bank_num % cart->ext_ram->num_banks];
}
