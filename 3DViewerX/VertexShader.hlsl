
#include "Header.hlsli"

PS_INPUT main(VS_INPUT input)
{
	PS_INPUT output;
	output.Pos = mul(input.Pos, World);
	output.Pos = mul(output.Pos, View);
	output.Pos = mul(output.Pos, Projection);

	return output;
}