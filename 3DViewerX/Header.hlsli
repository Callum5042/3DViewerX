
cbuffer ConstantBuffer : register(b0)
{
	matrix World;
	matrix View;
	matrix Projection;
}

struct VS_INPUT
{
	float3 PosL : POSITION;
	float3 NormalL : NORMAL;
	float2 Tex : TEXCOORD;
};

struct PS_INPUT
{
	float3 PosW : POSITION;
	float4 PosH : SV_POSITION;
	float3 NormalW : NORMAL;
	float2 Tex : TEXCOORD;
};

Texture2D gDiffuseMap : register(t0);

SamplerState samAnisotropic
{
	Filter = ANISOTROPIC;
	MaxAnisotropy = 4;

	AddressU = WRAP;
	AddressV = WRAP;
};