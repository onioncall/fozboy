#include "ext_ram.h"
#include "../static/cart_type_data.h"
#include <stdlib.h>

uint16_t RAM_BANK_SIZE = 0xBFFF - 0xA000;

void ext_ram_destroy(ext_ram_t *ram) {
  if (ram->banks != NULL) {

    for (int i = 0; i < ram->num_banks; i++) {
      if (ram->banks[i] == NULL) {
        break;
      } 
      else {
        free(ram->banks[i]);
      }
    }

    free(ram->banks);
  }
  free(ram);
}


ext_ram_t* ext_ram_create(cart_type_enum cart_type) {
  ext_ram_t* ext_ram = malloc(sizeof(ext_ram_t));
  if (ext_ram == NULL) {
    return NULL;
  }

  ext_ram->num_banks = CART_TYPE_MAP[cart_type].ram_banks;
  ext_ram->banks = malloc(sizeof(uint8_t) * ext_ram->num_banks);
  if (ext_ram->banks == NULL) {
    ext_ram_destroy(ext_ram);
    return NULL;
  }

  // Alloc separate banks rather than one large bank so we can easily 
  // call ext_ram[bank_num]; to get the desired ram bank
  for (int i = 0; i < ext_ram->num_banks; i++) {
    // calloc for 0 initialization, as carts can freely read/write here
    // TODO - initialize from stored cartridge state
    ext_ram->banks[i] = calloc(1, RAM_BANK_SIZE);
    if (ext_ram->banks[i] == NULL) {
      ext_ram_destroy(ext_ram);
    }
  }

  return ext_ram;
}
