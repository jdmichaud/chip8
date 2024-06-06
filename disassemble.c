#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "disassemble.h"
#include "file.h"
#include "chip8.h"

// 256 is enough for everyone
#define BUFFER_SIZE 256

typedef struct line_list_s {
  char *line;
  struct line_list_s *next;
} line_list_t;

char *disassemble(uint8_t *dump, uint16_t size, char *result) {
  memset(result, 0, (size + 1) * sizeof (char *));
  uint8_t *content = dump;
  for (uint16_t i = 0; i < size; i += 2, content += 2) {
    uint8_t lsb;
    uint8_t msb;
    fetch(content, &lsb, &msb);

    uint8_t opcode = 0;
    uint8_t instructionIndex;
    decode(lsb, msb, &opcode, &instructionIndex);
    // Append new line
    g_dinstructions[instructionIndex].instruction(lsb, msb, &result[(i >> 1) * 256]);
  }
  return result;
}

void print_dump(uint8_t *dump, uint16_t size, uint16_t offset) {
  assert(size < BUFFER_SIZE);
  char result[BUFFER_SIZE * BUFFER_SIZE];
  disassemble(dump, size, result);
  for (uint16_t i = 0; i < size; ++i) {
    fprintf(stdout, "(0x%04X) 0x%02X%02X %s\n", offset + (i << 1), dump[i << 1], dump[(i << 1) + 1],
      &result[i * BUFFER_SIZE]);
  }
}

char *registerOperation(char *name, uint8_t lsb, uint8_t msb, char *line) {
  const uint8_t size = strnlen(name, 4) + 1 + sizeof("Vx, Vy") + 1;
  assert(BUFFER_SIZE > size);
  // char *line = malloc(size * sizeof (char));
  memset(line, 0, size * sizeof (char));
  const uint8_t x = lsb & 0x0F;
  const uint8_t y = (msb & 0xF0) >> 4;
  sprintf(line, "%s V%X, V%X", name, x, y);
  return line;
}

char *disassembler_SYS(uint8_t lsb, uint8_t msb, char *line) {
  assert(BUFFER_SIZE > (sizeof("SYS 0xkkk")));
  uint8_t k = lsb & 0x0F;
  sprintf(line, "SYS 0x%X", (k << 8) | msb);
  return line;
}

char *disassembler_CLS(uint8_t lsb, uint8_t msb, char *line) {
  assert(BUFFER_SIZE > (sizeof("CLS")));
  sprintf(line, "CLS");
  return line;
}

char *disassembler_RET(uint8_t lsb, uint8_t msb, char *line) {
  assert(BUFFER_SIZE > (sizeof("RET")));
  sprintf(line, "RET");
  return line;
}

char *disassembler_JP(uint8_t lsb, uint8_t msb, char *line) {
  assert(BUFFER_SIZE > (sizeof("JP 0x0nnn")));
  uint8_t n = lsb & 0x0F;
  sprintf(line, "JP 0x%04X", (n << 8) | msb);
  return line;
}

char *disassembler_CALL(uint8_t lsb, uint8_t msb, char *line) {
  assert(BUFFER_SIZE > (sizeof("CALL 0x0nnn")));
  uint8_t n = lsb & 0x0F;
  sprintf(line, "CALL 0x%04X", (n << 8) | msb);
  return line;
}

char *disassembler_SE(uint8_t lsb, uint8_t msb, char *line) {
  assert(BUFFER_SIZE > (sizeof("SE Vx, 0xkk")));
  uint8_t x = lsb & 0x0F;
  sprintf(line, "SE V%X, 0x%02X", x, msb);
  return line;
}

char *disassembler_SNE(uint8_t lsb, uint8_t msb, char *line) {
  assert(BUFFER_SIZE > (sizeof("SNE Vx, 0xkk")));
  uint8_t x = lsb & 0x0F;
  sprintf(line, "SNE V%X, 0x%02X", x, msb);
  return line;
}

char *disassembler_SE2(uint8_t lsb, uint8_t msb, char *line) {
  return registerOperation("SE", lsb, msb, line);
}

char *disassembler_LD1(uint8_t lsb, uint8_t msb, char *line) {
  assert(BUFFER_SIZE > (sizeof("LD Vx, 0xkk")));
  uint8_t x = lsb & 0x0F;
  sprintf(line, "LD V%X, 0x%02X", x, msb);
  return line;
}

char *disassembler_ADD(uint8_t lsb, uint8_t msb, char *line) {
  assert(BUFFER_SIZE > (sizeof("ADD Vx, 0xkk")));
  uint8_t x = lsb & 0x0F;
  sprintf(line, "ADD V%X, 0x%02X", x, msb);
  return line;
}

char *disassembler_LD2(uint8_t lsb, uint8_t msb, char *line) {
  return registerOperation("LD", lsb, msb, line);
}

char *disassembler_OR(uint8_t lsb, uint8_t msb, char *line) {
  return registerOperation("OR", lsb, msb, line);
}

char *disassembler_AND(uint8_t lsb, uint8_t msb, char *line) {
  return registerOperation("AND", lsb, msb, line);
}

char *disassembler_XOR(uint8_t lsb, uint8_t msb, char *line) {
  return registerOperation("XOR", lsb, msb, line);
}

char *disassembler_ADD2(uint8_t lsb, uint8_t msb, char *line) {
  return registerOperation("ADD", lsb, msb, line);
}

char *disassembler_SUB(uint8_t lsb, uint8_t msb, char *line) {
  return registerOperation("SUB", lsb, msb, line);
}

