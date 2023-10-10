Texture2D texture0 : register(t0);
SamplerState sampler0 : register(s0);


cbuffer LightBuffer : register(b0)
{
    float4 normals;
}

struct InputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
    float3 worldPosition : TEXCOORD1;
    float3 viewVector : TEXCOORD2;
};

float4 main(InputType input) : SV_TARGET
{
    float4 TexColour;
    TexColour = texture0.Sample(sampler0, input.tex);
    
    if (normals.a == 1)
    {   // render colour as normals
        TexColour.xyz = input.normal.xyz * 0.5 + 0.5;
        return TexColour;
    }
    
    return saturate(TexColour); // apply texture
}