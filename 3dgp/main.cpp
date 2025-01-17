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

// 3D Terrains
C3dglTerrain terrain, road;

// 3D Models
C3dglModel cristmasTree;
C3dglModel streetLamp;
C3dglModel house;

//skyBoxes
C3dglSkyBox skybox;
C3dglSkyBox NightSkybox;

//textures
GLuint idTexSnow;
GLuint idTexRoad;
GLuint idTexNone; // null TExture

//normal
GLuint idTexNormal;
GLuint idTexNormalICe;

// The View Matrix
mat4 matrixView;

// Camera & navigation
float maxspeed = 28.f;	// camera max speed
float accel = 8.f;		// camera acceleration
vec3 _acc(0), _vel(0);	// camera acceleration and velocity vectors
float _fov = 60.f;		// field of view (zoom)

// GLSL Program
C3dglProgram program;


//Day Night Regime
bool isItNight = false;
bool IsTimerReseted = false;
bool isPointLightOff = false;
float timeAccelerator = 1;// time contrroll

//day night global vars
float dayFraction = 0;
float DayRotationAngle = 0;
 
//sunset effect
float sunsetColorDivider = 1;

//lampControll 0 = auto, 1 = manual ON, 2 = Manual off
int AreLeftLampsOff = 0;
int AreRightLampsOff = 0;

//***************** TEXTURES *****************
C3dglBitmap bm;
bool TextureSetup(const char textureName[], GLuint& textureId, GLuint textureNr)
{
	bm.load(textureName, GL_RGBA);
	if (!bm.getBits()) return false;

	glActiveTexture(GL_TEXTURE + textureNr);
	glGenTextures(1, &textureId);
	glBindTexture(GL_TEXTURE_2D, textureId);


	//GL_LINEAR_MIPMAP_LINEAR fo minimilisation (GL_EXT_texture_filter_anisotropic for testing looks the same)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); // GL_CLAMP for skybox  GL_LINEAR_MIPMAP_LINEAR - for resizing
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, bm.getWidth(), bm.getHeight(), 0, GL_RGBA,
	GL_UNSIGNED_BYTE, bm.getBits());
	glGenerateMipmap(GL_TEXTURE_2D);

	return true;
}

//NULL TEXture
void NullTexture()
{
	glGenTextures(1, &idTexNone);
	glBindTexture(GL_TEXTURE_2D, idTexNone);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	BYTE bytes[] = { 255, 255, 255 };
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 1, 1, 0, GL_BGR, GL_UNSIGNED_BYTE, &bytes);
}

