#ifndef ICONS_H
#define ICONS_H

#include <raylib.h>

Texture loadIcon(const unsigned char *data, int dataLen);

extern const int squareIconLen;
extern const int circleIconLen;
extern const unsigned char squareIcon[];
extern const unsigned char circleIcon[];

#endif 