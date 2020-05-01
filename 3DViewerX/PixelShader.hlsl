
#include "Header.hlsli"

float4 main(PS_INPUT pin) : SV_TARGET
{
	// Interpolating normal can unnormalize it, so normalize it.
	//pin.NormalW = normalize(pin.NormalW);

	float3 lightDir = float3(0.0f, 0.0f, -0.5f);

	float4 litColor = saturate(dot(lightDir, pin.NormalW));

	litColor.a = 1.0f;
	return litColor;
}