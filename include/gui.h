#ifndef GUI_H
#define GUI_H

#include <stdbool.h>

#include "../include/canvas.h"

typedef struct TextInput {
	Rectangle rect;
	const char *label;
	char text[64];
	int index;
	void (*callback)(int, struct TextInput *const);
	bool hovering;
	bool inputting;
} TextInput;

typedef struct RadioButtons {
	Rectangle rect;
	Texture icons[8];
	int buttonCount;
	int selected;
} RadioButtons ;

void isValidBrushSize(int ch, TextInput *const textInput);
void isValidColor(int ch, TextInput *const textInput);
void drawCoordinates(Canvas canvas);
TextInput createTextInput(const int x, const int y, const int width, const int height, const char* label, const char *defaultValue, const void (*callback)(int, TextInput *const));
void drawTextInput(const TextInput *const textInput);
void checkTextInput(TextInput *const textInput);
RadioButtons createRadioButtons(const int x, const int y, const int buttonWidth, const int buttonHeight, const int buttonCount, const char* icons[]);
void drawRadioButtons(const RadioButtons* const radioButtons);
int checkRadioButtons(RadioButtons* const radioButtons);

#endif
