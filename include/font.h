#ifndef FONT_H
#define FONT_H

#include <raylib.h>

extern Font currentFont;
extern GlyphInfo currentFontInfo;

void setCurrentFont(const unsigned char *data);

extern const unsigned char jetBrainsMonoMedium[273860];

#endif