bool init()
{
	//SHADERS CREATION
	C3dglShader vertexShader;
	C3dglShader fragmentShader;

	if (!vertexShader.create(GL_VERTEX_SHADER)) return false;
	if (!vertexShader.loadFromFile("shaders/basic.vert.shader")) return false;
	if (!vertexShader.compile()) return false;
	if (!fragmentShader.create(GL_FRAGMENT_SHADER)) return false;
	if (!fragmentShader.loadFromFile("shaders/basic.frag.shader")) return false;
	if (!fragmentShader.compile()) return false;

	if (!program.create()) return false;
	if (!program.attach(vertexShader)) return false;
	if (!program.attach(fragmentShader)) return false;
	if (!program.link()) return false;
	if (!program.use(true)) return false;

	// rendering states
	glEnable(GL_DEPTH_TEST);	// depth test is necessary for most 3D scenes
	glEnable(GL_NORMALIZE);		// normalization is needed by AssImp library models
	glShadeModel(GL_SMOOTH);	// smooth shading mode is the default one; try GL_FLAT here!
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);	// this is the default one; try GL_LINE!

	// load your 3D models here!
	if (!terrain.load("models\\terrain\\worldHM.png", 25)) return false;
	if (!road.load("models\\terrain\\roadHM.png", 25)) return false;

	//EASY TEXTURE LOAD
	if (!cristmasTree.load("models\\christmas_tree\\christmas_tree.obj")) return false;
	cristmasTree.loadMaterials("models\\christmas_tree\\");
 
	//pretty sure, it's something wrong in code that I have to use this
	cristmasTree.getMaterial(0)->setAmbient({ 0.75, 0.75, 0.75 });
	cristmasTree.getMaterial(1)->setAmbient({ 0.75, 0.75, 0.75 });
	cristmasTree.getMaterial(2)->setAmbient({ 0.75, 0.75, 0.75 });
	cristmasTree.getMaterial(3)->setAmbient({ 0.75, 0.75, 0.75 });
	cristmasTree.getMaterial(4)->setAmbient({ 0.75, 0.75, 0.75 });
	cristmasTree.getMaterial(5)->setAmbient({ 0.75, 0.75, 0.75 });
 

	//	cout << cristmasTree.getMaterialCount() << endl;
	//EASY TEXTURE LOAD

	if (!house.load("models\\house\\source\\house.fbx")) return false;
	house.loadMaterials("models\\house\\textures\\");
	house.getMaterial(0)->setAmbient({ 0.7, 0.7, 0.7 });
	house.getMaterial(1)->setAmbient({ 0.7, 0.7, 0.7 });
	house.getMaterial(2)->setAmbient({ 0.7, 0.7, 0.7 });


	if (!streetLamp.load("models\\streetLamp\\streetLamp.obj")) return false;
 

	//textures Setup
	if (!TextureSetup("models/PaintTextures/snow.jpg", idTexSnow, 0))
		return false;
	if (!TextureSetup("models/PaintTextures/road.jpg", idTexRoad, 0))
		return false;

	//NORMAL MAP For Snow
	 if (!TextureSetup("models/PaintTextures/snowNormal.jpg", idTexNormal, 2))
	 	return false;
	 if (!TextureSetup("models/PaintTextures/iceNormals.jpg", idTexNormalICe, 2))
		 return false;

	 program.sendUniform("texture0", 0);
	 program.sendUniform("textureNormal", 2); // why 2 not 1??? It's an ancient religion

	if (!skybox.load
	(
		"models\\skyBox\\sky\\TropicalSunnyDayBack1024.jpg",
		"models\\skyBox\\sky\\TropicalSunnyDayRight1024.jpg",
		"models\\skyBox\\sky\\TropicalSunnyDayFront1024.jpg",
		"models\\skyBox\\sky\\TropicalSunnyDayLeft1024.jpg",
		"models\\skyBox\\sky\\TropicalSunnyDayUp1024.jpg",
		"models\\skyBox\\sky\\TropicalSunnyDayDown1024.jpg"
	))
		return false;

	if (!NightSkybox.load
	(
		"models\\skyBox\\back_even.png",
		"models\\skyBox\\left_even.png",
		"models\\skyBox\\front_even.png",
		"models\\skyBox\\Right_even.png",
		"models\\skyBox\\Top_even.png",
		"models\\skyBox\\bottom_even.png"	
	))
		return false;
		
 
	// Initialise the View Matrix (initial position of the camera)
	matrixView = rotate(mat4(1), radians(12.f), vec3(1, 0, 0));
	matrixView *= lookAt(
		vec3(4.0, 1.5, 5.0),
		vec3(4.0, 1.5, 0.0),
		vec3(0.0, 1.0, 0.0));

	// setup the screen background colour
	glClearColor(0.0f, 0.0f, 0.f, 1.0f);   // blue sky background

	return true;
}

//SETUP FRO MATRIX EG OBJECTS IN THE WORLD
mat4 setMatrix(vec3 Translate, float RotAngle, vec3 Rotate, vec3 Scale, vec3 objColour)
{
	mat4 m = matrixView;
	m = translate(m, Translate);
	m = rotate(m, radians(RotAngle), Rotate);
	m = scale(m, Scale);

	//ambient Mat
	program.sendUniform("materialAmbient", vec3(objColour));

	//Directional Light Mat
	program.sendUniform("materialDiffuse", vec3(objColour));

	return m;
}

//DIRECTIONAL LIGHT
void Directional()
{
	if (isItNight)
		return;
 
	float ambientColor = 0;

	if (DayRotationAngle  < 180)
		ambientColor = dayFraction * 2;
	else if (DayRotationAngle < 360 )
		ambientColor = (2 - dayFraction * 2);
 
	// Calculate the sun's position in the sky
	float lightX = sin(radians(DayRotationAngle / 2)); // Light movement along X axis (180 degreee onle where 0 = 0 & 180 = 1)
	// For better RESULT so we are moving from -90 to 270 bacily from -1 to 0 to 1 and back to -1
	float lightY = sin(radians(DayRotationAngle - 90)); //movement Y axis (height) 

	//Directional Light
	program.sendUniform("lightDir.direction", vec3(lightX, lightY, 0)) ;
	program.sendUniform("lightDir.diffuse", vec3(ambientColor, ambientColor, ambientColor)); // set "lightDir.diffuse", vec3(0.0, 0.0, 0.0) to switch off

	//Specular reflection
	program.sendUniform("lightDir.specular", vec3(0.5f, 0.5f, 0.5f));
}

