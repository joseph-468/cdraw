#include <raylib.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "../include/paint.h"
#include "../include/gui.h"
#include "../include/font.h"

int main() {
	// Initilization
	init();

	Canvas canvas = createBlankCanvas(319, 159, screenWidth - 481, screenHeight - 321);

	Brush brush = {
		.type = PENCIL,
		.shape = SQUARE,
		.color = (Color) { 0, 0, 0, 255 },
		.size = 9,
	};


	Font font = setCurrentFont(jetBrainsMonoMedium);
	TextInput redBox = createTextInput(50, 100, 60, 30, "red", "0", isValidColor);
	TextInput greenBox = createTextInput(50, 150, 60, 30, "green", "0", isValidColor);
	TextInput blueBox = createTextInput(50, 200, 60, 30, "blue", "0", isValidColor);

	// Main loop
	while (!WindowShouldClose()) {
		// Update canvas buffer
		drawLine(canvas, brush, IsMouseButtonDown(MOUSE_BUTTON_LEFT));

		// Start rendering to screen
		BeginDrawing();

		// Calculate and display coordinates
		Vector2 mousePos = GetMousePosition();
		double widthRatio = (double)canvas.width / (double)canvas.viewWidth;
		double heightRatio = (double)canvas.height / (double)canvas.viewHeight;
		int mouseX = (mousePos.x - canvas.x) * widthRatio;
		int mouseY = (mousePos.y - canvas.y) * heightRatio;
		DrawTextEx(font, TextFormat("X: %i Y: %i", mouseX, mouseY), (Vector2){319, screenHeight - 161}, 32, 0, BLACK);

		// Update all other UI elements
		getTextInput(&redBox);
		drawTextInput(&redBox);
		getTextInput(&greenBox);
		drawTextInput(&greenBox);
		getTextInput(&blueBox);
		drawTextInput(&blueBox);

		brush.color.r = atoi(redBox.text);
		brush.color.g = atoi(greenBox.text);
		brush.color.b = atoi(blueBox.text);

		// Draw canvas to screen
		DrawRectangleLines(318, 158, screenWidth - 479, screenHeight - 319, BLACK);
		renderCanvas(canvas);

		EndDrawing();
	}

	// De-Initialization
	CloseWindow();
	return 0;
}