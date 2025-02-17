#include <iostream>
#include <GL/glew.h>
#include <3dgl/3dgl.h>
#include <GL/glut.h>
#include <GL/freeglut_ext.h>

// Include GLM core features
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#pragma comment (lib, "glew32.lib")
#include "globalVar.h"
#include "initFun.h"
#include "LightAndSkyBox.h"
#include "keyboardAndMouseActions.h"


//*********** RENDER SCENE ***********
void renderScene(mat4& matrixView, float time, float deltaTime)
{

	CalcCurrentDayTime();
	SkyBoxAndDayCalculation();
	Directional();
	AmbientLight();

	//cristmasTree
	cristmasTree.render(setMatrix({ 4.f, 8.5f, -7.f }, 0.f, { 1.0f, 1.0f, 0.0f }, { 7.f,  7.f ,  7.f }, { 0.f, 1.f, 0.f }));
	cristmasTree.render(setMatrix({ 40.f, 24.5f, 27.f }, 0.f, { 1.0f, 1.0f, 0.0f }, { 7.f,  7.f ,  7.f }, { 0.f, 1.f, 0.f }));
	cristmasTree.render(setMatrix({ 70.f, 24.5f, 9.f }, 0.f, { 1.0f, 1.0f, 0.0f }, { 7.f,  7.f ,  7.f }, { 0.f, 1.f, 0.f }));

	//House
	house.render(setMatrix({ 53.f, 24.5f, 10.f }, 0.f, { 1.0f, 1.0f, 0.0f }, { 0.1f,  0.1f ,  0.1f }, { 1.f, 1.f, 1.f }));

	//Terrain

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, idTexNormal);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, idTexSnow);

	program.sendUniform("materialSpecular", vec3(0.1, 0.1, 0.1)); 	//shininess
	terrain.render(setMatrix({ 0.f, 0.f, 0.f }, 0.f, { 0.0f, 1.0f, 0.0f }, { 1.f,  1.f ,  1.f }, { 1.2f, 1.2f, 1.2f }));

	//Road
	program.sendUniform("materialSpecular", vec3(1, 1, 1.5)); 	//shininess

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, idTexNormalICe);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, idTexRoad);
 
	 road.render(setMatrix({ 0.f, 0.2f, 0.f }, 0.f, { 0.0f, 1.0f, 0.0f }, { 1.f,  1.f ,  1.f }, { 1.f, 1.f, 1.f }));

	 
	//WATER
	programTerrain.use();
	mat4 m;
	m = matrixView;
	iceWater.render(m);
	
	// render the water
	programWater.use();
 
	m = matrixView;
	m = translate(m, vec3(0, waterLevel, 0));
	m = scale(m, vec3(1.f, 1.0f, 1.f));
	programWater.sendUniform("matrixModelView", m);
	water.render(m);
	 
	program.use();
 

	

	//STREET LAMPS
	streetLampFun(5.5f, 13.4f, -14.f, "lightPoint.position", "lightPoint.diffuse", "lightPoint.specular", AreLeftLampsOff);
	streetLampFun(16.f, 16.f, -13.f, "lightPoint2.position", "lightPoint2.diffuse", "lightPoint2.specular", AreLeftLampsOff);
	streetLampFun(49.f, 17.2f, -21.f, "lightPoint3.position", "lightPoint3.diffuse", "lightPoint3.specular", AreRightLampsOff);
	streetLampFun(63.f, 18.2f, -44.f, "lightPoint4.position", "lightPoint4.diffuse", "lightPoint4.specular", AreLeftLampsOff);
	streetLampFun(76.f, 19.2f, -30.f, "lightPoint5.position", "lightPoint5.diffuse", "lightPoint5.specular", AreRightLampsOff);
	streetLampFun(99.f, 19.6f, -20.f, "lightPoint6.position", "lightPoint6.diffuse", "lightPoint6.specular", AreLeftLampsOff);
	streetLampFun(87.f, 22.4f, 8.f, "lightPoint7.position", "lightPoint7.diffuse", "lightPoint7.specular", AreRightLampsOff);
	streetLampFun(90.f, 24.f, 37.f, "lightPoint8.position", "lightPoint8.diffuse", "lightPoint8.specular", AreLeftLampsOff);
	streetLampFun(57.f, 28.5f, 37.f, "lightPoint9.position", "lightPoint9.diffuse", "lightPoint9.specular", AreRightLampsOff);
	streetLampFun(29.f, 28.9f, 37.f, "lightPoint10.position", "lightPoint10.diffuse", "lightPoint10.specular", AreRightLampsOff);

	// delete shininess for rest of scene  
	program.sendUniform("materialSpecular", vec3(0, 0, 0)); // bluish colouring
}


