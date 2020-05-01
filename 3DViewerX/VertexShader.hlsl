
#include "Header.hlsli"

PS_INPUT main(VS_INPUT input)
{
	PS_INPUT output;

	// Transform to world space space.
	output.PosW = mul(float4(input.PosL, 1.0f), World).xyz;
	output.NormalW = mul(input.NormalL, (float3x3)World);

	// Transform to homogeneous clip space.
	output.PosH = mul(float4(input.PosL, 1.0f), World);
	output.PosH = mul(output.PosH, View);
	output.PosH = mul(output.PosH, Projection);

	output.Tex = input.Tex;

	return output;
}