cbuffer cbPerObject{
	float4x4 matWorldViewProj : WORLDVIEWPROJECTION;
};

RasterizerState Debug
{
	FillMode = SOLID;
	//CullMode = NONE;
};

DepthStencilState NoDepth
{
	DepthEnable = FALSE;
};

struct VS_INPUT{
	float3 Pos : POSITION;
	float4 Color : COLOR;
};

struct VS_OUTPUT{
	float4 Pos : SV_POSITION;
	float4 Color : COLOR;
};

VS_OUTPUT VS(VS_INPUT input){

	VS_OUTPUT output = (VS_OUTPUT)0;

	output.Pos = mul(float4(input.Pos,1), matWorldViewProj);
	output.Color = input.Color;

	return output;
}

float4 PS(VS_OUTPUT input):SV_TARGET
{
	return input.Color;
}

technique11 Default
{
	pass one
	{
		SetVertexShader( CompileShader ( vs_4_0, VS() ));
		SetGeometryShader( NULL );
		SetPixelShader( CompileShader ( ps_4_0, PS() ));
		//SetRasterizerState(Debug);
		//SetDepthStencilState(NoDepth,0);
	}
}

