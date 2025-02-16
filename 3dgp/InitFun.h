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

// Declare initialization functions
 
void shadowMapInit();
void PostProcessInit();
bool TextureSetup(const char textureName[], GLuint& textureId, GLuint textureNr);
bool init();