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
	int x, y;
	int width, height;
	int viewWidth, viewHeight;
	Color *buffer;
} Canvas;

typedef enum BrushType {
	PENCIL,
	PEN,
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

void init();
Canvas createBlankCanvas(const int x, const int y, const int width, const int height);
void renderCanvas(const Canvas canvas);
void drawSquare(const Canvas canvas, const Brush brush, const int x, const int y);
void drawLine(const Canvas canvas, const Brush brush, bool shouldDraw);

#endif