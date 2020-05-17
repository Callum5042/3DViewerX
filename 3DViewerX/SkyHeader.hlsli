
cbuffer ConstantBuffer : register(b0)
{
	matrix World;
	matrix View;
	matrix Proj;
}

struct VS_INPUT
{
	float3 PosL : POSITION;
};

struct PS_INPUT
{
	float4 PosH : SV_POSITION;
	float3 PosL : POSITION;
};

TextureCube gCubeMap : register(t0);
SamplerState samAnisotropic : register(s0);

DepthStencilState LessEqualDSS
{
	// Make sure the depth function is LESS_EQUAL and not just LESS.  
	// Otherwise, the normalized depth values at z = 1 (NDC) will 
	// fail the depth test if the depth buffer was cleared to 1.
	DepthFunc = LESS_EQUAL;
};