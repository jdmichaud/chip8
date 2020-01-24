#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "chip8.h"

void init(chip8_t *chip8, uint8_t *screen) {
  memset(chip8, 0, sizeof(chip8_t));
  // Initialize the program counter to the program start address.
  chip8->pc = 0x200;
  // Initialize the stack pointer to the first stack addresse.
  chip8->sp = (void *) chip8->stack - (void *) chip8->memory;
  // Initialize the character map.
  memcpy(chip8->charmap, g_charmap, sizeof(g_charmap));
  // Initialize the screen
  chip8->screen = screen;
}

void load(chip8_t *chip8, uint8_t *buffer, uint16_t size) {
  memcpy(chip8->program, buffer, size);
}

void step(chip8_t *chip8) {
  // fetch next instruction opcode.
  uint8_t lsb;
  uint8_t msb;
  fetch(&chip8->memory[chip8->pc], &lsb, &msb);
  // Decode instruction.
  uint8_t opcode = 0;
  uint8_t instructionIndex = 0;
  decode(lsb, msb, &opcode, &instructionIndex);
  if (instructionIndex == -1) {
    fprintf(stderr, "Unknown instruction opcode: 0x%02x at 0x%04x\n", opcode, chip8->pc);
  }

  execute(chip8, lsb, msb, &g_instructions[instructionIndex]);
  // Move program counter.
  chip8->pc += 2;
}

void fetch(uint8_t *memory, uint8_t *lsb, uint8_t *msb) {
  *lsb = memory[0];
  *msb = memory[1];
}

void decode(uint8_t lsb, uint8_t msb, uint8_t *opcode, uint8_t *instructionIndex) {
  *instructionIndex = 0;
  *opcode = (lsb & 0xF0) >> 4;
  switch (*opcode) {
    case 0x0:
      // SYS instruction is mostly ignored now. We will handle it but after
      // testing for CLS and RET.
      if (msb == 0xE0) *instructionIndex = 1; // CLS
      if (msb == 0xEE) *instructionIndex = 2; // RET
      *instructionIndex = 0; // SYS
      break;
    case 0x1:
      *instructionIndex = 3; // JP
      break;
    case 0x2:
      *instructionIndex = 4; // CALL
      break;
    case 0x3:
      *instructionIndex = 5; // SE
      break;
    case 0x4:
      *instructionIndex = 6; // SNE
      break;
    case 0x5:
      *instructionIndex = 7; // SE2
      break;
    case 0x6:
      *instructionIndex = 8; // LD1
      break;
    case 0x7:
      *instructionIndex = 9; // ADD
      break;
    case 0x8: {
      uint8_t suffix = msb & 0x0F;
      if (suffix < 0x8) *instructionIndex = 10 + suffix;
      if (suffix == 0xE) *instructionIndex = 18; // SHL
      break;
    }
    case 0x9:
      *instructionIndex = 19; // SNE2
      break;
    case 0xA:
      *instructionIndex = 20; // LDI
      break;
    case 0xB:
      *instructionIndex = 21; // JP2
      break;
    case 0xC:
      *instructionIndex = 22; // RND
      break;
    case 0xD:
      *instructionIndex = 23; // DRW
      break;
    case 0xE:
      if (msb == 0x9E) *instructionIndex = 24; // SKP
      if (msb == 0xA1) *instructionIndex = 25; // SKNP
      break;
    case 0xF:
      switch (msb) {
        case 0x07:
          *instructionIndex = 26; // LD3
          break;
        case 0x0A:
          *instructionIndex = 27; // LD4
          break;
        case 0x15:
          *instructionIndex = 28; // LD5
          break;
        case 0x18:
          *instructionIndex = 29; // LD6
          break;
        case 0x1E:
          *instructionIndex = 30; // ADD3
          break;
        case 0x29:
          *instructionIndex = 31; // LD7
          break;
        case 0x33:
          *instructionIndex = 32; // LD8
          break;
        case 0x55:
          *instructionIndex = 33; // LD9
          break;
        case 0x65:
          *instructionIndex = 34; // LD10
          break;
      }
      break;
  }
}

void execute(chip8_t *chip, uint8_t lsb, uint8_t msb, const instruction_t *instruction) {
  instruction->instruction(chip, lsb, msb);
}

void SYS(chip8_t *chip, uint8_t lsb, uint8_t msb) {}

void CLS(chip8_t *chip, uint8_t lsb, uint8_t msb) {}

void RET(chip8_t *chip, uint8_t lsb, uint8_t msb) {}

// prio
void JP(chip8_t *chip, uint8_t lsb, uint8_t msb) {
  uint16_t nnn = ((lsb & 0x0F) << 8) | msb;
  chip->pc = nnn;
}

void CALL(chip8_t *chip, uint8_t lsb, uint8_t msb) {}

// prio
void SE(chip8_t *chip, uint8_t lsb, uint8_t msb) {
  uint8_t x = lsb & 0x0F;
  if (x == msb) chip->pc += 2;
}

void SNE(chip8_t *chip, uint8_t lsb, uint8_t msb) {}

void SE2(chip8_t *chip, uint8_t lsb, uint8_t msb) {}

void LD1(chip8_t *chip, uint8_t lsb, uint8_t msb) {
  uint8_t registerIndex = lsb & 0x0F;
  chip->v[registerIndex] = msb;
}

// prio
void ADD(chip8_t *chip, uint8_t lsb, uint8_t msb) {
  uint8_t x = lsb & 0x0F;
  chip->v[x] += msb;
}

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

// prio
void LDI(chip8_t *chip, uint8_t lsb, uint8_t msb) {
  uint16_t nnn = ((lsb & 0x0F) << 8) | msb;
  chip->i = nnn;
}

void JP2(chip8_t *chip, uint8_t lsb, uint8_t msb) {}

// prio
void RND(chip8_t *chip, uint8_t lsb, uint8_t msb) {
  uint8_t x = lsb & 0x0F;
  uint8_t randomValue = (uint8_t) rand() / ((float) (255 / RAND_MAX));
  chip->v[x] += randomValue & msb;
}

// prio
void DRW(chip8_t *chip, uint8_t lsb, uint8_t msb) {
  uint8_t x = lsb & 0x0F;
  uint8_t y = (msb & 0xF0) >> 4;
  uint8_t n = msb & 0x0F;
  fprintf(stdout, "DRAW! %u %u from 0x%04X nimble %u\n", chip->v[x], chip->v[y], chip->i, n);
  if (chip->screen != NULL) {
    printf("%04X >>> %04X\n", chip->i & 0x0FFF, x + y * 32);
    memcpy(&chip->screen[x + y * 32], &chip->memory[chip->i & 0x0FFF], n);
  }
}

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
