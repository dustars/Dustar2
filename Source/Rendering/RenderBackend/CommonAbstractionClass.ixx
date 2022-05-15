/*
    Description:
    The interface of CommonAbstractionClass module

    Created Date:
    5/15/2022 3:46:27 PM

    Notes:
*/

export module CommonAbstractionClass;

import <string>;
import <vector>;

export enum class RENDER_API
{
	VULKAN,
	D3D12,
	METAL,
	MAX
};

export enum class ShaderType
{
	VS,
	FS,
	GS,
	TCS,
	TES,
	CS,
	Max
};

export struct ShaderFile
{
	std::string shaderFile;
	std::string entry;
	ShaderType type;

	ShaderFile(const std::string& file, const std::string& entryPoint, ShaderType type)
		: shaderFile(file), entry(entryPoint), type(type)
	{}
};

export typedef std::vector<ShaderFile> ShaderArray;