#include <raylib.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "../include/canvas.h"
#include "../include/font.h"
#include "../include/gui.h"
#include "../include/icon.h"

bool hoveringGUI = false;

bool isNumber(int64_t ch) {
	return (ch > 47 && ch < 58);
}

void isValidResolution(int64_t ch, TextInput *const textInput) {
	if (textInput->index > 5) {
		textInput->text[--textInput->index] = '\0';
	}
	if (atoi(textInput->text) == 0 && ch == '0') {
		textInput->text[--textInput->index] = '\0';
	}
	if (!isNumber(ch) && ch != KEY_BACKSPACE && ch != '\0') {
		textInput->text[--textInput->index] = '\0';
	}
	if (textInput->index < 1 && ch != '\0') {
		textInput->text[0] = '1';
		textInput->text[1] = '\0';
		textInput->index = 0;
		return;
	}
	if (atoi(textInput->text) > 65536) {
		strcpy_s(textInput->text, 64, "65536");
		return;
	}
	if (atoi(textInput->text) < 1) {
		strcpy_s(textInput->text, 64, "1");
		return;
	}
}

void isValidBrushSize(int64_t ch, TextInput *const textInput) {
	if (textInput->index > 3) {
		textInput->text[--textInput->index] = '\0';
	}
	if (atoi(textInput->text) == 0 && ch == '0') {
		textInput->text[--textInput->index] = '\0';
	}
	if (!isNumber(ch) && ch != KEY_BACKSPACE && ch != '\0') {
		textInput->text[--textInput->index] = '\0';
	}
	if (textInput->index < 1 && ch != '\0') {
		textInput->text[0] = '1';
		textInput->text[1] = '\0';
		textInput->index = 0;
		return;
	}
	if (atoi(textInput->text) > 128) {
		strcpy_s(textInput->text, 64, "128");
		return;
	}
	if (atoi(textInput->text) < 1) {
		strcpy_s(textInput->text, 64, "1");
		return;
	}
}

void isValidColor(int64_t ch, TextInput *const textInput) {
	if (textInput->index > 3) {
		textInput->text[--textInput->index] = '\0';
	}
	if (atoi(textInput->text) == 0 && ch == '0') {
		textInput->text[--textInput->index] = '\0';
	}
	if (!isNumber(ch) && ch != KEY_BACKSPACE && ch != '\0') {
		textInput->text[--textInput->index] = '\0';
	}
	if (textInput->index < 1 && ch != '\0') {
		textInput->text[0] = '0';
		textInput->text[1] = '\0';
		textInput->index = 0;
		return;
	}
	if (atoi(textInput->text) > 255) {
		strcpy_s(textInput->text, 64, "255");
		return;
	}
	if (atoi(textInput->text) < 0) {
		strcpy_s(textInput->text, 64, "0");
		return;
	}
}

void drawCoordinates(const Viewport *viewport) {
	int64_t mouseX = getRelativePos(viewport).x;
	int64_t mouseY = getRelativePos(viewport).y;
	float widthRatio = (float)viewport->canvas->width / viewport->canvasWidth;
	float heightRatio = (float)viewport->canvas->height / viewport->canvasHeight;
	DrawTextEx(currentFont, TextFormat("X: %i Y: %i", mouseX, mouseY), (Vector2){viewport->x, viewport->y + viewport->height}, 32, 0, BLACK);
}

TextInput createTextInput(const uint64_t x, const uint64_t y, const uint64_t width, const uint64_t height, const char* label, const char* defaultValue, const void (*callback)(int64_t, TextInput* const)) {
	TextInput textInput = {
		.rect = { x, y, width, height },
		.label = label,
		.text = "",
		.callback = callback,
		.index = 0,
		.hovering = false,
		.inputting = false,
	};
	strcpy_s(textInput.text, 64, defaultValue);
	return textInput;
}

