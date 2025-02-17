// VERTEX SHADER
#version 330

// Matrices
uniform mat4 matrixProjection; // position in space (perspective, near Plain etc.)
uniform mat4 matrixView; // camera
uniform mat4 matrixModelView; // combined two to simplify calculations (one operation instead of two!)

// Materials - can be one material instead of 3 
uniform vec3 materialAmbient;
uniform vec3 materialDiffuse;
uniform vec3 materialSpecular;
uniform float shininess; // addition to specular light

//imput variables automaticly taken from a mesh (order Matter!!!! becouse they are ordered in a mesh)
in vec3 aVertex;
in vec3 aNormal;

//just to provide vertexes to fragment shader
in vec2 aTexCoord;
out vec2 texCoord0;

//gloabl vars
out vec4 color;
out vec4 position;
out vec3 normal;

//********** LIGHT DECLARATION **********

//AMBIENT LIGHT
struct AMBIENT
{
	vec3 color;
};

uniform AMBIENT lightAmbient, lightEmissive; // lightEmissive for bulb (not used)

vec4 AmbientLight(AMBIENT light)
{
	// Calculate Ambient Light
	return vec4(materialAmbient * light.color, 1);
}

//Normal Map
in vec3 aTangent;
in vec3 aBiTangent;
out mat3 matrixTangent;

//fog
out float fogFactor;
uniform float fogDensity;
 
//shadow
uniform mat4 matrixShadow;
out vec4 shadowCoord;
uniform mat4 shadowCameraView;

//water reflection
uniform vec4 planeClip;	

void main(void)
{
	// calculate position
	position = matrixModelView * vec4(aVertex, 1.0);
	gl_Position = matrixProjection * position;


		//water reflection
	// setup the clip distance
	gl_ClipDistance[0] = dot(inverse(matrixView) * position, planeClip);


	//don't want 4D transformation matrix to transform normals – or any other direction vectors! so mat3
	normal = normalize(mat3(matrixModelView) * aNormal);

	//normal map calculations
	// calculate tangent local system transformation
	vec3 tangent = normalize(mat3(matrixModelView) * aTangent);
	vec3 biTangent = normalize(mat3(matrixModelView) * aBiTangent);
	//tangent = normalize(tangent - dot(tangent, normal) * normal); // Gramm-Schmidt process
	//vec3 biTangent = cross(normal, tangent);

	matrixTangent = mat3(tangent, biTangent, normal);


	// calculate texture coordinate //just to provide vertexes to fragment shader
	texCoord0 = aTexCoord;

	// calculate shadow coordinate – using the Shadow Matrix
	mat4 matrixModel = inverse(shadowCameraView) * matrixModelView;
	shadowCoord = matrixShadow * matrixModel * vec4(aVertex, 1);

	//fog
	fogFactor = exp2(-fogDensity * length(position));
 
 	// calculate light
	color = vec4(0, 0, 0, 1);

	color += AmbientLight(lightAmbient);




}