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

void init();
Color* createBlankCanvas(const int width, const int height);
void renderCanvas(const Color *canvasBuffer);

#endif