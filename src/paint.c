#include <raylib.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#include "../include/paint.h"
#include "../include/gui.h"
#include "../include/font.h"

const int screenWidth = 1280;
const int screenHeight = 720;
static Texture canvasTexture;

void init() {
	SetConfigFlags(FLAG_WINDOW_RESIZABLE);
	InitWindow(screenWidth, screenHeight, "Paint");
	SetTargetFPS(0);
	currentFont = GetFontDefault();
}

Canvas createBlankCanvas(const int x, const int y, const int width, const int height) {
	Canvas canvas = {
		.x = x,
		.y = y,
		.width = width,
		.height = height,
		.viewWidth = width,
		.viewHeight = height,
		.buffer = NULL,
	};
	Image tempImage = GenImageColor(width, height, WHITE);
	canvasTexture = LoadTextureFromImage(tempImage);
    UnloadImage(tempImage);
	Color* canvasBuffer = (Color*)malloc(width * height * sizeof(Color));
	HANDLE_MALLOC_FAILURE(canvasBuffer);
	memset(canvasBuffer, 255, width * height * sizeof(Color));
	canvas.buffer = canvasBuffer;
	return canvas;
}

void renderCanvas(const Canvas canvas) {
	ClearBackground(GRAY);
	UpdateTexture(canvasTexture, canvas.buffer);
	Rectangle source = { 0, 0, canvasTexture.width, canvasTexture.height };
	Rectangle dest = { canvas.x, canvas.y, canvas.viewWidth, canvas.viewHeight };
	DrawTexturePro(canvasTexture, source, dest, (Vector2){ 0, 0 }, 0, WHITE);
}

void drawSquare(const Canvas canvas, const Brush brush, const int x, const int y) {
	const int halfSize = brush.size / 2;
	const int left = max(x - halfSize, 0);
	if (left > canvas.width-1) return;
	const int top = max(y - halfSize, 0);
	if (top > canvas.height-1) return;
	const int right = min(x + halfSize + ((brush.size % 2 == 0) ? 0 : 1), canvas.width);
	if (right < 0) return;
	const int bottom = min(y + halfSize + ((brush.size % 2 == 0) ? 0 : 1), canvas.height);
	if (bottom < 0) return;

	for (int i = top; i < bottom; i++) {
		for (int j = left; j < right; j++) {
			canvas.buffer[i * canvas.width + j] = brush.color;
		}
	}
}

void drawLine(const Canvas canvas, const Brush brush, bool shouldDraw) {
	static bool drawing = false;
	static int prevMouseX = 0;
	static int prevMouseY = 0;

	if (shouldDraw) {
		Vector2 mousePos = GetMousePosition();
		double widthRatio = (double)canvas.width / (double)canvas.viewWidth;
		double heightRatio = (double)canvas.height / (double)canvas.viewHeight;
		int mouseX = (mousePos.x - canvas.x) * widthRatio;
		int mouseY = (mousePos.y - canvas.y) * heightRatio;
		int topLeft = (mouseY - brush.size/2) * screenWidth + (mouseX - brush.size/2);

		if (drawing) {
			int dx = prevMouseX - mouseX;
			int dy = prevMouseY - mouseY;
			int steps = abs(dx) > abs(dy) ? abs(dx) : abs(dy);
			float xInc = (float)dx / (float)steps;
			float yInc = (float)dy / (float)steps;
			float x = mouseX;
			float y = mouseY;
			for (int i = 0; i <= steps; i++) {
				drawSquare(canvas, brush, (int)roundf(x), (int)roundf(y));
				x += xInc;
				y += yInc;
			}
		}

		drawing = true;
		prevMouseX = mouseX;
		prevMouseY = mouseY;
	}
	else {
		drawing = false;
	}
}
