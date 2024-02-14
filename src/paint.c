#include <raylib.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "../include/paint.h"

const int screenWidth = 1024;
const int screenHeight = 576;
static Texture canvasTexture;

void init() {
	SetConfigFlags(FLAG_WINDOW_RESIZABLE);
	InitWindow(screenWidth, screenHeight, "Paint");
	SetTargetFPS(0);
}

Canvas createBlankCanvas(const int width, const int height) {
	Canvas canvas = {
		.width = width,
		.height = height,
		.buffer = NULL,
	};
	Image tempImage = GenImageColor(width, height, WHITE);
	canvasTexture = LoadTextureFromImage(tempImage);
    UnloadImage(tempImage);
	Color* canvasBuffer = (Color*)malloc(width * height * sizeof(Color));
	HANDLE_MALLOC_FAILURE(canvasBuffer);
	canvas.buffer = canvasBuffer;
	return canvas;
}

void renderCanvas(const Canvas canvas) {
	ClearBackground(WHITE);
	UpdateTexture(canvasTexture, canvas.buffer);
	Rectangle source = { 0, 0, canvasTexture.width, canvasTexture.height };
	Rectangle dest = { 0, 0, canvasTexture.width, canvasTexture.height };
	DrawTexturePro(canvasTexture, source, dest, (Vector2){ 0, 0 }, 0, WHITE);
}

void drawSquare(const Canvas canvas, const int size, const int x, const int y) {
	const int left = x - size / 2;
	const int top = y - size / 2;
	const unsigned int offset = top * canvas.width + left;
	// Can probably be optimized by clamping i, j, and their respective max
	for (int i = 0; i < size; i++) {
		for (int j = 0; j < size; j++) {
			if (left + j < 0 || left + j >= canvas.width) continue;
			if (top + i < 0 || top + i >= canvas.height) continue;
			canvas.buffer[offset + i * canvas.width + j] = (Color){ 0, 255, 0, 255 };
		}
	}
}
