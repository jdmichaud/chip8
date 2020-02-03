#include <stdio.h>
#include <stdint.h>
#include <stdarg.h>
#include <string.h>

#include "raylib.h"

#include "text.h"

/**
 * Load an image as a font. Expect each character to be 5x7 pixels and separated
 * each by 1 pixel.
 * Example: dash charater:
 * 010100
 * 010100
 * 111110
 * 010100
 * 111110
 * 010100
 * 010100
 */
Texture2D loadFont(char *filename) {
  const Image image = LoadImage(filename);
  return LoadTextureFromImage(image);
}

/**
 * Print a string of characters of font at position posX, posY
 */
void print(const char *s, const uint16_t posX, const uint16_t posY,
  const Texture2D *font, const Color color) {
  for (uint16_t i = 0; s[i] != 0; ++i) {
    const char c = s[i];
    if (c > 32 && c < 126) {
      DrawTextureRec(
        *font,
        (Rectangle) { (c - 33) * 6, 0, 5, 7 },
        (Vector2) { posX + i * 6, posY },
        color
      );
    }
  }
}

void vprint(const uint16_t posX, const uint16_t posY, const Texture2D *font,
  const Color color, const char *format, ...) {
  va_list args;
  static char s[64];
  va_start(args, format);
  vsnprintf(s, 64, format, args);
  va_end(args);
  print(s, posX, posY, font, color);
}
