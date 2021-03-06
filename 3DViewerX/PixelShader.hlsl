
#include "Header.hlsli"

float4 main(PS_INPUT pin) : SV_TARGET
{
	// Interpolating normal can unnormalize it, so normalize it.
	pin.NormalW = normalize(pin.NormalW);

	// Diffuse map
	float4 diffuse = gDiffuseMap.Sample(samAnisotropic, pin.Tex);

	// Normal mapping
	if (UseNormalTexture)
	{
		float3 normalMapSample = gNormalMap.Sample(samAnisotropic, pin.Tex).rgb;

		// Uncompress each component from [0,1] to [-1,1].
		float3 normalT = 2.0f * normalMapSample - 1.0f;

		// Build orthonormal basis.
		float3 N = pin.NormalW;
		// float3 T = normalize(pin.TangentW - dot(pin.TangentW, N) * N); //< This seems to fix the normal swap, but it feels wrong
		float3 T = normalize(pin.NormalW - dot(pin.TangentW, N) * N);
		float3 B = cross(N, T);

		float3x3 TBN = float3x3(T, B, N);

		// Transform from tangent space to world space.
		float3 bumpedNormalW = mul(normalT, TBN);

		pin.NormalW = bumpedNormalW;
	}

	// Lighting
	float3 lightDir = float3(Direction.xyz);
	if (UseDiffuseTexture)
	{
		float4 litColor = diffuse;

		if (UseNormalTexture)
		{
			litColor += saturate(dot(lightDir, pin.NormalW)) * diffuse;
		}
		else
		{
			litColor += saturate(dot(lightDir, pin.NormalW)) * diffuse;
		}

		litColor.a = 1.0f;
		return litColor;
	}
	else
	{
		float4 purple = float4(1.0f, 0.0f, 1.0f, 1.0f);
		float4 litColor = saturate(dot(lightDir, pin.NormalW)) * purple;

		litColor.a = 1.0f;
		return litColor;
	}
}