// Creates a shadow map and stores in idFBO
// lightTransform - lookAt transform corresponding to the light position predominant direction
void createShadowMap(mat4 lightTransform, float time, float deltaTime)
{
	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);

	// Store the current viewport in a safe place
	GLint viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);
	int w = viewport[2], h = viewport[3];

	// setup the viewport to 2x2 the original and wide (120 degrees) FoV (Field of View)
	glViewport(0, 0, w * 2, h * 2);
	mat4 matrixProjection = perspective(radians(60.f), (float)w / (float)h, 0.5f, 50.0f);
	program.sendUniform("matrixProjection", matrixProjection);

	// prepare the camera
	mat4 matrixView = lightTransform;


	// send the View Matrix
	program.sendUniform("shadowCameraView", matrixView);

	// Bind the Framebuffer
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, idFBO);

	// OFF-SCREEN RENDERING FROM NOW!
	// Clear previous frame values - depth buffer only!
	glClear(GL_DEPTH_BUFFER_BIT);

	// Disable color rendering, we only want to write to the Z-Buffer (this is to speed-up)
	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);


	const mat4 bias = {
	{ 0.5, 0.0, 0.0, 0.0 },
	{ 0.0, 0.5, 0.0, 0.0 },
	{ 0.0, 0.0, 0.5, 0.0 },
	{ 0.5, 0.5, 0.5, 1.0 }
	};

	program.sendUniform("matrixShadow", bias * matrixProjection * matrixView);

	// Render all objects in the scene
	renderScene(matrixView, time, deltaTime);
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	glDisable(GL_CULL_FACE);
	onReshape(w, h);
}

//** POOOOOOSSSSTTT PROOOOOOCEEEESING
void PostProcesing()
{
	programEffect.use();

	programEffect.sendUniform("texture0", 0);

	// Pass 2: on-screen rendering POST PROC
	glBindFramebufferEXT(GL_FRAMEBUFFER, 0);
	programEffect.sendUniform("matrixProjection", ortho(0, 1, 0, 1, -1, 1));

	// clear screen and buffers & bind textures
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glBindTexture(GL_TEXTURE_2D, idTexScreen);

	// setup identity matrix as the model-view
	programEffect.sendUniform("matrixModelView", mat4(1));

	GLuint attribVertex = programEffect.getAttribLocation("aVertex");
	GLuint attribTextCoord = programEffect.getAttribLocation("aTexCoord");
	glEnableVertexAttribArray(attribVertex);
	glEnableVertexAttribArray(attribTextCoord);
	glBindBuffer(GL_ARRAY_BUFFER, bufQuad);
	glVertexAttribPointer(attribVertex, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), 0);
	glVertexAttribPointer(attribTextCoord, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glDrawArrays(GL_QUADS, 0, 4);
	glDisableVertexAttribArray(attribVertex);
	glDisableVertexAttribArray(attribTextCoord);
	program.use();

}

