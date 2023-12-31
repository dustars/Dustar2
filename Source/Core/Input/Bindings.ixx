export module Input:Bindings;

import <stdint.h>;

namespace Input
{

// Bindins Abstraction
export enum class Bindings : uint8_t
{
	//-----------------Mouse------------------
	LBUTTON,				// Left mouse button	
	RBUTTON,				// Right mouse button
	MBUTTON,				// Middle mouse button(three - button mouse)
	X1BUTTON,				// X1 mouse button
	X2BUTTON,				// X2 mouse button

	//----------------Keyboard----------------
	// functional
	SPACE,					// SPACE key
	SHIFT,					// SHIFT key
	CONTROL,				// CTRL key
	ALT,					// ALT key
	ESCAPE,					// ESC key

	// numbers
	ZERO,					// 0 key
	ONE,					// 1 key
	TWO,					// 2 key
	THREE,					// 3 key
	FOUR,					// 4 key
	FIVE,					// 5 key
	SIVE,					// 6 key
	SEVEN,					// 7 key
	EIGHT,					// 8 key
	NINE,					// 9 key

	// Characters
	A,						// A key
	B,						// B key
	C,						// C key
	D,						// D key
	E,						// E key
	F,						// F key
	G,						// G key
	H,						// H key
	I,						// I key
	J,						// J key
	K,						// K key
	L,						// L key
	M,						// M key
	N,						// N key
	O,						// O key
	P,						// P key
	Q,						// Q key
	R,						// R key
	S,						// S key
	T,						// T key
	U,						// U key
	V,						// V key
	W,						// W key
	X,						// X key
	Y,						// Y key
	Z,						// Z key

	F1,						// F1 key
	F2,						// F2 key
	F3,						// F3 key
	F4,						// F4 key
	F5,						// F5 key
	F6,						// F6 key
	F7,						// F7 key
	F8,						// F8 key
	F9,						// F9 key
	F10,					// F10 key
	F11,					// F11 key
	F12,					// F12 key

	MAX
};

}