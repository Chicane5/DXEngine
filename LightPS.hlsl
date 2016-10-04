/////////////
// GLOBALS //
/////////////
Texture2D shaderTexture;
SamplerState SampleType;

//2 vars inside the LightBuffer that hold diffuse color and direction of light. These will be set from the new LightClass object.

cbuffer LightBuffer
{
	float4 diffuseColor;
	float3 lightDirection;
	float  padding;
};

//////////////
// TYPEDEFS //
//////////////
struct PixelInputType
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
};

////////////////////////////////////////////////////////////////////////////////
// Pixel Shader
////////////////////////////////////////////////////////////////////////////////
float4 LightPixelShader(PixelInputType input) : SV_TARGET
{
	float4 textureColor;
	float3 lightDir;
	float lightIntensity;
	float4 color;


	// Sample the pixel color from the texture using the sampler at this texture coordinate location.
	textureColor = shaderTexture.Sample(SampleType, input.tex);

	//This is where the lighting equation that was discussed earlier is now implemented. 
	//The light intensity value is calculated as the dot product between the normal vector of triangle and the light direction vector.
	
	//invert the light direction for calculations
	lightDir = -lightDirection;

	//calculate the amount of light on this pixel
	lightIntensity = saturate(dot(input.normal, lightDir));

	//difuse color final
	color = saturate(diffuseColor * lightIntensity);
	color = color * textureColor;

	return color;
}