#include <raylib.h>
#include <string.h>

#include "../include/canvas.h"
#include "../include/font.h"
#include "../include/gui.h"
#include "../include/icon.h"

bool hoveringGUI = false;

bool isNumber(int ch) {
	return (ch > 47 && ch < 58);
}

void isValidBrushSize(int ch, TextInput *const textInput) {
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

void isValidColor(int ch, TextInput *const textInput) {
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
	Vector2 mousePos = GetMousePosition();
	double widthRatio = (double)viewport->canvas->width / viewport->canvasWidth;
	double heightRatio = (double)viewport->canvas->height / viewport->canvasHeight;
	int mouseX = (mousePos.x - viewport->canvasX - viewport->x) * widthRatio;
	int mouseY = (mousePos.y - viewport->canvasY - viewport->y) * heightRatio;
	DrawTextEx(currentFont, TextFormat("X: %i Y: %i", mouseX, mouseY), (Vector2){viewport->x, viewport->y + viewport->height}, 32, 0, BLACK);
}

TextInput createTextInput(const int x, const int y, const int width, const int height, const char *label, const char *defaultValue, const void (*callback)(int, TextInput *const)) {
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
		double fontSizeRatio = (double)currentFont.baseSize / 32;
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
		int ch = GetKeyPressed();
		if (ch) {
			if (ch == KEY_BACKSPACE && textInput->index > 0) {
				textInput->text[--textInput->index] = '\0';
			}
			else if (ch != KEY_BACKSPACE) {
				if (textInput->index < 3) {
					textInput->text[textInput->index++] = ch;
					textInput->text[textInput->index] = '\0';
				}
			}
			textInput->callback(ch, textInput);
		}
	}

	if (textInput->hovering) {
		int scroll = (int)GetMouseWheelMoveV().y;
		if (scroll) {
			int currentValue = atoi(textInput->text) + scroll;
			sprintf(textInput->text, "%d", currentValue);
			if (currentValue > 0) {
				textInput->index = strlen(textInput->text);
			}
			else {
				textInput->index = 0;
			}
			textInput->callback(0, textInput);
		}
	}
}

RadioButtons createRadioButtons(const int x, const int y, const int buttonWidth, const int buttonHeight, const int buttonCount, const unsigned char *icons[], const int iconLens[]) {
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

	for (int i = 0; i < buttonCount; i++) {
		radioButtons.icons[i] = loadIcon(icons[i], iconLens[i]);
	}

	return radioButtons;
}

void drawRadioButtons(const RadioButtons *const radioButtons) {
	int buttonWidth = radioButtons->rect.width / radioButtons->buttonCount;
	Rectangle iconRect = { radioButtons->rect.x, radioButtons->rect.y, buttonWidth, radioButtons->rect.height };
	for (int i = 0; i < radioButtons->buttonCount; i++) {
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

int checkRadioButtons(RadioButtons *const radioButtons) {
	Vector2 mousePos = GetMousePosition();
	bool hovering = CheckCollisionPointRec(mousePos, radioButtons->rect);
	if (hovering) {
		hoveringGUI = true;
	}
	if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && hovering) {
		int button = (mousePos.x - radioButtons->rect.x) / (radioButtons->rect.width / radioButtons->buttonCount);
		radioButtons->selected = button;
	}
	return radioButtons->selected;
}

TextButton createTextButton(const int x, const int y, const int width, const int height, const char *label) {
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

void drawTextButton(const TextButton *const textButton) {
	Color backgroundColor = CheckCollisionPointRec(GetMousePosition(), textButton->rect) ? RAYWHITE : (Color) { 200, 200, 200, 200 };
	DrawRectangleRec(textButton->rect, backgroundColor);
	DrawRectangleLinesEx(textButton->rect, 1, BLACK);
	DrawTextEx(currentFont, TextFormat("%s", textButton->label), (Vector2){textButton->rect.x+4, textButton->rect.y}, 32, 0, BLACK);
}

bool textButtonPressed(TextButton* const textButton) {
	Vector2 mousePos = GetMousePosition();
	bool hovering = CheckCollisionPointRec(mousePos, textButton->rect);
	if (hovering) {
		hoveringGUI = true;
	}
	return (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && hovering);
}
