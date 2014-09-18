

cbuffer cbPerFrame
{
	float3 gEyePosW;

	//float gFogStart;
	//float gFogRange;
	//float gFogColour;
};

cbuffer cbPerObject
{
	float4x4 gWVP;
};

Texture2D gDiffuseMap;

cbuffer cbFixed
{
	float2 gTexC[4] =
	{
		float2(0.0f, 1.0f),
		float2(0.0f, 0.0f),
		float2(1.0f, 1.0f),
		float2(1.0f, 0.0f)
	};
};

SamplerState samLinear
{
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = CLAMP;
	AddressV = CLAMP;
};

struct VertexIn
{
	float3 CenterW : POSITION;
	float2 SizeW : SIZE;
};

struct VertexOut
{
	float3 CenterW : POSITION;
	float2 SizeW : SIZE;
};

struct GeoOut
{
	float4 PosH : SV_POSITION;
	float3 PosW : POSITION;
	float3 NormalW : NORMAL;
	float2 Tex : TEXCOORD;
	uint PrimID : SV_PrimitiveID;
};


VertexOut VS(VertexIn vin)
{
	VertexOut vout;
	vout.CenterW = vin.CenterW;
	vout.SizeW = vin.SizeW;

	return vout;
}

[maxvertexcount(4)]
void GS(point VertexOut gin[1], uint primID : SV_PrimitiveID,
	inout TriangleStream<GeoOut> triStream)
{
	float3 look = gEyePosW - gin[0].CenterW;
	look = normalize(look);
	float3 right = normalize(cross(float3(0, 1, 0), look));
	float3 up = cross(look, right);

	float    halfWidth  =   0.5f *  gin[0].SizeW.x;
	float halfHeight = 0.5f * gin[0].SizeW.y;

	float4 v[4];
	v[0] = float4(gin[0].CenterW + halfWidth * right -
		halfHeight * up, 1.0f);
	v[1] = float4(gin[0].CenterW + halfWidth * right +
		halfHeight * up, 1.0f);
	v[2] = float4(gin[0].CenterW - halfWidth * right -
		halfHeight * up, 1.0f);
	v[3] = float4(gin[0].CenterW - halfWidth * right +
		halfHeight * up, 1.0f);

	GeoOut gout;
	[unroll]
	for (int i = 0; i < 4; ++i)
	{
		gout.PosH = mul(v[i], gWVP);
		gout.PosW = v[i].xyz;
		gout.Tex = gTexC[i];
		gout.PrimID = primID;
		gout.NormalW = look;

		triStream.Append(gout);
	}
}

float4 PS(GeoOut pin) : SV_Target
{
	// Interpolating normal can unnormalize it, so normalize it.
	pin.NormalW = normalize(pin.NormalW);

	// The toEye vector is used in lighting.
	float3 toEye = gEyePosW - pin.PosW;

	// Cache the distance to the eye from this surface point.
	float distToEye = length(toEye);

	// Normalize.
	toEye /= distToEye;

	// Default to multiplicative identity.
	float4 texColour = float4(1, 1, 1, 1);

		// Sample texture.
	texColour = gDiffuseMap.Sample(samLinear, pin.Tex);


	float4 retColour;

	//float fogLerp = saturate((distToEye - gFogStart) / gFogRange);

	// Blend the fog color and the lit color.
	//retColour = lerp(texColor, gFogColor, fogLerp);

	retColour = texColour;

	// Common to take alpha from diffuse material and texture.
	retColour.a = texColour.a;

	return retColour;
}

technique11 TestTech
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_5_0, VS()));
		SetGeometryShader(CompileShader(gs_5_0, GS()));
		SetPixelShader(CompileShader(ps_5_0, PS()));
	}
}