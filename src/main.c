#include <raylib.h>
#include <stdlib.h>
#include <string.h>

#include "../include/canvas.h"
#include "../include/gui.h"
#include "../include/font.h"

int main() {
	// Initilization
	SetConfigFlags(FLAG_WINDOW_RESIZABLE);
	InitWindow(screenWidth, screenHeight, "Paint");
	SetTargetFPS(0);
	BeginBlendMode(BLEND_ALPHA);

	setCurrentFont(jetBrainsMonoMedium);
	Canvas canvas = createBlankCanvas(319, 159, screenWidth - 481, screenHeight - 321);
	Brush brush = {
		.type = PENCIL,
		.shape = SQUARE,
		.color = (Color) { 0, 0, 0, 255 },
		.size = 8,
	};

	const char* icons[2] = { "assets/square.png", "assets/circle.png" };
	RadioButtons brushTypeButtons = createRadioButtons(48, 48, 32, 32, 2, icons, switchBrushType);
	TextInput brushSizeBox = createTextInput(200, 100, 70, 30, "Brush Size", "8", isValidBrushSize);
	TextInput redBox = createTextInput(50, 100, 70, 30, "red", "0", isValidColor);
	TextInput greenBox = createTextInput(50, 150, 70, 30, "green", "0", isValidColor);
	TextInput blueBox = createTextInput(50, 200, 70, 30, "blue", "0", isValidColor);

	// Main loop
	while (!WindowShouldClose()) {
		if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)) {
			Image image = GenImageColor(canvas.width, canvas.height, (Color){ 0, 0, 0 });
			for (int i = 0; i < canvas.height; i++) {
				for (int j = 0; j < canvas.width; j++) {
					ImageDrawPixel(&image, j, i, canvas.buffer[i * canvas.width + j]);
				}
			}
			ExportImage(image, "image.png");
		}

		// Update canvas buffer
		tryDrawToCanvas(canvas, brush, IsMouseButtonDown(MOUSE_BUTTON_LEFT));

		// Start rendering to screen
		BeginDrawing();

		// Update GUI
		drawCoordinates(canvas);

		brush.shape = checkRadioButtons(&brushTypeButtons);
		drawRadioButtons(&brushTypeButtons);

		checkTextInput(&brushSizeBox);
		drawTextInput(&brushSizeBox);

		checkTextInput(&redBox);
		drawTextInput(&redBox);
		checkTextInput(&greenBox);
		drawTextInput(&greenBox);
		checkTextInput(&blueBox);
		drawTextInput(&blueBox);

		brush.size = atoi(brushSizeBox.text);
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