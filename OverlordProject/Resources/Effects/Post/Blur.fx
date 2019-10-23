//=============================================================================
//// Shader uses position and texture
//=============================================================================
SamplerState samPoint
{
    Filter = MIN_MAG_MIP_POINT;
    AddressU = Mirror;
    AddressV = Mirror;
};

Texture2D gTexture;

/// Create Depth Stencil State (ENABLE DEPTH WRITING)
DepthStencilState gDSS_NoDepth
{
    DepthEnable = FALSE;
    DepthWriteMask = 0;
};

/// Create Rasterizer State (Backface culling) 
RasterizerState gRS_NoBackfaceCulling
{
    CullMode = BACK;
};

//IN/OUT STRUCTS
//--------------
struct VS_INPUT
{
    float3 Position : POSITION;
	float2 TexCoord : TEXCOORD0;

};

struct PS_INPUT
{
    float4 Position : SV_POSITION;
	float2 TexCoord : TEXCOORD1;
};


//VERTEX SHADER
//-------------
PS_INPUT VS(VS_INPUT input)
{
	PS_INPUT output = (PS_INPUT)0;
	// Set the Position
	output.Position = float4(input.Position, 1.0f);
	   
	// Set the TexCoord
	output.TexCoord = input.TexCoord;
	
	return output;
}


//PIXEL SHADER
//------------
float4 PS(PS_INPUT input): SV_Target
{
	// Step 1: find the dimensions of the texture (the texture has a method for that)
	float texWidth;
	float texHeight;
	
    gTexture.GetDimensions(texWidth, texHeight);
	
	// Step 2: calculate dx and dy (UV space for 1 pixel)
	float dx = 1.0f / texWidth;
    float dy = 1.0f / texHeight;
	
	// Step 3: Create a double for loop (5 iterations each)
	float4 color = float4(0,0,0,0);
	float totalPasses;
	
	for(int i = 0; i < 5; ++i)
	{
		for(int j = 0; j < 5; ++j)
		{		
		
			float x = input.TexCoord.x - (2 * dx) + (j * dx);
			float y = input.TexCoord.y + (2 * dy) - (i * dy);
			
		  if(x > 0 && x < 1 && y > 0 && y < 1)
            {
			color += gTexture.Sample(samPoint, float2(x,y));
			++totalPasses;
			}
		}
	}
	
	color /= totalPasses;
	color.a =  1.0f;
	return color;
}


//TECHNIQUE
//---------
technique11 Blur
{
    pass P0
    {
		// Set states...
		SetRasterizerState(gRS_NoBackfaceCulling);
        SetDepthStencilState(gDSS_NoDepth, 0);
        SetVertexShader( CompileShader( vs_4_0, VS() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_4_0, PS() ) );
    }
}