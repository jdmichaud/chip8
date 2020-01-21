// ls *.c *.h | entr bash -c 'make && echo "done"'
#include <stdio.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>

#include "chip8.h"
#include "debug_chip8.h"

#include "debug.h"

#define ERROR 1

typedef struct file_s {
  int16_t fd;
  ssize_t size;
  uint8_t *content;
  char    *filename;
} file_t;

void usage() {
  fprintf(stderr, "usage: chip8 <binfile>\n");
}

int8_t load_file(char *filename, file_t *file) {
  // Open the file
  file->fd = open(filename, O_RDONLY);
  if (file->fd < 0) {
    perror(filename);
    return ERROR;
  }
  // probe its size
  file->size = lseek(file->fd, 0, SEEK_END);
  if (file->size < 0) {
    perror("lseek");
    close(file->fd);
    return ERROR;
  }
  // Be kind, rewind
  lseek(file->fd, 0, SEEK_SET);
  // Load the file
  file->content = mmap(NULL, file->size, PROT_READ, MAP_SHARED, file->fd, 0);
  if (file == NULL) {
    perror("mmap");
    close(file->fd);
    return ERROR;
  }
  file->filename = filename;
  return 0;
}

int main(int argc, char **argv) {
  if (argc != 2) {
    fprintf(stderr, "error: wrong number of argument, Expected 1.");
    usage();
    return ERROR;
  }

  file_t file;
  if (load_file(argv[1], &file) != 0) {
    return ERROR;
  }

  chip8_t chip8;
  init(&chip8);
  load(&chip8, file.content, file.size);
  print_chip8(chip8);
  step(&chip8);
  print_chip8(chip8);
}
