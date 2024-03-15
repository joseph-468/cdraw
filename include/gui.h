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
	int64_t index;
	void (*callback)(int64_t, struct TextInput *const);
	bool hovering;
	bool inputting;
} TextInput;

typedef struct RadioButtons {
	Rectangle rect;
	Texture icons[8];
	const uint64_t buttonCount;
	int64_t selected;
} RadioButtons;

typedef struct TextButton {
	Rectangle rect;
	const char *label;
} TextButton;

typedef struct OptionalCanvas {
	bool hasCanvas;
	Canvas canvas;
} OptionalCanvas;

void isValidResolution(int64_t ch, TextInput* const textInput);
void isValidBrushSize(int64_t ch, TextInput *const textInput);
void isValidColor(int64_t ch, TextInput *const textInput);
void drawCoordinates(const Viewport *viewport);
TextInput createTextInput(const uint64_t x, const uint64_t y, const uint64_t width, const uint64_t height, const char *label, const char *defaultValue, const void (*callback)(int64_t, TextInput *const));
void drawTextInput(const TextInput *const textInput);
void checkTextInput(TextInput *const textInput);
RadioButtons createRadioButtons(const uint64_t x, const uint64_t y, const uint64_t buttonWidth, const uint64_t buttonHeight, const uint64_t buttonCount, const uint8_t *icons[], const uint64_t iconLens[]);
void drawRadioButtons(const RadioButtons *const radioButtons);
int64_t checkRadioButtons(RadioButtons *const radioButtons);
TextButton createTextButton(const uint64_t x, const uint64_t y, const uint64_t width, const uint64_t height, const char *label);
void drawTextButton(const TextButton *const textButton);
bool textButtonPressed(TextButton *const textButton);
OptionalCanvas newCanvasPrompt();
void saveImagePrompt(const Canvas *canvas);

#endif
