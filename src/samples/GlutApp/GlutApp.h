#ifndef GLUT_APP_H
#define GLUT_APP_H

#include <Rendering/oploGL.h>
#include <gl/freeglut.h>

#include <Math/Angle.h>
#include <Camera/Camera.h>
#include <Camera/EulerCameraController.h>
#include <Rendering\Debug\RenderingDebugOutput.h>
#include <Rendering/Shaders/UniformHandler.h>

namespace oplo
{

class GlutApp
{
public:

	GlutApp();

	virtual ~GlutApp();

	virtual void initialize(bool initializeDebug = true);

	virtual void resize(int w, int h);

	virtual void update();

	virtual void mouseFunc(int button, int state, int x, int y);

	virtual void passiveMotion(int x, int y);

	virtual void motionFunc(int x, int y);

	virtual void keyEvent(unsigned char key, int x, int y);

	virtual void preDraw();

	virtual void drawFunc();

	virtual void postDraw();

	virtual void draw() = 0;

	virtual void shutdown();

protected:

	Anglef m_globalLightDirection;

	Anglef m_fov;

	bool m_wireframe;

	bool m_translating;

	bool m_reversing;

	bool m_rotating;

	int m_mx, m_my;

	float m_motionSpeed;

	oplo::UniformHandler m_uniforms;

	RenderingDebugOutput m_debugOutput;

	Camera m_camera;

	EulerCameraController m_cameraController;
};

}


#endif
