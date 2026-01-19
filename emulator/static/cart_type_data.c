// Static data relevant to cartridge types

#include "cart_type_data.h"
#include <stdint.h>

uint8_t CODES_ROM[] = { 0x00, 0x08, 0x09 };
uint8_t CODES_MBC1[] = { 0x01, 0x02, 0x03 };
uint8_t CODES_MBC2[] = { 0x05, 0x06 };
uint8_t CODES_MMM01[] = { 0x0C, 0x0D };
uint8_t CODES_MBC3[] = { 0x0F, 0x10, 0x11, 0x12, 0x13 };
uint8_t CODES_MBC5[] = { 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E };
uint8_t CODES_MBC6[] = { 0x20 };
uint8_t CODES_MBC7[] = { 0x22 };
uint8_t CODES_POCKETCAM[] = { 0xFC };
uint8_t CODES_TAMA5[] = { 0xFD };
uint8_t CODES_HUC3[] = { 0xFE };
uint8_t CODES_HUC1[] = { 0xFF };

const uint8_t CODES_RAM[15] = { 0x02, 0x03, 0x08, 0x09, 0x0C, 0x0D, 0x10, 0x12, 0x13, 0x1A,0x1B, 0x1D, 0x1E, 0x22, 0xFF };
const uint8_t CODES_BATT[11] = { 0x03, 0x06, 0x09, 0x0D, 0x0F, 0x10, 0x13, 0x1B, 0x1E, 0x22, 0xFF };
const uint8_t CODES_TIMER[2] = { 0x0F, 0x10 };
const uint8_t CODES_RUMBLE[4] = { 0x1C, 0x1D, 0x1E, 0x22 };
const uint8_t CODES_SENSOR[1] = { 0x22 };

const cart_type_data_item CART_TYPE_MAP[] = {
  {
    .cart_type = ROM,
    .codes = CODES_ROM,
    .codes_len = sizeof(CODES_ROM) / sizeof(CODES_ROM[0])
  },
  {
    .cart_type = MBC1,
    .codes = CODES_MBC1,
    .codes_len = sizeof(CODES_MBC1) / sizeof(CODES_MBC1[0])
  },
  {
    .cart_type = MBC2,
    .codes = CODES_MBC2,
    .codes_len = sizeof(CODES_MBC2) / sizeof(CODES_MBC2[0])
  },
  {
    .cart_type = MMM01,
    .codes = CODES_MMM01,
    .codes_len = sizeof(CODES_MMM01) / sizeof(CODES_MMM01[0])
  },
  {
    .cart_type = MBC3,
    .codes = CODES_MBC3,
    .codes_len = sizeof(CODES_MBC3) / sizeof(CODES_MBC3[0])
  },
  { 
    .cart_type = MBC5,
    .codes = CODES_MBC5,
    .codes_len = sizeof(CODES_MBC5) / sizeof(CODES_MBC5[0])
  },
  {
    .cart_type = MBC6,
    .codes = CODES_MBC6,
    .codes_len = sizeof(CODES_MBC6) / sizeof(CODES_MBC6[0])
  },
  {
    .cart_type = MBC7,
    .codes = CODES_MBC7,
    .codes_len = sizeof(CODES_MBC7) / sizeof(CODES_MBC7[0])
  },
  {
    .cart_type = POCKET_CAM,
    .codes = CODES_POCKETCAM,
    .codes_len = sizeof(CODES_POCKETCAM) / sizeof(CODES_POCKETCAM[0])
  },
  {
    .cart_type = BANDAI_TAMA5,
    .codes = CODES_TAMA5, 
    .codes_len = 1
  },
  {
    .cart_type = HUC3,
    .codes = CODES_HUC3,
    .codes_len = sizeof(CODES_HUC3) / sizeof(CODES_HUC3[0])
  },
  {
    .cart_type = HUC1,
    .codes = CODES_HUC1,
    .codes_len = sizeof(CODES_HUC1) / sizeof(CODES_HUC1[0])
  }
};

