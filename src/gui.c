#include <raylib.h>
#include <string.h>

#include "../include/gui.h"

Font currentFont;

bool isNumber(int ch) {
	return (ch > 47 && ch < 58);
}

void isValidColor(int ch, TextInput *const textInput) {
	if (textInput->index < 1) {
		textInput->text[0] = '0';
		textInput->text[1] = '\0';
		textInput->index = 0;
		return;
	}
	if (!isNumber(ch) && ch != KEY_BACKSPACE) {
		textInput->text[--textInput->index] = '\0';
		return;
	}
	if (atoi(textInput->text) > 255) {
		strcpy_s(textInput->text, 256, "255");
		return;
	}
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
	strcpy_s(textInput.text, 256, defaultValue);
	return textInput;
}

void drawTextInput(const TextInput *const textInput) {
	DrawRectangleRec(textInput->rect, RAYWHITE);
	DrawRectangleLinesEx(textInput->rect, 1, BLACK);
	DrawTextEx(currentFont, TextFormat("%s", textInput->text), (Vector2){textInput->rect.x+4, textInput->rect.y}, 32, 0, BLACK);
	DrawTextEx(currentFont, TextFormat("%s", textInput->label), (Vector2){textInput->rect.x+textInput->rect.width+4, textInput->rect.y}, 32, 0, BLACK);
}

void getTextInput(TextInput *const textInput) {
	textInput->hovering = CheckCollisionPointRec(GetMousePosition(), textInput->rect);
	if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
		textInput->inputting = textInput->hovering;
	}
	if (textInput->inputting) {
		int ch = GetKeyPressed();
		if (!ch) return;
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