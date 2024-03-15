#include <raylib.h>
#include <raymath.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdint.h>

#include "../include/canvas.h"
#include "../include/gui.h"
#include "../include/font.h"

#define DISTANCE_FROM_WIDTH 480;
#define DISTANCE_FROM_HEIGHT 270;
#define MIN_WIDTH 854
#define MIN_HEIGHT 480 

static Texture backgroundTexture;
static Texture canvasTexture;

bool drawing = false;
bool *drawnPixels;

Viewport createViewport(Canvas *canvas, const float x, const float y) {
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
	float widthRatio = (float)viewport->canvas->width / viewport->canvasWidth;
	float heightRatio = (float)viewport->canvas->height / viewport->canvasHeight;
	Vector2 relativePos;
	relativePos.x = (mousePos.x - viewport->canvasX - viewport->x) * widthRatio;
	relativePos.y = (mousePos.y - viewport->canvasY - viewport->y) * heightRatio;

	return relativePos;
}

void keepMinimumWindowSize() {
	if (IsWindowResized()) {
		int64_t width = GetScreenWidth();
		int64_t height = GetScreenHeight();
		if (width < MIN_WIDTH || height < MIN_HEIGHT) {
			SetWindowSize((width < MIN_WIDTH) ? MIN_WIDTH : width, (height < MIN_HEIGHT) ? MIN_HEIGHT : height);
		}
	}
}

void handleCanvasZoom(Viewport *viewport) {
	if (GetMouseWheelMoveV().y && !hoveringGUI) {
		float zoomFactor = max(10, viewport->width / 50);
		float widthRatio = (float)viewport->canvas->width / (float)viewport->canvas->height;
		float heightRatio = (float)viewport->canvas->height / (float)viewport->canvas->width;
		float x, y;
		if (widthRatio < heightRatio) {
			x = widthRatio * zoomFactor * GetMouseWheelMoveV().y;
			y = zoomFactor * GetMouseWheelMoveV().y;
		}
		else {
			x = zoomFactor * GetMouseWheelMoveV().y;
			y = heightRatio * zoomFactor * GetMouseWheelMoveV().y;
		}
		if (viewport->canvasWidth + x > widthRatio && viewport->canvasWidth + x > heightRatio ||
			viewport->canvasHeight + y > widthRatio && viewport->canvasHeight + y > heightRatio) {
			viewport->canvasWidth += x;
			viewport->canvasHeight += y;
			viewport->canvasX = (float)(viewport->width - viewport->canvasWidth) / 2;
			viewport->canvasY = (float)(viewport->height - viewport->canvasHeight) / 2;
		}
	}
}

void resizeViewport(Viewport* viewport) {
	float prevWidth = viewport->width;
	float prevHeight = viewport->height;
	viewport->width = GetScreenWidth() - DISTANCE_FROM_WIDTH;
	viewport->height = GetScreenHeight() - DISTANCE_FROM_HEIGHT;

	float widthIncrease = viewport->width / prevWidth;
	float heightIncrease = viewport->height / prevHeight;
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
		float newScale = viewport->width / viewport->canvasWidth;
		viewport->canvasWidth *= newScale;
		viewport->canvasHeight *= newScale;
	}
	if (viewport->canvasHeight > viewport->height) {
		float newScale = viewport->height / viewport->canvasHeight;
		viewport->canvasWidth *= newScale;
		viewport->canvasHeight *= newScale;
	}

	float widthRatio = (float)viewport->canvas->width / (float)viewport->canvas->height;
	viewport->canvasX = (float)(viewport->width - viewport->canvasWidth) / 2;
	viewport->canvasY = (float)(viewport->height - viewport->canvasHeight) / 2;
}

