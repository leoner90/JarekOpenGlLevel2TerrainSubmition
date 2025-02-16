#include "InitFun.h"
#include "globalVar.h" // Ensure global variables are accessible

//FORWARD DECLARATION
void shadowMapInit();
void PostProcessInit();
bool TextureSetup(const char textureName[], GLuint& textureId, GLuint textureNr);
void NullTexture();
//bool waterInit();
bool shadersInit();


bool waterInit()
{
	// load your 3D models here!
	programTerrain.use();
	if (!water.load("models\\terrain\\waterMap2.jpg", 25, &programWater)) return false;
	if (!iceWater.load("models\\terrain\\iceWater.jpg", 25)) return false;
 
	// setup materials  and light
	programTerrain.sendUniform("materialAmbient", vec3(1.0, 1.0, 1.0));
	programWater.sendUniform("materialAmbient", vec3(1.0, 1.0, 1.0));
	programTerrain.sendUniform("materialDiffuse", vec3(1.0, 1.0, 1.0));


	// setup the water colours and level
	programWater.sendUniform("waterColor", vec3(0.2f, 0.2f, 0.34f));
	programWater.sendUniform("skyColor", vec3(0.2f, 0.6f, 1.f));
	programTerrain.sendUniform("waterColor", vec3(0.2f, 0.2f, 0.34f));
	programTerrain.sendUniform("waterLevel", waterLevel);


	// setup the textures
	if (!TextureSetup("models/grass.png", idTexSand, 0))
		return false;
	if (!TextureSetup("models/pebbles.png", idTexGrass, 0))
		return false;


	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, idTexSand);
	programTerrain.sendUniform("textureBed", 0);

	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, idTexGrass);
	programTerrain.sendUniform("textureShore", 0);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);




}


bool init()
{
	if(!shadersInit()) return false;

	// glut additional setup
	glutSetVertexAttribCoord3(program.getAttribLocation("aVertex"));
	glutSetVertexAttribNormal(program.getAttribLocation("aNormal"));
	glutSetVertexAttribCoord3(programTerrain.getAttribLocation("aVertex"));
	glutSetVertexAttribNormal(programTerrain.getAttribLocation("aNormal"));

	// rendering states
	glEnable(GL_DEPTH_TEST);	// depth test is necessary for most 3D scenes
	glEnable(GL_NORMALIZE);		// normalization is needed by AssImp library models
	glShadeModel(GL_SMOOTH);	// smooth shading mode is the default one; try GL_FLAT here!
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);	// this is the default one; try GL_LINE!

	// load your 3D models here!
	if (!terrain.load("models\\terrain\\worldHM.jpg", 25)) return false;
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
	if (!TextureSetup("models/PaintTextures/water.png", idTexWater, 0))
		return false;

	//NORMAL MAP For Snow
	if (!TextureSetup("models/PaintTextures/snowNormal.jpg", idTexNormal, 2))
		return false;
	if (!TextureSetup("models/PaintTextures/iceNormals.jpg", idTexNormalICe, 2))
		return false;

	program.sendUniform("texture0", 0);
	program.sendUniform("textureNormal", 2);

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


	//shadowMapInit();
//	PostProcessInit();
	waterInit();


 
	// setup the screen background colour
	//glEnable(GL_CULL_FACE);
	glClearColor(0.0f, 0.0f, 0.f, 1.0f);   // blue sky background
	return true;
}





//***************** TEXTURES SETUP  INIT *****************

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



// **************** POST PROCES INIT ******************

void PostProcessInit()
{
	//POST PROCESSING
// Create screen space texture
	glGenTextures(1, &idTexScreen);
	glBindTexture(GL_TEXTURE_2D, idTexScreen);

	// Texture parameters - to get nice filtering
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);


	// This will allocate an uninitilised texture
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, WImage, HImage, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);

	// Create a framebuffer object (FBO)
	glGenFramebuffers(1, &idFBOPostProc);
	glBindFramebuffer(GL_FRAMEBUFFER, idFBOPostProc);


	// Attach a depth buffer
	GLuint depth_rb;
	glGenRenderbuffers(1, &depth_rb);
	glBindRenderbuffer(GL_RENDERBUFFER, depth_rb);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, WImage, HImage);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depth_rb);

	// attach the texture to FBO colour attachment point
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, idTexScreen, 0);

	// switch back to window-system-provided framebuffer
	glBindFramebufferEXT(GL_FRAMEBUFFER, 0);

	// Create Quad

	float vertices[] = {
	0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
	1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
	1.0f, 1.0f, 0.0f, 1.0f, 1.0f,
	0.0f, 1.0f, 0.0f, 0.0f, 1.0f
	};

	// Generate the buffer name
	glGenBuffers(1, &bufQuad);

	// Bind the vertex buffer and send data
	glBindBuffer(GL_ARRAY_BUFFER, bufQuad);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

}





