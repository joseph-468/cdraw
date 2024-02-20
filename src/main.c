#include <raylib.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "../include/paint.h"

int main() {
	// Initilization
	init();
	Canvas canvas = createBlankCanvas(319, 159, screenWidth - 481, screenHeight-321);

	Brush brush = {
		.type = PENCIL,
		.shape = SQUARE,
		.color = (Color) { 0, 255, 255, 255 },
		.size = 9,
	};

	double ratio = (double)screenHeight / (double)screenWidth;

	// Main loop
	while (!WindowShouldClose()) {
		// Update
		drawLine(canvas, brush, IsMouseButtonDown(MOUSE_BUTTON_LEFT));

		// Draw
		BeginDrawing();
		renderCanvas(canvas);

		Vector2 mousePos = GetMousePosition();
		double widthRatio = (double)canvas.width / (double)canvas.viewWidth;
		double heightRatio = (double)canvas.height / (double)canvas.viewHeight;
		int mouseX = (mousePos.x - canvas.x) * widthRatio;
		int mouseY = (mousePos.y - canvas.y) * heightRatio;
		DrawText(TextFormat("X: %i Y: %i", mouseX, mouseY), 319, screenHeight - 161, 20, BLACK);

		DrawRectangleLines(318, 158, screenWidth - 479, screenHeight - 319, BLACK);

		EndDrawing();
	}

	// De-Initialization
	CloseWindow();
	return 0;
}