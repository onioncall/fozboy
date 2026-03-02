#include "ext_ram.h"
#include "../static/cart_type_data.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const uint16_t RAM_BANK_SIZE = 0xBFFF - 0xA000;
const uint16_t SNAPSHOT_RATE = 512;
uint16_t snapshot_counter = 0;

int get_snapshot_name(char* rom_file_name, char* buf, uint16_t buf_size) {
  const char* xdg_data_home = getenv("XDG_DATA_HOME");
  const char* home = getenv("HOME");
  const char* extension = ".sav";
  char base[4096];

  if (xdg_data_home != NULL) {
    snprintf(base, sizeof(base), "%s/fozboy/", xdg_data_home);
  } else if (home != NULL) {
    snprintf(base, sizeof(base), "%s/.local/share/fozboy/", home);
  } else {
    return -1;
  }

  if (strlen(base) + strlen(rom_file_name) + strlen(extension) >= buf_size) {
    return -1;
  }

  strcpy(buf, base);
  strcat(buf, rom_file_name);
  strcat(buf, extension);

  return 0;
}

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

  if (ram->snapshot_filename != NULL) {
    free(ram->snapshot_filename);
  }

  free(ram);
}

int load_snapshot(ext_ram_t* ext_ram) {
  if (ext_ram->snapshot_filename == NULL) {
    return 0;
  }

  FILE* fptr = fopen(ext_ram->snapshot_filename, "rb");
  if (fptr == NULL) {
    return 0; // No snapshot exists yet, not an error
  }

  for (int i = 0; i < ext_ram->num_banks; i++) {
    size_t bytes_read = fread(ext_ram->banks[i], sizeof(uint8_t), RAM_BANK_SIZE, fptr);
    if (bytes_read != RAM_BANK_SIZE) {
      fclose(fptr);
      return -1;
    }
  }

  fclose(fptr);
  return 0;
}

ext_ram_t* ext_ram_create(cart_type_enum cart_type, char* rom_file_name) {
  ext_ram_t* ext_ram = malloc(sizeof(ext_ram_t));
  if (ext_ram == NULL) {
    return NULL;
  }

  ext_ram->banks = NULL;
  ext_ram->snapshot_filename = NULL;

  ext_ram->num_banks = CART_TYPE_MAP[cart_type].ram_banks;
  ext_ram->banks = malloc(sizeof(uint8_t*) * ext_ram->num_banks);
  if (ext_ram->banks == NULL) {
    ext_ram_destroy(ext_ram);
    return NULL;
  }

  // Alloc separate banks rather than one large bank so we can easily
  // call ext_ram[bank_num]; to get the desired ram bank
  for (int i = 0; i < ext_ram->num_banks; i++) {
    // calloc for 0 initialization, as carts can freely read/write here
    ext_ram->banks[i] = calloc(1, RAM_BANK_SIZE);
    if (ext_ram->banks[i] == NULL) {
      ext_ram_destroy(ext_ram);
      return NULL;
    }
  }

  if (rom_file_name != NULL) {
    uint16_t max_path = 4096;
    char* filename = malloc(max_path);
    if (filename == NULL) {
      ext_ram_destroy(ext_ram);
      return NULL;
    }

    if (get_snapshot_name(rom_file_name, filename, max_path) != 0) {
      free(filename);
      ext_ram_destroy(ext_ram);
      return NULL;
    }

    ext_ram->snapshot_filename = filename;
    load_snapshot(ext_ram);
  }

  return ext_ram;
}

int snapshot_ram(ext_ram_t *ext_ram) {
  if (ext_ram->snapshot_filename == NULL) {
    return 0;
  }

  // overwrite file every time
  FILE* fptr = fopen(ext_ram->snapshot_filename, "wb");
  if (fptr == NULL) {
    return -1; // TODO standard errors and handling
  }

  for (int i = 0; i < ext_ram->num_banks; i++) {
    fwrite(ext_ram->banks[i],
        sizeof(uint8_t),
        RAM_BANK_SIZE,
        fptr);
  }

  fclose(fptr);
  return 0;
}

int snapshot_ram_throttled(ext_ram_t *ext_ram) {
  if (snapshot_counter++ % SNAPSHOT_RATE == 0) {
    snapshot_counter = 0;
    return load_snapshot(ext_ram);
  }
  return 0;
}
