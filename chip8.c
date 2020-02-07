#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>

#include "chip8.h"

void init(chip8_t *chip8, uint8_t *screen) {
  memset(chip8, 0, sizeof(chip8_t));
  // Initialize the program counter to the program start address.
  chip8->pc = 0x200;
  // Initialize the character map.
  memcpy(chip8->charmap, g_charmap, sizeof(g_charmap));
  // Initialize the screen
  chip8->screen = screen;
  // Initialize timer
  struct timeval start;
  gettimeofday(&start, NULL);
  chip8->timer = (start.tv_sec * 1000000 + start.tv_usec) / 1000;
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

  // Move program counter.
  chip8->pc += 2;
  execute(chip8, lsb, msb, &g_instructions[instructionIndex]);
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
      *instructionIndex = 0; // SYS
      if (msb == 0xE0) *instructionIndex = 1; // CLS
      if (msb == 0xEE) *instructionIndex = 2; // RET
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
  // Manager timer and DT/ST register
  struct timeval start;
  gettimeofday(&start, NULL);
  uint32_t now = start.tv_sec * 1000000 + start.tv_usec / 1000;
  if (now - chip->timer > 16) {
    if (chip->dt) chip->dt--;
    if (chip->st) chip->st--;
    chip->timer = now;
  }
}

void SYS(chip8_t *chip, uint8_t lsb, uint8_t msb) {}

void CLS(chip8_t *chip, uint8_t lsb, uint8_t msb) {
  memset(chip->screen, 0, 64 * 32 * sizeof (uint8_t));
}

void RET(chip8_t *chip, uint8_t lsb, uint8_t msb) {
  // chip->pc = chip->stack[chip->sp];
  // chip->sp -= chip->sp > 0 ? 1 : 0;
  chip->sp -= chip->sp > 0 ? 1 : 0;
  chip->pc = chip->stack[chip->sp];
}

void JP(chip8_t *chip, uint8_t lsb, uint8_t msb) {
  uint16_t nnn = ((lsb & 0x0F) << 8) | msb;
  if (chip->pc == nnn + 2) getchar(); // If we jump to the same address, we're noping.
  chip->pc = nnn;
}

void CALL(chip8_t *chip, uint8_t lsb, uint8_t msb) {
  // uint16_t nnn = ((lsb & 0x0F) << 8) | msb;
  // chip->sp += 1;
  // chip->stack[chip->sp] = chip->pc;
  // chip->pc = nnn;
  uint16_t nnn = ((lsb & 0x0F) << 8) | msb;
  chip->stack[chip->sp] = chip->pc;
  chip->sp += 1;
  chip->pc = nnn;
}

void SE(chip8_t *chip, uint8_t lsb, uint8_t msb) {
  uint8_t x = lsb & 0x0F;
  if (chip->v[x] == msb) chip->pc += 2;
}

void SNE(chip8_t *chip, uint8_t lsb, uint8_t msb) {
  uint8_t x = lsb & 0x0F;
  if (chip->v[x] != msb) chip->pc += 2;
}

void SE2(chip8_t *chip, uint8_t lsb, uint8_t msb) {
  uint8_t x = lsb & 0x0F;
  uint8_t y = (msb & 0xF0) >> 4;
  if (chip->v[x] == chip->v[y]) chip->pc += 2;
}

void LD1(chip8_t *chip, uint8_t lsb, uint8_t msb) {
  uint8_t x = lsb & 0x0F;
  chip->v[x] = msb;
}

void ADD(chip8_t *chip, uint8_t lsb, uint8_t msb) {
  uint8_t x = lsb & 0x0F;
  chip->v[x] += msb;
}

void LD2(chip8_t *chip, uint8_t lsb, uint8_t msb) {
  uint8_t x = lsb & 0x0F;
  uint8_t y = (msb & 0xF0) >> 4;
  chip->v[x] = chip->v[y];
}

void OR(chip8_t *chip, uint8_t lsb, uint8_t msb) {
  uint8_t x = lsb & 0x0F;
  uint8_t y = (msb & 0xF0) >> 4;
  chip->v[x] |= chip->v[y];
}

void AND(chip8_t *chip, uint8_t lsb, uint8_t msb) {
  uint8_t x = lsb & 0x0F;
  uint8_t y = (msb & 0xF0) >> 4;
  chip->v[x] &= chip->v[y];
}

void XOR(chip8_t *chip, uint8_t lsb, uint8_t msb) {
  uint8_t x = lsb & 0x0F;
  uint8_t y = (msb & 0xF0) >> 4;
  chip->v[x] ^= chip->v[y];
}

