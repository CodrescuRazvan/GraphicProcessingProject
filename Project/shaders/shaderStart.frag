#version 410 core

layout(location=0) in vec3 vPosition;
layout(location=1) in vec3 vNormal;

//matrices
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat3 normalMatrix;

//in vec3 fPosition;
in vec3 fNormal;
in vec4 fPosEye;
in vec2 fTexCoords;
in vec4 fragPosLightSpace;

uniform float lightStatus;
uniform float ok;

out vec4 fColor;

out vec3 color;

//lighting
uniform	vec3 lightDir;
uniform	vec3 lightColor;
uniform vec3 baseColor;

//texture
uniform sampler2D diffuseTexture;
uniform sampler2D specularTexture;
uniform sampler2D shadowMap;

vec3 ambient;
float ambientStrength = 0.2f;
vec3 diffuse;
vec3 specular;
float specularStrength = 0.5f;
float shininess = 32.0f;
float linear = 0.0045f;
float quadratic = 0.0075f;

uniform float transparency;
uniform float transp;

float computeFog()
{
 float fogDensity = 0.05f;
 float fragmentDistance = length(fPosEye);
 float fogFactor = exp(-pow(fragmentDistance * fogDensity, 2));

 return clamp(fogFactor, 0.0f, 1.0f);
}

float computeShadow()
{
	// Check whether current frag pos is in shadow
	float bias = 0.005f;
	

	vec3 normalizedCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
	normalizedCoords = normalizedCoords * 0.5 + 0.5;

	if (normalizedCoords.z > 1.0f)
		return 0.0f;


	float closestDepth = texture(shadowMap, normalizedCoords.xy).r;

	// Get depth of current fragment from light's perspective
	float currentDepth = normalizedCoords.z;

	currentDepth = currentDepth - bias;

	// Check whether current frag pos is in shadow
	float shadow = currentDepth > closestDepth ? 1.0f : 0.0f;

	return shadow;
}
//modulate with shadow


void computeLightComponents()
{		
	if(lightStatus > 0.5f){
		vec3 cameraPosEye = vec3(0.0f);//in eye coordinates, the viewer is situated at the origin
	
		float dist = length(vec3(0.0f, 5.0f, 4.0f) - fPosEye.xyz);

		float att = 1.0f / (1.0f + linear * dist + quadratic * (dist * dist));

		//transform normal
		vec3 normalEye = normalize(fNormal);	
	
		//compute light direction
		vec3 lightDirN = normalize(lightDir);
	
		//compute view direction 
		vec3 viewDirN = normalize(cameraPosEye - fPosEye.xyz);
		
		//compute ambient light
		ambient = att * ambientStrength * lightColor;
	
		//compute diffuse light
		diffuse = att * max(dot(normalEye, lightDirN), 0.0f) * lightColor;
	
		//compute specular light
		vec3 reflection = reflect(-lightDirN, normalEye);
		float specCoeff = pow(max(dot(viewDirN, reflection), 0.0f), shininess);
		specular = att * specularStrength * specCoeff * lightColor;
	} else {
		ambientStrength = 0.5f;
		specularStrength = 0.75f;
		//compute ambient light
		ambient = ambientStrength * lightColor;

		//normalize the light's direction
		vec3 lightDirN = normalize(lightDir);

		//compute eye coordinates for normals
		vec3 normalEye = normalize(normalMatrix * vNormal);

		//compute diffuse light
		diffuse = max(dot(normalEye, lightDirN), 0.0f) * lightColor;

		//compute the view (Eye) direction (in eye coordinates, the camera is at the origin)
		vec3 viewDir = normalize(- fPosEye.xyz);

		//compute the light's reflection (the reflect function requires a direction pointing towards the vertex, not away from it)
		vec3 reflectDir = reflect(-lightDir, normalEye);
		//vec3 reflectDir = normalize(reflect(-lightDir, normalEye));

		//compute specular light
		float specCoeff = pow(max(dot(viewDir, reflectDir), 0.0f), shininess);
		specular = specularStrength * specCoeff * lightColor;
		
		//compute final vertex color
		//color = min((ambient + diffuse) * baseColor + specular, 1.0f);

	}
}

void main() 
{
	float shadow = computeShadow();

	computeLightComponents();
	
	vec3 baseColor = vec3(0.9f, 0.35f, 0.0f);//orange
	
	ambient *= texture(diffuseTexture, fTexCoords).rgb;
	diffuse *= texture(diffuseTexture, fTexCoords).rgb;
	specular *= texture(specularTexture, fTexCoords).rgb;

	vec3 color = min((ambient + (1.0f - shadow)*diffuse) + (1.0f - shadow)*specular, 1.0f);
    //vec3 color = min((ambient + diffuse) + specular, 1.0f);

	//ceata
    if(ok < 0.5f){
		if(transp > 0.5f){
			float fogFactor = computeFog();
			vec4 fogColor = vec4(0.5f, 0.5f, 0.5f, 1.0f);
			fColor = mix(fogColor, vec4(color, transparency), fogFactor);
		} else {
			float fogFactor = computeFog();
			vec4 fogColor = vec4(0.5f, 0.5f, 0.5f, 1.0f);
			fColor = mix(fogColor, vec4(color, 1.0f), fogFactor);
		}
    } else {
		//fColor = vec4(color, 1.0f);
        if(transp > 0.5f){
			fColor = vec4(color, transparency);
		} else {
			fColor = vec4(color, 1.0f);
	}
    }
}