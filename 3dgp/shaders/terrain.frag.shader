#version 330

// Input Variables (received from Vertex Shader)
in vec4 color;
in vec4 position;
in vec3 normal;
in vec2 texCoord0;

// Output Variable (sent down through the Pipeline)
out vec4 outColor;
 
//2D texture samplers, one for the shore features (for example grass) and one for the lake bed
uniform sampler2D textureBed;
uniform sampler2D textureShore;

// Water-related
uniform vec3 waterColor;

// Input: Water Related

in float waterDepth; // water depth (positive for underwater, negative for the shore)


//fog
in float fogFactor;


void main(void) 
{
	outColor = color;

	float isAboveWater = clamp(-waterDepth, 0, 1);
	outColor *= mix(texture(textureBed, texCoord0), texture(textureShore, texCoord0), isAboveWater);

	//fog
	outColor = mix(vec4(waterColor, 2), outColor, fogFactor);
}