// Ambient light is considered to be the light that was reflected for so many times that it appears to be emanating from everywhere.
void AmbientLight()	
{
	program.sendUniform("lightAmbient.color", vec3(0.3, 0.3, 0.33));
}

//DAY NIGHT CALC. - calculates and updates global variables which repsents day time
void CalcCurrentDayTime()
{	
	float oneDayTimeInSec = 160; // day circle in seconds! 3min default
	//Use FRAME / delta TIME INSTEAD !!!!!!!!!!!!!!?? 
	float elapsedTime = glutGet(GLUT_ELAPSED_TIME) / 1000.0f * timeAccelerator; // Time in seconds since the program started
	dayFraction = fmod(elapsedTime, oneDayTimeInSec) / oneDayTimeInSec; // Fraction of the day
	DayRotationAngle = dayFraction * 360;
	//	float timeOfDay = dayFraction * 24h;
}

//SKYBOX 
void SkyBoxAndDayCalculation()
{
	//FRAME TIME!!!!!!!!!!!!!! delta 
	//directionlight off ambient like TO THE MAX , small sky box  moves with  a player , render skybox first
	program.sendUniform("lightDir.diffuse", vec3(0.0f, 0.0f, 0.0f));	//off directional before sky box render

	//point light (point ligh will be ON for some time during Early morning, and a bit before night) , TODO move to point light function~!
	if(dayFraction < 0.2 || isItNight || dayFraction > 0.93)
		isPointLightOff = false; // on
	else
		isPointLightOff = true; // off


	//sunset (10 point up to increase red , 10 points down to decrease)
	if (dayFraction >= 0.75f && dayFraction <= 0.85f && !isItNight)
		sunsetColorDivider = 1 + ((dayFraction - 0.75) * 10);

	if (dayFraction >= 0.86f && dayFraction <= 0.96f && !isItNight)
		sunsetColorDivider = 2 - ((dayFraction - 0.86) * 10 );


	// DAY TIME
	float ambientColor = 0;

	if (dayFraction < 0.5f)
	{
		IsTimerReseted = true;
		ambientColor = dayFraction * 2;
	}
	
	else if (dayFraction < 0.98f)
		ambientColor = 2 - dayFraction * 2;
	else if (dayFraction < 1.0f && IsTimerReseted)
	{
		IsTimerReseted = false;
		isItNight = !isItNight;
	}

	//SkyBox Color Setup
	program.sendUniform("lightAmbient.color", vec3(ambientColor, ambientColor / sunsetColorDivider, ambientColor / sunsetColorDivider));
	program.sendUniform("materialAmbient", vec3(1.0f, 1.0f, 1.0f));
	program.sendUniform("materialDiffuse", vec3(0.0f, 0.0f, 0.0f));

	//fooog with same color as skybox
	program.sendUniform("fogColour", vec3(ambientColor, ambientColor / sunsetColorDivider, ambientColor / sunsetColorDivider));
	program.sendUniform("fogDensity", 0.005f);

	//SKY BOX RENDER + ROTATION
	mat4 m = matrixView;
	m = rotate(m, radians(DayRotationAngle), { 0,1,0 });
	

	if (isItNight)
	{
		m = rotate(m, radians(DayRotationAngle / 1.4f), { 0,0,1 });
		NightSkybox.render(m);
	}
	else
		skybox.render(m);
}

