Texture2D texture0 : register(t0);
SamplerState sampler0 : register(s0);

cbuffer LightBuffer : register(b0)
{
    float4 ambient;
    float4 diffuse;
    float4 position;
    float4 specularPower;
    float4 specular;
    float4 direction;
}

struct InputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
    float3 worldPosition : TEXCOORD1;
    float3 viewVector : TEXCOORD2;
};



float4 calculateLighting(float3 lightDirection, float3 normal, float4 diffuse)
{
    float intensity = saturate(dot(normal, lightDirection));
    float4 colour = saturate(diffuse * intensity);
    return colour;

}


float4 calcSpecular(float3 lightDirection, float3 normal, float3 viewVector, float4 specularColour, float specularPower)
{
    // blinn-phong specular calculation
    float3 halfway = normalize(lightDirection + viewVector);
    float specularIntensity = pow(max(dot(normal, halfway), 0.0), specularPower);
    return saturate(specularColour * specularIntensity);
}


float4 main(InputType input) : SV_TARGET
{ 
    float4 lightColourResult = float(0);
    float4 waveTexColour = float(0);
    float4 lightColour = float(0);
    
    waveTexColour = texture0.Sample(sampler0, input.tex);  

    if (specularPower.a == 1)
    { 
     // render normals as colour      
        waveTexColour.xyz = input.normal.xyz * 0.5 + 0.5;      
        return waveTexColour;
    }
    
    // directional light
    float3 dir = normalize(-direction);     
    // specular lighting
    float4 specularCol = calcSpecular(dir, input.normal, input.viewVector, specular, specularPower.x);
    lightColourResult +=  ambient + calculateLighting(dir, input.normal, diffuse) +specularCol;
    
    return lightColourResult * waveTexColour;
}