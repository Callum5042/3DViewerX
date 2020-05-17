#include "SkyHeader.hlsli"

float4 main(PS_INPUT input) : SV_TARGET
{
	return gCubeMap.Sample(samAnisotropic, input.PosL);
}