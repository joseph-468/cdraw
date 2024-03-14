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

typedef struct Canvas {
	int width, height;
	Color *buffer;
} Canvas;

typedef struct Viewport {
	Canvas* canvas;
	double canvasX, canvasY;
	double canvasWidth, canvasHeight;
	int x, y;
	int width, height;
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
	int size;
} Brush;

Canvas createBlankCanvas(const int width, const int height);
Viewport createViewport(const Canvas *canvas, const double x, const double y);
Vector2 getRelativePos(const Viewport *viewport);
void resizeViewport(Viewport *viewport);
void scaleCanvasInViewport(Viewport *viewport);
void renderCanvas(const Viewport *viewport);
void tryDrawToCanvas(const Viewport *viewport, const Canvas *canvas, const Brush brush);
bool saveImage(const Canvas* canvas);

#endif