void planarReflection(mat4& matrixView, float time, float deltaTime)
{
	// Prepare the stencil test
	glEnable(GL_STENCIL_TEST);
	glStencilFunc(GL_ALWAYS, 1, 1);
	glStencilOp(GL_REPLACE, GL_REPLACE, GL_REPLACE);

	// Disable screen rendering
	glDisable(GL_DEPTH_TEST);
	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);

	//WATER
	programWater.use();
	mat4 m;
	m = matrixView;
	m = translate(m, vec3(0, waterLevel, 0));
	m = scale(m, vec3(1.f, 1.0f, 1.f));
	programWater.sendUniform("matrixModelView", m);
	water.render(m);
	matrixView *= matrixReflection;
	program.sendUniform("matrixView", matrixView);
	programWater.sendUniform("matrixView", matrixView);
	programTerrain.sendUniform("matrixView", matrixView);


	// Use stencil test
	glStencilFunc(GL_EQUAL, 1, 1);
	glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);

	// Enable screen rendering
	glEnable(GL_DEPTH_TEST);
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);

 
	// Enable clipping plane
	glEnable(GL_CLIP_PLANE0);
	//clipping
	programWater.sendUniform("planeClip", vec4(a, b, c, d));
	program.sendUniform("planeClip", vec4(a, b, c, d));
	programTerrain.sendUniform("planeClip", vec4(a, b, c, d));
	renderScene(matrixView, time, deltaTime); // render the scene objects
	 



	// disable stencil test and clip plane
	glDisable(GL_STENCIL_TEST);
	glDisable(GL_CLIP_PLANE0);
	matrixView *= matrixReflection;

	


}

//*********** RENDER GLUT ***********
void onRender()
{
	//these variables control time & animation
	static float prev = 0;
	float time = glutGet(GLUT_ELAPSED_TIME) * 0.001f;	// time since start in seconds
	float deltaTime = time - prev;						// time since last frame
	prev = time;		// framerate is 1/deltaTime
	programWater.sendUniform("t", time);

	//SHADOW MAP CREATION
	createShadowMap(lookAt
	(
		vec3(5.5f, 5.4f, -14.f), // coordinates of the source of the light
		vec3(0.0f, -3.0f, 0.0f), // coordinates of a point within or behind the scene
		vec3(0.0f, 1.f, 0.0f)), // a reasonable "Up" vector
		time, deltaTime
	);

	// Pass 1: off-screen rendering POST PROC
	//glBindFramebufferEXT(GL_FRAMEBUFFER, idFBOPostProc);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);




	
	 

	// setup the View Matrix (camera)
	_vel = clamp(_vel + _acc * deltaTime, -vec3(maxspeed), vec3(maxspeed));
	float pitch = getPitch(matrixView);
	matrixView = rotate(translate(rotate(mat4(1),
		pitch, vec3(1, 0, 0)),	// switch the pitch off
		_vel * deltaTime),		// animate camera motion (controlled by WASD keys)
		-pitch, vec3(1, 0, 0))	// switch the pitch on
		* matrixView;
 
	// move the camera up following the profile of terrain (Y coordinate of the terrain)
	float terrainY = -terrain.getInterpolatedHeight(inverse(matrixView)[3][0], inverse(matrixView)[3][2]);
	matrixView = translate(matrixView, vec3(0, terrainY, 0));

	// the camera must be moved down by terrainY to avoid unwanted effects
	matrixView = translate(matrixView, vec3(0, -terrainY, 0));
 




	planarReflection(matrixView, time, deltaTime);

	// setup View Matrix
	program.sendUniform("matrixView", matrixView);
	programTerrain.sendUniform("matrixView", matrixView);
	programWater.sendUniform("matrixView", matrixView);
	renderScene(matrixView, time, deltaTime); // render the scene objects
 
	//POST PROCESING
	//PostProcesing();

	//______________
	glutSwapBuffers(); 	// essential for double-buffering technique
	glutPostRedisplay();// proceed the animation

	//SHOW POS ON THE MAP!!!!!!!!!!!!!!!!!!!!!!
	//cout << inverse(matrixView)[3][0] << "       " << terrainY << "       " << inverse(matrixView)[3][2] << endl;
}



// ************* MAIN *************
int main(int argc, char** argv)
{
	// init GLUT and create Window
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(1280, 720);
	glutCreateWindow("3DGL Scene: First Terrain");

	// init glew
	GLenum err = glewInit();
	if (GLEW_OK != err) return 0;

	// register callbacks
	glutDisplayFunc(onRender);
	glutReshapeFunc(onReshape);
	glutKeyboardFunc(onKeyDown);
	glutSpecialFunc(onSpecDown);
	glutKeyboardUpFunc(onKeyUp);
	glutSpecialUpFunc(onSpecUp);
	glutMouseFunc(onMouse);
	glutMotionFunc(onMotion);
	glutMouseWheelFunc(onMouseWheel);

	if (!init()) return 0;
	glutMainLoop();	// enter GLUT event processing cycle
	return 1;
}