void ADD2(chip8_t *chip, uint8_t lsb, uint8_t msb) {
  uint8_t x = lsb & 0x0F;
  uint8_t y = (msb & 0xF0) >> 4;
  chip->v[0xF] = ((uint16_t) chip->v[x] + (uint16_t) chip->v[y] > 255) ? 1 : 0;
  chip->v[x] += chip->v[y];
}

void SUB(chip8_t *chip, uint8_t lsb, uint8_t msb) {
  uint8_t x = lsb & 0x0F;
  uint8_t y = (msb & 0xF0) >> 4;
  chip->v[0xF] = (chip->v[x] > chip->v[y]) ? 1 : 0;
  chip->v[x] -= chip->v[y];
}

void SHR(chip8_t *chip, uint8_t lsb, uint8_t msb) {
  uint8_t x = lsb & 0x0F;
  chip->v[0xF] = chip->v[x] & 0x01;
  chip->v[x] >>= 1;
}

void SUBN(chip8_t *chip, uint8_t lsb, uint8_t msb) {
  uint8_t x = lsb & 0x0F;
  uint8_t y = (msb & 0xF0) >> 4;
  chip->v[0xF] = (chip->v[y] > chip->v[x]) ? 1 : 0;
  chip->v[y] -= chip->v[x];
}

void SHL(chip8_t *chip, uint8_t lsb, uint8_t msb) {
  uint8_t x = lsb & 0x0F;
  chip->v[0xF] = chip->v[x] & 0x80 >> 7;
  chip->v[x] >>= 1;
}

void SNE2(chip8_t *chip, uint8_t lsb, uint8_t msb) {
  uint8_t x = lsb & 0x0F;
  uint8_t y = (msb & 0xF0) >> 4;
  if (chip->v[x] != chip->v[y]) chip->pc += 2;
}

void LDI(chip8_t *chip, uint8_t lsb, uint8_t msb) {
  uint16_t nnn = ((lsb & 0x0F) << 8) | msb;
  chip->i = nnn;
}

void JP2(chip8_t *chip, uint8_t lsb, uint8_t msb) {
  uint16_t nnn = ((lsb & 0x0F) << 8) | msb;
  chip->pc = nnn + chip->v[0];
}

void RND(chip8_t *chip, uint8_t lsb, uint8_t msb) {
  uint8_t x = lsb & 0x0F;
  uint8_t randomValue = (uint8_t) ((float) rand() / (float) RAND_MAX * 255);
  chip->v[x] = randomValue & msb;
}

// To test setSpriteLine in C playground
// int main() {
//   uint8_t buffer[] = { 0x08, 0x0, 0x0, 0x0 };
//   setSpriteLine(buffer, 0, 0x80);
//   for (uint8_t i = 0; i < 4; ++i) {
//     fprintf(stdout, "0x%02X ", buffer[i]);
//   }
//   fprintf(stdout, "\n");
//   return 0;
// }

uint8_t setSpriteLine(uint8_t *screen, uint16_t bitPosition, uint8_t line) {
  // Retrieve the first byte position and the bit offset in that first byte.
  uint16_t bytePosition = bitPosition >> 3;
  uint16_t bitOffset = bitPosition & 0x0007;
  // Retrieve the fully formed byte from either the first (if position is a
  // multiple of 8 ot of the two consecutive bytes).
  uint8_t screenByte = screen[bytePosition];
  if (bitOffset) {
    screenByte <<= bitOffset;
    if (bytePosition < (64 * 32 / 8) - 1) {
      uint8_t secondByte = screen[bytePosition + 1];
      screenByte = screenByte | (secondByte >> (8 - bitOffset));
    }
  }
  // XOR the screen byte with the sprite line.
  uint8_t result = screenByte ^ line;
  // Check if a pixel was turned off
  uint8_t collision = (result | screenByte) > result ? 1 : 0;
  // Write the resulting byte into the screen memory
  if (bitOffset != 0) {
    // Example:
    //    XXXX XXXX >> 3
    // -> 000X XXXX
    //    1000 0000 >> (3 - 1)
    // -> 0010 0000 - 1
    // -> 0001 1111 ~
    // -> 1110 0000
    //    ssss ssss & 1110 0000 (1)
    // -> sss0 0000 | 000X XXXX
    // -> sssX XXXX
    uint8_t tmp = screen[bytePosition] & ((uint8_t) ~((0x80 >> (bitOffset - 1)) - 1));
    screen[bytePosition] = (result >> bitOffset) | tmp;
    // Example:
    //    XXXX XXXX << 3
    // -> XXXX X000
    //    0000 0001 << 3
    // -> 0000 1000 - 1
    // -> 0000 0111
    //    ssss ssss & 0000 0111 (1)
    // -> 0000 0sss | XXXX X000
    // -> XXXX Xsss
    if (bytePosition < (64 * 32 / 8) - 1) {
      tmp = screen[bytePosition + 1] & ((1 << (8 - bitOffset)) - 1); // (1)
      screen[bytePosition + 1] = (result << (8 - bitOffset)) | tmp;
    }
  } else {
    screen[bytePosition] = result;
  }
  return collision;
}


