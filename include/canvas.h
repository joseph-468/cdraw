#ifndef PAINT_H
#define PAINT_H

#include <stdint.h>

#define HANDLE_MALLOC_FAILURE(p) \
do { \
	if (p == NULL) { \
		fprintf(stderr, "Memory allocation failure"); \
		CloseWindow(); \
		exit(EXIT_FAILURE);	\
	} \
} while(0) \

typedef struct Canvas {
	int64_t width, height;
	Color *buffer;
} Canvas;

typedef struct Viewport {
	Canvas* canvas;
	float canvasX, canvasY;
	float canvasWidth, canvasHeight;
	int64_t x, y;
	int64_t width, height;
} Viewport;

typedef enum BrushType {
	PENCIL,
	ERASER,
} BrushType;

typedef enum Shape {
	SQUARE,
	CIRCLE,
} Shape;

typedef struct Brush {
	BrushType type;
	Shape shape;
	Color color;
	int64_t size;
} Brush;

Canvas createBlankCanvas(const int64_t width, const int64_t height);
Viewport createViewport(Canvas *canvas, const float x, const float y);
Vector2 getRelativePos(const Viewport *viewport);
void keepMinimumWindowSize();
void handleCanvasZoom(Viewport *viewport);
void resizeViewport( Viewport *viewport);
void scaleCanvasInViewport(Viewport *viewport);
void renderCanvas(const Viewport *viewport);
void tryDrawToCanvas(const Viewport *viewport, const Brush brush);
bool saveImage(const Canvas* canvas, const char *filename);

#endif