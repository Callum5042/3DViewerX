#include "SkyHeader.hlsli"

PS_INPUT main(VS_INPUT input)
{
	PS_INPUT output;

	// Set z = w so that z/w = 1 (i.e., skydome always on far plane).
	output.PosH = mul(float4(input.PosL, 1.0f), World);
	output.PosH = mul(output.PosH, View);
	output.PosH = mul(output.PosH, Proj).xyww;

	// Use local vertex position as cubemap lookup vector.
	output.PosL = input.PosL;

	return output;
}