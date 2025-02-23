#pragma once

#include <iostream>
#include <GL/glew.h>
#include <3dgl/3dgl.h>
#include <GL/glut.h>
#include <GL/freeglut_ext.h>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

using namespace std;
using namespace _3dgl;
using namespace glm;


mat4 setMatrix(vec3 Translate, float RotAngle, vec3 Rotate, vec3 Scale, vec3 objColour);

void CalcCurrentDayTime();
void Directional();
void AmbientLight();
void SkyBoxAndDayCalculation(bool isReflected = false);
void streetLampFun(float x, float y, float z, string shaderNamePos, string shaderDiffuse, string shaderSpecular, int lampControll, vec3 color = { 0.1 , 0.2, 0.7 });