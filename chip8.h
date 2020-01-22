#ifndef __CHIP8_H__
#define __CHIP8_H__

#include <stdint.h>

// Documentatin provided by http://devernay.free.fr/hacks/chip8/C8TECH10.HTM

typedef union {
  /**
   * The Chip-8 language is capable of accessing up to 4KB (4,096 bytes) of RAM,
   * from location 0x000 (0) to 0xFFF (4095).
   */
  uint8_t     memory[0xFFF];
  struct {
    /**
     * The first 512 bytes, from 0x000 to 0x1FF, are where the original
     * interpreter was located, and should not be used by programs.
     */
    uint8_t   reserved[0x200];
    uint8_t   program[0xDFF];
  };
  /**
   * The first 135 bytes of the reserved memory space will be used for chip8
   * registers and state.
   */
  struct {
    /**
     * Programs may also refer to a group of sprites representing the
     * hexadecimal digits 0 through F. These sprites are 5 bytes long, or 8x5
     * pixels. The data should be stored in the interpreter area of Chip-8
     * memory (0x000 to 0x1FF).
     */
    uint8_t   charmap[0x10][0x5];
    /**
     * Chip-8 has 16 general purpose 8-bit registers, usually referred to as Vx,
     * where x is a hexadecimal digit (0 through F).
     */
    uint8_t   v[0xF];
    /**
     * There is also a 16-bit
     * register called I. This register is generally used to store memory
     * addresses, so only the lowest (rightmost) 12 bits are usually used.
     */
    uint16_t  i;
    /**
     * Chip-8 also has two special purpose 8-bit registers, for the delay and
     * sound timers. When these registers are non-zero, they are automatically
     * decremented at a rate of 60Hz. See the section 2.5, Timers & Sound, for
     * more information on these.
     */
    uint8_t   dt;
    uint8_t   st;
    /**
     * There are also some "pseudo-registers" which are not accessable from
     * Chip-8 programs. The program counter (PC) should be 16-bit, and is used
     * to store the currently executing address.
     */
    uint16_t  pc;
    /**
     * The stack pointer (SP) can be 8-bit, it is used to point to the topmost
     * level of the stack.
     */
    uint16_t  sp;
    /**
     * The stack is an array of 16 16-bit values, used to store the address that
     * the interpreter shoud return to when finished with a subroutine. Chip-8
     * allows for up to 16 levels of nested subroutines.
     */
    uint16_t  stack[0xF];
  };
} chip8_t;

static const uint8_t g_charmap[0x10][0x5] = {
  { 0xF0, 0x90, 0x90, 0x90, 0xF0 }, // 0
  { 0x20, 0x60, 0x20, 0x20, 0x70 }, // 1
  { 0xF0, 0x10, 0xF0, 0x80, 0xF0 }, // 2
  { 0xF0, 0x10, 0xF0, 0x10, 0xF0 }, // 3
  { 0x90, 0x90, 0xF0, 0x10, 0x10 }, // 4
  { 0xF0, 0x80, 0xF0, 0x10, 0xF0 }, // 5
  { 0xF0, 0x80, 0xF0, 0x90, 0xF0 }, // 6
  { 0xF0, 0x10, 0x20, 0x40, 0x40 }, // 7
  { 0xF0, 0x90, 0xF0, 0x90, 0xF0 }, // 8
  { 0xF0, 0x90, 0xF0, 0x10, 0xF0 }, // 9
  { 0xF0, 0x90, 0xF0, 0x90, 0x90 }, // A
  { 0xE0, 0x90, 0xE0, 0x90, 0xE0 }, // B
  { 0xF0, 0x80, 0x80, 0x80, 0xF0 }, // C
  { 0xE0, 0x90, 0x90, 0x90, 0xE0 }, // D
  { 0xF0, 0x80, 0xF0, 0x80, 0xF0 }, // E
  { 0xF0, 0x80, 0xF0, 0x80, 0x80 }  // F
};

typedef void (*instruction_ptr_t)(chip8_t *, uint8_t, uint8_t);
typedef struct {
  const char *name;
  instruction_ptr_t instruction;
} instruction_t;

/**
 * Initializes the chip8 cpu.
 * Set the PC to the beginning of the program, the stack pointer to the top of
 * the stack and the character map.
 */
void init(chip8_t *chip8);
/**
 * Loads into program memory (starting at 0x200) the provided buffer.
 */
void load(chip8_t *chip8, uint8_t *buffer, uint16_t size);
/**
 * Step the chip.
 */
void step(chip8_t *chip8);

/**
 * Fetch the instruction from the program memory.
 */
void fetch(uint8_t *memory, uint8_t *lsb, uint8_t *msb);
/**
 * Decode the instruction.
 */
void decode(uint8_t lsb, uint8_t msb, const instruction_t *instructions,
  uint8_t *opcode, const instruction_t **instruction);
/**
 * Execute the instruction.
 */
void execute(chip8_t *chip, uint8_t lsb, uint8_t msb, const instruction_t *instruction);

