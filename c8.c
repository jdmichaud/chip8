// ls *.c *.h | entr bash -c 'make && echo "done"'
// LD_LIBRARY_PATH=$LD_LIBRARY_PATH:./raylib-2.5.0-Linux-amd64/lib/ ./c8 roms/demos/maze.ch8
#include <stdio.h>
#include <string.h>
#include <sys/time.h>

#include "raylib.h"

#include "file.h"
#include "chip8.h"
#include "disassemble.h"
#include "debug_chip8.h"
#include "text.h"

#include "debug.h"

const uint16_t WIDTH = 750;
const uint16_t HEIGHT = 166;

void usage() {
  fprintf(stderr, "usage: chip8 <binfile>\n");
}

#define COOLDARK (Color){ 0x20, 0x2A, 0x35, 255 }
#define COOLGREY (Color){ 0x8F, 0x91, 0x85, 255 }

void putImage(uint8_t *screen, uint16_t screenW, uint16_t screenH, // chip8 screen info
  uint16_t originX, uint16_t originY, uint16_t windowW, uint16_t windowH, // window info
  Color background, Color foreground) {
  DrawRectangle(originX, originY, windowW, windowH, background);
  for (uint16_t i = 0; i < (screenW * screenH / 8); ++i) {
    for (uint8_t j = 0; j < 8; ++j) {
      if ((128 >> j) & screen[i]) {
        uint16_t screenOffset = i * 8 + j;
        uint16_t screenX = (uint16_t) screenOffset % 64;
        uint16_t screenY = (uint16_t) screenOffset / 64;
        uint16_t wRatio = windowW / screenW;
        uint16_t hRatio = windowH / screenH;
        DrawRectangle(screenX * wRatio + originX, screenY * hRatio + originY,
          wRatio, hRatio, foreground);
      }
    }
  }
}

void scanKeyboard(chip8_t *chip) {
  chip->keyboard[0x0] = IsKeyPressed(KEY_ZERO) ? 1 : 0;
  chip->keyboard[0x1] = IsKeyPressed(KEY_ONE) ? 1 : 0;
  chip->keyboard[0x2] = IsKeyPressed(KEY_TWO) ? 1 : 0;
  chip->keyboard[0x3] = IsKeyPressed(KEY_THREE) ? 1 : 0;
  chip->keyboard[0x4] = IsKeyPressed(KEY_FOUR) ? 1 : 0;
  chip->keyboard[0x5] = IsKeyPressed(KEY_FIVE) ? 1 : 0;
  chip->keyboard[0x6] = IsKeyPressed(KEY_SIX) ? 1 : 0;
  chip->keyboard[0x7] = IsKeyPressed(KEY_SEVEN) ? 1 : 0;
  chip->keyboard[0x8] = IsKeyPressed(KEY_EIGHT) ? 1 : 0;
  chip->keyboard[0x9] = IsKeyPressed(KEY_NINE) ? 1 : 0;

  chip->keyboard[0xA] = IsKeyPressed(KEY_A) ? 1 : 0;
  chip->keyboard[0xB] = IsKeyPressed(KEY_B) ? 1 : 0;
  chip->keyboard[0xC] = IsKeyPressed(KEY_C) ? 1 : 0;
  chip->keyboard[0xD] = IsKeyPressed(KEY_D) ? 1 : 0;
  chip->keyboard[0xE] = IsKeyPressed(KEY_E) ? 1 : 0;
  chip->keyboard[0xF] = IsKeyPressed(KEY_F) ? 1 : 0;
}

void displayDebugger(chip8_t *chip, const uint16_t nbDisplayedLine,
  const uint16_t originX, const uint16_t originY,
  const uint16_t windowW, const uint16_t windowH,
  const Texture2D *font, const Color color) {
  static uint16_t cursor = 0;
  // Keep the cursor within the window
  if (cursor > chip->pc) cursor = chip->pc >= 0x202 ? chip->pc - 2 : chip->pc;
  const uint16_t upperLimit = chip->pc - ((nbDisplayedLine - 1) * 2);
  if (cursor <= upperLimit) { /* 2 bytes per instruction */
    cursor = upperLimit > 0x200 ? upperLimit + nbDisplayedLine: 0x200;
  }
  char **listing = disassemble(chip->memory + cursor, nbDisplayedLine * 2);
  static char line[256];
  memset(line, 0, 256 * sizeof (char));
  const uint16_t pcCursorPosY= (((chip->pc - cursor) >> 1) * 9) - 1;
  DrawRectangle(3, pcCursorPosY + 3, windowW, 9, (Color) { 0xFF, 0x7F, 0, 160 });
  for (uint16_t i = 0; listing[i] != NULL && i < nbDisplayedLine; ++i) {
    if (listing[i] != NULL) {
      sprintf(line,
        " (0X%04X) 0X%02X%02X %s\n",
        cursor + (i << 1),
        chip->memory[cursor + (i << 1)],
        chip->memory[cursor + (i << 1) + 1],
        listing[i]);
      print(line, originX, originY + (i * 9), font, color);
    }
  }
}

