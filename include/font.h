#ifndef FONT_H
#define FONT_H

#include <raylib.h>
#include <stdint.h>

extern Font currentFont;
extern GlyphInfo currentFontInfo;

void setCurrentFont(const uint8_t *data);

extern const uint8_t jetBrainsMonoMedium[273860];

#endif


