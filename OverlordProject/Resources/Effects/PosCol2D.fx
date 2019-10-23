struct VS_INPUT{
	float3 pos : POSITION;
	float4 color : COLOR;
};
struct VS_OUTPUT{
	float4 pos : SV_POSITION;
	float4 color : COLOR;
};


struct PS_OUTPUT{
	float4 color : SV_TARGET;
};


//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
VS_OUTPUT VS(VS_INPUT input){
	VS_OUTPUT output;
	output.pos=float4(input.pos,1.0f);
	output.color=input.color;
	return output;
}

//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
PS_OUTPUT PS(VS_OUTPUT input){
	PS_OUTPUT output;
	output.color=input.color;
	return output;
}

//--------------------------------------------------------------------------------------
// Technique
//--------------------------------------------------------------------------------------
technique11 PosCol2DTech
{
    pass P0
    {
        SetVertexShader( CompileShader( vs_4_0, VS() ) );
		SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_4_0, PS() ) );
    }
}

