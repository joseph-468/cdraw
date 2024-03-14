#include <raylib.h>
#include <raymath.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#include "../include/canvas.h"
#include "../include/gui.h"
#include "../include/font.h"

#define DISTANCE_FROM_WIDTH 480;
#define DISTANCE_FROM_HEIGHT 270;

static Texture backgroundTexture;
static Texture canvasTexture;

bool drawing = false;
bool *drawnPixels;

Viewport createViewport(const Canvas *canvas, const double x, const double y) {
	Viewport viewport = {
		.canvas = canvas,
		.canvasX = 0,
		.canvasY = 0,
		.canvasWidth = canvas->width,
		.canvasHeight = canvas->height,
		.x = x,
		.y = y,
		.width = 0,
		.height = 0,
	};

	resizeViewport(&viewport);
	scaleCanvasInViewport(&viewport);

	return viewport;
}

Vector2 getRelativePos(const Viewport *viewport) {
	Vector2 mousePos = GetMousePosition();
	double widthRatio = (double)viewport->canvas->width / viewport->canvasWidth;
	double heightRatio = (double)viewport->canvas->height / viewport->canvasHeight;
	Vector2 relativePos;
	relativePos.x = (mousePos.x - viewport->canvasX - viewport->x) * widthRatio;
	relativePos.y = (mousePos.y - viewport->canvasY - viewport->y) * heightRatio;

	return relativePos;
}


void resizeViewport(Viewport* viewport) {
	double prevWidth = viewport->width;
	double prevHeight = viewport->height;
	viewport->width = GetScreenWidth() - DISTANCE_FROM_WIDTH;
	viewport->height = GetScreenHeight() - DISTANCE_FROM_HEIGHT;

	double widthIncrease = viewport->width / prevWidth;
	double heightIncrease = viewport->height / prevHeight;
	if (widthIncrease > heightIncrease) {
		viewport->canvasWidth *= widthIncrease;
		viewport->canvasHeight *= widthIncrease;
	}
	else if (heightIncrease > widthIncrease) {
		viewport->canvasWidth *= heightIncrease;
		viewport->canvasHeight *= heightIncrease;
	}
}

void scaleCanvasInViewport(Viewport* viewport) {
	if (viewport->canvasWidth > viewport->width) {
		double newScale = viewport->width / viewport->canvasWidth;
		viewport->canvasWidth *= newScale;
		viewport->canvasHeight *= newScale;
	}
	if (viewport->canvasHeight > viewport->height) {
		double newScale = viewport->height / viewport->canvasHeight;
		viewport->canvasWidth *= newScale;
		viewport->canvasHeight *= newScale;
	}

	double widthRatio = (double)viewport->canvas->width / (double)viewport->canvas->height;
	viewport->canvasX = (double)(viewport->width - viewport->canvasWidth) / 2;
	viewport->canvasY = (double)(viewport->height - viewport->canvasHeight) / 2;
}

Canvas createBlankCanvas(const int width, const int height) {
	Canvas canvas = {
		.width = width,
		.height = height,
		.buffer = NULL,
	};

	drawnPixels = (bool*)malloc(width * height * sizeof(bool));
	HANDLE_MALLOC_FAILURE(drawnPixels);
	memset(drawnPixels, false, width * height * sizeof(bool));

	Image tempImage = GenImageColor(width, height, WHITE);
	canvasTexture = LoadTextureFromImage(tempImage);

	canvas.buffer = (Color*)malloc(width * height * sizeof(Color));
	HANDLE_MALLOC_FAILURE(canvas.buffer);
	memset(canvas.buffer, 255, width * height * sizeof(Color));

	// Background stuff
	backgroundTexture = LoadTextureFromImage(tempImage);
	UnloadImage(tempImage);
	Color* backgroundBuffer = (Color*)malloc(width * height * sizeof(Color));
	HANDLE_MALLOC_FAILURE(backgroundBuffer);
	memset(backgroundBuffer, 255, width * height * sizeof(Color));

	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			if ((x / 64 + y / 64) % 2 == 0) {
				backgroundBuffer[y * width + x] = GRAY;
			}
			else {
				backgroundBuffer[y * width + x] = LIGHTGRAY;
			}
		}
	}
	UpdateTexture(backgroundTexture, backgroundBuffer);
	free(backgroundBuffer);

	return canvas;
}

// Might move to seperate file
bool saveImage(const Canvas *canvas) {
	Image image = GenImageColor(canvas->width, canvas->height, (Color){ 0, 0, 0 });
	for (int i = 0; i < canvas->height; i++) {
		for (int j = 0; j < canvas->width; j++) {
			ImageDrawPixel(&image, j, i, canvas->buffer[i * canvas->width + j]);
		}
	}
	ExportImage(image, "image.png");
	return true;
}

void renderCanvas(const Viewport *viewport) {
	Rectangle source = { 0, 0, canvasTexture.width, canvasTexture.height };
	Rectangle dest = { viewport->x + viewport->canvasX, viewport->y + viewport->canvasY, viewport->canvasWidth, viewport->canvasHeight };
	UpdateTexture(canvasTexture, viewport->canvas->buffer);
	DrawTexturePro(backgroundTexture, source, dest, (Vector2){ 0, 0 }, 0, WHITE);
	DrawTexturePro(canvasTexture, source, dest, (Vector2){ 0, 0 }, 0, WHITE);
}