// **************** SHADOW MAP INIT ******************
void shadowMapInit()
{
	/**************** SHADOW MAP (INITIALISATION: SHADOW MAP) ****************/
	GLuint idTexShadowMap;

	// Create shadow map texture
	glEnable(GL_CULL_FACE);

	glActiveTexture(GL_TEXTURE7);
	glGenTextures(1, &idTexShadowMap);
	glBindTexture(GL_TEXTURE_2D, idTexShadowMap);

	// Texture parameters - to get nice filtering & avoid artefact on the edges of the shadowmap
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LESS);

	// This will associate the texture with the depth component in the Z-buffer
	GLint viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);
	int w = viewport[2], h = viewport[3];
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, w * 2, h * 2, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);

	// Send the texture info to the shaders
	program.sendUniform("shadowMap", 7);

	// revert to texture unit 0
	glActiveTexture(GL_TEXTURE0);


	//(INITIALISATION: FRAME BUFFER OBJECT)
	// Create a framebuffer object (FBO)

	glGenFramebuffers(1, &idFBO);
	glBindFramebuffer(GL_FRAMEBUFFER_EXT, idFBO);

	// Instruct openGL that we won't bind a color texture with the currently binded FBO
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);

	// attach the texture to FBO depth attachment point
	glFramebufferTexture2D(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_TEXTURE_2D, idTexShadowMap, 0);

	// switch back to window-system-provided framebuffer
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);

}


// **************** NULL TEXTURE INIT ******************
void NullTexture()
{
	glGenTextures(1, &idTexNone);
	glBindTexture(GL_TEXTURE_2D, idTexNone);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	BYTE bytes[] = { 255, 255, 255 };
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 1, 1, 0, GL_BGR, GL_UNSIGNED_BYTE, &bytes);
}


// **************** SHADERS INIT ******************
bool shadersInit()
{
	//SHADERS CREATION
	C3dglShader vertexShader;
	C3dglShader fragmentShader;
	C3dglShader effectVertexShader;
	C3dglShader effectFragmentShader;

	// water
	C3dglShader waterVertexShader;
	C3dglShader waterFragmentShader;
	C3dglShader terrainVertexShader;
	C3dglShader terrainFragmentShader;

	if (!vertexShader.create(GL_VERTEX_SHADER)) return false;
	if (!vertexShader.loadFromFile("shaders/basic.vert.shader")) return false;
	if (!vertexShader.compile()) return false;
	if (!fragmentShader.create(GL_FRAGMENT_SHADER)) return false;
	if (!fragmentShader.loadFromFile("shaders/basic.frag.shader")) return false;
	if (!fragmentShader.compile()) return false;

	if (!effectVertexShader.create(GL_VERTEX_SHADER)) return false;
	if (!effectVertexShader.loadFromFile("shaders/effect.vert.shader")) return false;
	if (!effectVertexShader.compile()) return false;
	if (!effectFragmentShader.create(GL_FRAGMENT_SHADER)) return false;
	if (!effectFragmentShader.loadFromFile("shaders/effect.frag.shader")) return false;
	if (!effectFragmentShader.compile()) return false;

	if (!program.create()) return false;
	if (!program.attach(vertexShader)) return false;
	if (!program.attach(fragmentShader)) return false;
	if (!program.link()) return false;
	if (!program.use(true)) return false;


	if (!programEffect.create()) return false;
	if (!programEffect.attach(effectVertexShader)) return false;
	if (!programEffect.attach(effectFragmentShader)) return false;
	if (!programEffect.link()) return false;
	//if (!programEffect.use(true)) return false;

	//water

	if (!waterVertexShader.create(GL_VERTEX_SHADER)) return false;
	if (!waterVertexShader.loadFromFile("shaders/water.vert.shader")) return false;
	if (!waterVertexShader.compile()) return false;

	if (!waterFragmentShader.create(GL_FRAGMENT_SHADER)) return false;
	if (!waterFragmentShader.loadFromFile("shaders/water.frag.shader")) return false;
	if (!waterFragmentShader.compile()) return false;

	if (!programWater.create()) return false;
	if (!programWater.attach(waterVertexShader)) return false;
	if (!programWater.attach(waterFragmentShader)) return false;
	if (!programWater.link()) return false;
	//if (!programWater.use(true)) return false;


	if (!terrainVertexShader.create(GL_VERTEX_SHADER)) return false;
	if (!terrainVertexShader.loadFromFile("shaders/terrain.vert.shader")) return false;
	if (!terrainVertexShader.compile()) return false;

	if (!terrainFragmentShader.create(GL_FRAGMENT_SHADER)) return false;
	if (!terrainFragmentShader.loadFromFile("shaders/terrain.frag.shader")) return false;
	if (!terrainFragmentShader.compile()) return false;

	if (!programTerrain.create()) return false;
	if (!programTerrain.attach(terrainVertexShader)) return false;
	if (!programTerrain.attach(terrainFragmentShader)) return false;
	if (!programTerrain.link()) return false;
	//if (!programTerrain.use(true)) return false;

	return true;
}