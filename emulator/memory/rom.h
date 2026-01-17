#ifndef LOADER_H
#define LOADER_H

#include <stdint.h>

typedef struct {
  uint8_t* data;
  long size;
} rom_t;

uint8_t load_rom(rom_t* rom, char* file_name);

#endif