void drawSquare(const Canvas *canvas, const Brush brush, const int x, const int y) {
	const int halfSize = brush.size / 2;
	const int left = max(x - halfSize, 0);
	if (left > canvas->width-1) return;
	const int top = max(y - halfSize, 0);
	if (top > canvas->height-1) return;
	const int right = min(x + halfSize + ((brush.size % 2 == 0) ? 0 : 1), canvas->width);
	if (right < 0) return;
	const int bottom = min(y + halfSize + ((brush.size % 2 == 0) ? 0 : 1), canvas->height);
	if (bottom < 0) return;

	for (int i = top; i < bottom; i++) {
		for (int j = left; j < right; j++) {
			size_t dest = i * canvas->width + j;
			if (drawnPixels[dest]) continue;
			int r = 0, g = 0, b = 0, a = 0;
			if (brush.type == PENCIL) {
				Color dstColor = canvas->buffer[dest];
				double normalizedAlpha = (double)brush.color.a / 255;
				r = (int)(brush.color.r * normalizedAlpha + dstColor.r * (1 - normalizedAlpha));
				g = (int)(brush.color.g * normalizedAlpha + dstColor.g * (1 - normalizedAlpha));
				b = (int)(brush.color.b * normalizedAlpha + dstColor.b * (1 - normalizedAlpha)); 
				a = (int)(normalizedAlpha + ((double)dstColor.a / 255 * (1 - normalizedAlpha))) * 255;
			}
			canvas->buffer[i * canvas->width + j] = (Color){ r, g, b, a };
			drawnPixels[dest] = true;
		}
	}
}

void drawCircle(const Canvas *canvas, const Brush brush, int x, int y) {
	static bool* shapeCache = NULL;
	static int shapeCacheSize = 0;

	float evenOffset = (float)((brush.size % 2 == 0) ? 0.5 : 0);
	const int halfSize = brush.size / 2;
	const double radius = (double)brush.size / 2;
	const int left = x - halfSize;
	if (left > canvas->width-1) return;
	const int top = y - halfSize;
	if (top > canvas->height-1) return;
	const int right = min(x + halfSize + ((brush.size % 2 == 0) ? 0 : 1), canvas->width);
	if (right < 0) return;
	const int bottom = min(y + halfSize + ((brush.size % 2 == 0) ? 0 : 1), canvas->height);
	if (bottom < 0) return;
	double distance;

	// Calculate pixels that should be drawn in circle and cache it
	if (shapeCacheSize != brush.size) {
		if (shapeCache) {
			free(shapeCache);
		}
		shapeCache = calloc(brush.size * brush.size, sizeof(bool));
		HANDLE_MALLOC_FAILURE(shapeCache);
		shapeCacheSize = brush.size;
		for (int i = 0; i < brush.size; i++) {
			for (int j = 0; j < brush.size; j++) {
				distance = Vector2Distance((Vector2) { halfSize - evenOffset, halfSize - evenOffset }, (Vector2) { (float)j, (float)i });
				if (distance <= radius) {
					shapeCache[i * brush.size + j] = true;
				}
			}
		}
	}
	else if (shapeCacheSize == brush.size) {
		for (int i = top; i < bottom; i++) {
			for (int j = left; j < right; j++) {
				if (i < 0 || j < 0) continue;
				size_t dest = i * canvas->width + j;
				if (!shapeCache[(i-top) * brush.size + (j-left)]) continue;
				if (drawnPixels[dest]) continue;
				int r = 0, g = 0, b = 0, a = 0;
				if (brush.type == PENCIL) {
					Color dstColor = canvas->buffer[dest];
					double normalizedAlpha = (double)brush.color.a / 255;
					r = (int)(brush.color.r * normalizedAlpha + dstColor.r * (1 - normalizedAlpha));
					g = (int)(brush.color.g * normalizedAlpha + dstColor.g * (1 - normalizedAlpha));
					b = (int)(brush.color.b * normalizedAlpha + dstColor.b * (1 - normalizedAlpha));
					a = (int)(normalizedAlpha + ((double)dstColor.a / 255 * (1 - normalizedAlpha))) * 255;
				}
				canvas->buffer[i * canvas->width + j] = (Color){ r, g, b, a };
				drawnPixels[dest] = true;
			}
		}
	}
}

void tryDrawToCanvas(const Viewport *viewport,const Canvas *canvas, const Brush brush) {
	static int prevMouseX = 0;
	static int prevMouseY = 0;

	if (IsMouseButtonDown(MOUSE_BUTTON_LEFT) && !hoveringGUI) {
		int mouseX = getRelativePos(viewport).x;
		int mouseY = getRelativePos(viewport).y;

		if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
			drawing = true;
			prevMouseX = mouseX;
			prevMouseY = mouseY;
		}
		if (drawing) {
			int dx = prevMouseX - mouseX;
			int dy = prevMouseY - mouseY;
			int steps = abs(dx) > abs(dy) ? abs(dx) : abs(dy);
			float xInc = (float)dx / (float)steps;
			float yInc = (float)dy / (float)steps;
			float x = (float)mouseX;
			float y = (float)mouseY;
			for (int i = 0; i <= steps; i++) {
				if (brush.shape == SQUARE) {
					drawSquare(canvas, brush, (int)roundf(x), (int)roundf(y));
				}
				if (brush.shape == CIRCLE) {
					drawCircle(canvas, brush, (int)roundf(x), (int)roundf(y));
				}
				x += xInc;
				y += yInc;
			}
		}

		drawing = true;
		prevMouseX = mouseX;
		prevMouseY = mouseY;
	}
	else {
		if (drawing) {
			memset(drawnPixels, false, canvas->width * canvas->height * sizeof(bool));
		}
		drawing = false;
	}
}
