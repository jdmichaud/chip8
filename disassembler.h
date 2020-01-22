#ifndef __DISASSEMBLER_H__
#define __DISASSEMBLER_H__

#include "file.h"

char **disassemble(uint8_t *dump, uint16_t size);
void print_dump(uint8_t *dump, uint16_t size);

char *disassembler_SYS(uint8_t lsb, uint8_t msb);
char *disassembler_CLS(uint8_t lsb, uint8_t msb);
char *disassembler_RET(uint8_t lsb, uint8_t msb);
char *disassembler_JP(uint8_t lsb, uint8_t msb);
char *disassembler_CALL(uint8_t lsb, uint8_t msb);
char *disassembler_SE(uint8_t lsb, uint8_t msb);
char *disassembler_SNE(uint8_t lsb, uint8_t msb);
char *disassembler_SE2(uint8_t lsb, uint8_t msb);
char *disassembler_LD1(uint8_t lsb, uint8_t msb);
char *disassembler_ADD(uint8_t lsb, uint8_t msb);
char *disassembler_LD2(uint8_t lsb, uint8_t msb);
char *disassembler_OR(uint8_t lsb, uint8_t msb);
char *disassembler_AND(uint8_t lsb, uint8_t msb);
char *disassembler_XOR(uint8_t lsb, uint8_t msb);
char *disassembler_ADD2(uint8_t lsb, uint8_t msb);
char *disassembler_SUB(uint8_t lsb, uint8_t msb);
char *disassembler_SHR(uint8_t lsb, uint8_t msb);
char *disassembler_SUBN(uint8_t lsb, uint8_t msb);
char *disassembler_SHL(uint8_t lsb, uint8_t msb);
char *disassembler_SNE2(uint8_t lsb, uint8_t msb);
char *disassembler_LDI(uint8_t lsb, uint8_t msb);
char *disassembler_JP2(uint8_t lsb, uint8_t msb);
char *disassembler_RND(uint8_t lsb, uint8_t msb);
char *disassembler_DRW(uint8_t lsb, uint8_t msb);
char *disassembler_SKP(uint8_t lsb, uint8_t msb);
char *disassembler_SKNP(uint8_t lsb, uint8_t msb);
char *disassembler_LD3(uint8_t lsb, uint8_t msb);
char *disassembler_LD4(uint8_t lsb, uint8_t msb);
char *disassembler_LD5(uint8_t lsb, uint8_t msb);
char *disassembler_LD6(uint8_t lsb, uint8_t msb);
char *disassembler_ADD3(uint8_t lsb, uint8_t msb);
char *disassembler_LD7(uint8_t lsb, uint8_t msb);
char *disassembler_LD8(uint8_t lsb, uint8_t msb);
char *disassembler_LD9(uint8_t lsb, uint8_t msb);
char *disassembler_LD10(uint8_t lsb, uint8_t msb);

typedef char *(*dinstruction_ptr_t)(uint8_t, uint8_t);
typedef struct {
  const char *name;
  dinstruction_ptr_t instruction;
} dinstruction_t;

static const dinstruction_t g_dinstructions[38] = {
  { "SYS",  disassembler_SYS  }, //  0 - 0nnn: Jump to a machine code routine at nnn.
  { "CLS",  disassembler_CLS  }, //  1 - 00E0: Clear the display.
  { "RET",  disassembler_RET  }, //  2 - 00EE: Return from a subroutine.
  { "JP",   disassembler_JP   }, //  3 - 1nnn: Jump to location nnn.
  { "CALL", disassembler_CALL }, //  4 - 2nnn: Call subroutine at nnn.
  { "SE",   disassembler_SE   }, //  5 - 3xkk: Skip next instruction if Vx = kk.
  { "SNE",  disassembler_SNE  }, //  6 - 4xkk: Skip next instruction if Vx != kk.
  { "SE2",  disassembler_SE2  }, //  7 - 5xy0: Skip next instruction if Vx = Vy.
  { "LD1",  disassembler_LD1  }, //  8 - 6xkk: Set Vx = kk.
  { "ADD",  disassembler_ADD  }, //  9 - 7xkk: Set Vx = Vx + kk.
  { "LD2",  disassembler_LD2  }, // 10 - 8xy0: Set Vx = Vy.
  { "OR",   disassembler_OR   }, // 11 - 8xy1: Set Vx = Vx OR Vy.
  { "AND",  disassembler_AND  }, // 12 - 8xy2: Set Vx = Vx AND Vy.
  { "XOR",  disassembler_XOR  }, // 13 - 8xy3: Set Vx = Vx XOR Vy.
  { "ADD2", disassembler_ADD2 }, // 14 - 8xy4: Set Vx = Vx + Vy, set VF = carry.
  { "SUB",  disassembler_SUB  }, // 15 - 8xy5: Set Vx = Vx - Vy, set VF = NOT borrow.
  { "SHR",  disassembler_SHR  }, // 16 - 8xy6: Set Vx = Vx SHR 1.
  { "SUBN", disassembler_SUBN }, // 17 - 8xy7: Set Vx = Vy - Vx, set VF = NOT borrow.
  { "SHL",  disassembler_SHL  }, // 18 - 8xyE: Set Vx = Vx SHL 1.
  { "SNE2", disassembler_SNE2 }, // 19 - 9xy0: Skip next instruction if Vx != Vy.
  { "LDI",  disassembler_LDI  }, // 20 - Annn: Set I = nnn.
  { "JP2",  disassembler_JP2  }, // 21 - Bnnn: Jump to location nnn + V0.
  { "RND",  disassembler_RND  }, // 22 - Cxkk: Set Vx = random byte AND kk.
  { "DRW",  disassembler_DRW  }, // 23 - Dxyn: Display n-byte sprite starting at memory location I at (Vx, Vy), set VF = collision.
  { "SKP",  disassembler_SKP  }, // 24 - Ex9E: Skip next instruction if key with the value of Vx is pressed.
  { "SKNP", disassembler_SKNP }, // 25 - ExA1: Skip next instruction if key with the value of Vx is not pressed.
  { "LD3",  disassembler_LD3  }, // 26 - Fx07: Set Vx = delay timer value.
  { "LD4",  disassembler_LD4  }, // 27 - Fx0A: Wait for a key press, store the value of the key in Vx.
  { "LD5",  disassembler_LD5  }, // 28 - Fx15: Set delay timer = Vx.
  { "LD6",  disassembler_LD6  }, // 29 - Fx18: Set sound timer = Vx.
  { "ADD3", disassembler_ADD3 }, // 30 - Fx1E: Set I = I + Vx.
  { "LD7",  disassembler_LD7  }, // 31 - Fx29: Set I = location of sprite for digit Vx.
  { "LD8",  disassembler_LD8  }, // 32 - Fx33: Store BCD representation of Vx in memory locations I, I+1, and I+2.
  { "LD9",  disassembler_LD9  }, // 33 - Fx55: Store registers V0 through Vx in memory starting at location I.
  { "LD10", disassembler_LD10 }, // 34 - Fx65: Read registers V0 through Vx from memory starting at location I.
};

#endif // !__DISASSEMBLER_H__
