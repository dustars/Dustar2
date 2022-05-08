/*
    Module Description:
    Temporary entry point for the program

    Author:
    Zhao Han Ning

    Created Date:
    2022.4.17

    Notes:
    1.不知道全程用Module是不是一个好主意......如果可以，我想用更多的 C++ 最新特性~！
    2.目前去掉了Console：Linker的Subsystem设为Window，Advanced的Entry设为mainCRTStartup
    3.
*/

import TinyVkRenderer;

int main()
{
    //for () {
    //    World.Update();
    //    Renderer.Update();
    //}
	TinyVkRenderer renderer(800, 600);
	renderer.Run();
	return 0;
}