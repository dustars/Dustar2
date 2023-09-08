/*
    Description:
    The interface of Model module

    Created Date:
    5/21/2022 9:16:45 AM

    Notes:
*/

export module Model;

import std;
import <stdint.h>;

export struct Vertex
{
	float x, y, z, w;
	float u, v;
	float pad1;
	float pad2;
};

export class Model
{
    std::vector<Vertex> vertices;
    std::vector<uint16_t> indices;
public:

    const std::vector<Vertex>& GetVertexData() const { return vertices; }
    const std::vector<uint16_t>& GetIndexData() const { return indices; }

    void CreateTriangle()
    {
        vertices =
		{
			{0.f, 0.5f, 0.f, 1.f, 0.5f, 1.f, 0.f, 0.f},
			{-0.5f, -0.5f, 0.f, 1.f, 0.f, 0.f, 0.f, 0.f},
			{0.5f, -0.5f, 0.f, 1.f, 1.f, 0.f, 0.f, 0.f}
		};;
    }

    void CreateCube()
    {
        vertices =
		{
			{-0.5f, -0.5f, -0.5f, 1.f, 1.f, 0.f, 0.f, 0.f},
			{-0.5f,  0.5f, -0.5f, 1.f, 1.f, 1.f, 0.f, 0.f},
			{ 0.5f, -0.5f, -0.5f, 1.f, 0.f, 0.f, 0.f, 0.f},
			{ 0.5f,  0.5f, -0.5f, 1.f, 0.f, 1.f, 0.f, 0.f},
			{ 0.5f, -0.5f,  0.5f, 1.f, 1.f, 1.f, 0.f, 0.f},
			{ 0.5f,  0.5f,  0.5f, 1.f, 1.f, 0.f, 0.f, 0.f},
			{-0.5f, -0.5f,  0.5f, 1.f, 0.f, 1.f, 0.f, 0.f},
			{-0.5f,  0.5f,  0.5f, 1.f, 0.f, 0.f, 0.f, 0.f},
		};
        indices =
		{
			0, 1, 2,
			2, 1, 3,
			2, 3, 4,
			4, 3, 5,
			4, 5, 6,
			6, 5, 7,
			6, 7, 0,
			0, 7, 1,
			6, 0, 2,
			2, 4, 6,
			7, 5, 3,
			7, 3, 1
		};
    }
};
