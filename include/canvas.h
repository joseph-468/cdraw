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
	int width, height;
	double x, y;
	double viewWidth, viewHeight;
	int viewportWidth, viewportHeight;
	double viewportX, viewportY;
	Color *buffer;
} Canvas;

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

Canvas createBlankCanvas(const double x, const double y, const int width, const int height);
void renderCanvas(const Canvas *canvas);
void tryDrawToCanvas(const Canvas *canvas, const Brush brush);

#endif