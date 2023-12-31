/*
    Module Description:
    The interface of module Input

    Created Date:
    2022.5.8

	Notes:
	目前注册Input Callback的方式不够统一，而且调用的方式也千奇百怪……Lambda的类型是固定的，这一块还需要改进
*/

export module Input;

export import :Bindings;

import std;
import <stdint.h>;

namespace Input
{
export class InputManager {
	typedef std::vector<std::pair<Bindings, std::function<void()>>> RegisterActions;
public:

	static void RegisterCallback(Bindings binding, std::function<void()> callback)
	{
		registeredCallbacks.emplace_back(binding, callback);
	}

	static void Reset()
	{
		offX = 0; offY = 0;
		for (auto key : enabledBindings) key = false;
	}

	static void Execute()
	{
		for (auto& entry : registeredCallbacks)
		{
			if (enabledBindings[static_cast<uint8_t>(entry.first)])
			{
				enabledBindings[static_cast<uint8_t>(entry.first)] = false;
				entry.second();
			}
		}
	}

	static void UpdateBindingWindows(uint32_t, bool);
	//static void UpdateBindingLinux(uint32_t, bool);
	//static void UpdateBindingMacro(uint32_t, bool);


	static void UpdateMouse(float newX, float newY)
	{
		offX = posX - newX;
		offY = posY - newY;
		posX = newX; posY = newY; 
	}
	static void UpdateMouse(int del) { delta = delta; }

	static float GetMouseXOffset() { return offX; }
	static float GetMouseYOffset() { return offY; }
	static float GetMouseDelta() { return delta; }
	static bool GetKeyPressed(Bindings key) { return enabledBindings[static_cast<uint8_t>(key)]; }

private:
	// Mouse Position
	// TODO:这里的inline只是为了初始化,不能去掉是因为什么odr-use的backward compatibility, 总之还挺复杂...看了一圈半懂不懂
	// 等以后功力深厚了再看看怎么处理吧
	inline static float offX, offY = 0.f;
	inline static float posX, posY = 0.f;
	// Mouse wheel movement
	inline static float delta;

	// If certain binding is pressed for the frame, it will be set to TRUE
	// the enum order corresponds to the index.
	// TODO: add holding state.
	static std::vector<bool> enabledBindings;

	static RegisterActions registeredCallbacks;
};
}