
cbuffer ConstantBuffer : register(b0)
{
	matrix World;
	matrix View;
	matrix Projection;
}

struct VS_INPUT
{
	float4 Pos : POSITION;
};

struct PS_INPUT
{
	float4 Pos : SV_POSITION;
};