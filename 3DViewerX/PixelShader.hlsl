
#include "Header.hlsli"

float4 main(PS_INPUT pin) : SV_TARGET
{
	// Interpolating normal can unnormalize it, so normalize it.
	pin.NormalW = normalize(pin.NormalW);
	
	// Diffuse map
	float4 diffuse = gDiffuseMap.Sample(samAnisotropic, pin.Tex);

	// Normal mapping
	float3 normalMap = gNormalMap.Sample(samAnisotropic, pin.Tex).rgb;
	normalMap = (2.0f * normalMap) - 1.0f;
	pin.TangentW = normalize(pin.NormalW - dot(pin.TangentW, pin.NormalW) * pin.NormalW);

	//Create the biTangent
	float3 biTangent = cross(pin.NormalW, pin.TangentW);

	//Create the "Texture Space"
	float3x3 texSpace = float3x3(pin.TangentW, biTangent, pin.NormalW);

	pin.NormalW = normalize(mul(normalMap, texSpace));

	// Lighting
	float3 lightDir = float3(Direction.xyz);
	float4 litColor = diffuse;
	litColor += saturate(dot(-lightDir, pin.NormalW)) * diffuse;

	litColor.a = 1.0f;
	return litColor;
}