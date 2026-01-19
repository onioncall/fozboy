#ifndef CART_TYPE_DATA_H
#define CART_TYPE_DATA_H

#include <stdint.h>

typedef enum {
  ROM = 0,
  MBC1,
  MBC2,
  MMM01,
  MBC3,
  MBC5,
  MBC6,
  MBC7,
  POCKET_CAM,
  BANDAI_TAMA5,
  HUC3,
  HUC1
} cart_type_enum;

typedef struct {
  cart_type_enum cart_type;
  uint8_t* codes;
  uint8_t codes_len;
} cart_type_data_item;

const uint8_t CART_TYPE_MAP_LEN = 12;
extern const cart_type_data_item CART_TYPE_MAP[CART_TYPE_MAP_LEN];

const uint8_t CODES_RAM_LEN = 15;
extern const uint8_t CODES_RAM[CODES_RAM_LEN];
const uint8_t CODES_BATT_LEN = 11;
extern const uint8_t CODES_BATT[CODES_BATT_LEN];
const uint8_t CODES_TIMER_LEN = 2;
extern const uint8_t CODES_TIMER[CODES_TIMER_LEN];
const uint8_t CODES_RUMBLE_LEN = 4;
extern const uint8_t CODES_RUMBLE[CODES_RUMBLE_LEN];
const uint8_t CODES_SENSOR_LEN = 1;
extern const uint8_t CODES_SENSOR[CODES_SENSOR_LEN];

#endif
