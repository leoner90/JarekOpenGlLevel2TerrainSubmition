#pragma once
#include <iostream>
#include <GL/glew.h>
#include <3dgl/3dgl.h>
#include <GL/glut.h>
#include <GL/freeglut_ext.h>

// Include GLM core features
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#pragma comment (lib, "glew32.lib")

using namespace std;
using namespace _3dgl;
using namespace glm;

// Window Size
extern GLuint WImage;
extern GLuint HImage;

// Post Processing
extern GLuint bufQuad;

// 3D Terrains
extern C3dglTerrain terrain, road, iceWater, water;

// 3D Models
extern C3dglModel cristmasTree;
extern C3dglModel streetLamp;
extern C3dglModel house;

// Skyboxes
extern C3dglSkyBox skybox;
extern C3dglSkyBox NightSkybox;

// Textures
extern GLuint idTexSnow;
extern GLuint idTexRoad;
extern GLuint idTexWater;
extern GLuint idTexNone; // null Texture
extern GLuint idTexGrass;
extern GLuint idTexSand;

// Post Processing
extern GLuint idTexScreen;

// Shadow Map
extern GLuint idFBO;
extern GLuint idFBOPostProc;

// Normal Mapping
extern GLuint idTexNormal;
extern GLuint idTexNormalICe;

// The View Matrix
extern mat4 matrixView;

// Camera & Navigation
extern float maxspeed;
extern float accel;
extern vec3 _acc, _vel;
extern float _fov;

// GLSL Program
extern C3dglProgram program;
extern C3dglProgram programEffect;
extern C3dglProgram programWater;


// Day/Night Regime
extern bool isItNight;
extern bool IsTimerReseted;
extern bool isPointLightOff;
extern float timeAccelerator;

// Day/Night global vars
extern float dayFraction;
extern float DayRotationAngle;

// Sunset effect
extern float sunsetColorDivider;

// Lamp Control: 0 = auto, 1 = manual ON, 2 = Manual OFF
extern int AreLeftLampsOff;
extern int AreRightLampsOff;

//bit map
extern C3dglBitmap bm;

//water
extern float waterLevel;

//reflection
extern mat4 matrixReflection;
extern float a , b, c, d;