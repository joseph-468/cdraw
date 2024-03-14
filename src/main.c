#include <raylib.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "../include/canvas.h"
#include "../include/gui.h"
#include "../include/font.h"
#include "../include/icon.h"

#define DEFAULT_WIDTH 1024
#define DEFAULT_HEIGHT 576

int main() {
	// Initilization
	SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_VSYNC_HINT);
	InitWindow(DEFAULT_WIDTH, DEFAULT_HEIGHT, "Paint");
	SetTargetFPS(360);

	setCurrentFont(jetBrainsMonoMedium);
	Canvas canvas = createBlankCanvas(1088, 612);
	Viewport viewport = createViewport(&canvas, 319, 119);
	Brush brush = {
		.type = PENCIL,
		.shape = SQUARE,
		.color = (Color) { 0, 0, 0, 255 },
		.size = 8,
	};

	TextButton saveButton = createTextButton(10, 10, 100, 30, "Save");

	const unsigned char *shapeIcons[2] = { squareIcon, circleIcon };
	const int shapeIconLens[2] = { squareIconLen, circleIconLen };
	RadioButtons brushShapeButtons = createRadioButtons(50, 119, 32, 32, 2, shapeIcons, shapeIconLens);

	const unsigned char *typeIcons[2] = { pencilIcon, eraserIcon };
	const int typeIconLens[2] = { pencilIconLen, eraserIconLen };
	RadioButtons brushTypeButtons = createRadioButtons(150, 119, 32, 32, 2, typeIcons, typeIconLens);

	TextInput brushSizeBox = createTextInput(50, 219, 70, 30, "Brush Size", "8", isValidBrushSize);
	TextInput redBox = createTextInput(50, 269, 70, 30, "Red", "0", isValidColor);
	TextInput greenBox = createTextInput(50, 319, 70, 30, "Green", "0", isValidColor);
	TextInput blueBox = createTextInput(50, 369, 70, 30, "Blue", "0", isValidColor);
	TextInput alphaBox = createTextInput(50, 419, 70, 30, "Alpha", "255", isValidColor);

	// Main loop
	while (!WindowShouldClose()) {
		if (GetMouseWheelMoveV().y && !hoveringGUI) {
			double widthRatio = (double)canvas.width / (double)canvas.height;
			double heightRatio = (double)canvas.height / (double)canvas.width;
			double x, y;
			if (widthRatio < heightRatio) {
				x = widthRatio * 10 * GetMouseWheelMoveV().y;
				y = 10 * GetMouseWheelMoveV().y;
			}
			else {
				x = 10 * GetMouseWheelMoveV().y;
				y = heightRatio * 10 * GetMouseWheelMoveV().y;
			}
			if (viewport.canvasWidth + x > widthRatio && viewport.canvasWidth + x > heightRatio ||
				viewport.canvasHeight + y > widthRatio && viewport.canvasHeight + y > heightRatio) {
				viewport.canvasWidth += x;
				viewport.canvasHeight += y;
				viewport.canvasX = (double)(viewport.width - viewport.canvasWidth) / 2;
				viewport.canvasY = (double)(viewport.height - viewport.canvasHeight) / 2;
			}
		}

		// Update canvas buffer
		tryDrawToCanvas(&viewport, &canvas, brush);

		// Start rendering to screen
		BeginDrawing();
		ClearBackground(GRAY);

		// Update GUI
		resizeViewport(&viewport);
		scaleCanvasInViewport(&viewport);

		hoveringGUI = false;
		drawCoordinates(&viewport);

		drawTextButton(&saveButton);
		if (textButtonPressed(&saveButton)) {
			saveImage(&canvas);
		}

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
		DrawRectangleLines(viewport.x-1, viewport.y-1, viewport.width+2, viewport.height+2, BLACK);
		renderCanvas(&viewport);

		EndDrawing();
	}

	// De-Initialization
	CloseWindow();
	return 0;
}