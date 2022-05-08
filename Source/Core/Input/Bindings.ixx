module;
#include <windows.h>
export module Input:Bindings;

namespace Input
{
// Windows 专用
export enum class MouseBindings
{
	EMPTY,
	LEFT_DOWN = WM_LBUTTONDOWN,
	LEFT_UP = WM_LBUTTONUP,
	MIDDLE_DOWN = WM_MBUTTONDOWN,
	MIDDLE_UP = WM_MBUTTONUP,
	RIGHT_DOWN = WM_RBUTTONDOWN,
	RIGHT_UP = WM_RBUTTONUP,
	EXTRA_DOWN = WM_XBUTTONDOWN,
	EXTRA_UP = WM_XBUTTONUP,
	MAX_NUM
};

export enum class KeyBoardBindings
{
	EMPTY,
	MAX_NUM
};

}