#ifndef __FILE_H__
#define __FILE_H__

#include <stdint.h>
#include <sys/types.h>

typedef struct file_s {
  int16_t fd;
  ssize_t size;
  uint8_t *content;
  char    *filename;
} file_t;

int8_t load_file(char *filename, file_t *file);

#endif // !__FILE_H__
