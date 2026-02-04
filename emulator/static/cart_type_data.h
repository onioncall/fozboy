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

extern const uint8_t CART_TYPE_MAP_LEN;
extern const cart_type_data_item CART_TYPE_MAP[];

extern const uint8_t CODES_RAM_LEN;
extern const uint8_t CODES_RAM[];
extern const uint8_t CODES_BATT_LEN;
extern const uint8_t CODES_BATT[];
extern const uint8_t CODES_TIMER_LEN;
extern const uint8_t CODES_TIMER[];
extern const uint8_t CODES_RUMBLE_LEN;
extern const uint8_t CODES_RUMBLE[];
extern const uint8_t CODES_SENSOR_LEN;
extern const uint8_t CODES_SENSOR[];

#endif
