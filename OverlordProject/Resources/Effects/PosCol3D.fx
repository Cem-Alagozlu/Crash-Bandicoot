//--------------------------------------------------
//	Variables
//--------------------------------------------------
cbuffer cbPerObject
{
	float4x4 gWorldViewProj; 
};

//--------------------------------------------------
//	Input/Output Structs
//--------------------------------------------------
struct VS_INPUT{
	float3 LocalPosition : POSITION;
	float4 Color : COLOR;
};

struct VS_OUTPUT{
	float4 Position : SV_POSITION; //FLOAT4!!
	float4 Color : COLOR;
};

struct PS_OUTPUT {
	float4 Color : SV_TARGET; //FLOAT4!! > COLOR
};

//---------------------------------------------------------------------------
//	Vertex Shader
//---------------------------------------------------------------------------
VS_OUTPUT VS(VS_INPUT input){
	VS_OUTPUT output;

	//Transforming the position
	output.Position = mul (float4(input.LocalPosition, 1), gWorldViewProj );
	output.Color = input.Color;
	return output;
}

//---------------------------------------------------------------------------
// Pixel Shader
//---------------------------------------------------------------------------
PS_OUTPUT PS(VS_OUTPUT input){
	PS_OUTPUT output;

	output.Color = input.Color;
	return output;
}

//---------------------------------------------------------------------------
// Technique
//---------------------------------------------------------------------------
technique11 PosCol3DTech
{
	pass P0
	{
		SetVertexShader( CompileShader( vs_4_0, VS() ) );
		SetGeometryShader( NULL );
		SetPixelShader( CompileShader( ps_4_0, PS() ) );
	}
}