Canvas createBlankCanvas(const int64_t width, const int64_t height) {
	Canvas canvas = {
		.width = width,
		.height = height,
		.buffer = NULL,
	};

	if (drawnPixels ) free(drawnPixels);
	drawnPixels = (bool*)malloc(width * height * sizeof(bool));
	HANDLE_MALLOC_FAILURE(drawnPixels);
	memset(drawnPixels, false, width * height * sizeof(bool));

	Image tempImage = GenImageColor(width, height, WHITE);
	UnloadTexture(canvasTexture);
	canvasTexture = LoadTextureFromImage(tempImage);

	canvas.buffer = (Color*)malloc(width * height * sizeof(Color));
	HANDLE_MALLOC_FAILURE(canvas.buffer);
	memset(canvas.buffer, 255, width * height * sizeof(Color));

	// Background stuff
	UnloadTexture(backgroundTexture);
	backgroundTexture = LoadTextureFromImage(tempImage);
	UnloadImage(tempImage);
	Color* backgroundBuffer = (Color*)malloc(width * height * sizeof(Color));
	HANDLE_MALLOC_FAILURE(backgroundBuffer);
	memset(backgroundBuffer, 255, width * height * sizeof(Color));

	for (int64_t y = 0; y < height; y++) {
		for (int64_t x = 0; x < width; x++) {
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

bool saveImage(const Canvas *canvas, const char *filename) {
	Image image = GenImageColor(canvas->width, canvas->height, (Color){ 0, 0, 0 });
	// There's probably a more effecient way to do this
	for (int64_t i = 0; i < canvas->height; i++) {
		for (int64_t j = 0; j < canvas->width; j++) {
			ImageDrawPixel(&image, j, i, canvas->buffer[i * canvas->width + j]);
		}
	}
	const char* finalFilename = calloc(strlen(filename) + 6, sizeof(char));
	HANDLE_MALLOC_FAILURE(finalFilename);
	strcpy(finalFilename, filename);
	strcpy(finalFilename + strlen(filename), ".png");
	bool success = ExportImage(image, finalFilename);
	UnloadImage(image);
	return success;
}

void renderCanvas(const Viewport *viewport) {
	Rectangle source = { 0, 0, canvasTexture.width, canvasTexture.height };
	Rectangle dest = { viewport->x + viewport->canvasX, viewport->y + viewport->canvasY, viewport->canvasWidth, viewport->canvasHeight };
	UpdateTexture(canvasTexture, viewport->canvas->buffer);
	DrawTexturePro(backgroundTexture, source, dest, (Vector2){ 0, 0 }, 0, WHITE);
	DrawTexturePro(canvasTexture, source, dest, (Vector2){ 0, 0 }, 0, WHITE);
	DrawRectangleLines(viewport->x-1, viewport->y-1, viewport->width+2, viewport->height+2, BLACK);
}

void drawSquare(const Canvas *canvas, const Brush brush, const int64_t x, const int64_t y) {
	const int64_t halfSize = brush.size / 2;
	const int64_t left = max(x - halfSize, 0);
	if (left > canvas->width-1) return;
	const int64_t top = max(y - halfSize, 0);
	if (top > canvas->height-1) return;
	const int64_t right = min(x + halfSize + ((brush.size % 2 == 0) ? 0 : 1), canvas->width);
	if (right < 0) return;
	const int64_t bottom = min(y + halfSize + ((brush.size % 2 == 0) ? 0 : 1), canvas->height);
	if (bottom < 0) return;

	for (int64_t i = top; i < bottom; i++) {
		for (int64_t j = left; j < right; j++) {
			size_t dest = i * canvas->width + j;
			if (drawnPixels[dest]) continue;
			uint8_t r = 0, g = 0, b = 0, a = 0;
			if (brush.type == PENCIL) {
				Color dstColor = canvas->buffer[dest];
				float normalizedAlpha = (float)brush.color.a / 255;
				r = (uint8_t)(brush.color.r * normalizedAlpha + dstColor.r * (1 - normalizedAlpha));
				g = (uint8_t)(brush.color.g * normalizedAlpha + dstColor.g * (1 - normalizedAlpha));
				b = (uint8_t)(brush.color.b * normalizedAlpha + dstColor.b * (1 - normalizedAlpha)); 
				a = (uint8_t)((normalizedAlpha + ((float)dstColor.a / 255) * (1 - normalizedAlpha)) * 255);

			}
			canvas->buffer[i * canvas->width + j] = (Color){ r, g, b, a };
			drawnPixels[dest] = true;
		}
	}
}

void drawCircle(const Canvas *canvas, const Brush brush, int64_t x, int64_t y) {
	static bool* shapeCache = NULL;
	static int64_t shapeCacheSize = 0;

	float evenOffset = (float)((brush.size % 2 == 0) ? 0.5 : 0);
	const int64_t halfSize = brush.size / 2;
	const float radius = (float)brush.size / 2;
	const int64_t left = x - halfSize;
	if (left > canvas->width-1) return;
	const int64_t top = y - halfSize;
	if (top > canvas->height-1) return;
	const int64_t right = min(x + halfSize + ((brush.size % 2 == 0) ? 0 : 1), canvas->width);
	if (right < 0) return;
	const int64_t bottom = min(y + halfSize + ((brush.size % 2 == 0) ? 0 : 1), canvas->height);
	if (bottom < 0) return;
	float distance;

	// Calculate pixels that should be drawn in circle and cache it
	if (shapeCacheSize != brush.size) {
		if (shapeCache) free(shapeCache);
		shapeCache = calloc(brush.size * brush.size, sizeof(bool));
		HANDLE_MALLOC_FAILURE(shapeCache);
		shapeCacheSize = brush.size;
		for (int64_t i = 0; i < brush.size; i++) {
			for (int64_t j = 0; j < brush.size; j++) {
				distance = Vector2Distance((Vector2) { halfSize - evenOffset, halfSize - evenOffset }, (Vector2) { (float)j, (float)i });
				if (distance <= radius) {
					shapeCache[i * brush.size + j] = true;
				}
			}
		}
	}
	else if (shapeCacheSize == brush.size) {
		for (int64_t i = top; i < bottom; i++) {
			for (int64_t j = left; j < right; j++) {
				if (i < 0 || j < 0) continue;
				size_t dest = i * canvas->width + j;
				if (!shapeCache[(i-top) * brush.size + (j-left)]) continue;
				if (drawnPixels[dest]) continue;
				uint8_t r = 0, g = 0, b = 0, a = 0;
				if (brush.type == PENCIL) {
					Color dstColor = canvas->buffer[dest];
					float normalizedAlpha = (float)brush.color.a / 255;
					r = (uint8_t)(brush.color.r * normalizedAlpha + dstColor.r * (1 - normalizedAlpha));
					g = (uint8_t)(brush.color.g * normalizedAlpha + dstColor.g * (1 - normalizedAlpha));
					b = (uint8_t)(brush.color.b * normalizedAlpha + dstColor.b * (1 - normalizedAlpha));
					a = (uint8_t)((normalizedAlpha + ((float)dstColor.a / 255) * (1 - normalizedAlpha)) * 255);
				}
				canvas->buffer[i * canvas->width + j] = (Color){ r, g, b, a };
				drawnPixels[dest] = true;
			}
		}
	}
}

void tryDrawToCanvas(const Viewport *viewport, const Brush brush) {
	static bool justPressedGUI = false;
	static int64_t prevMouseX = 0;
	static int64_t prevMouseY = 0;

	if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
		if (hoveringGUI) justPressedGUI = true;
	}
	else if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
		justPressedGUI = false;
	}
	if (IsMouseButtonDown(MOUSE_BUTTON_LEFT) && !hoveringGUI && !justPressedGUI) {
		int64_t mouseX = getRelativePos(viewport).x;
		int64_t mouseY = getRelativePos(viewport).y;

		if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
			drawing = true;
			prevMouseX = mouseX;
			prevMouseY = mouseY;
		}
		if (drawing) {
			int64_t dx = prevMouseX - mouseX;
			int64_t dy = prevMouseY - mouseY;
			int64_t steps = abs(dx) > abs(dy) ? abs(dx) : abs(dy);
			float xInc = (float)dx / (float)steps;
			float yInc = (float)dy / (float)steps;
			float x = (float)mouseX;
			float y = (float)mouseY;
			for (int64_t i = 0; i <= steps; i++) {
				if (brush.shape == SQUARE) {
					drawSquare(viewport->canvas, brush, (int64_t)roundf(x), (int64_t)roundf(y));
				}
				if (brush.shape == CIRCLE) {
					drawCircle(viewport->canvas, brush, (int64_t)roundf(x), (int64_t)roundf(y));
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
			memset(drawnPixels, false, viewport->canvas->width * viewport->canvas->height * sizeof(bool));
		}
		drawing = false;
	}
}