// Instructions
void SYS(chip8_t *chip, uint8_t lsb, uint8_t msb);
void CLS(chip8_t *chip, uint8_t lsb, uint8_t msb);
void RET(chip8_t *chip, uint8_t lsb, uint8_t msb);
void JP(chip8_t *chip, uint8_t lsb, uint8_t msb);
void CALL(chip8_t *chip, uint8_t lsb, uint8_t msb);
void SE(chip8_t *chip, uint8_t lsb, uint8_t msb);
void SNE(chip8_t *chip, uint8_t lsb, uint8_t msb);
void SE2(chip8_t *chip, uint8_t lsb, uint8_t msb);
void LD1(chip8_t *chip, uint8_t lsb, uint8_t msb);
void ADD(chip8_t *chip, uint8_t lsb, uint8_t msb);
void LD2(chip8_t *chip, uint8_t lsb, uint8_t msb);
void OR(chip8_t *chip, uint8_t lsb, uint8_t msb);
void AND(chip8_t *chip, uint8_t lsb, uint8_t msb);
void XOR(chip8_t *chip, uint8_t lsb, uint8_t msb);
void ADD2(chip8_t *chip, uint8_t lsb, uint8_t msb);
void SUB(chip8_t *chip, uint8_t lsb, uint8_t msb);
void SHR(chip8_t *chip, uint8_t lsb, uint8_t msb);
void SUBN(chip8_t *chip, uint8_t lsb, uint8_t msb);
void SHL(chip8_t *chip, uint8_t lsb, uint8_t msb);
void SNE2(chip8_t *chip, uint8_t lsb, uint8_t msb);
void LDI(chip8_t *chip, uint8_t lsb, uint8_t msb);
void JP2(chip8_t *chip, uint8_t lsb, uint8_t msb);
void RND(chip8_t *chip, uint8_t lsb, uint8_t msb);
void DRW(chip8_t *chip, uint8_t lsb, uint8_t msb);
void SKP(chip8_t *chip, uint8_t lsb, uint8_t msb);
void SKNP(chip8_t *chip, uint8_t lsb, uint8_t msb);
void LD3(chip8_t *chip, uint8_t lsb, uint8_t msb);
void LD4(chip8_t *chip, uint8_t lsb, uint8_t msb);
void LD5(chip8_t *chip, uint8_t lsb, uint8_t msb);
void LD6(chip8_t *chip, uint8_t lsb, uint8_t msb);
void ADD3(chip8_t *chip, uint8_t lsb, uint8_t msb);
void LD7(chip8_t *chip, uint8_t lsb, uint8_t msb);
void LD8(chip8_t *chip, uint8_t lsb, uint8_t msb);
void LD9(chip8_t *chip, uint8_t lsb, uint8_t msb);
void LD10(chip8_t *chip, uint8_t lsb, uint8_t msb);

static const instruction_t g_instructions[38] = {
  { "SYS",  SYS  }, //  0 - 0nnn: Jump to a machine code routine at nnn.
  { "CLS",  CLS  }, //  1 - 00E0: Clear the display.
  { "RET",  RET  }, //  2 - 00EE: Return from a subroutine.
  { "JP",   JP   }, //  3 - 1nnn: Jump to location nnn.
  { "CALL", CALL }, //  4 - 2nnn: Call subroutine at nnn.
  { "SE",   SE   }, //  5 - 3xkk: Skip next instruction if Vx = kk.
  { "SNE",  SNE  }, //  6 - 4xkk: Skip next instruction if Vx != kk.
  { "SE2",  SE2  }, //  7 - 5xy0: Skip next instruction if Vx = Vy.
  { "LD1",  LD1  }, //  8 - 6xkk: Set Vx = kk.
  { "ADD",  ADD  }, //  9 - 7xkk: Set Vx = Vx + kk.
  { "LD2",  LD2  }, // 10 - 8xy0: Set Vx = Vy.
  { "OR",   OR   }, // 11 - 8xy1: Set Vx = Vx OR Vy.
  { "AND",  AND  }, // 12 - 8xy2: Set Vx = Vx AND Vy.
  { "XOR",  XOR  }, // 13 - 8xy3: Set Vx = Vx XOR Vy.
  { "ADD2", ADD2 }, // 14 - 8xy4: Set Vx = Vx + Vy, set VF = carry.
  { "SUB",  SUB  }, // 15 - 8xy5: Set Vx = Vx - Vy, set VF = NOT borrow.
  { "SHR",  SHR  }, // 16 - 8xy6: Set Vx = Vx SHR 1.
  { "SUBN", SUBN }, // 17 - 8xy7: Set Vx = Vy - Vx, set VF = NOT borrow.
  { "SHL",  SHL  }, // 18 - 8xyE: Set Vx = Vx SHL 1.
  { "SNE2", SNE2 }, // 19 - 9xy0: Skip next instruction if Vx != Vy.
  { "LDI",  LDI  }, // 20 - Annn: Set I = nnn.
  { "JP2",  JP2  }, // 21 - Bnnn: Jump to location nnn + V0.
  { "RND",  RND  }, // 22 - Cxkk: Set Vx = random byte AND kk.
  { "DRW",  DRW  }, // 23 - Dxyn: Display n-byte sprite starting at memory location I at (Vx, Vy), set VF = collision.
  { "SKP",  SKP  }, // 24 - Ex9E: Skip next instruction if key with the value of Vx is pressed.
  { "SKNP", SKNP }, // 25 - ExA1: Skip next instruction if key with the value of Vx is not pressed.
  { "LD3",  LD3  }, // 26 - Fx07: Set Vx = delay timer value.
  { "LD4",  LD4  }, // 27 - Fx0A: Wait for a key press, store the value of the key in Vx.
  { "LD5",  LD5  }, // 28 - Fx15: Set delay timer = Vx.
  { "LD6",  LD6  }, // 29 - Fx18: Set sound timer = Vx.
  { "ADD3", ADD3 }, // 30 - Fx1E: Set I = I + Vx.
  { "LD7",  LD7  }, // 31 - Fx29: Set I = location of sprite for digit Vx.
  { "LD8",  LD8  }, // 32 - Fx33: Store BCD representation of Vx in memory locations I, I+1, and I+2.
  { "LD9",  LD9  }, // 33 - Fx55: Store registers V0 through Vx in memory starting at location I.
  { "LD10", LD10 }, // 34 - Fx65: Read registers V0 through Vx from memory starting at location I.
};

#endif // !__CHIP8_H__
