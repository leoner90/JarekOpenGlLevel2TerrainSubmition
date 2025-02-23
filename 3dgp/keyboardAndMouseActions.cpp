#include "keyboardAndMouseActions.h"
#include "globalVar.h" // Ensure global variables are accessible


//*********** RESHAPE***********
// called before window opened or resized - to setup the Projection Matrix
void onReshape(int w, int h)
{
	float ratio = w * 1.0f / h;      // we hope that h is not zero
	glViewport(0, 0, w, h);
	mat4 matrixProjection = perspective(radians(_fov), ratio, 0.02f, 1000.f);

	// Setup the Projection Matrix
	program.sendUniform("matrixProjection", matrixProjection);
	programWater.sendUniform("matrixProjection", matrixProjection);
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