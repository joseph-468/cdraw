#include <raylib.h>
#include <stdlib.h>
#include <string.h>

#include "../include/canvas.h"
#include "../include/gui.h"
#include "../include/font.h"
#include "../include/icon.h"

int main() {
	// Initilization
	SetConfigFlags(FLAG_WINDOW_RESIZABLE);
	InitWindow(screenWidth, screenHeight, "Paint");
	SetTargetFPS(360);
	BeginBlendMode(BLEND_ALPHA);

	setCurrentFont(jetBrainsMonoMedium);
	Canvas canvas = createBlankCanvas(319, 159, screenWidth - 481, screenHeight - 321);
	Brush brush = {
		.type = PENCIL,
		.shape = SQUARE,
		.color = (Color) { 0, 0, 0, 255 },
		.size = 8,
	};

	const unsigned char *shapeIcons[2] = { squareIcon, circleIcon };
	const int shapeIconLens[2] = { squareIconLen, circleIconLen };
	RadioButtons brushShapeButtons = createRadioButtons(50, 100, 32, 32, 2, shapeIcons, shapeIconLens);

	const unsigned char *typeIcons[2] = { pencilIcon, eraserIcon };
	const int typeIconLens[2] = { pencilIconLen, eraserIconLen };
	RadioButtons brushTypeButtons = createRadioButtons(150, 100, 32, 32, 2, typeIcons, typeIconLens);

	TextInput brushSizeBox = createTextInput(50, 175, 70, 30, "Brush Size", "8", isValidBrushSize);
	TextInput redBox = createTextInput(50, 225, 70, 30, "Red", "0", isValidColor);
	TextInput greenBox = createTextInput(50, 275, 70, 30, "Green", "0", isValidColor);
	TextInput blueBox = createTextInput(50, 325, 70, 30, "Blue", "0", isValidColor);
	TextInput alphaBox = createTextInput(50, 375, 70, 30, "Alpha", "255", isValidColor);

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

		if (GetMouseWheelMoveV().y && !hoveringGUI) {
			Vector2 cursorPos = GetMousePosition();
			double widthRatio = (double)canvas.width / (double)canvas.height;
			double x = widthRatio * 10 * GetMouseWheelMoveV().y;
			double y = 10 * GetMouseWheelMoveV().y;
			canvas.viewWidth += x;
			canvas.viewHeight += y;
			canvas.x = 319 + (double)(screenWidth-481 - canvas.viewWidth) / 2;
			canvas.y = 159 + (double)(screenHeight-321 - canvas.viewHeight) / 2;
		}

		// Update canvas buffer
		tryDrawToCanvas(&canvas, brush);

		// Start rendering to screen
		BeginDrawing();

		// Update GUI
		hoveringGUI = false;
		drawCoordinates(&canvas);

		brush.shape = checkRadioButtons(&brushShapeButtons);
		drawRadioButtons(&brushShapeButtons);

		brush.type = checkRadioButtons(&brushTypeButtons);
		drawRadioButtons(&brushTypeButtons);

		checkTextInput(&brushSizeBox);
		drawTextInput(&brushSizeBox);

		checkTextInput(&redBox);
		drawTextInput(&redBox);
		checkTextInput(&greenBox);
		drawTextInput(&greenBox);
		checkTextInput(&blueBox);
		drawTextInput(&blueBox);
		checkTextInput(&alphaBox);
		drawTextInput(&alphaBox);

		brush.size = atoi(brushSizeBox.text);
		brush.color.r = atoi(redBox.text);
		brush.color.g = atoi(greenBox.text);
		brush.color.b = atoi(blueBox.text);
		brush.color.a = atoi(alphaBox.text);

		// Draw canvas to screen
		DrawRectangleLines(318, 158, screenWidth - 479, screenHeight - 319, BLACK);
		renderCanvas(&canvas);

		EndDrawing();
	}

	// De-Initialization
	CloseWindow();
	return 0;
}