// LAMPS 
void streetLampFun(float x , float y, float z , string shaderNamePos, string shaderDiffuse, string shaderSpecular, int lampControll, vec3 color = { 0.1 , 0.2, 0.7 })
{ 
	bool isLightON = true;

	switch (lampControll)
	{
	case 0:	//lampControll = 0; Automatic
		isLightON = !isPointLightOff;
		break;
	case 1:	//lampControll = 1; Allways ON MANUAL CONTROLL
		isLightON = true;
		break;
	case 2:	//lampControll = 2; Allways OFF MANUAL CONTROLL
		isLightON = false;
		break;
	default:
		break;
	}

	if ((isLightON)) // !isPointLightOff || ON 
	{
		program.sendUniform(shaderNamePos, vec3(x, y, z));
		program.sendUniform(shaderDiffuse, color);

		program.sendUniform("lightAmbient.color", vec3(0, 3, 11));

		//  Shine // not natural as u said but looks cool
		program.sendUniform(shaderSpecular, vec3(0.4, 0.4, 0.4)); // only for point light in our case
		program.sendUniform("materialSpecular", vec3(0.5, 0.5, 0.5)); // bluish colouring
		program.sendUniform("shininess", 20.0);
	}
	
	else // OFF
	{
		program.sendUniform(shaderDiffuse, vec3(0, 0, 0));
		program.sendUniform("lightAmbient.color", vec3(0.6, 0.6, 0.6)); // to give glutSolidSphere some color when off
		program.sendUniform(shaderSpecular, vec3(0.0f, 0.0f, 0.0f)); // delete Shine if off
	}
		
	//glutSolidSphere position and Ambient Light
	mat4 m;
	m = matrixView;
	m = translate(m, vec3( x, y, z ));
	m = scale(m, vec3(0.5f, 0.5f, 0.5f));
	program.sendUniform("matrixModelView", m);
		
	//Mat
	program.sendUniform("materialAmbient", vec3(1.f, 1.f, 1.1f));
	program.sendUniform("materialDiffuse", vec3((1.f, 1.f, 1.f)));
 
	//render bulb
	glutSolidSphere(1, 32, 32);

	//reset Ambient Light And redner StreetLamp
	program.sendUniform("lightAmbient.color", vec3(1.1, 1.1, 1.1));

	//shine for street lamp
	program.sendUniform("materialSpecular", vec3(0.15, 0.15, 0.15));  

	streetLamp.render(setMatrix({ x, y - 4, z }, 0.f, { 0.0f, 1.0f, 0.0f }, { 0.03f,  0.03f ,  0.03f }, { 0.1f,  0.1f, 0.15f }));
}
 

