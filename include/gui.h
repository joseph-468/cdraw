#ifndef GUI_H
#define GUI_H

#include <stdbool.h>

extern Font currentFont;

typedef struct TextInput {
	Rectangle rect;
	const char *label;
	char text[256];
	int index;
	void (*callback)(int, struct TextInput *const);
	bool hovering;
	bool inputting;
} TextInput;

bool isNumber(int ch);
void isValidColor(int ch, TextInput *const textInput);
TextInput createTextInput(const int x, const int y, const int width, const int height, const char* label, const char *defaultValue, const void (*callback)(int, TextInput const));
void drawTextInput(const TextInput *const textInput);
void getTextInput(TextInput *const textInput);

#endif
