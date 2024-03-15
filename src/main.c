#include <raylib.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>

#include "../include/canvas.h"
#include "../include/gui.h"
#include "../include/font.h"
#include "../include/icon.h"

#define DEFAULT_WIDTH 1024
#define DEFAULT_HEIGHT 576

int WinMain() {
	// Initilization
	SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_VSYNC_HINT);
	InitWindow(DEFAULT_WIDTH, DEFAULT_HEIGHT, "CDraw");
	setCurrentFont(jetBrainsMonoMedium);

	Canvas canvas = createBlankCanvas(DEFAULT_WIDTH, DEFAULT_HEIGHT);
	Viewport viewport = createViewport(&canvas, 319, 119);
	Brush brush = {
		.type = PENCIL,
		.shape = SQUARE,
		.color = (Color) { 0, 0, 0, 255 },
		.size = 8,
	};

	TextButton newButton = createTextButton(10, 10, 100, 30, "New");
	TextButton saveButton = createTextButton(120, 10, 100, 30, "Save");

	const uint8_t *shapeIcons[2] = { squareIcon, circleIcon };
	const uint64_t shapeIconLens[2] = { squareIconLen, circleIconLen };
	RadioButtons brushShapeButtons = createRadioButtons(50, 119, 32, 32, 2, shapeIcons, shapeIconLens);

	const uint8_t *typeIcons[2] = { pencilIcon, eraserIcon };
	const uint64_t typeIconLens[2] = { pencilIconLen, eraserIconLen };
	RadioButtons brushTypeButtons = createRadioButtons(150, 119, 32, 32, 2, typeIcons, typeIconLens);

	TextInput brushSizeBox = createTextInput(50, 219, 70, 30, "Brush Size", "8", isValidBrushSize);
	TextInput redBox = createTextInput(50, 269, 70, 30, "Red", "0", isValidColor);
	TextInput greenBox = createTextInput(50, 319, 70, 30, "Green", "0", isValidColor);
	TextInput blueBox = createTextInput(50, 369, 70, 30, "Blue", "0", isValidColor);
	TextInput alphaBox = createTextInput(50, 419, 70, 30, "Alpha", "255", isValidColor);

	// Main loop
	while (!WindowShouldClose()) {
		// Handle all modifications to viewport and canvas size
		keepMinimumWindowSize();
		resizeViewport(&viewport);
		handleCanvasZoom(&viewport);
		scaleCanvasInViewport(&viewport);
		
		// Update canvas buffer
		tryDrawToCanvas(&viewport, brush);

		// Start rendering to screen
		BeginDrawing();
		ClearBackground(GRAY);

		// Update GUI
		hoveringGUI = false;
		drawCoordinates(&viewport);

		drawTextButton(&newButton);
		if (textButtonPressed(&newButton)) {
			OptionalCanvas result = newCanvasPrompt();
			if (result.hasCanvas) {
				free(canvas.buffer);
				canvas = result.canvas;
				viewport = createViewport(&canvas, 319, 119);
			}
		}
		drawTextButton(&saveButton);
		if (textButtonPressed(&saveButton)) {
			saveImagePrompt(viewport.canvas);
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
		renderCanvas(&viewport);

		EndDrawing();
	}

	// De-Initialization
	CloseWindow();
	return 0;
}