//*********** RENDER SCENE ***********
void renderScene(mat4& matrixView, float time, float deltaTime)
{
	CalcCurrentDayTime();
	SkyBoxAndDayCalculation();
	Directional();
	AmbientLight();
 
	//cristmasTree
	cristmasTree.render(setMatrix({ 0.f, 8.5f, 0.f }, 0.f, { 1.0f, 1.0f, 0.0f }, { 7.f,  7.f ,  7.f }, { 0.f, 1.f, 0.f }));
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
 
	road.render(setMatrix({ 0.f, 0.15f, 0.f }, 0.f, { 0.0f, 1.0f, 0.0f }, { 1.f,  1.f ,  1.f }, { 1.f, 1.f, 1.f }));
 
 
	//reset of textures, pretty sure  there is no point to do this (aks you latter)
	NullTexture();

	//STREET LAMPS
	streetLampFun(5.5f, 13.4f, -9.f, "lightPoint.position", "lightPoint.diffuse", "lightPoint.specular", AreLeftLampsOff);
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

//*********** RENDER GLUT ***********
void onRender()
{
	// these variables control time & animation
	static float prev = 0;
	float time = glutGet(GLUT_ELAPSED_TIME) * 0.001f;	// time since start in seconds
	float deltaTime = time - prev;						// time since last frame
	prev = time;										// framerate is 1/deltaTime

	// clear screen and buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

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

 
	//SHOW POS ON THE MAP!!!!!!!!!!!!!!!!!!!!!!
	//cout << inverse(matrixView)[3][0] << "       " << terrainY << "       " << inverse(matrixView)[3][2] << endl;

 
	// setup View Matrix
	program.sendUniform("matrixView", matrixView);

	renderScene(matrixView, time, deltaTime); // render the scene objects

	// the camera must be moved down by terrainY to avoid unwanted effects
	matrixView = translate(matrixView, vec3(0, -terrainY, 0));

	glutSwapBuffers(); 	// essential for double-buffering technique
	glutPostRedisplay();// proceed the animation
}

//*********** RESHAPE***********
// called before window opened or resized - to setup the Projection Matrix
void onReshape(int w, int h)
{
	float ratio = w * 1.0f / h;      // we hope that h is not zero
	glViewport(0, 0, w, h);
	mat4 matrixProjection = perspective(radians(_fov), ratio, 0.02f, 1000.f);

	// Setup the Projection Matrix
	program.sendUniform("matrixProjection", matrixProjection);
}

// Handle WASDQE keys
void onKeyDown(unsigned char key, int x, int y)
{
	switch (tolower(key))
	{
	case 'w': _acc.z = accel; break;
	case 's': _acc.z = -accel; break;
	case 'a': _acc.x = accel; break;
	case 'd': _acc.x = -accel; break;
	case 'e': _acc.y = accel; break;
	case 'q': _acc.y = -accel; break;
	//sunsetColorDivider = 1; just to avoid bug if sunset color is not 1 and we sudenly increasing time, day or night can be become red
	case 'n': timeAccelerator += 5.f; sunsetColorDivider = 1; break;
	case 'm': timeAccelerator = 1; break;
	case '1': AreLeftLampsOff >= 2 ? AreLeftLampsOff = 0 : AreLeftLampsOff++; break; // just resets int when is over 2 or increase if it's not over
	case '2': AreRightLampsOff >= 2 ? AreRightLampsOff = 0 : AreRightLampsOff++; break; // just resets int when is over 2 or increase if it's not over
	}
}

// Handle WASDQE keys (key up)
void onKeyUp(unsigned char key, int x, int y)
{
	switch (tolower(key))
	{
	case 'w':
	case 's': _acc.z = _vel.z = 0; break;
	case 'a':
	case 'd': _acc.x = _vel.x = 0; break;
	case 'q':
	case 'e': _acc.y = _vel.y = 0; break;
	}
}

// Handle arrow keys and Alt+F4
void onSpecDown(int key, int x, int y)
{
	maxspeed = glutGetModifiers() & GLUT_ACTIVE_SHIFT ? 20.f : 4.f;
	switch (key)
	{
	case GLUT_KEY_F4:		if ((glutGetModifiers() & GLUT_ACTIVE_ALT) != 0) exit(0); break;
	case GLUT_KEY_UP:		onKeyDown('w', x, y); break;
	case GLUT_KEY_DOWN:		onKeyDown('s', x, y); break;
	case GLUT_KEY_LEFT:		onKeyDown('a', x, y); break;
	case GLUT_KEY_RIGHT:	onKeyDown('d', x, y); break;
	case GLUT_KEY_PAGE_UP:	onKeyDown('q', x, y); break;
	case GLUT_KEY_PAGE_DOWN:onKeyDown('e', x, y); break;
	case GLUT_KEY_F11:		glutFullScreenToggle();
	}
}

// Handle arrow keys (key up)
void onSpecUp(int key, int x, int y)
{
	maxspeed = glutGetModifiers() & GLUT_ACTIVE_SHIFT ? 20.f : 4.f;
	switch (key)
	{
	case GLUT_KEY_UP:		onKeyUp('w', x, y); break;
	case GLUT_KEY_DOWN:		onKeyUp('s', x, y); break;
	case GLUT_KEY_LEFT:		onKeyUp('a', x, y); break;
	case GLUT_KEY_RIGHT:	onKeyUp('d', x, y); break;
	case GLUT_KEY_PAGE_UP:	onKeyUp('q', x, y); break;
	case GLUT_KEY_PAGE_DOWN:onKeyUp('e', x, y); break;
	}
}

// Handle mouse click
void onMouse(int button, int state, int x, int y)
{
	glutSetCursor(state == GLUT_DOWN ? GLUT_CURSOR_CROSSHAIR : GLUT_CURSOR_INHERIT);
	glutWarpPointer(glutGet(GLUT_WINDOW_WIDTH) / 2, glutGet(GLUT_WINDOW_HEIGHT) / 2);
	if (button == 1)
	{
		_fov = 60.0f;
		onReshape(glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
	}
}

// handle mouse move
void onMotion(int x, int y)
{
	glutWarpPointer(glutGet(GLUT_WINDOW_WIDTH) / 2, glutGet(GLUT_WINDOW_HEIGHT) / 2);

	// find delta (change to) pan & pitch
	float deltaYaw = 0.005f * (x - glutGet(GLUT_WINDOW_WIDTH) / 2);
	float deltaPitch = 0.005f * (y - glutGet(GLUT_WINDOW_HEIGHT) / 2);

	if (abs(deltaYaw) > 0.3f || abs(deltaPitch) > 0.3f)
		return;	// avoid warping side-effects

	// View = Pitch * DeltaPitch * DeltaYaw * Pitch^-1 * View;
	constexpr float maxPitch = radians(80.f);
	float pitch = getPitch(matrixView);
	float newPitch = glm::clamp(pitch + deltaPitch, -maxPitch, maxPitch);
	matrixView = rotate(rotate(rotate(mat4(1.f),
		newPitch, vec3(1.f, 0.f, 0.f)),
		deltaYaw, vec3(0.f, 1.f, 0.f)),
		-pitch, vec3(1.f, 0.f, 0.f))
		* matrixView;
}

void onMouseWheel(int button, int dir, int x, int y)
{
	_fov = glm::clamp(_fov - dir * 5.f, 5.0f, 175.f);
	onReshape(glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
}

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
	if (GLEW_OK != err)
		return 0;

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

	if (!init())
		return 0;

	glutMainLoop();	// enter GLUT event processing cycle

	return 1;
}