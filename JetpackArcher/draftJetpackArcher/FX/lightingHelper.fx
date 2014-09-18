struct PointLight
{
	float4 lightColour;
	float3 pos;
	float range;

	float3 att;
	float pad;
};

struct SpotLight
{
	float4 lightColour;
	float3 pos;
	float range;

	float3 direction;
	float spot;

	float3 att;
	float pad;	
};

struct Material
{
	//float4 ambient;
	float4 specular; // how shiny it is (usually is grayscale)
	float4 diffuse; // we are going to use for ambient
};

//Note: toEye must be normalized
void ComputePointLight(Material mat, PointLight l, float3 pos, 
					   float3 normal, float3 toEye, out float4 diffuse,
					   out float4 specular)
{
	diffuse = float4(0.0f, 0.0f, 0.0f, 1.0f);
	specular = float4(0.0f, 0.0f,0.0f, 1.0f);

	float3 lightVec = pos - l.pos;
	float d = length(lightVec);

	if(d > l.range)
	{
		return;
	}

	lightVec /= d;

	float kd = dot(-lightVec, normal);
	if(kd > 0)
	{
		diffuse = (kd * l.lightColour) * mat.diffuse;
		
		float3 r = reflect(lightVec, normal);

		float ks = pow(max(dot(toEye, r), 0.0f), mat.specular.w);

		specular = (ks * l.lightColour) * mat.specular;
	}

	float attenuation = 1.0f / (l.att.x + d * l.att.y + pow(d,2) * l.att.z);
	diffuse *= attenuation;
	specular *= attenuation;
}

void ComputeSpotLight(Material mat, SpotLight light, float3 pos, 
					  float3 normal, float3 toEye, out float4 diffuse,
					  out float4 specular)
{
	diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
	specular    = float4(0.0f, 0.0f, 0.0f, 0.0f);

	PointLight temp;
	temp.lightColour = light.lightColour;
	temp.pos = light.pos;
	temp.range = light.range;
	temp.att = light.att;

	ComputePointLight(mat, temp, pos, normal, toEye, diffuse, specular);

	float3 lightVec = pos - light.pos;
	lightVec = normalize(lightVec);

	float spotFactor = pow(max(dot(lightVec, light.direction), 0.0f), light.spot);

	diffuse *= spotFactor;
	specular *= spotFactor;
}

//Note: toEye must be normalized
void ComputePointLightToon(Material mat, PointLight l, float3 pos, 
					   float3 normal, float3 toEye, out float4 diffuse,
					   out float4 specular)
{
	diffuse = float4(0.0f, 0.0f, 0.0f, 1.0f);
	specular = float4(0.0f, 0.0f,0.0f, 1.0f);

	float3 lightVec = pos - l.pos;
	float d = length(lightVec);

	if(d > l.range)
	{
		return;
	}

	lightVec /= d;

	float kd = dot(-lightVec, normal);
	if(kd <= 0.0f)
	{
		kd = 0.4f;
	}
	else if(kd <= 0.5)
	{
		kd = 0.6f;
	}
	else
	{
		kd = 1.0f;
	}

		
		diffuse = (kd * l.lightColour) * mat.diffuse;
		
		float3 r = reflect(lightVec, normal);

		float ks = pow(max(dot(toEye, r), 0.0f), mat.specular.w);

		if(ks <= 0.1f)
	{
		kd = 0.0f;
	}
	else if(ks <= 0.8)
	{
		kd = 0.5f;
	}
	else
	{
		kd = 0.8f;
	}

		specular = (ks * l.lightColour) * mat.specular;
	

	float attenuation = 1.0f / (l.att.x + d * l.att.y + pow(d,2) * l.att.z);
	diffuse *= attenuation;
	specular *= attenuation;
}

void ComputeSpotLightToon(Material mat, SpotLight light, float3 pos, 
					  float3 normal, float3 toEye, out float4 diffuse,
					  out float4 specular)
{
	diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
	specular    = float4(0.0f, 0.0f, 0.0f, 0.0f);

	PointLight temp;
	temp.lightColour = light.lightColour;
	temp.pos = light.pos;
	temp.range = light.range;
	temp.att = light.att;

	ComputePointLightToon(mat, temp, pos, normal, toEye, diffuse, specular);

	float3 lightVec = pos - light.pos;
	lightVec = normalize(lightVec);

	float spotFactor = pow(max(dot(lightVec, light.direction), 0.0f), light.spot);

	if(spotFactor < 0.2f)
	{
		spotFactor = 0.0f;
	}
	else if(spotFactor < 0.8f)
	{
		spotFactor = 0.5f;
	}
	else
	{
		spotFactor = 1.0f;
	}

	diffuse *= spotFactor;
	specular *= spotFactor;
}