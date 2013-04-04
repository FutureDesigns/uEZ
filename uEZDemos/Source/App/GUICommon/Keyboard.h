#ifndef KEYBOARD_H_
#define KEYBOARD_H_

#define KEYBOARD_USE_UPPER_CASE         (1<<0)
#define KEYBOARD_USE_LOWER_CASE         (1<<1)
#define KEYBOARD_USE_DIGITS             (1<<2)
#define KEYBOARD_USE_SYMBOLS            (1<<3)
#define KEYBOARD_USE_SPACE              (1<<4)
#define KEYBOARD_USE_ALL                (KEYBOARD_USE_UPPER_CASE | KEYBOARD_USE_LOWER_CASE | KEYBOARD_USE_DIGITS | KEYBOARD_USE_SYMBOLS | KEYBOARD_USE_SPACE)

TUInt16 Keyboard(char* aData, char* message, TUInt16 aNumChars, TUInt8 aShowKeys);

#endif
