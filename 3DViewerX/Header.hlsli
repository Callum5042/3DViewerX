
cbuffer ConstantBuffer : register(b0)
{
	matrix World;
	matrix View;
	matrix Projection;
}

cbuffer LightBuffer : register(b1)
{
	float4 Direction;
};

struct VS_INPUT
{
	float3 PosL : POSITION;
	float3 NormalL : NORMAL;
	float2 Tex : TEXCOORD;
	float3 TangentL : TANGENT;
};

struct PS_INPUT
{
	float3 PosW : POSITION;
	float4 PosH : SV_POSITION;
	float3 NormalW : NORMAL;
	float2 Tex : TEXCOORD;
	float3 TangentW : TANGENT;
};

Texture2D gDiffuseMap : register(t0);
Texture2D gNormalMap : register(t1);

SamplerState samAnisotropic
{
	Filter = ANISOTROPIC;
	MaxAnisotropy = 16;

	AddressU = WRAP;
	AddressV = WRAP;
};