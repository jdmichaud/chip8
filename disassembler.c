#include <stdio.h>
#include <stdlib.h>

#include "disassembler.h"
#include "file.h"
#include "chip8.h"

void disassemble(file_t *file, char **result) {
  for (uint16_t i = 0; i < file->size; ++i) {
    uint8_t lsb;
    uint8_t msb;
    fetch(file->content, &lsb, &msb);

    uint8_t opcode = 0;
    const instruction_t *instruction = NULL;
    decode(lsb, msb, &opcode, &instruction);

    
  }
}
