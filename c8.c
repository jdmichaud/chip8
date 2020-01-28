// ls *.c *.h | entr bash -c 'make && echo "done"'
// LD_LIBRARY_PATH=$LD_LIBRARY_PATH:./raylib-2.5.0-Linux-amd64/lib/ ./c8 roms/demos/maze.ch8
#include <stdio.h>
#include <string.h>

#include "file.h"
#include "chip8.h"
#include "disassemble.h"
#include "debug_chip8.h"

#include "debug.h"

#include "raylib.h"

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

  InitWindow(640, 320, "chip8");

  chip8_t chip8;
  uint8_t *screen = malloc(64 * 32 / 8 * sizeof (uint8_t));
  memset(screen, 0, 64 * 32 / 8 * sizeof (uint8_t));
  // Initialize the chip. Attach the screen.
  init(&chip8, screen);
  // Load the roms into memory.
  load(&chip8, file.content, file.size);
  // Execute.
  while (1) {
    step(&chip8);
    // print_chip8(chip8);
    BeginDrawing();
      ClearBackground(COOLDARK);
      putImage(screen, 64, 32, 3, 3, 320, 160, COOLGREY, COOLDARK);
    EndDrawing();
    // while ('\n' != getchar());
  }
}
