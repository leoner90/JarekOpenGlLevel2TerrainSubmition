#include "LightAndSkyBox.h"
#include "globalVar.h" // Ensure global variables are accessible



//***************** SETUP FRO MATRIX EG OBJECTS IN THE WORLD
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

vec3 DlightDirection;
//DIRECTIONAL LIGHT
void Directional()
{
	if (isItNight)
		return;

	float diffuseColor = 0;

	if (DayRotationAngle < 180)
		diffuseColor = dayFraction * 2;
	else if (DayRotationAngle < 360)
		diffuseColor = (2 - dayFraction * 2);

	// Calculate the sun's position in the sky
	float lightX = sin(radians(DayRotationAngle / 2)); // Light movement along X axis (180 degreee onle where 0 = 0 & 180 = 1)
	// For better RESULT so we are moving from -90 to 270 bacily from -1 to 0 to 1 and back to -1
	float lightY = sin(radians(DayRotationAngle - 90)); //movement Y axis (height) 

	//Directional Light
	program.sendUniform("lightDir.direction", vec3(lightX, lightY, 0));

	DlightDirection = vec3(lightX, lightY, 0);

	program.sendUniform("lightDir.diffuse", vec3(diffuseColor, diffuseColor, diffuseColor)); // set "lightDir.diffuse", vec3(0.0, 0.0, 0.0) to switch off


	//Specular reflection
	program.sendUniform("lightDir.specular", vec3(0.5f, 0.5f, 0.5f));

	
}

// Ambient light is considered to be the light that was reflected for so many times that it appears to be emanating from everywhere.
void AmbientLight()
{
	program.sendUniform("lightAmbient.color", vec3(0.3, 0.3, 0.33));
	programWater.sendUniform("lightAmbient.color", vec3(0.3, 0.3, 0.33));
}

//SKYBOX 
void SkyBoxAndDayCalculation(bool isReflected)
{
	//FRAME TIME!!!!!!!!!!!!!! delta 
	//directionlight off ambient like TO THE MAX , small sky box  moves with  a player , render skybox first
	program.sendUniform("lightDir.diffuse", vec3(0.0f, 0.0f, 0.0f));	//off directional before sky box render

	//point light (point ligh will be ON for some time during Early morning, and a bit before night) , TODO move to point light function~!
	if (dayFraction < 0.2 || isItNight || dayFraction > 0.93)
		isPointLightOff = false; // on
	else
		isPointLightOff = true; // off


	//sunset (10 point up to increase red , 10 points down to decrease)
	if (dayFraction >= 0.75f && dayFraction <= 0.85f && !isItNight)
		sunsetColorDivider = 1 + ((dayFraction - 0.75) * 10);

	if (dayFraction >= 0.86f && dayFraction <= 0.96f && !isItNight)
		sunsetColorDivider = 2 - ((dayFraction - 0.86) * 10);

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

	//fooog with same color as skybox (THE BEST PLACE FOR FOG DON'T YOU THINK!?!?!?!?)
	program.sendUniform("fogColour", vec3(ambientColor, ambientColor / sunsetColorDivider, ambientColor / sunsetColorDivider));
	program.sendUniform("fogDensity", 0.005f);

	//SKY BOX RENDER + ROTATION
	mat4 m = matrixView;
	m = rotate(m, radians(DayRotationAngle), { 0,1,0 });

	if (isReflected)
		m = rotate(m, radians(180.f), vec3(0.f, 0.f, 1.f));

	if (isItNight)
	{
		m = rotate(m, radians(DayRotationAngle / 1.4f), { 0,0,1 });
		NightSkybox.render(m);
	}
	else
		skybox.render(m);
}

// LAMPS 
void streetLampFun(float x, float y, float z, string shaderNamePos, string shaderDiffuse, string shaderSpecular, int lampControll, vec3 color  )
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
	m = translate(m, vec3(x, y, z));
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