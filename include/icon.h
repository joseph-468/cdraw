#ifndef ICONS_H
#define ICONS_H

#include <raylib.h>
#include <stdint.h>

Texture loadIcon(const uint8_t *data, uint64_t dataLen);

extern const uint64_t squareIconLen;
extern const uint64_t circleIconLen;
extern const uint64_t pencilIconLen;
extern const uint64_t eraserIconLen;

extern const uint8_t squareIcon[];
extern const uint8_t circleIcon[];
extern const uint8_t pencilIcon[];
extern const uint8_t eraserIcon[];

#endif 