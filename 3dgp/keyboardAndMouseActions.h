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
void onKeyDown(unsigned char key, int x, int y);
void onKeyUp(unsigned char key, int x, int y);
void onSpecDown(int key, int x, int y);
void onSpecUp(int key, int x, int y);
void onMouse(int button, int state, int x, int y);
void onMotion(int x, int y);
void onMouseWheel(int button, int dir, int x, int y);

//to move
void onReshape(int w, int h);