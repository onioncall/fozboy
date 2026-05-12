#ifndef HANDLERS_H
#define HANDLERS_H

#include "core.h"
#include "../static/instructions_meta.h"

typedef enum {
  ERR_LD_OK = 0,
  ERR_LD_ARG1_TYPE,
  ERR_LD_ARG1_VAL,
  ERR_LD_ARG2_TYPE,
  ERR_LD_ARG2_VAL,
} ERR_LD;

typedef enum {
  ERR_INC_OK = 0
} ERR_INC;

ERR_LD handle_load(core_t *core, instruction_meta_t *meta, uint8_t opcode, uint8_t opdata[]);
ERR_INC handle_inc(core_t *core, instruction_meta_t *meta);

#endif
