/*
    Description:
    The implementation of Input module

    Created Date:
    5/22/2022 10:34:39 AM
*/

module;
#include <windows.h>
module Input;

namespace Input
{

    std::vector<bool> InputManager::enabledBindings(static_cast<uint8_t>(Bindings::MAX), false);
    InputManager::RegisterActions InputManager::registeredCallbacks;

    //See https://learn.microsoft.com/en-us/windows/win32/inputdev/about-keyboard-input#scan-codes
    void InputManager::UpdateBindingWindowsRaw(uint32_t scanCode, bool bEnable)
    {
        switch (scanCode)
        {
        //TODO:把鼠标按键和滚轮设置了
        // Mouse
        //case VK_LBUTTON: {enabledBindings[static_cast<uint8_t>(Bindings::LBUTTON)] = bEnable; break; }
        //case VK_RBUTTON: {enabledBindings[static_cast<uint8_t>(Bindings::RBUTTON)] = bEnable; break; }
        //case VK_MBUTTON: {enabledBindings[static_cast<uint8_t>(Bindings::MBUTTON)] = bEnable; break; }
        //case VK_XBUTTON1: {enabledBindings[static_cast<uint8_t>(Bindings::X1BUTTON)] = bEnable; break; }
        //case VK_XBUTTON2: {enabledBindings[static_cast<uint8_t>(Bindings::X2BUTTON)] = bEnable; break; }

        case 0x0039: {enabledBindings[static_cast<uint8_t>(Bindings::SPACE)] = bEnable; break; }
        case 0x0001: {enabledBindings[static_cast<uint8_t>(Bindings::ESCAPE)] = bEnable; break; }
        case 0x001C: {enabledBindings[static_cast<uint8_t>(Bindings::ENTER)] = bEnable; break; }
        case 0x002A: {enabledBindings[static_cast<uint8_t>(Bindings::L_SHIFT)] = bEnable; break; }
        case 0x001D: {enabledBindings[static_cast<uint8_t>(Bindings::L_CONTROL)] = bEnable; break; }
        case 0x0038: {enabledBindings[static_cast<uint8_t>(Bindings::L_ALT)] = bEnable; break; }
        case 0x0036: {enabledBindings[static_cast<uint8_t>(Bindings::R_SHIFT)] = bEnable; break; }
        case 0xE01D: {enabledBindings[static_cast<uint8_t>(Bindings::R_CONTROL)] = bEnable; break; }
        case 0xE038: {enabledBindings[static_cast<uint8_t>(Bindings::R_ALT)] = bEnable; break; }

        case 0x0002: {enabledBindings[static_cast<uint8_t>(Bindings::ONE)] = bEnable; break; }
        case 0x0003: {enabledBindings[static_cast<uint8_t>(Bindings::TWO)] = bEnable; break; }
        case 0x0004: {enabledBindings[static_cast<uint8_t>(Bindings::THREE)] = bEnable; break; }
        case 0x0005: {enabledBindings[static_cast<uint8_t>(Bindings::FOUR)] = bEnable; break; }
        case 0x0006: {enabledBindings[static_cast<uint8_t>(Bindings::FIVE)] = bEnable; break; }
        case 0x0007: {enabledBindings[static_cast<uint8_t>(Bindings::SIVE)] = bEnable; break; }
        case 0x0008: {enabledBindings[static_cast<uint8_t>(Bindings::SEVEN)] = bEnable; break; }
        case 0x0009: {enabledBindings[static_cast<uint8_t>(Bindings::EIGHT)] = bEnable; break; }
        case 0x000A: {enabledBindings[static_cast<uint8_t>(Bindings::NINE)] = bEnable; break; }
        case 0x000B: {enabledBindings[static_cast<uint8_t>(Bindings::ZERO)] = bEnable; break; }

        // Keyboard
        case 0x001E: {enabledBindings[static_cast<uint8_t>(Bindings::A)] = bEnable; break; }
        case 0x0030: {enabledBindings[static_cast<uint8_t>(Bindings::B)] = bEnable; break; }
        case 0x002E: {enabledBindings[static_cast<uint8_t>(Bindings::C)] = bEnable; break; }
        case 0x0020: {enabledBindings[static_cast<uint8_t>(Bindings::D)] = bEnable; break; }
        case 0x0012: {enabledBindings[static_cast<uint8_t>(Bindings::E)] = bEnable; break; }
        case 0x0021: {enabledBindings[static_cast<uint8_t>(Bindings::F)] = bEnable; break; }
        case 0x0022: {enabledBindings[static_cast<uint8_t>(Bindings::G)] = bEnable; break; }
        case 0x0023: {enabledBindings[static_cast<uint8_t>(Bindings::H)] = bEnable; break; }
        case 0x0017: {enabledBindings[static_cast<uint8_t>(Bindings::I)] = bEnable; break; }
        case 0x0024: {enabledBindings[static_cast<uint8_t>(Bindings::J)] = bEnable; break; }
        case 0x0025: {enabledBindings[static_cast<uint8_t>(Bindings::K)] = bEnable; break; }
        case 0x0026: {enabledBindings[static_cast<uint8_t>(Bindings::L)] = bEnable; break; }
        case 0x0032: {enabledBindings[static_cast<uint8_t>(Bindings::M)] = bEnable; break; }
        case 0x0031: {enabledBindings[static_cast<uint8_t>(Bindings::N)] = bEnable; break; }
        case 0x0018: {enabledBindings[static_cast<uint8_t>(Bindings::O)] = bEnable; break; }
        case 0x0019: {enabledBindings[static_cast<uint8_t>(Bindings::P)] = bEnable; break; }
        case 0x0010: {enabledBindings[static_cast<uint8_t>(Bindings::Q)] = bEnable; break; }
        case 0x0013: {enabledBindings[static_cast<uint8_t>(Bindings::R)] = bEnable; break; }
        case 0x001F: {enabledBindings[static_cast<uint8_t>(Bindings::S)] = bEnable; break; }
        case 0x0014: {enabledBindings[static_cast<uint8_t>(Bindings::T)] = bEnable; break; }
        case 0x0016: {enabledBindings[static_cast<uint8_t>(Bindings::U)] = bEnable; break; }
        case 0x002F: {enabledBindings[static_cast<uint8_t>(Bindings::V)] = bEnable; break; }
        case 0x0011: {enabledBindings[static_cast<uint8_t>(Bindings::W)] = bEnable; break; }
        case 0x002D: {enabledBindings[static_cast<uint8_t>(Bindings::X)] = bEnable; break; }
        case 0x0015: {enabledBindings[static_cast<uint8_t>(Bindings::Y)] = bEnable; break; }
        case 0x002C: {enabledBindings[static_cast<uint8_t>(Bindings::Z)] = bEnable; break; }

        case 0x003B: {enabledBindings[static_cast<uint8_t>(Bindings::F1)] = bEnable; break; }
        case 0x003C: {enabledBindings[static_cast<uint8_t>(Bindings::F2)] = bEnable; break; }
        case 0x003D: {enabledBindings[static_cast<uint8_t>(Bindings::F3)] = bEnable; break; }
        case 0x003E: {enabledBindings[static_cast<uint8_t>(Bindings::F4)] = bEnable; break; }
        case 0x003F: {enabledBindings[static_cast<uint8_t>(Bindings::F5)] = bEnable; break; }
        case 0x0040: {enabledBindings[static_cast<uint8_t>(Bindings::F6)] = bEnable; break; }
        case 0x0041: {enabledBindings[static_cast<uint8_t>(Bindings::F7)] = bEnable; break; }
        case 0x0042: {enabledBindings[static_cast<uint8_t>(Bindings::F8)] = bEnable; break; }
        case 0x0043: {enabledBindings[static_cast<uint8_t>(Bindings::F9)] = bEnable; break; }
        case 0x0044: {enabledBindings[static_cast<uint8_t>(Bindings::F10)] = bEnable; break; }
        case 0x0057: {enabledBindings[static_cast<uint8_t>(Bindings::F11)] = bEnable; break; }
        case 0x0058: {enabledBindings[static_cast<uint8_t>(Bindings::F12)] = bEnable; break; }
        }
    }

