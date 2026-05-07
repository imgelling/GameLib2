#pragma once

// Digits

#define geK_1 0x31
#define geK_2 0x32
#define geK_3 0x33
#define geK_4 0x34
#define geK_5 0x35
#define geK_6 0x36
#define geK_7 0x37
#define geK_8 0x38
#define geK_9 0x39
#define geK_0 0x30


// Alpha

#define geK_A 0x41
#define geK_B 0x42
#define geK_C 0x43
#define geK_D 0x44
#define geK_E 0x45
#define geK_F 0x46
#define geK_G 0x47
#define geK_H 0x48
#define geK_I 0x49
#define geK_J 0x4A
#define geK_K 0x4B
#define geK_L 0x4C
#define geK_M 0x4D
#define geK_N 0x4E
#define geK_O 0x4F
#define geK_P 0x50
#define geK_Q 0x51
#define geK_R 0x52
#define geK_S 0x53
#define geK_T 0x54
#define geK_U 0x55
#define geK_V 0x56
#define geK_W 0x57
#define geK_X 0x58
#define geK_Y 0x59
#define geK_Z 0x5A

// F Keys

#define geK_F1	0x70
#define geK_F2	0x71
#define geK_F3	0x72
#define geK_F4	0x73
#define geK_F5	0x74
#define geK_F6	0x75
#define geK_F7	0x76
#define geK_F8	0x77
#define geK_F9	0x78
#define geK_F10	0x79
#define geK_F11 0x7A
#define geK_F12	0x7B


// Other text keys

#define geK_ESCAPE 0x1B
#define geK_SPACE 0x20
#define geK_COMMA 0xBC
#define geK_PERIOD 0xBE
#define geK_MINUS 0xBD
#define geK_PLUS 0xBB
#define geK_BACK 0x08
#define geK_DELETE 0x2E
#define geK_QUESTION 0xBF
#define geK_TILDE 0xC0
#define geK_LBRACKET 0xDB
#define geK_RBRACKET 0xDD
#define geK_BACKSLASH 0xDC
#define geK_SEMICOLON 0xBA
#define geK_APOSTROPHE 0xDE

// Key modifier keys

#define geK_SHIFT 0x10

// Arrow keys

#define geK_LEFT 0x25
#define geK_UP 0x26
#define geK_RIGHT 0x27
#define geK_DOWN 0x28

// Text modifier keys

#define geK_RETURN 0x0D


//Windows Keypad(Numeric Pad) Virtual - Key Codes
//In Windows, the numeric keypad keys have specific virtual - key codes(VK codes) that can be used in programming, automation, or input handling.These codes are defined in the WinUser.h header and are distinct from the main keyboard’s number keys.
//
//Numeric Keypad Key Codes
//From the Microsoft WinUser.h reference Microsoft Learn :
//
//Key	Decimal	Hex
//Description
//
//VK_NUMPAD0	96	0x60
//Numeric keypad 0
//
//VK_NUMPAD1	97	0x61
//Numeric keypad 1
//
//VK_NUMPAD2	98	0x62
//Numeric keypad 2
//
//VK_NUMPAD3	99	0x63
//Numeric keypad 3
//
//VK_NUMPAD4	100	0x64
//Numeric keypad 4
//
//VK_NUMPAD5	101	0x65
//Numeric keypad 5
//
//VK_NUMPAD6	102	0x66
//Numeric keypad 6
//
//VK_NUMPAD7	103	0x67
//Numeric keypad 7
//
//VK_NUMPAD8	104	0x68
//Numeric keypad 8
//
//VK_NUMPAD9	105	0x69
//Numeric keypad 9
//
//Keypad Operation Keys
//These are the function keys on the numeric pad :
//
//Key	Decimal	Hex
//Description
//
//VK_MULTIPLY	106	0x6A
//Multiply key
//
//VK_ADD	107	0x6B
//Add key
//
//VK_SEPARATOR	108	0x6C
//Separator key
//
//VK_SUBTRACT	109	0x6D
//Subtract key
//
//VK_DECIMAL	110	0x6E
//Decimal key
//
//VK_DIVIDE	111	0x6F	Divide key
//Notes
//These codes are virtual - key codes, not scan codes.They are used in APIs like GetAsyncKeyState, GetKeyState, or in automation scripts.
//
//The numeric keypad keys are only active when the Num Lock key is on.
//
//In C / C++ or other languages, you can use these constants(e.g., VK_NUMPAD0) in if (GetAsyncKeyState(VK_NUMPAD0) & 0x8000) to detect when the key is pressed.
//
//If you need to map these to Unicode characters, you may need to use MapVirtualKey or similar functions, as keypad keys don’t directly produce the same character as their main keyboard counterparts.
