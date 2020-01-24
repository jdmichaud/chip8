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

  InitWindow(64, 32, "chip8");

  chip8_t chip8;
  uint8_t *screen = malloc(64 * 32 * sizeof (uint8_t));
  memset(screen, 0, 64 * 32 * sizeof (uint8_t));
  // Initialize the chip. Attach the screen.
  init(&chip8, screen);
  // Load the roms into memory.
  load(&chip8, file.content, file.size);
  // Execute.
  while (1) {
    step(&chip8);
    print_chip8(chip8);
    BeginDrawing(); {
      printf("After BeginDrawing\n");
        uint16_t count = 0;
        for (uint16_t i = 0; i < (64 * 32); ++i) {
          if (screen[i] != 0) {
            DrawPixel((uint16_t) i % 32, (uint16_t) i / 32, BLACK);
            count++;
          }
        }
        // DrawText("Congrats! You created your first window!", 190, 200, 20, LIGHTGRAY);
      printf("Before EndDrawing %u\n", count);
    }
    EndDrawing();
    while ('\n' != getchar());
  }
}