    //Not used
    void InputManager::UpdateBindingWindowsLegacy(uint32_t vkCode, bool bEnable)
    {
        switch (vkCode)
        {
        // Mouse
        case VK_LBUTTON: {enabledBindings[static_cast<uint8_t>(Bindings::LBUTTON)] = bEnable; break; }
        case VK_RBUTTON: {enabledBindings[static_cast<uint8_t>(Bindings::RBUTTON)] = bEnable; break; }
        case VK_MBUTTON: {enabledBindings[static_cast<uint8_t>(Bindings::MBUTTON)] = bEnable; break; }
        case VK_XBUTTON1: {enabledBindings[static_cast<uint8_t>(Bindings::X1BUTTON)] = bEnable; break; }
        case VK_XBUTTON2: {enabledBindings[static_cast<uint8_t>(Bindings::X2BUTTON)] = bEnable; break; }

        case VK_SPACE : {enabledBindings[static_cast<uint8_t>(Bindings::SPACE)] = bEnable; break; }
        case VK_SHIFT: {enabledBindings[static_cast<uint8_t>(Bindings::L_SHIFT)] = bEnable; break; }
        case VK_CONTROL: {enabledBindings[static_cast<uint8_t>(Bindings::L_CONTROL)] = bEnable; break; }
        case VK_MENU: {enabledBindings[static_cast<uint8_t>(Bindings::L_ALT)] = bEnable; break; }
        case VK_ESCAPE: {enabledBindings[static_cast<uint8_t>(Bindings::ESCAPE)] = bEnable; break; }

        case 0x30: {enabledBindings[static_cast<uint8_t>(Bindings::ZERO)] = bEnable; break; }
        case 0x31: {enabledBindings[static_cast<uint8_t>(Bindings::ONE)] = bEnable; break; }
        case 0x32: {enabledBindings[static_cast<uint8_t>(Bindings::TWO)] = bEnable; break; }
        case 0x33: {enabledBindings[static_cast<uint8_t>(Bindings::THREE)] = bEnable; break; }
        case 0x34: {enabledBindings[static_cast<uint8_t>(Bindings::FOUR)] = bEnable; break; }
        case 0x35: {enabledBindings[static_cast<uint8_t>(Bindings::FIVE)] = bEnable; break; }
        case 0x36: {enabledBindings[static_cast<uint8_t>(Bindings::SIVE)] = bEnable; break; }
        case 0x37: {enabledBindings[static_cast<uint8_t>(Bindings::SEVEN)] = bEnable; break; }
        case 0x38: {enabledBindings[static_cast<uint8_t>(Bindings::EIGHT)] = bEnable; break; }
        case 0x39: {enabledBindings[static_cast<uint8_t>(Bindings::NINE)] = bEnable; break; }

        // Keyboard
        case 0x41: {enabledBindings[static_cast<uint8_t>(Bindings::A)] = bEnable; break; }
        case 0x42: {enabledBindings[static_cast<uint8_t>(Bindings::B)] = bEnable; break; }
        case 0x43: {enabledBindings[static_cast<uint8_t>(Bindings::C)] = bEnable; break; }
        case 0x44: {enabledBindings[static_cast<uint8_t>(Bindings::D)] = bEnable; break; }
        case 0x45: {enabledBindings[static_cast<uint8_t>(Bindings::E)] = bEnable; break; }
        case 0x46: {enabledBindings[static_cast<uint8_t>(Bindings::F)] = bEnable; break; }
        case 0x47: {enabledBindings[static_cast<uint8_t>(Bindings::G)] = bEnable; break; }
        case 0x48: {enabledBindings[static_cast<uint8_t>(Bindings::H)] = bEnable; break; }
        case 0x49: {enabledBindings[static_cast<uint8_t>(Bindings::I)] = bEnable; break; }
        case 0x4A: {enabledBindings[static_cast<uint8_t>(Bindings::J)] = bEnable; break; }
        case 0x4B: {enabledBindings[static_cast<uint8_t>(Bindings::K)] = bEnable; break; }
        case 0x4C: {enabledBindings[static_cast<uint8_t>(Bindings::L)] = bEnable; break; }
        case 0x4D: {enabledBindings[static_cast<uint8_t>(Bindings::M)] = bEnable; break; }
        case 0x4E: {enabledBindings[static_cast<uint8_t>(Bindings::N)] = bEnable; break; }
        case 0x4F: {enabledBindings[static_cast<uint8_t>(Bindings::O)] = bEnable; break; }
        case 0x50: {enabledBindings[static_cast<uint8_t>(Bindings::P)] = bEnable; break; }
        case 0x51: {enabledBindings[static_cast<uint8_t>(Bindings::Q)] = bEnable; break; }
        case 0x52: {enabledBindings[static_cast<uint8_t>(Bindings::R)] = bEnable; break; }
        case 0x53: {enabledBindings[static_cast<uint8_t>(Bindings::S)] = bEnable; break; }
        case 0x54: {enabledBindings[static_cast<uint8_t>(Bindings::T)] = bEnable; break; }
        case 0x55: {enabledBindings[static_cast<uint8_t>(Bindings::U)] = bEnable; break; }
        case 0x56: {enabledBindings[static_cast<uint8_t>(Bindings::V)] = bEnable; break; }
        case 0x57: {enabledBindings[static_cast<uint8_t>(Bindings::W)] = bEnable; break; }
        case 0x58: {enabledBindings[static_cast<uint8_t>(Bindings::X)] = bEnable; break; }
        case 0x59: {enabledBindings[static_cast<uint8_t>(Bindings::Y)] = bEnable; break; }
        case 0x5A: {enabledBindings[static_cast<uint8_t>(Bindings::Z)] = bEnable; break; }

	    case VK_F1: {enabledBindings[static_cast<uint8_t>(Bindings::F1)] = bEnable; break; }
	    case VK_F2: {enabledBindings[static_cast<uint8_t>(Bindings::F2)] = bEnable; break; }
	    case VK_F3: {enabledBindings[static_cast<uint8_t>(Bindings::F3)] = bEnable; break; }
	    case VK_F4: {enabledBindings[static_cast<uint8_t>(Bindings::F4)] = bEnable; break; }
	    case VK_F5: {enabledBindings[static_cast<uint8_t>(Bindings::F5)] = bEnable; break; }
	    case VK_F6: {enabledBindings[static_cast<uint8_t>(Bindings::F6)] = bEnable; break; }
	    case VK_F7: {enabledBindings[static_cast<uint8_t>(Bindings::F7)] = bEnable; break; }
	    case VK_F8: {enabledBindings[static_cast<uint8_t>(Bindings::F8)] = bEnable; break; }
	    case VK_F9: {enabledBindings[static_cast<uint8_t>(Bindings::F9)] = bEnable; break; }
	    case VK_F10: {enabledBindings[static_cast<uint8_t>(Bindings::F10)] = bEnable; break; }
	    case VK_F11: {enabledBindings[static_cast<uint8_t>(Bindings::F11)] = bEnable; break; }
	    case VK_F12: {enabledBindings[static_cast<uint8_t>(Bindings::F12)] = bEnable; break; }
        }
    }

}