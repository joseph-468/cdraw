#include <raylib.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#include "../include/paint.h"

int main() {
	// Initilization
	init();
	Canvas canvas = createBlankCanvas(screenWidth, screenHeight);

	const int brushWidth = 3;
	bool drawing = false;
	Vector2 prevMousePos = { 0, 0 };

	// Main loop
	while (!WindowShouldClose()) {
		// Update
		if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
			Vector2 mousePos = GetMousePosition();
			int topLeft = ((int)mousePos.y - brushWidth/2) * screenWidth + ((int)mousePos.x - brushWidth/2);
			if (drawing) {
				int dx = prevMousePos.x - mousePos.x;
				int dy = prevMousePos.y - mousePos.y;
				int steps = abs(dx) > abs(dy) ? abs(dx) : abs(dy);
				float xInc = (float)dx / (float)steps;
				float yInc = (float)dy / (float)steps;

				float x = mousePos.x;
				float y = mousePos.y;
				for (int i = 0; i <= steps; i++) {
					drawSquare(canvas, 5, (int)roundf(x), (int)roundf(y));
					x += xInc;
					y += yInc;
				}
			}
			drawing = true;
			prevMousePos = mousePos;
		}
		else {
			drawing = false;
		}

		// Draw
		BeginDrawing();
		renderCanvas(canvas);
		EndDrawing();
	}

	// De-Initialization
	CloseWindow();
	return 0;
}