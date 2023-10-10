// Pixel shader used to render the terrain, adds directional and specular lighting 
// and textures the terrain according to slope and height.

Texture2D texture0 : register(t0);
Texture2D texture1 : register(t1);
Texture2D texture2 : register(t2);

SamplerState sampler0 : register(s0);

cbuffer LightBuffer : register(b0)
{
    float4 ambient;
    float4 diffuse;
    float4 position;
    float4 specularPower_volcano_normals;
    float4 specular;
    float4 direction;
};

struct InputType
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
    float3 worldPosition : TEXCOORD1;
    float3 viewVector : TEXCOORD2;

};

// Calculate lighting intensity based on direction and normal. Combine with light colour.
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
    
    int specularPower = specularPower_volcano_normals.r;
    int volcano = specularPower_volcano_normals.g;
    int normals = specularPower_volcano_normals.b;  
    float4 lightColourResult = float(0);
	float4 sandText;
    float4 grassText;
    float4 rockText;


	// Sample the textures
    sandText = texture0.Sample(sampler0, input.tex);
    grassText = texture1.Sample(sampler0, input.tex);
    rockText = texture2.Sample(sampler0, input.tex);

    
    if (normals == 1)
    {
     // render normals as colour       
        grassText.xyz = input.normal.xyz * 0.5 + 0.5;     
        return grassText;
    }
        
    // directional light
    float3 dir = normalize(-direction);
    // specular lighting
    float4 specularCol = calcSpecular(dir, input.normal, input.viewVector, specular, specularPower) * 0.2;
    lightColourResult += ambient + calculateLighting(dir, input.normal, diffuse);
    
	
    
    //// SAND ////
    if (input.worldPosition.y < 0) // sand texture
    {
            return lightColourResult * sandText;
    }

    if (input.worldPosition.y >= 0 && input.worldPosition.y < 1) // sand blend
    {
        
        if (input.normal.y <= 0.80){
            // sand and rock blend
            return lightColourResult * (lerp(sandText, rockText, input.worldPosition.y + 0));
        }
              
        if (input.normal.y > 0.80 && input.normal.y <= 0.90){
            // blend of sand with blend of rock and grass
            return lightColourResult * lerp(sandText, (lerp(rockText, grassText, (input.normal.y - 0.8) * 10)), input.worldPosition.y + 0);
        }

        if (input.normal.y > 0.90){
            //blend of sand and grass
            return lightColourResult * (lerp(sandText, grassText, input.worldPosition.y + 0));
        }
        
        return lightColourResult * (lerp(sandText, grassText, input.worldPosition.y + 0));
    }
    


    //// ROCK ////
    if (input.normal.y <= 0.80) // steep terrain
    {
       // rock texture with specular lighting
       return (lightColourResult + specularCol) * rockText;
    }    
   
    ////  ROCK AND GRASS BLEND ////
    if (input.normal.y > 0.80 && input.normal.y <= 0.90){
        
        // volcano textures if terrain is volcano:   
        if (volcano == 1 && input.worldPosition.y > 20 && input.worldPosition.y <= 21)
        {
            // blend of rock with blend of rock and grass, the specular colour is increased
            // as the texture moves more to rock texture since grass has low specular reflection 
            return (lightColourResult + specularCol * (1 - ((input.normal.y - 0.8) * 10))) * (lerp((lerp(rockText, grassText, (input.normal.y - 0.8) * 10)), rockText, input.worldPosition.y - 20));
        }

        if (volcano == 1 && input.worldPosition.y > 21)
        {
            // rock texture with specular lighting
            return (lightColourResult + specularCol) * rockText;
        }
        

        return (lightColourResult + specularCol * (1 - ((input.normal.y - 0.8) * 10))) * (lerp(rockText, grassText, (input.normal.y - 0.8) * 10));
    }    
    
    
    //// GRASS ////
    if (input.normal.y > 0.90) // flat terrain
    { 
       
        // volcano textures if terrain is volcano:   
        if (volcano == 1 && input.worldPosition.y > 20 && input.worldPosition.y <= 21)
        {
            // blend of rock and grass, the specular colour is increased as the texture moves 
            // more to rock texture since grass has low specular reflection         
            return (lightColourResult + specularCol * (((input.normal.y - 0.9) * 10))) * (lerp(grassText, rockText, input.worldPosition.y - 20));
                     
        }
        
        if (volcano == 1 && input.worldPosition.y > 21)
        {
            // rock texture with specular lighting
            return (lightColourResult + specularCol) * rockText;
        }
        
        return lightColourResult * grassText;
    }

            
    return (lightColourResult + specularCol) * rockText;
}



