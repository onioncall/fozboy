// Load rom file 

#include <stdio.h>
#include <stdlib.h>
#include "rom.h"


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

void rom_destroy(rom_t* rom) {
  free(rom->data);
}

rom_t* rom_create(char* file_name) {
  rom_t* rom = malloc(sizeof(rom_t));
  if (load_rom(rom, file_name) < 0) {
    free(rom);
    return NULL;
  }

  return rom;
}