void displayChip(chip8_t *chip, const uint16_t originX, const uint16_t originY,
  const Texture2D *font, const Color color) {
  vprint(originX, originY + 9 *  0, font, color, "  I: 0X%04x V0: 0X%02x V8: 0X%02x", chip->i, chip->v[0x0], chip->v[0x8]);
  vprint(originX, originY + 9 *  1, font, color, " PC: 0X%04x V1: 0X%02x V9: 0X%02x", chip->pc, chip->v[0x1], chip->v[0x9]);
  vprint(originX, originY + 9 *  2, font, color, "            V2: 0X%02x VA: 0X%02x", chip->v[0x2], chip->v[0xA]);
  vprint(originX, originY + 9 *  3, font, color, " DT: 0X%02x   V3: 0X%02x VB: 0X%02x", chip->dt, chip->v[0x3], chip->v[0xB]);
  vprint(originX, originY + 9 *  4, font, color, " ST: 0X%02x   V4: 0X%02x VC: 0X%02x", chip->st, chip->v[0x4], chip->v[0xC]);
  vprint(originX, originY + 9 *  5, font, color, "            V5: 0X%02x VD: 0X%02x", chip->v[0x5], chip->v[0xD]);
  vprint(originX, originY + 9 *  6, font, color, "            V6: 0X%02x VE: 0X%02x", chip->v[0x6], chip->v[0xE]);
  vprint(originX, originY + 9 *  7, font, color, "            V7: 0X%02x VF: 0X%02x", chip->v[0x7], chip->v[0xF]);

  vprint(originX, originY + 9 *  9, font, color, " SP: 0X%04x", chip->sp);

  vprint(originX, originY + 9 * 10, font, color, "      &0X0 0X%04x &0X8 0X%04x", chip->stack[0x0], chip->stack[0x8]);
  vprint(originX, originY + 9 * 11, font, color, "      &0X1 0X%04x &0X9 0X%04x", chip->stack[0x1], chip->stack[0x9]);
  vprint(originX, originY + 9 * 12, font, color, "      &0X2 0X%04x &0XA 0X%04x", chip->stack[0x2], chip->stack[0xA]);
  vprint(originX, originY + 9 * 13, font, color, "      &0X3 0X%04x &0XB 0X%04x", chip->stack[0x3], chip->stack[0xB]);
  vprint(originX, originY + 9 * 14, font, color, "      &0X4 0X%04x &0XC 0X%04x", chip->stack[0x4], chip->stack[0xC]);
  vprint(originX, originY + 9 * 15, font, color, "      &0X5 0X%04x &0XD 0X%04x", chip->stack[0x5], chip->stack[0xD]);
  vprint(originX, originY + 9 * 16, font, color, "      &0X6 0X%04x &0XE 0X%04x", chip->stack[0x6], chip->stack[0xE]);
  vprint(originX, originY + 9 * 17, font, color, "      &0X7 0X%04x &0XF 0X%04x", chip->stack[0x7], chip->stack[0xF]);
}

int main(int argc, char **argv) {
  if (argc != 2) {
    fprintf(stderr, "error: wrong number of argument, Expected 1.");
    usage();
    return 1;
  }

  file_t file;
  if (load_file(argv[1], &file) != 0) {
    return 1;
  }

  InitWindow(WIDTH, HEIGHT, "chip8");
  Texture2D font = loadFont("assets/font.png");

  chip8_t chip8;
  uint8_t *screen = malloc(64 * 32 / 8 * sizeof (uint8_t));
  memset(screen, 0, 64 * 32 / 8 * sizeof (uint8_t));
  // Initialize the chip. Attach the screen.
  init(&chip8, screen);
  // Load the roms into memory.
  load(&chip8, file.content, file.size);

  // uint32_t count = 0;
  // struct timeval stop, start;
  // gettimeofday(&start, NULL);
  // Execute.
  while (!IsKeyDown(KEY_Q) && !IsKeyDown(KEY_ESCAPE)) {
    // print_chip8(chip8);
    scanKeyboard(&chip8);
    BeginDrawing();
      ClearBackground(COOLDARK);
      putImage(screen, 64, 32, WIDTH - 320 - 3, 3, 320, 160, COOLGREY, COOLDARK);
      displayDebugger(&chip8, 18, 3, 3, 220, HEIGHT - 6, &font, WHITE);
      displayChip(&chip8, 3 + 223, 3, &font, WHITE);
    EndDrawing();
    // getchar();
    step(&chip8);
    // ++count;
    // if (count % 1000000 == 0) {
    //   gettimeofday(&stop, NULL);
    //   uint64_t diff = (stop.tv_sec - start.tv_sec) * 1000000 + stop.tv_usec - start.tv_usec;
    //   printf("%f, %u, %lu\n", (double) count / (double) diff * 1000000, count, diff);
    // }
  }
  CloseWindow();
}
