#ifndef __TEXT_H__
#define __TEXT_H__

#include "raylib.h"

Texture2D loadFont(char *filename);
void print(const char *s, const uint16_t posX, const uint16_t posY,
  const Texture2D *font, const Color color);
void vprint(const uint16_t posX, const uint16_t posY, const Texture2D *font,
  const Color color, const char *format, ...);

#endif // !__TEXT_H__