void drawTextInput(const TextInput *const textInput) {
	DrawRectangleRec(textInput->rect, RAYWHITE);
	DrawRectangleLinesEx(textInput->rect, 1, BLACK);
	DrawTextEx(currentFont, TextFormat("%s", textInput->text), (Vector2){textInput->rect.x+4, textInput->rect.y}, 32, 0, BLACK);
	DrawTextEx(currentFont, TextFormat("%s", textInput->label), (Vector2){textInput->rect.x+textInput->rect.width+4, textInput->rect.y}, 32, 0, BLACK);
	if (textInput->inputting) {
		float fontSizeRatio = (float)currentFont.baseSize / 32;
		DrawRectangle(textInput->rect.x + 4 + currentFontInfo.advanceX * textInput->index / fontSizeRatio,
			textInput->rect.y + 26, 16, 2, (Color) { 0, 0, 0, 255 });
	}
}

void checkTextInput(TextInput *const textInput) {
	textInput->hovering = CheckCollisionPointRec(GetMousePosition(), textInput->rect);
	if (textInput->hovering) {
		hoveringGUI = true;
	}
	if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) || GetMouseWheelMoveV().y) {
		textInput->inputting = textInput->hovering;
	}
	if (textInput->inputting) {
		int64_t ch = GetKeyPressed();
		if (ch) {
			if (ch == KEY_BACKSPACE && textInput->index > 0) {
				textInput->text[--textInput->index] = '\0';
			}
			else if (ch != KEY_BACKSPACE) {
				if (textInput->index < 16) {
					textInput->text[textInput->index++] = ch;
					textInput->text[textInput->index] = '\0';
				}
			}
			if (textInput->callback) {
				textInput->callback(ch, textInput);
			}
		}
	}

	if (textInput->hovering) {
		int64_t scroll = (int64_t)GetMouseWheelMoveV().y;
		if (scroll) {
			int64_t currentValue = atoi(textInput->text) + scroll;
			sprintf(textInput->text, "%d", (int)currentValue);
			if (currentValue > 0) {
				textInput->index = strlen(textInput->text);
			}
			else {
				textInput->index = 0;
			}
			if (textInput->callback) {
				textInput->callback(0, textInput);
			}
		}
	}
}

RadioButtons createRadioButtons(const uint64_t x, const uint64_t y, const uint64_t buttonWidth, const uint64_t buttonHeight, const uint64_t buttonCount, const uint8_t *icons[], const uint64_t iconLens[]) {
	RadioButtons radioButtons = {
		.rect = {
			.x = x,
			.y = y,
			.width = buttonWidth * buttonCount,
			.height = buttonHeight,
		},
		.icons = NULL,
		.buttonCount = buttonCount,
		.selected = 0,
	};

	for (uint64_t i = 0; i < buttonCount; i++) {
		radioButtons.icons[i] = loadIcon(icons[i], iconLens[i]);
	}

	return radioButtons;
}

void drawRadioButtons(const RadioButtons *const radioButtons) {
	int buttonWidth = radioButtons->rect.width / radioButtons->buttonCount;
	Rectangle iconRect = { radioButtons->rect.x, radioButtons->rect.y, buttonWidth, radioButtons->rect.height };
	for (uint64_t i = 0; i < radioButtons->buttonCount; i++) {
		Rectangle selectedRect = { iconRect.x + 4, iconRect.y + 4, buttonWidth - 8, radioButtons->rect.height - 8 };
		if (i == radioButtons->selected) {
			DrawRectangleRec(iconRect, WHITE);
		}
		else {
			DrawRectangleRec(iconRect, LIGHTGRAY);
		}
		DrawRectangleLinesEx(iconRect, 1, BLACK);
		DrawTexture(radioButtons->icons[i], iconRect.x, iconRect.y , WHITE);
		if (i == radioButtons->selected) {
			DrawRectangleLinesEx(selectedRect, 2, BEIGE);
		}
		iconRect.x += buttonWidth;
	}
}

int64_t checkRadioButtons(RadioButtons *const radioButtons) {
	Vector2 mousePos = GetMousePosition();
bool hovering = CheckCollisionPointRec(mousePos, radioButtons->rect);
if (hovering) {
	hoveringGUI = true;
}
if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && hovering) {
	int64_t button = (mousePos.x - radioButtons->rect.x) / (radioButtons->rect.width / radioButtons->buttonCount);
	radioButtons->selected = button;
}
return radioButtons->selected;
}

TextButton createTextButton(const uint64_t x, const uint64_t y, const uint64_t width, const uint64_t height, const char* label) {
	TextButton textButton = {
		.rect = {
			.x = x,
			.y = y,
			.width = width,
			.height = height,
		},
		.label = label,
	};

	return textButton;
}

