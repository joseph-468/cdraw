#ifndef PAINT_H
#define PAINT_H

#define HANDLE_MALLOC_FAILURE(p) \
do { \
	if (p == NULL) { \
		fprintf(stderr, "Memory allocation failure"); \
		CloseWindow(); \
		exit(EXIT_FAILURE);	\
	} \
} while(0) \

extern const int screenWidth;
extern const int screenHeight;

typedef struct Canvas {
	int width;
	int height;
	Color *buffer;
} Canvas;

void init();
Canvas createBlankCanvas(const int width, const int height);
void renderCanvas(const Canvas canvas);
void drawSquare(const Canvas canvas, const int size, const int x, const int y);

#endif