void DRW(chip8_t *chip, uint8_t lsb, uint8_t msb) {
  uint8_t x = lsb & 0x0F;
  uint8_t y = (msb & 0xF0) >> 4;
  uint8_t n = msb & 0x0F;
  // fprintf(stdout, "DRAW! %u %u from 0x%04X nimble %u\n", chip->v[x], chip->v[y], chip->i, n);
  if (chip->screen != NULL) {
    chip->v[0xF] = 0;
    for (uint16_t i = 0; i < n; ++i) {
      uint16_t bitPosition = chip->v[x] + (chip->v[y] + i) * 64;
      chip->v[0xF] = setSpriteLine(chip->screen, bitPosition, chip->memory[(chip->i & 0x0FFF) + i]);
    }
  }
}

void SKP(chip8_t *chip, uint8_t lsb, uint8_t msb) {
  uint8_t x = lsb & 0x0F;
  if (chip->keyboard[chip->v[x]] != 0) chip->pc += 2;
}

void SKNP(chip8_t *chip, uint8_t lsb, uint8_t msb) {
  uint8_t x = lsb & 0x0F;
  if (chip->keyboard[chip->v[x]] == 0) chip->pc += 2;
}

void LD3(chip8_t *chip, uint8_t lsb, uint8_t msb) {
  uint8_t x = lsb & 0x0F;
  chip->v[x] = chip->dt;
}

void LD4(chip8_t *chip, uint8_t lsb, uint8_t msb) {
  uint8_t x = lsb & 0x0F;
  uint8_t key = 0x10;
  // As long as no key is pressed
  while (key == 0x10) {
    // Scan the keyboard
    for (uint8_t i = 0; i < 0x10; ++i) {
      if (chip->keyboard[i] != 0) {
        key = i;
        break;
      }
    }
    if (key >= 0 && key < 0x10) { // between 0 and F
      chip->v[x] = key;
    }
    nanosleep((const struct timespec[]){{0, 16000000L}}, NULL); // wait for 16ms.
  }
}

void LD5(chip8_t *chip, uint8_t lsb, uint8_t msb) {
  uint8_t x = lsb & 0x0F;
  chip->dt = chip->v[x];
}

void LD6(chip8_t *chip, uint8_t lsb, uint8_t msb) {
  uint8_t x = lsb & 0x0F;
  chip->st  = chip->v[x];
}

void ADD3(chip8_t *chip, uint8_t lsb, uint8_t msb) {
  uint8_t x = lsb & 0x0F;
  chip->i += chip->v[x];
}

void LD7(chip8_t *chip, uint8_t lsb, uint8_t msb) {
  uint8_t x = lsb & 0x0F;
  chip->i = (uint16_t) ((uint8_t *) chip->charmap - chip->memory + chip->v[x]);
  printf("LD7 Vx: 0x%02X I: 0x%04X\n", chip->v[x], chip->i);
}

void LD8(chip8_t *chip, uint8_t lsb, uint8_t msb) {
  uint8_t x = lsb & 0x0F;
  chip->memory[chip->i    ] = chip->v[x] / 100 % 10;
  chip->memory[chip->i + 1] = chip->v[x] / 10 % 10;
  chip->memory[chip->i + 2] = chip->v[x] % 10;
}

void LD9(chip8_t *chip, uint8_t lsb, uint8_t msb) {
  uint8_t x = lsb & 0x0F;
  for (uint8_t i = 0; i <= x; ++i) {
    chip->memory[chip->i + i] = chip->v[i];
  }
}

void LD10(chip8_t *chip, uint8_t lsb, uint8_t msb) {
  uint8_t x = lsb & 0x0F;
  for (uint8_t i = 0; i <= x; ++i) {
    printf("0x%02X -> %i - 0x%02X\n", chip->i + i, i, chip->memory[chip->i + i]);
    chip->v[i] = chip->memory[chip->i + i];
  }
}
