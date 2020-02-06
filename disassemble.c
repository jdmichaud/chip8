#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "disassemble.h"
#include "file.h"
#include "chip8.h"

typedef struct line_list_s {
  char *line;
  struct line_list_s *next;
} line_list_t;

char **disassemble(uint8_t *dump, uint16_t size) {
  char **result = malloc((size + 1) * sizeof (char *));
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
    result[i >> 1] = g_dinstructions[instructionIndex].instruction(lsb, msb);
  }
  return result;
}

void print_dump(uint8_t *dump, uint16_t size, uint16_t offset) {
  char **listing = disassemble(dump, size);
  for (uint16_t i = 0; i < size; ++i) {
    if (listing[i] != NULL) {
      fprintf(stdout, "(0x%04X) 0x%02X%02X %s\n", offset + (i << 1), dump[i << 1], dump[(i << 1) + 1], listing[i]);
      free(listing[i]);
    }
  }
  free(listing);
}

char *registerOperation(char *name, uint8_t lsb, uint8_t msb) {
  const uint8_t size = strnlen(name, 4) + 1 + sizeof("Vx, Vy") + 1;
  char *line = malloc(size * sizeof (char));
  memset(line, 0, size * sizeof (char));
  const uint8_t x = lsb & 0x0F;
  const uint8_t y = (msb & 0xF0) >> 4;
  sprintf(line, "%s V%X, V%X", name, x, y);
  return line;
}

char *disassembler_SYS(uint8_t lsb, uint8_t msb) {
  char *line = malloc(sizeof("SYS 0xkkk"));
  uint8_t k = lsb & 0x0F;
  sprintf(line, "SYS 0x%X", (k << 8) | msb);
  return line;
}

char *disassembler_CLS(uint8_t lsb, uint8_t msb) {
  char *line = malloc(sizeof("CLS"));
  sprintf(line, "CLS");
  return line;
}

char *disassembler_RET(uint8_t lsb, uint8_t msb) {
  char *line = malloc(sizeof("RET"));
  sprintf(line, "RET");
  return line;
}

char *disassembler_JP(uint8_t lsb, uint8_t msb) {
  char *line = malloc(sizeof("JP 0x0nnn"));
  uint8_t n = lsb & 0x0F;
  sprintf(line, "JP 0x%04X", (n << 8) | msb);
  return line;
}

char *disassembler_CALL(uint8_t lsb, uint8_t msb) {
  char *line = malloc(sizeof("CALL 0x0nnn"));
  uint8_t n = lsb & 0x0F;
  sprintf(line, "CALL 0x%04X", (n << 8) | msb);
  return line;
}

char *disassembler_SE(uint8_t lsb, uint8_t msb) {
  char *line = malloc(sizeof("SE Vx, 0xkk"));
  uint8_t x = lsb & 0x0F;
  sprintf(line, "SE V%X, 0x%02X", x, msb);
  return line;
}

char *disassembler_SNE(uint8_t lsb, uint8_t msb) {
  char *line = malloc(sizeof("SNE Vx, 0xkk"));
  uint8_t x = lsb & 0x0F;
  sprintf(line, "SNE V%X, 0x%02X", x, msb);
  return line;
}

char *disassembler_SE2(uint8_t lsb, uint8_t msb) {
  return registerOperation("SE", lsb, msb);
}

char *disassembler_LD1(uint8_t lsb, uint8_t msb) {
  char *line = malloc(sizeof("LD Vx, 0xkk"));
  uint8_t x = lsb & 0x0F;
  sprintf(line, "LD V%X, 0x%02X", x, msb);
  return line;
}

char *disassembler_ADD(uint8_t lsb, uint8_t msb) {
  char *line = malloc(sizeof("ADD Vx, 0xkk"));
  uint8_t x = lsb & 0x0F;
  sprintf(line, "ADD V%X, 0x%02X", x, msb);
  return line;
}

char *disassembler_LD2(uint8_t lsb, uint8_t msb) {
  return registerOperation("LD", lsb, msb);
}

char *disassembler_OR(uint8_t lsb, uint8_t msb) {
  return registerOperation("OR", lsb, msb);
}

char *disassembler_AND(uint8_t lsb, uint8_t msb) {
  return registerOperation("AND", lsb, msb);
}

char *disassembler_XOR(uint8_t lsb, uint8_t msb) {
  return registerOperation("XOR", lsb, msb);
}

char *disassembler_ADD2(uint8_t lsb, uint8_t msb) {
  return registerOperation("ADD", lsb, msb);
}

char *disassembler_SUB(uint8_t lsb, uint8_t msb) {
  return registerOperation("SUB", lsb, msb);
}

char *disassembler_SHR(uint8_t lsb, uint8_t msb) {
  char *line = malloc(sizeof("SHR Vx{, Vy}"));
  uint8_t x = lsb & 0x0F;
  uint8_t y = (msb & 0xF0) >> 4;
  sprintf(line, "SHR V%X{, V%X}", x, y);
  return line;
}

