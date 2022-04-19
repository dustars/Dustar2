/*
    Module Description:
    The module primary interface for TinyVkREnderer.
    TinyRenderer is exclusively for learning Vulkan, thus
    very likely to be removed in the near future.

    Author:
    Zhao Han Ning

    Created Date:
    2022.4.17

    Last Updated:
    2022.4.18

    Notes:
*/

export module TinyVkRenderer;

import WindowSystem;
import std.core;
import Core;

//Create Window
//Create Vulkan Instance
//Do Vulkan Initializaton
//Render Triangle
//Continue loop

export class TinyVkRenderer
{
public:
	TinyVkRenderer(uint windowWidth, uint windowHeight);

    void Run()      const;
	
private:
    void Render()   const;

	void CreateRendererWindow();

    Window::Win32Window surface;
    //VkSurfaceKHR surface;
};