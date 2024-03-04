#include <raylib.h>
#include <raymath.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#include "../include/canvas.h"
#include "../include/gui.h"
#include "../include/font.h"

const int screenWidth = 1280;
const int screenHeight = 720;
static Texture canvasTexture;

bool drawing = false;
bool *drawnPixels;

Canvas createBlankCanvas(const double x, const double y, const int width, const int height) {
	Canvas canvas = {
		.x = x,
		.y = y,
		.width = width,
		.height = height,
		.viewWidth = width,
		.viewHeight = height,
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

void renderCanvas(const Canvas *canvas) {
	ClearBackground(GRAY);
	UpdateTexture(canvasTexture, canvas->buffer);
	Rectangle source = { 0, 0, (float)canvasTexture.width, (float)canvasTexture.height };
	Rectangle dest = { (float)canvas->x, (float)canvas->y, (float)canvas->viewWidth, (float)canvas->viewHeight };
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

	size_t dest;
	int r, g, b, a;
	for (int i = top; i < bottom; i++) {
		for (int j = left; j < right; j++) {
			dest = i * canvas->width + j;
			if (!drawnPixels[dest]) {
				if (brush.type == PENCIL) {
					Color dstColor = canvas->buffer[dest];
					double normalizedAlpha = (double)brush.color.a / 255;
					r = brush.color.r * normalizedAlpha + dstColor.r * (1 - normalizedAlpha);
					g = brush.color.g * normalizedAlpha + dstColor.g * (1 - normalizedAlpha);
					b = brush.color.b * normalizedAlpha + dstColor.b * (1 - normalizedAlpha); 
					a = (normalizedAlpha + ((double)dstColor.a / 255 * (1 - normalizedAlpha))) * 255;
				}
				else if (brush.type == ERASER) {
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

	float evenOffset = ((brush.size % 2 == 0) ? 0.5 : 0);
	double distance;
	size_t dest;
	int r, g, b, a;
	for (int i = top; i < bottom; i++) {
		for (int j = left; j < right; j++) {
			distance = Vector2Distance((Vector2){ x-evenOffset, y-evenOffset }, (Vector2){ j, i });
			if (distance <= radius) {
				dest = i * canvas->width + j;
				if (!drawnPixels[dest]) {
					if (brush.type == PENCIL) {
						Color dstColor = canvas->buffer[dest];
						double normalizedAlpha = (double)brush.color.a / 255;
						r = brush.color.r * normalizedAlpha + dstColor.r * (1 - normalizedAlpha);
						g = brush.color.g * normalizedAlpha + dstColor.g * (1 - normalizedAlpha);
						b = brush.color.b * normalizedAlpha + dstColor.b * (1 - normalizedAlpha); 
						a = (normalizedAlpha + ((double)dstColor.a / 255 * (1 - normalizedAlpha))) * 255;
					}
					else if (brush.type == ERASER) {
						r = g = b = a = 0;
					}
					canvas->buffer[i * canvas->width + j] = (Color){ r, g, b, a };
					drawnPixels[dest] = true;
				}
			}
		}
	}
}

void tryDrawToCanvas(const Canvas *canvas, const Brush brush) {
	static int prevMouseX = 0;
	static int prevMouseY = 0;

	if (IsMouseButtonDown(MOUSE_BUTTON_LEFT) && !hoveringGUI) {
		Vector2 mousePos = GetMousePosition();
		double widthRatio = (double)canvas->width / (double)canvas->viewWidth;
		double heightRatio = (double)canvas->height / (double)canvas->viewHeight;
		int mouseX = (int)((mousePos.x - canvas->x) * widthRatio);
		int mouseY = (int)((mousePos.y - canvas->y) * heightRatio);
		int topLeft = (mouseY - brush.size/2) * screenWidth + (mouseX - brush.size/2);

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
