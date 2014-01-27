#include "GlutApp.h"
#include <Rendering/oploGL.h>
#include <GL/glut.h>

namespace oplo
{

	GlutApp::GlutApp() :
		m_globalLightDirection(0),
		m_fov(Anglef::fromDeg(45.f)),
		m_wireframe(false),
		m_translating(false),
		m_rotating(false),
		m_motionSpeed(2),
		m_reversing(false)
	{}

	GlutApp::~GlutApp()
	{}

	void GlutApp::initialize(bool initializeDebug /*= true*/)
	{
		if (initializeDebug)
		{
			m_debugOutput.enable();
			m_debugOutput.disableMultithreading();
			glDebugMessageCallback(PrimaryDebugFunc, &m_debugOutput);
		}

		m_cameraController.setCamera(&m_camera);

		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);
		glDepthFunc(GL_LESS);

		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
	}

	void GlutApp::resize(int w, int h)
	{
		if (h == 0)
			h = 1;

		glViewport(0, 0, w, h);

		m_camera.SetViewport(0, 0, w, h);
		m_camera.SetFieldOfView(m_fov);
		m_camera.SetClippingRanges(1.f, 160000.f);
	}

	void GlutApp::update()
	{
		if (m_translating)
		{
			m_cameraController.moveForward(m_reversing ? m_motionSpeed * -1 : m_motionSpeed);
		}

		m_cameraController.updateCamera();

		//elevationPage.beginFrame(camera);
		//cameraController.UpdateCamera();
		//float cosLight = cos(fLightAngle * 3.1415928f / 180.f);
		//float sinLight = sin(fLightAngle * 3.1415928f / 180.f);

		//Math::Vec3F vLightDirDowncast = Math::Vec3F(cosLight, sinLight, 0.15f);
		//vLightDirDowncast.normalize();

		//float maxTan = camera.GetTangentFOV();
		//const float tangentAxisDivisor = static_cast<float>(std::max(camera.GetViewingWidth(), camera.GetViewingHeight()));

		//maxTan = std::max(maxTan * camera.GetAspect(), maxTan) * 2;

		//Math::AffineMatrix< float > mModelview = camera.GetViewMatrix();
		//Math::AffineMatrix< float > mProjection = camera.GetProjectionMatrix();
		//Math::AffineMatrix< float > mModelviewProjection = mProjection * mModelview;

		//Engine::staticGet<UniformHandler>().SetUniform("modelView", mModelview.Begin(), 16, true);
		//Engine::staticGet<UniformHandler>().SetUniform("modelViewProjection", mModelviewProjection.Begin(), 16, true);

		//mModelviewProjection.Invert();

		//Engine::staticGet<UniformHandler>().SetUniform("sunDir", vLightDirDowncast[0], vLightDirDowncast[1], vLightDirDowncast[2]);
		//Engine::staticGet<UniformHandler>().SetUniform("cameraPos", camera.GetWorldPosition()[0], camera.GetWorldPosition()[1], camera.GetWorldPosition()[2]);
		//Engine::staticGet<UniformHandler>().SetUniform("maximumFrustumTangentAxis", tangentAxisDivisor * (1.f / (lodAggressiveness * maxTan)));
		//Engine::staticGet<UniformHandler>().SetUniform("inverseScreenWidthHeight", 1.f / camera.GetViewingWidth(), 1.f / camera.GetViewingHeight());
		//Engine::staticGet<UniformHandler>().SetUniform("inverseModelviewProjection", mModelviewProjection.Begin(), 16, true);
		//Engine::staticGet<UniformHandler>().SetUniform("hg", hg_[0], hg_[1], hg_[2]);
		//Engine::staticGet<UniformHandler>().SetUniform("hgHalf", hgHalf_[0], hgHalf_[1], hgHalf_[2]);

	}

	void GlutApp::mouseFunc(int button, int state, int x, int y)
	{
		if (state == GLUT_UP)
		{
			if (button == GLUT_RIGHT_BUTTON)
				m_rotating = false;

			if (button == GLUT_LEFT_BUTTON)
				m_translating = false;
		}
		else if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN)
		{
			m_rotating = true;
		}
		else if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
		{
			m_translating = true;
		}

		m_mx = x;
		m_my = y;
	}

	void GlutApp::passiveMotion(int x, int y)
	{
		m_mx = x;
		m_my = y;
	}

	void GlutApp::motionFunc(int x, int y)
	{
		if (m_rotating)
		{
			float deltaH = m_mx - x;
			float deltaP = m_my - y;
			m_cameraController.rotatePitchDelta(deltaP * 0.5f);
			m_cameraController.rotateHeadingDelta(deltaH * 0.5f);
		}

		m_mx = x;
		m_my = y;
	}

	void GlutApp::keyEvent(unsigned char key, int, int)
	{
		switch (key)
		{
		case '1':
			m_globalLightDirection.setDegrees(m_globalLightDirection.getDegrees() + 1);
			break;
		case '!':
			m_globalLightDirection.setDegrees(m_globalLightDirection.getDegrees() - 1);
			break;

		case 'w':
			m_wireframe = !m_wireframe;
			break;

		case '6':
			m_motionSpeed += std::max(m_motionSpeed * 1.2f, 1.f);
			break;

		case '^':
			m_motionSpeed -= std::max(m_motionSpeed * -1.2f, 1.f);
			break;

			//case 't':
			//	bRenderText = !bRenderText;
			//	break;
			//case 'y':
			//	statRenderer.increaseSecondaryIndex();
			//	break;
			//case 'k':
			//	Engine::staticGet<UniformHandler>().Recompile();
			//	break;
		case 'q':
			m_reversing = !m_reversing;
			break;
		case 27:
			exit(-1);
		default:
			break;
		}
	}

	void GlutApp::preDraw()
	{
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);

		glClearColor(0.0, 0.0, 0.0, 1.0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		if (m_wireframe)
		{
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		}
		else
		{
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		}
	}

	void GlutApp::drawFunc()
	{
		update();
		preDraw();
		draw();
		postDraw();
	}

	void GlutApp::postDraw()
	{
		if (m_wireframe)
		{
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		}
		else
		{
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		}

		glutSwapBuffers();
		glutPostRedisplay();
	}

}
