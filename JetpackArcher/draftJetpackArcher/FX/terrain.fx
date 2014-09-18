//***************************************************************************************
// lighting.fx by Super Rob (C) 2014 No Rights Reserved.
//
// Transforms and colors geometry.
//***************************************************************************************

#include "lightingHelper.fx"

SamplerState mySampler
{
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = WRAP;
	AddressV = WRAP;
};

Texture2D gDiffuseMap;
Texture2D gDiffuseMap2;
Texture2D gDiffuseMap3;
Texture2D gDiffuseMap4;
Texture2D gBlend;

cbuffer cbPerObject
{
	float4x4 gWVP;
	float4x4 gW;
	float4x4 gInvTransposeW;
};

cbuffer cbPerFrame
{
	float4 gAmbientLight;
	PointLight gPointLight;
	float3 gEyePos;
	SpotLight gSpotLight;
};

cbuffer cbFixed
{
		float4 fogColour = float4(0.5f, 0.5f, 0.5f, 1.0f);
		float startFogDist = 0.0f;
		float maxFogDist = 75.0f;
};



struct VertexIn
{
	float3 PosL  : POSITION;
	float3 NormalL  : NORMAL;
    float2 tilingUV : TEXCOORD0;
	float2 splatUV : TEXCOORD1;
};

struct VertexOut
{
	float4 PosH  : SV_POSITION;
    float2 tilingUV : TEXCOORD0;
	float2 splatUV : TEXCOORD1;
	float3 PosW : POSITION;
	float3 NormalW : NORMAL;
};

VertexOut VS(VertexIn vin)
{
	VertexOut vout;
	
	vout.NormalW = mul(vin.NormalL, (float3x3)gInvTransposeW);

	vout.PosW = (float3)mul(float4(vin.PosL, 1.0f), gW);

	// Transform to homogeneous clip space.
	vout.PosH = mul(float4(vin.PosL, 1.0f), gWVP);
	
	// Just pass vertex color into the pixel shader.
    vout.tilingUV = vin.tilingUV;
	vout.splatUV = vin.splatUV;
    
    return vout;
}

float4 PS(VertexOut pin) : SV_Target
{

	Material mat;

	float4 blendColour = gBlend.Sample(mySampler, pin.splatUV);
	float4 c1 = gDiffuseMap.Sample(mySampler, pin.tilingUV);
	float4 c2 = gDiffuseMap2.Sample(mySampler, pin.tilingUV);
	float4 c3 = gDiffuseMap3.Sample(mySampler, pin.tilingUV);
	float4 c4 = gDiffuseMap4.Sample(mySampler, pin.tilingUV);

	mat.diffuse = c1;
	mat.diffuse = lerp(mat.diffuse, c2, blendColour.r);
	mat.diffuse = lerp(mat.diffuse, c3, blendColour.g);
	mat.diffuse = lerp(mat.diffuse, c4, blendColour.b);
	mat.specular = float4(0.3f, 0.3f, 0.3f, 100.0f);
	float3 toEye = gEyePos - pin.PosW;
	float dist = length(toEye);
	toEye = normalize(toEye); 


	float4 d, s;

	ComputePointLight(mat, gPointLight, pin.PosW, pin.NormalW, 
					 toEye, d, s);

	float4 diffuse = d;
	float4 specular = s;

	ComputeSpotLight(mat, gSpotLight, pin.PosW, pin.NormalW, toEye, d, s);

	diffuse += d;
	specular += s;

	float4 litColour = gAmbientLight * mat.diffuse + diffuse + specular;
	litColour.a = mat.diffuse.a;

	float rat = saturate((dist - startFogDist) / maxFogDist);
	litColour = lerp(litColour, fogColour, rat);

	return litColour;
}

technique11 TestTech
{
    pass P0
    {
        SetVertexShader( CompileShader( vs_5_0, VS() ) );
		SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_5_0, PS() ) );
		
    }
}
