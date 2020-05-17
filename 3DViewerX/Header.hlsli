
cbuffer ConstantBuffer : register(b0)
{
	matrix World;
	matrix View;
	matrix Projection;
	int UseDiffuseTexture;
	int UseNormalTexture;
	int UseCubeMap;
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
	float3 BitTangentL : BITTANGENT;
};

struct PS_INPUT
{
	float3 PosW : POSITION;
	float4 PosH : SV_POSITION;
	float3 NormalW : NORMAL;
	float2 Tex : TEXCOORD;
	float3 TangentW : TANGENT;
	float3 BitTangentW : BITTANGENT;
};

Texture2D gDiffuseMap : register(t0);
Texture2D gNormalMap : register(t1);
TextureCube gCubeMap : register(t2);

SamplerState samAnisotropic : register(s0);