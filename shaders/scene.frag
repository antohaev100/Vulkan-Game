#version 450

layout (binding = 1) uniform samplerCube shadowCubeMap;

layout (location = 0) in vec3 inNormal;
layout (location = 1) in vec3 inColor;
layout (location = 2) in vec3 inEyePos;
layout (location = 3) in vec3 inLightVec;
layout (location = 4) in vec3 inWorldPos;
layout (location = 5) in vec3 inLightPos;

layout (location = 0) out vec4 outFragColor;

#define EPSILON 0.005
#define SHADOW_OPACITY 0.96
#define Ambient 0.7
#define distanceLightFactor 4.0
#define lightSourceSize 0.08
//#define SPECULAR_INTENSITY 1.0
//#define SHININESS 32.0

void main() 
{

	// Diffuse light
	vec3 N = normalize(inNormal);
	vec3 L = normalize(inLightVec);	
	float diffuse = max(dot(N, L), 0.0)/2 + Ambient;

	// Specular light
	//vec3 Eye = normalize(-inEyePos);
	//vec3 Reflected = normalize(reflect(-inLightVec, inNormal)); 
    //float specular = pow(max(dot(Eye, Reflected), 0.0), SHININESS) * SPECULAR_INTENSITY;
	//outFragColor.rgb += vec3(specular * inColor);

		
	// Shadow
	vec3 lightVec = (inWorldPos - inLightPos);
    float sampledDist = texture(shadowCubeMap, lightVec).r;
    float dist = length(lightVec) / length(vec3(10.1));
	if(dist > sampledDist + EPSILON)
	{
		diffuse = Ambient;
	} 
	float diff = dist - sampledDist;

	outFragColor = vec4(diffuse * vec4(inColor, 1.0));	

	//light distance effect:
	outFragColor.rgb *= 1.0 - dist / distanceLightFactor;

	//light source effect:
	if(dist < lightSourceSize){
	outFragColor.rgb *= lightSourceSize/dist;
	}
}