char *disassembler_SUBN(uint8_t lsb, uint8_t msb) {
  return registerOperation("SUBN", lsb, msb);
}

char *disassembler_SHL(uint8_t lsb, uint8_t msb) {
  char *line = malloc(sizeof("SHL Vx{, Vy}"));
  uint8_t x = lsb & 0x0F;
  uint8_t y = (msb & 0xF0) >> 4;
  sprintf(line, "SHL V%X{, V%X}", x, y);
  return line;
}

char *disassembler_SNE2(uint8_t lsb, uint8_t msb) {
  return registerOperation("SNE", lsb, msb);
}

char *disassembler_LDI(uint8_t lsb, uint8_t msb) {
  char *line = malloc(sizeof("LD I, 0x0nnn"));
  uint8_t n = lsb & 0x0F;
  sprintf(line, "LD I, 0x%03X", (n << 8) | msb);
  return line;
}

char *disassembler_JP2(uint8_t lsb, uint8_t msb) {
  char *line = malloc(sizeof("JP V0, 0x0nnn"));
  uint8_t n = lsb & 0x0F;
  sprintf(line, "JP V0, 0x%03X", (n << 8) | msb);
  return line;
}

char *disassembler_RND(uint8_t lsb, uint8_t msb) {
  char *line = malloc(sizeof("RND Vx, 0xkkk"));
  uint8_t x = lsb & 0x0F;
  sprintf(line, "RND V%X, 0x%03X", x, msb);
  return line;
}

char *disassembler_DRW(uint8_t lsb, uint8_t msb) {
  char *line = malloc(sizeof("DRW Vx, Vy, 0xkkk"));
  uint8_t x = lsb & 0x0F;
  uint8_t y = (msb & 0xF0) >> 4;
  uint8_t n = msb & 0x0F;
  sprintf(line, "DRW V%X, V%X, 0x%03X", x, y, n);
  return line;
}

char *disassembler_SKP(uint8_t lsb, uint8_t msb) {
  char *line = malloc(sizeof("SKP Vx"));
  uint8_t x = lsb & 0x0F;
  sprintf(line, "SKP V%X", x);
  return line;
}

char *disassembler_SKNP(uint8_t lsb, uint8_t msb) {
  char *line = malloc(sizeof("SKNP Vx"));
  uint8_t x = lsb & 0x0F;
  sprintf(line, "SKNP V%X", x);
  return line;
}

char *disassembler_LD3(uint8_t lsb, uint8_t msb) {
  char *line = malloc(sizeof("LD Vx, DT"));
  uint8_t x = lsb & 0x0F;
  sprintf(line, "LD V%X, DT", x);
  return line;
}

char *disassembler_LD4(uint8_t lsb, uint8_t msb) {
  char *line = malloc(sizeof("LD Vx, DT"));
  uint8_t x = lsb & 0x0F;
  sprintf(line, "LD V%X, DT", x);
  return line;
}

char *disassembler_LD5(uint8_t lsb, uint8_t msb) {
  char *line = malloc(sizeof("LD DT, Vx"));
  uint8_t x = lsb & 0x0F;
  sprintf(line, "LD DT, V%X", x);
  return line;
}

char *disassembler_LD6(uint8_t lsb, uint8_t msb) {
  char *line = malloc(sizeof("LD ST, Vx"));
  uint8_t x = lsb & 0x0F;
  sprintf(line, "LD ST, V%X", x);
  return line;
}

char *disassembler_ADD3(uint8_t lsb, uint8_t msb) {
  char *line = malloc(sizeof("ADD I, Vx"));
  uint8_t x = lsb & 0x0F;
  sprintf(line, "ADD I, V%X", x);
  return line;
}

char *disassembler_LD7(uint8_t lsb, uint8_t msb) {
  char *line = malloc(sizeof("LD F, Vx"));
  uint8_t x = lsb & 0x0F;
  sprintf(line, "LD F, V%X", x);
  return line;
}

char *disassembler_LD8(uint8_t lsb, uint8_t msb) {
  char *line = malloc(sizeof("LD B, Vx"));
  uint8_t x = lsb & 0x0F;
  sprintf(line, "LD B, V%X", x);
  return line;
}

char *disassembler_LD9(uint8_t lsb, uint8_t msb) {
  char *line = malloc(sizeof("LD [I], Vx"));
  uint8_t x = lsb & 0x0F;
  sprintf(line, "LD [I], V%X", x);
  return line;
}

char *disassembler_LD10(uint8_t lsb, uint8_t msb) {
  char *line = malloc(sizeof("LD Vx, [I]"));
  uint8_t x = lsb & 0x0F;
  sprintf(line, "LD V%X, [I]", x);
  return line;
}

