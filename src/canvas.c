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

	drawnPixels = (bool*)calloc(width * height, sizeof(bool));
	HANDLE_MALLOC_FAILURE(drawnPixels);
	memset(drawnPixels, false, width * height * sizeof(bool));

	Image tempImage = GenImageColor(width, height, WHITE);
	canvasTexture = LoadTextureFromImage(tempImage);
    UnloadImage(tempImage);

	canvas.buffer = (Color*)malloc(width * height * sizeof(Color));
	HANDLE_MALLOC_FAILURE(canvas.buffer);
	memset(canvas.buffer, 255, width * height * sizeof(Color));

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
	UpdateTexture(canvasTexture, viewport->canvas->buffer);
	Rectangle source = { 0, 0, (float)canvasTexture.width, (float)canvasTexture.height };
	Rectangle dest = { (float)viewport->x+viewport->canvasX, (float)viewport->y+viewport->canvasY, (float)viewport->canvasWidth, (float)viewport->canvasHeight };
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
			if (!drawnPixels[dest]) {
				int r, g, b, a;
				if (brush.type == PENCIL) {
					Color dstColor = canvas->buffer[dest];
					double normalizedAlpha = (double)brush.color.a / 255;
					r = (int)(brush.color.r * normalizedAlpha + dstColor.r * (1 - normalizedAlpha));
					g = (int)(brush.color.g * normalizedAlpha + dstColor.g * (1 - normalizedAlpha));
					b = (int)(brush.color.b * normalizedAlpha + dstColor.b * (1 - normalizedAlpha)); 
					a = (int)(normalizedAlpha + ((double)dstColor.a / 255 * (1 - normalizedAlpha))) * 255;
				}
				else {
					r = g = b = a = 0;
				}
				canvas->buffer[i * canvas->width + j] = (Color){ r, g, b, a };
				drawnPixels[dest] = true;
			}
		}
	}
}

void drawCircle(const Canvas *canvas, const Brush brush, int x, int y) {
	const int halfSize = brush.size / 2;
	const double radius = (double)brush.size / 2;
	const int left = max(x - halfSize, 0);
	if (left > canvas->width-1) return;
	const int top = max(y - halfSize, 0);
	if (top > canvas->height-1) return;
	const int right = min(x + halfSize + ((brush.size % 2 == 0) ? 0 : 1), canvas->width);
	if (right < 0) return;
	const int bottom = min(y + halfSize + ((brush.size % 2 == 0) ? 0 : 1), canvas->height);
	if (bottom < 0) return;

	float evenOffset = (float)((brush.size % 2 == 0) ? 0.5 : 0);
	double distance;
	for (int i = top; i < bottom; i++) {
		for (int j = left; j < right; j++) {
			distance = Vector2Distance((Vector2){ x-evenOffset, y-evenOffset }, (Vector2){ (float)j, (float)i });
			if (distance <= radius) {
				int r, g, b, a;
				size_t dest = i * canvas->width + j;
				if (!drawnPixels[dest]) {
					if (brush.type == PENCIL) {
						Color dstColor = canvas->buffer[dest];
						double normalizedAlpha = (double)brush.color.a / 255;
						r = (int)(brush.color.r * normalizedAlpha + dstColor.r * (1 - normalizedAlpha));
						g = (int)(brush.color.g * normalizedAlpha + dstColor.g * (1 - normalizedAlpha));
						b = (int)(brush.color.b * normalizedAlpha + dstColor.b * (1 - normalizedAlpha)); 
						a = (int)(normalizedAlpha + ((double)dstColor.a / 255 * (1 - normalizedAlpha))) * 255;
					}
					else {
						r = g = b = a = 0;
					}
					canvas->buffer[i * canvas->width + j] = (Color){ r, g, b, a };
					drawnPixels[dest] = true;
				}
			}
		}
	}
}

void tryDrawToCanvas(const Viewport *viewport,const Canvas *canvas, const Brush brush) {
	static int prevMouseX = 0;
	static int prevMouseY = 0;

	if (IsMouseButtonDown(MOUSE_BUTTON_LEFT) && !hoveringGUI) {
		Vector2 mousePos = GetMousePosition();
		double widthRatio = (double)canvas->width / (double)viewport->canvasWidth;
		double heightRatio = (double)canvas->height / (double)viewport->canvasHeight;
		int mouseX = (int)((mousePos.x - viewport->canvasX - viewport->x) * widthRatio);
		int mouseY = (int)((mousePos.y - viewport->canvasY - viewport->y) * heightRatio);

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
