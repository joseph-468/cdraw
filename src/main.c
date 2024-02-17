#include <raylib.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "../include/paint.h"

int main() {
	// Initilization
	init();
	Canvas canvas = createBlankCanvas(0, 0, screenWidth, screenHeight);

	Brush brush = {
		.type = PENCIL,
		.shape = SQUARE,
		.color = (Color) { 0, 255, 255, 255 },
		.size = 9,
	};

	double ratio = (double)screenHeight / (double)screenWidth;
	printf("%d", screenWidth);

	// Main loop
	while (!WindowShouldClose()) {
		// Update
		drawLine(canvas, brush, IsMouseButtonDown(MOUSE_BUTTON_LEFT));

		if (IsKeyDown(KEY_LEFT)) {
			canvas.viewWidth -= 2;
			canvas.viewHeight = canvas.viewWidth * ratio;
		}
		else if (IsKeyDown(KEY_RIGHT)) {
			canvas.viewWidth += 2;
			canvas.viewHeight = canvas.viewWidth * ratio;
		}

		// Draw
		BeginDrawing();
		renderCanvas(canvas, 0, 0);
		EndDrawing();
	}

	// De-Initialization
	CloseWindow();
	return 0;
}