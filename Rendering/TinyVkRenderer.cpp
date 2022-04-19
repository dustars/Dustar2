/*
    Module Description:
    The implementation of TinyVkRenderer.

    Author:
    Zhao Han Ning

    Created Date:
    2022.4.18

    Last Updated:
    2022.4.18

    Notes:
*/

module;

module TinyVkRenderer;

import Core;

TinyVkRenderer::TinyVkRenderer(uint windowWidth, uint windowHeight)
    : surface(windowWidth, windowHeight)
{
}

void TinyVkRenderer::Run() const
{
    while (surface.Update())
    {
        Render();
    }
}

void TinyVkRenderer::Render() const
{
}