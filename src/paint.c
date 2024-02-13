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

Color* createBlankCanvas(const int width, const int height) {
	Image tempImage = GenImageColor(width, height, WHITE);
	canvasTexture = LoadTextureFromImage(tempImage);
    UnloadImage(tempImage);
	Color* canvasBuffer = (Color*)malloc(width * height * sizeof(Color));
	HANDLE_MALLOC_FAILURE(canvasBuffer);
	return canvasBuffer;
}

void renderCanvas(const Color *canvasBuffer) {
	ClearBackground(WHITE);
	UpdateTexture(canvasTexture, canvasBuffer);
	Rectangle source = { 0, 0, canvasTexture.width, canvasTexture.height };
	Rectangle dest = { 0, 0, canvasTexture.width, canvasTexture.height };
	DrawTexturePro(canvasTexture, source, dest, (Vector2){ 0, 0 }, 0, WHITE);
}
