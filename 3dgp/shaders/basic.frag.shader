// FRAGMENT SHADER
#version 330

in vec4 color;
out vec4 outColor;

//textures
uniform sampler2D texture0;
in vec2 texCoord0;
 
//Per Fragment light every pixel calculates the light NOMRAL MAPS WORKS ONLY PER FRAGMENT LIGHT
in vec4 position;
//in vec3 normal;

// Matrices
uniform mat4 matrixView;


// Materials - can be one material instead of 3 
uniform vec3 materialDiffuse;
uniform vec3 materialSpecular;
uniform float shininess; // addition to specular light

//NORMAL map
uniform sampler2D textureNormal;
in mat3 matrixTangent;
vec3 normal;

//fog
uniform vec3 fogColour;
in float fogFactor;


//**** Directional light PER FRAGMENT
struct DIRECTIONAL
{
	vec3 direction;
	vec3 diffuse;
	vec3 specular;
};

uniform DIRECTIONAL lightDir;

vec4 DirectionalLight(DIRECTIONAL light)
{
	// Calculate Directional Light
	vec4 color = vec4(0, 0, 0, 0);
	vec3 L = normalize(mat3(matrixView) * light.direction); // matrixView makes camera depending light // //cordinats 4d  direction 3d( mat3)
	float NdotL = dot(normal, L); // control light intesivity depending on rotation to light source
	color += vec4(materialDiffuse * light.diffuse, 1) * max(NdotL, 0); // mix two colors // max(NdotL, 0) to not drop below 0 if away of light
 
	//shine
	vec3 V = normalize(-position.xyz);
	vec3 R = reflect(-L, normal);
	float RdotV = dot(R, V);
	color += vec4(materialSpecular * light.specular * pow(max(RdotV, 0), shininess), 1);


	return color;
}


//**** POINT LIGET PER FRAGMNT
struct POINT
{
	vec3 position;
	vec3 diffuse;
	vec3 specular;
};

uniform POINT lightPoint, lightPoint2, lightPoint3, lightPoint4, lightPoint5, lightPoint6, lightPoint7, lightPoint8, lightPoint9, lightPoint10;

vec4 PointLight(POINT light)
{
	// Calculate point Light
	vec4 color = vec4(0, 0, 0, 0);
	vec3 L = vec3(normalize(matrixView * vec4(light.position,1) - position));
	float NdotL = dot(normal, L);
	color += vec4(materialDiffuse * light.diffuse, 1) * max(NdotL, 0);

	//shine
	vec3 V = normalize(-position.xyz);
	vec3 R = reflect(-L, normal);
	float RdotV = dot(R, V);
	color += vec4(materialSpecular * light.specular * pow(max(RdotV, 0), shininess), 1);

	//ATTENUATION qadratic light drops 4 times
	float dist = length(matrixView * vec4(light.position, 1) - position);
	float DropRating = 0.01;
	//float att = 1 / (att_const + att_linear * dist + att_quadratic * dist * dist);
	float att = 1 / (DropRating * dist * dist);
 
	return color * att;
}

//shadow
in vec4 shadowCoord;
uniform sampler2DShadow shadowMap;

void main(void) 
{
	//new Normal Map
	normal = 2.0 * texture(textureNormal, texCoord0).xyz - vec3(1.0, 1.0, 1.0);
	normal = normalize(matrixTangent * normal);
	

 
	outColor = color;
	// for fragment light
	outColor += PointLight(lightPoint);
	outColor += PointLight(lightPoint2);
	outColor += PointLight(lightPoint3);
	outColor += PointLight(lightPoint4);
	outColor += PointLight(lightPoint5);
	outColor += PointLight(lightPoint6);
	outColor += PointLight(lightPoint7);
	outColor += PointLight(lightPoint8);
	outColor += PointLight(lightPoint9);
	outColor += PointLight(lightPoint10);

	outColor += DirectionalLight(lightDir);

	// Calculation of the shadow
	float shadow = 1.0;
	if (shadowCoord.w > 0) // if shadowCoord.w < 0 fragment is out of the Light POV
		shadow = 0.5 + 0.5 * textureProj(shadowMap, shadowCoord);

	// outColor *= shadow;

	//texture multyplier
	outColor *= texture(texture0, texCoord0);

  //fog
	outColor = mix(vec4(fogColour, 1), outColor, fogFactor);
}