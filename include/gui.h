#ifndef GUI_H
#define GUI_H

#include <stdbool.h>

#include "../include/canvas.h"
#include "../include/icon.h"

extern bool hoveringGUI;

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
	const int buttonCount;
	int selected;
} RadioButtons ;

typedef struct TextButton {
	Rectangle rect;
	const char* label;
	const void (*callback)();
} TextButton;

void isValidBrushSize(int ch, TextInput *const textInput);
void isValidColor(int ch, TextInput *const textInput);
void drawCoordinates(Canvas *canvas);
TextInput createTextInput(const int x, const int y, const int width, const int height, const char* label, const char *defaultValue, const void (*callback)(int, TextInput *const));
void drawTextInput(const TextInput *const textInput);
void checkTextInput(TextInput *const textInput);
RadioButtons createRadioButtons(const int x, const int y, const int buttonWidth, const int buttonHeight, const int buttonCount, const unsigned char *icons[], const int iconLens[]);
void drawRadioButtons(const RadioButtons *const radioButtons);
int checkRadioButtons(RadioButtons *const radioButtons);
TextButton createTextButton(const int x, const int y, const int width, const int height, const char *label);
void drawTextButton(const TextButton *const textButton);
bool textButtonPressed(TextButton *const textButton);

#endif
