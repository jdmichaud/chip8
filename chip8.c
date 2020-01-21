#include <stdio.h>
#include <string.h>

#include "chip8.h"

void init(chip8_t *chip8) {
  memset(chip8, 0, sizeof(chip8_t));
  // Initialize the program counter to the program start address.
  chip8->pc = 0x200;
  // Initialize the stack pointer to the first stack addresse.
  chip8->sp = (void *) chip8->stack - (void *) chip8->memory;
  // Initialize the character map.
  memcpy(chip8->charmap, g_charmap, sizeof(g_charmap));
}

void load(chip8_t *chip8, uint8_t *buffer, uint16_t size) {
  memcpy(chip8->program, buffer, size);
}

void step(chip8_t *chip8) {
  uint8_t lsb;
  uint8_t msb;
  fetch(chip8, &lsb, &msb);

  const instruction_t *instruction;
  decode(chip8, lsb, msb, &instruction);

  execute(chip8, lsb, msb, instruction);
}

void fetch(chip8_t *chip, uint8_t *lsb, uint8_t *msb) {
  *lsb = chip->memory[chip->pc++];
  *msb = chip->memory[chip->pc++];
  fprintf(stderr, "fetch lsb %02x msn %02x\n", *lsb, *msb);
}

void decode(chip8_t *chip, uint8_t lsb, uint8_t msb, const instruction_t **instruction) {
  uint8_t opcode = (lsb & 0xF0) >> 4;
  switch (opcode) {
    case 0x0:
      break;
    case 0x1:
      break;
    case 0x2:
      break;
    case 0x3:
      break;
    case 0x4:
      break;
    case 0x5:
      break;
    case 0x6:
      *instruction = &g_instructions[8];
      break;
    case 0x7:
      break;
    case 0x8:
      break;
    case 0x9:
      break;
    case 0xA:
      break;
    case 0xB:
      break;
    case 0xC:
      break;
    case 0xD:
      break;
    case 0xE:
      break;
    case 0xF:
      break;
    default:
      fprintf(stderr, "Unknown instruction opcode: 0x%02x at 0x%04x\n", opcode, chip->pc);
  }
  fprintf(stderr, "instruction: %s\n", (*instruction)->name);
}

void execute(chip8_t *chip, uint8_t lsb, uint8_t msb, const instruction_t *instruction) {
  printf("%s\n", instruction->name);
  instruction->instruction(chip, lsb, msb);
}

void SYS(chip8_t *chip, uint8_t lsb, uint8_t msb) {}

void CLS(chip8_t *chip, uint8_t lsb, uint8_t msb) {}

void RET(chip8_t *chip, uint8_t lsb, uint8_t msb) {}

void JP(chip8_t *chip, uint8_t lsb, uint8_t msb) {}

void CALL(chip8_t *chip, uint8_t lsb, uint8_t msb) {}

void SE(chip8_t *chip, uint8_t lsb, uint8_t msb) {}

void SNE(chip8_t *chip, uint8_t lsb, uint8_t msb) {}

void SE2(chip8_t *chip, uint8_t lsb, uint8_t msb) {}

void LD1(chip8_t *chip, uint8_t lsb, uint8_t msb) {
  uint8_t registerIndex = lsb & 0x0F;
  chip->v[registerIndex] = msb;
}

void ADD(chip8_t *chip, uint8_t lsb, uint8_t msb) {}

void LD2(chip8_t *chip, uint8_t lsb, uint8_t msb) {}

void OR(chip8_t *chip, uint8_t lsb, uint8_t msb) {}

void AND(chip8_t *chip, uint8_t lsb, uint8_t msb) {}

void XOR(chip8_t *chip, uint8_t lsb, uint8_t msb) {}

void ADD2(chip8_t *chip, uint8_t lsb, uint8_t msb) {}

void SUB(chip8_t *chip, uint8_t lsb, uint8_t msb) {}

void SHR(chip8_t *chip, uint8_t lsb, uint8_t msb) {}

void SUBN(chip8_t *chip, uint8_t lsb, uint8_t msb) {}

void SHL(chip8_t *chip, uint8_t lsb, uint8_t msb) {}

void SNE2(chip8_t *chip, uint8_t lsb, uint8_t msb) {}

void LDI(chip8_t *chip, uint8_t lsb, uint8_t msb) {}

void JP2(chip8_t *chip, uint8_t lsb, uint8_t msb) {}

void RND(chip8_t *chip, uint8_t lsb, uint8_t msb) {}

void DRW(chip8_t *chip, uint8_t lsb, uint8_t msb) {}

void SKP(chip8_t *chip, uint8_t lsb, uint8_t msb) {}

void SKNP(chip8_t *chip, uint8_t lsb, uint8_t msb) {}

void LD3(chip8_t *chip, uint8_t lsb, uint8_t msb) {}

void LD4(chip8_t *chip, uint8_t lsb, uint8_t msb) {}

void LD5(chip8_t *chip, uint8_t lsb, uint8_t msb) {}

void LD6(chip8_t *chip, uint8_t lsb, uint8_t msb) {}

void ADD3(chip8_t *chip, uint8_t lsb, uint8_t msb) {}

void LD7(chip8_t *chip, uint8_t lsb, uint8_t msb) {}

void LD8(chip8_t *chip, uint8_t lsb, uint8_t msb) {}

void LD9(chip8_t *chip, uint8_t lsb, uint8_t msb) {}

void LD10(chip8_t *chip, uint8_t lsb, uint8_t msb) {}

