//-----------------------------------------------
// This Shader file is for learning purpose only
//-----------------------------------------------

// DXC use the following attributes to mimic the Vulkan specific feature: Specialization Constant
// If something went wrong, replace all occurrences of the following variables with their default values.
[[vk::constant_id(0)]] const uint testA = 1;
[[vk::constant_id(1)]] const uint testB = 1;
[[vk::constant_id(2)]] const uint testC = 1;

[numthreads(1, 1, 1)]
void CSMain( uint3 DTid : SV_DispatchThreadID )
{
	uint result = testA + testB + testC;
	// do absolutely nothing
}