void drawTextButton(const TextButton* const textButton) {
	Color backgroundColor = CheckCollisionPointRec(GetMousePosition(), textButton->rect) ? RAYWHITE : (Color) { 200, 200, 200, 200 };
	DrawRectangleRec(textButton->rect, backgroundColor);
	DrawRectangleLinesEx(textButton->rect, 1, BLACK);
	DrawTextEx(currentFont, TextFormat("%s", textButton->label), (Vector2) { textButton->rect.x + 4, textButton->rect.y }, 32, 0, BLACK);
}

bool textButtonPressed(TextButton* const textButton) {
	Vector2 mousePos = GetMousePosition();
	bool hovering = CheckCollisionPointRec(mousePos, textButton->rect);
	if (hovering) {
		hoveringGUI = true;
	}
	return (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && hovering);
}

OptionalCanvas newCanvasPrompt() {
	TextInput widthBox = createTextInput(0, 0, 100, 30, "Width", "1024", isValidResolution);
	TextInput heightBox = createTextInput(0, 0, 100, 30, "Height", "576", isValidResolution);
	TextButton cancelButton = createTextButton(0, 0, 120, 30, "Cancel");
	TextButton confirmButton = createTextButton(0, 10, 120, 30, "Confirm");
	while (!WindowShouldClose()) {
		keepMinimumWindowSize();

		widthBox.rect.x = GetScreenWidth() / 2 - 200;
		widthBox.rect.y = GetScreenHeight() / 2 - 15;
		heightBox.rect.x = GetScreenWidth() / 2;
		heightBox.rect.y = GetScreenHeight() / 2 - 15;
		cancelButton.rect.x = GetScreenWidth() / 2 - 200;
		cancelButton.rect.y = GetScreenHeight() / 2 + 35;
		confirmButton.rect.x = GetScreenWidth() / 2;
		confirmButton.rect.y = GetScreenHeight() / 2 + 35;

		if (textButtonPressed(&cancelButton)) {
			return (OptionalCanvas) { .hasCanvas = false };
		}
		if (textButtonPressed(&confirmButton)) {
			Canvas canvas = createBlankCanvas(atoi(widthBox.text), atoi(heightBox.text));
			return (OptionalCanvas) { .hasCanvas = true, .canvas = canvas };
		}

		BeginDrawing();
		ClearBackground(GRAY);
		drawTextInput(&widthBox);
		checkTextInput(&widthBox);
		drawTextInput(&heightBox);
		checkTextInput(&heightBox);
		drawTextButton(&cancelButton);
		drawTextButton(&confirmButton);
		EndDrawing();
	}
	if (WindowShouldClose()) {
		CloseWindow();
		exit(0);
	}
}

void saveImagePrompt(const Canvas* canvas) {
	TextInput nameBox = createTextInput(0, 0, 190, 30, "Filename", "image", NULL);
	TextButton cancelButton = createTextButton(0, 0, 120, 30, "Cancel");
	TextButton saveButton = createTextButton(0, 10, 120, 30, "Save");
	while (!WindowShouldClose()) {
		keepMinimumWindowSize();

		nameBox.rect.x = GetScreenWidth() / 2 - 200;
		nameBox.rect.y = GetScreenHeight() / 2 - 15;
		cancelButton.rect.x = GetScreenWidth() / 2 - 200;
		cancelButton.rect.y = GetScreenHeight() / 2 + 35;
		saveButton.rect.x = GetScreenWidth() / 2;
		saveButton.rect.y = GetScreenHeight() / 2 + 35;

		if (textButtonPressed(&cancelButton)) {
			return;
		}
		if (textButtonPressed(&saveButton)) {
			if (strlen(nameBox.text) != 0) {
				saveImage(canvas, nameBox.text);
				return;
			}
		}

		BeginDrawing();
		ClearBackground(GRAY);
		drawTextInput(&nameBox);
		checkTextInput(&nameBox);
		drawTextButton(&cancelButton);
		drawTextButton(&saveButton);
		EndDrawing();
	}
	if (WindowShouldClose()) {
		CloseWindow();
		exit(0);
	}
}
