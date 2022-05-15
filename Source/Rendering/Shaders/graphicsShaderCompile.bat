..\..\Core\Utilities\ShaderCompiler\DXC\bin\x64\dxc.exe -spirv -T vs_6_1 -E main SimpleVertexShader.hlsl -Fo SimpleVertexShader.spv
..\..\Core\Utilities\ShaderCompiler\DXC\bin\x64\dxc.exe -spirv -T ps_6_1 -E mainPS SimpleFragmentShader.hlsl -Fo SimpleFragmentShader.spv
pause