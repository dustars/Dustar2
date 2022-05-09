/*
    Module Description:
    The interface of module Input

    Created Date:
    2022.5.8

	Notes:
	目前注册Input Callback的方式不够统一，而且调用的方式也千奇百怪……Lambda的类型是固定的，这一块还需要改进
*/

export module Input;

export import :Mouse;
export import :Keyboard;
export import :Bindings;

import <functional>;
import <variant>;

namespace Input
{
export class InputManager {
public:
	typedef std::variant<std::monostate, MouseBindings, KeyBoardBindings> Binding;
	typedef std::function<void()> Callback;

	static void RegisterBinding(Binding binding, Callback callback)
	{
		registeredCallbacks.emplace_back(binding, callback);
	}
	static void Update()
	{
		if (std::holds_alternative<MouseBindings>(currentBinding))
		{
			for (auto& entry : registeredCallbacks)
			{
				if (entry.first == currentBinding) entry.second();
			}
		}
		else if (std::holds_alternative<KeyBoardBindings>(currentBinding))
		{
			for (auto& entry : registeredCallbacks)
			{
				if (entry.first == currentBinding) entry.second();
			}
		}
		currentBinding = std::monostate{}; // Set the binding to "empty" state.
	}

	static void UpdateMouse(MouseBindings binding) { currentBinding = binding ;}
	static void UpdateKeyBoard(KeyBoardBindings binding) { currentBinding = binding; }

private:
	inline static Binding currentBinding{};
	inline static std::vector<std::pair<Binding, Callback>> registeredCallbacks;
};
}