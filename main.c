// ls *.c *.h | entr bash -c 'make && echo "done"'
#include <stdio.h>

#include "file.h"
#include "chip8.h"
#include "disassembler.h"
#include "debug_chip8.h"

#include "debug.h"

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

  // chip8_t chip8;
  // init(&chip8);
  // load(&chip8, file.content, file.size);
  // print_chip8(chip8);
  // step(&chip8);
  // print_chip8(chip8);
  print_dump(file.content, file.size);
}
