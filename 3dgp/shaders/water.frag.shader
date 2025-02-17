#version 330

// Input Variables (received from Vertex Shader)
in vec4 color;
in vec4 position;
in vec3 normal;
in vec2 texCoord0;

// Output Variable (sent down through the Pipeline)
out vec4 outColor;


// Water-related
uniform vec3 waterColor;
uniform vec3 skyColor;

in float reflFactor;

void main(void) 
{
	//outColor = color;
	outColor = mix(vec4(waterColor, 0.2)  , vec4(skyColor, 1)   , reflFactor);
}