char *disassembler_SHR(uint8_t lsb, uint8_t msb, char *line) {
  assert(BUFFER_SIZE > (sizeof("SHR Vx{, Vy}")));
  uint8_t x = lsb & 0x0F;
  uint8_t y = (msb & 0xF0) >> 4;
  sprintf(line, "SHR V%X{, V%X}", x, y);
  return line;
}

char *disassembler_SUBN(uint8_t lsb, uint8_t msb, char *line) {
  return registerOperation("SUBN", lsb, msb, line);
}

char *disassembler_SHL(uint8_t lsb, uint8_t msb, char *line) {
  assert(BUFFER_SIZE > (sizeof("SHL Vx{, Vy}")));
  uint8_t x = lsb & 0x0F;
  uint8_t y = (msb & 0xF0) >> 4;
  sprintf(line, "SHL V%X{, V%X}", x, y);
  return line;
}

char *disassembler_SNE2(uint8_t lsb, uint8_t msb, char *line) {
  return registerOperation("SNE", lsb, msb, line);
}

char *disassembler_LDI(uint8_t lsb, uint8_t msb, char *line) {
  assert(BUFFER_SIZE > (sizeof("LD I, 0x0nnn")));
  uint8_t n = lsb & 0x0F;
  sprintf(line, "LD I, 0x%03X", (n << 8) | msb);
  return line;
}

char *disassembler_JP2(uint8_t lsb, uint8_t msb, char *line) {
  assert(BUFFER_SIZE > (sizeof("JP V0, 0x0nnn")));
  uint8_t n = lsb & 0x0F;
  sprintf(line, "JP V0, 0x%03X", (n << 8) | msb);
  return line;
}

char *disassembler_RND(uint8_t lsb, uint8_t msb, char *line) {
  assert(BUFFER_SIZE > (sizeof("RND Vx, 0xkkk")));
  uint8_t x = lsb & 0x0F;
  sprintf(line, "RND V%X, 0x%03X", x, msb);
  return line;
}

char *disassembler_DRW(uint8_t lsb, uint8_t msb, char *line) {
  assert(BUFFER_SIZE > (sizeof("DRW Vx, Vy, 0xkkk")));
  uint8_t x = lsb & 0x0F;
  uint8_t y = (msb & 0xF0) >> 4;
  uint8_t n = msb & 0x0F;
  sprintf(line, "DRW V%X, V%X, 0x%03X", x, y, n);
  return line;
}

char *disassembler_SKP(uint8_t lsb, uint8_t msb, char *line) {
  assert(BUFFER_SIZE > (sizeof("SKP Vx")));
  uint8_t x = lsb & 0x0F;
  sprintf(line, "SKP V%X", x);
  return line;
}

char *disassembler_SKNP(uint8_t lsb, uint8_t msb, char *line) {
  assert(BUFFER_SIZE > (sizeof("SKNP Vx")));
  uint8_t x = lsb & 0x0F;
  sprintf(line, "SKNP V%X", x);
  return line;
}

char *disassembler_LD3(uint8_t lsb, uint8_t msb, char *line) {
  assert(BUFFER_SIZE > (sizeof("LD Vx, DT")));
  uint8_t x = lsb & 0x0F;
  sprintf(line, "LD V%X, DT", x);
  return line;
}

char *disassembler_LD4(uint8_t lsb, uint8_t msb, char *line) {
  assert(BUFFER_SIZE > (sizeof("LD Vx, DT")));
  uint8_t x = lsb & 0x0F;
  sprintf(line, "LD V%X, DT", x);
  return line;
}

char *disassembler_LD5(uint8_t lsb, uint8_t msb, char *line) {
  assert(BUFFER_SIZE > (sizeof("LD DT, Vx")));
  uint8_t x = lsb & 0x0F;
  sprintf(line, "LD DT, V%X", x);
  return line;
}

char *disassembler_LD6(uint8_t lsb, uint8_t msb, char *line) {
  assert(BUFFER_SIZE > (sizeof("LD ST, Vx")));
  uint8_t x = lsb & 0x0F;
  sprintf(line, "LD ST, V%X", x);
  return line;
}

char *disassembler_ADD3(uint8_t lsb, uint8_t msb, char *line) {
  assert(BUFFER_SIZE > (sizeof("ADD I, Vx")));
  uint8_t x = lsb & 0x0F;
  sprintf(line, "ADD I, V%X", x);
  return line;
}

char *disassembler_LD7(uint8_t lsb, uint8_t msb, char *line) {
  assert(BUFFER_SIZE > (sizeof("LD F, Vx")));
  uint8_t x = lsb & 0x0F;
  sprintf(line, "LD F, V%X", x);
  return line;
}

char *disassembler_LD8(uint8_t lsb, uint8_t msb, char *line) {
  assert(BUFFER_SIZE > (sizeof("LD B, Vx")));
  uint8_t x = lsb & 0x0F;
  sprintf(line, "LD B, V%X", x);
  return line;
}

char *disassembler_LD9(uint8_t lsb, uint8_t msb, char *line) {
  assert(BUFFER_SIZE > (sizeof("LD [I], Vx")));
  uint8_t x = lsb & 0x0F;
  sprintf(line, "LD [I], V%X", x);
  return line;
}

char *disassembler_LD10(uint8_t lsb, uint8_t msb, char *line) {
  assert(BUFFER_SIZE > (sizeof("LD Vx, [I]")));
  uint8_t x = lsb & 0x0F;
  sprintf(line, "LD V%X, [I]", x);
  return line;
}

