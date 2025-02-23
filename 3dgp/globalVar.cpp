#include "globalVar.h"

// Window Size
GLuint WImage = 1280, HImage = 720;

// Post Processing
GLuint bufQuad;

// 3D Terrains
C3dglTerrain terrain, road, iceWater, water;

// 3D Models
C3dglModel cristmasTree;
C3dglModel streetLamp;
C3dglModel house;

// Skyboxes
C3dglSkyBox skybox;
C3dglSkyBox NightSkybox;

// Textures
GLuint idTexSnow;
GLuint idTexRoad;
GLuint idTexWater;
GLuint idTexNone; // null Texture
GLuint idTexGrass;
GLuint idTexSand;

// Post Processing
GLuint idTexScreen;

// Shadow Map
GLuint idFBO;
GLuint idFBOPostProc;

// Normal Mapping
GLuint idTexNormal;
GLuint idTexNormalICe;

// The View Matrix
mat4 matrixView;

// Camera & Navigation
float maxspeed = 28.f;
float accel = 8.f;
vec3 _acc(0), _vel(0);
float _fov = 60.f;

// GLSL Program
C3dglProgram program;
C3dglProgram programEffect;
C3dglProgram programWater;

// Day/Night Regime
bool isItNight = false;
bool IsTimerReseted = false;
bool isPointLightOff = false;
float timeAccelerator = 1;

// Day/Night global vars
float dayFraction = 0;
float DayRotationAngle = 0;

// Sunset effect
float sunsetColorDivider = 1;

// Lamp Control: 0 = auto, 1 = manual ON, 2 = Manual OFF
int AreLeftLampsOff = 0;
int AreRightLampsOff = 0;

//bit map
C3dglBitmap bm;


//WATER
float waterLevel = 6.2;



//WATER REFLECTION
// 
// Find the reflection surface (point and normal) rotation of the entire mirror around the Y axis (ry) and swivel of the mirror around the X axis (rx)
vec3 p(0, waterLevel , 0);
vec3 n = vec3(0, 1, 0);

// reflection matrix
float a = n.x, b = n.y, c = n.z, d = -dot(p, n);


// parameters of the reflection plane: Ax + By + Cz + d = 0

mat4 matrixReflection = mat4(1 - 2 * a * a, -2 * a * b, -2 * a * c, 0,
	-2 * a * b, 1 - 2 * b * b, -2 * b * c, 0,
	-2 * a * c, -2 * b * c, 1 - 2 * c * c, 0,
	-2 * a * d, -2 * b * d, -2 * c * d, 1);
