#include <stdio.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>

#include "file.h"

int8_t load_file(char *filename, file_t *file) {
  // Open the file
  file->fd = open(filename, O_RDONLY);
  if (file->fd < 0) {
    perror(filename);
    return 1;
  }
  // probe its size
  file->size = lseek(file->fd, 0, SEEK_END);
  if (file->size < 0) {
    perror("lseek");
    close(file->fd);
    return 1;
  }
  // Be kind, rewind
  lseek(file->fd, 0, SEEK_SET);
  // Load the file
  file->content = mmap(NULL, file->size, PROT_READ, MAP_SHARED, file->fd, 0);
  if (file == NULL) {
    perror("mmap");
    close(file->fd);
    return 1;
  }
  file->filename = filename;
  return 0;
}

#include "file.h"