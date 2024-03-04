#ifndef ICONS_H
#define ICONS_H

#include <raylib.h>

Texture loadIcon(const unsigned char *data, int dataLen);

extern const unsigned int squareIconLen;
extern const unsigned int circleIconLen;
extern const unsigned int pencilIconLen;
extern const unsigned int eraserIconLen;

extern const unsigned char squareIcon[];
extern const unsigned char circleIcon[];
extern const unsigned char pencilIcon[];
extern const unsigned char eraserIcon[];

#endif 