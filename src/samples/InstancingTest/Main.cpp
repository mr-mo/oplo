#include "../GlutApp/GlutApp.h"
#include "Rendering/BasicStructures/BufferObject.h"
#include "Rendering/Shapes/ShapeGenerator.h"
#include "Rendering/Shaders/Program.h"
#include "Rendering/Shaders/Shader.h"
#include "Math/Matrix.h"
#include <random>

class InstancingApp : public oplo::GlutApp
{
public:

	struct CubeData
	{
		float m_scale;
		oplo::Vec3F m_translation;
		oplo::Quat<float> m_rotation;
		oplo::Vec4F m_color;
	};

	static_assert(sizeof(CubeData) == (sizeof(float)* 12), "Must have proper alignment");

	class InstancingProgram : public oplo::Program
	{
	public:

		InstancingProgram() :
			Program("InstancingTest"),
			m_vertexLoc(-1),
			m_scaleAndTranslationLoc(-1),
			m_rotationLoc(-1),
			m_colorLoc(-1)
		{}

		void create()
		{
			std::shared_ptr<oplo::Shader> vtxShader, frgShader;
			vtxShader = std::make_shared<oplo::Shader>(oplo::Shader::VTX_SHADER, "InstancingVertex");
			frgShader = std::make_shared<oplo::Shader>(oplo::Shader::FRAG_SHADER, "InstancingFragment");
			vtxShader->addInline("#define VERTEX_SHADER");
			vtxShader->addFile("../shaders/InstancingTest.glsl");

			frgShader->addInline("#define FRAGMENT_SHADER");
			frgShader->addFile("../shaders/InstancingTest.glsl");

			vtxShader->create();
			frgShader->create();

			addShader(vtxShader);
			addShader(frgShader);

			compile();
		}

		void compileEvent()
		{
			m_vertexLoc = getInputLoc("vertexIn");
			m_colorLoc = getInputLoc("colorIn");
			m_rotationLoc = getInputLoc("rotationIn");
			m_normalLoc = getInputLoc("normalIn");
			m_scaleAndTranslationLoc = getInputLoc("scaleAndTranslation");
		}

		//Vertex attributes
		int m_vertexLoc;
		int m_scaleAndTranslationLoc;
		int m_rotationLoc;
		int m_colorLoc;
		int m_normalLoc;
	};

	virtual void initialize(bool initializeDebug = true) override
	{
		oplo::GlutApp::initialize(initializeDebug);
		m_instancingProgram = std::make_shared<InstancingProgram>();
		m_instancingProgram->create();
		m_uniforms.registerShader(m_instancingProgram);
		allocateCubes();
	}

	void shutdown()
	{
		m_cubePositionBuffer.deallocate();
		m_unitCube.deallocate();
		m_instancingProgram->destroy();
	}

	virtual void draw() override
	{
		m_instancingProgram->bind();

		glEnableVertexAttribArray(m_instancingProgram->m_vertexLoc);
		glEnableVertexAttribArray(m_instancingProgram->m_colorLoc);
		glEnableVertexAttribArray(m_instancingProgram->m_rotationLoc);
		glEnableVertexAttribArray(m_instancingProgram->m_scaleAndTranslationLoc);
		glEnableVertexAttribArray(m_instancingProgram->m_normalLoc);

		glBindBuffer(GL_ARRAY_BUFFER, m_unitCube.getId());
		glVertexAttribPointer(m_instancingProgram->m_vertexLoc, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), 0);
		glVertexAttribPointer(m_instancingProgram->m_normalLoc, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (char*)0 + 3 * sizeof(float));
		glBindBuffer(GL_ARRAY_BUFFER, m_cubePositionBuffer.getId());
		glVertexAttribPointer(m_instancingProgram->m_colorLoc, 4, GL_FLOAT, GL_FALSE, sizeof(CubeData), (GLchar*)0 + offsetof(CubeData, m_color));
		glVertexAttribPointer(m_instancingProgram->m_scaleAndTranslationLoc, 4, GL_FLOAT, GL_FALSE, sizeof(CubeData), (GLchar*)0 + offsetof(CubeData, m_scale));
		glVertexAttribPointer(m_instancingProgram->m_rotationLoc, 4, GL_FLOAT, GL_FALSE, sizeof(CubeData), (GLchar*)0 + offsetof(CubeData, m_rotation));
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glVertexAttribDivisor(m_instancingProgram->m_colorLoc, 1);
		glVertexAttribDivisor(m_instancingProgram->m_scaleAndTranslationLoc, 1);
		glVertexAttribDivisor(m_instancingProgram->m_rotationLoc, 1);

		glDrawArraysInstanced(GL_QUADS, 0, 4 * 6, m_numCubes);

		glDisableVertexAttribArray(m_instancingProgram->m_vertexLoc);
		glDisableVertexAttribArray(m_instancingProgram->m_colorLoc);
		glDisableVertexAttribArray(m_instancingProgram->m_scaleAndTranslationLoc);
		glDisableVertexAttribArray(m_instancingProgram->m_rotationLoc);
		glDisableVertexAttribArray(m_instancingProgram->m_normalLoc);
		
		m_instancingProgram->unbind();
	}

	void allocateCubes(unsigned numCubes = 7500)
	{
		m_numCubes = numCubes * 6;

		m_unitCube.deallocate();
		m_unitCube.create("CubeBuffer");
		m_unitCube.allocate(GL_ARRAY_BUFFER, 72 * 2 * sizeof(float), GL_MAP_WRITE_BIT, 0, true );

		m_cubePositionBuffer.deallocate();
		m_cubePositionBuffer.create("CubePositionBuffer");
		m_cubePositionBuffer.allocate(GL_ARRAY_BUFFER, m_numCubes *  sizeof(CubeData), GL_MAP_WRITE_BIT, 0, true);

		m_unitCube.map(GL_WRITE_ONLY);
		oplo::MakeCube(oplo::Vec3F(-1, -1, -1), oplo::Vec3F(1, 1, 1), m_unitCube.getMappedPointer<float>(), 6);
		oplo::InjectNormalsIntoPrimitive(GL_QUADS, m_unitCube.getMappedPointer<float>(), 6 * 4, 6);
		m_unitCube.unmap();

		m_cubePositionBuffer.map(GL_WRITE_ONLY);

		{
			CubeData* cubes = m_cubePositionBuffer.getMappedPointer<CubeData>();

			std::random_device rd;
			std::mt19937 twister(rd());
			std::uniform_real_distribution<float> translation(-200, 200);
			std::uniform_real_distribution<float> rotation(0, 360);
			std::uniform_real_distribution<float> axisRange(-1, 1);
			std::uniform_real_distribution<float> color(0, 1);
			std::uniform_real_distribution<float> scale(0.5, 5);

			oplo::Vec4F colorV;

			colorV.setXyzw(255 / 255., 243 / 255., 0 / 255., 1);
			for (int i = 0; i < numCubes; ++i)
			{
				double freqZ = 100;
				double angleR = (i / double(numCubes - 1)) * (3.1415926 * 2);

				oplo::Vec3F axis(axisRange(twister), axisRange(twister), axisRange(twister));
				oplo::Anglef angle = oplo::Anglef::fromDeg(rotation(twister));
				oplo::Vec3F l0(sin(angleR) * 200, cos(angleR) * 200, 0);

				float ct = 0.1f * axisRange(twister);

				cubes[i].m_translation = l0;
				cubes[i].m_scale = scale(twister);
				cubes[i].m_color = colorV + ct;
				cubes[i].m_rotation.fromAxisAngle(angle, axis);
			}

			colorV.setXyzw(124 / 255., 93 / 255., 178 / 255., 1);
			for (int i = numCubes, j = 0; i < numCubes * 2; ++i, ++j)
			{
				double phaseOffset = 0; // (3.1415926 * 2) / 3;
				double angleR = (j / double(numCubes - 1)) * (3.1415926 * 2);
				double angleN = ((j + 1) / double(numCubes - 1)) * (3.1415926 * 2);

				oplo::Vec3F axis(axisRange(twister), axisRange(twister), axisRange(twister));
				oplo::Anglef angle = oplo::Anglef::fromDeg(rotation(twister));

				oplo::Vec3F tmp0(sin(angleR), cos(angleR), 0);
				tmp0.normalize();

				oplo::Vec3F tmp1(sin(angleN), cos(angleN), 0);
				tmp1.normalize();

				oplo::Vec3F l0 = tmp0 * 50.f;
				oplo::Vec3F l1 = tmp1 * 50.f;

				oplo::Vec3F l = l1 - l0;
				l.normalize();

				oplo::Quat<float> q;
				q.fromAxisAngle(oplo::Angle<float>(( angleR) * 17), l);
				q.normalize();

				oplo::Vec3F d2(0, 0, 1);

				d2 = oplo::TransformVector(q, d2);

				float ct = 0.1f * axisRange(twister);

				cubes[i].m_translation = d2 * 200.f + l0;
				cubes[i].m_scale = d2[2] < 0 ? 1.f : 0.f;
				cubes[i].m_color = colorV + ct;
				cubes[i].m_rotation.fromAxisAngle(angle, axis);
			}

			//colorV.setXyzw(109 / 255., 25 / 255., 255 / 255., 1);
			colorV.setXyzw(25 / 255., 109 / 255., 255 / 255., 1);
			for (int i = numCubes * 2, j = 0; i < numCubes * 3; ++i, ++j)
			{
				double phaseOffset = 0; // (3.1415926 * 2) / 3;
				double angleR = (j / double(numCubes - 1)) * (3.1415926 * 2);
				double angleN = ((j + 1) / double(numCubes - 1)) * (3.1415926 * 2);

				oplo::Vec3F axis(axisRange(twister), axisRange(twister), axisRange(twister));
				oplo::Anglef angle = oplo::Anglef::fromDeg(rotation(twister));

				oplo::Vec3F tmp0(sin(angleR), cos(angleR), 0);
				tmp0.normalize();

				oplo::Vec3F tmp1(sin(angleN), cos(angleN), 0);
				tmp1.normalize();

				oplo::Vec3F l0 = tmp0 * 125.f;
				oplo::Vec3F l1 = tmp1 * 125.f;

				oplo::Vec3F l = l1 - l0;
				l.normalize();

				oplo::Quat<float> q;
				q.fromAxisAngle(oplo::Angle<float>((phaseOffset + angleR) * 17), l);
				q.normalize();

				oplo::Vec3F d2(0, 0, 1);
				d2 = oplo::TransformVector(q, d2);
				d2.normalize();

				float ct = 0.1f * axisRange(twister);

				cubes[i].m_translation = d2 * 30.f + l0;
				cubes[i].m_scale = 2.f;
				cubes[i].m_color = colorV + ct;
				cubes[i].m_rotation.fromAxisAngle(angle, axis);
			}

			colorV.setXyzw(93 / 255., 0 / 255., 255 / 255., 1);
			for (int i = numCubes * 3, j=0; i < numCubes * 4; ++i, ++j)
			{
				double phaseOffset = 0; // (3.1415926 * 2) / 3;
				double angleR = (j / double(numCubes - 1)) * (3.1415926 * 2);
				double angleN = ((j+1) / double(numCubes - 1)) * (3.1415926 * 2);

				oplo::Vec3F axis(axisRange(twister), axisRange(twister), axisRange(twister));
				oplo::Anglef angle = oplo::Anglef::fromDeg(rotation(twister));

				oplo::Vec3F tmp0(sin(angleR), cos(angleR), 0);
				tmp0.normalize();

				oplo::Vec3F tmp1(sin(angleN), cos(angleN), 0);
				tmp1.normalize();

				oplo::Vec3F l0 = tmp0 * 50.f;
				oplo::Vec3F l1 = tmp1 * 50.f;

				oplo::Vec3F l = l1 - l0;
				l.normalize();

				oplo::Quat<float> q;
				q.fromAxisAngle(oplo::Angle<float>((phaseOffset * 2 + angleR) * 7), l);
				q.normalize();

				oplo::Vec3F d2(0, 0, 1)
					;
				d2 = oplo::TransformVector(q, d2);

				float ct = 0.1f * axisRange(twister);

				cubes[i].m_translation = d2 * 20.f + l0;
				cubes[i].m_scale = 1.f; // scale(twister);
				cubes[i].m_color = colorV + ct;
				cubes[i].m_rotation.fromAxisAngle(angle, axis);
			}

			colorV.setXyzw(1, 0.7, 0, 1);
			for (int i = numCubes * 4; i < numCubes * 5; ++i)
			{
				double freqZ = 100;
				double angleR = (i / double(numCubes - 1)) * (3.1415926 * 2);

				oplo::Vec3F axis(axisRange(twister), axisRange(twister), axisRange(twister));
				oplo::Anglef angle = oplo::Anglef::fromDeg(rotation(twister));
				oplo::Vec3F l0(sin(angleR) * 125, cos(angleR) * 125, 0);

				float ct = 0.1f * axisRange(twister);

				cubes[i].m_translation = l0;
				cubes[i].m_scale = scale(twister);
				cubes[i].m_color = colorV + ct;
				cubes[i].m_rotation.fromAxisAngle(angle, axis);
			}

			colorV.setXyzw(1, 76 / 255., 0, 1);
			for (int i = numCubes * 5; i < numCubes * 6; ++i)
			{
				double freqZ = 100;
				double angleR = (i / double(numCubes - 1)) * (3.1415926 * 2);

				oplo::Vec3F axis(axisRange(twister), axisRange(twister), axisRange(twister));
				oplo::Anglef angle = oplo::Anglef::fromDeg(rotation(twister));
				oplo::Vec3F l0(sin(angleR) * 50, cos(angleR) * 50, 0);

				float ct = 0.1f * axisRange(twister);

				cubes[i].m_translation = l0;
				cubes[i].m_scale = scale(twister);
				cubes[i].m_color = colorV + ct;
				cubes[i].m_rotation.fromAxisAngle(angle, axis);
			}
		}

		m_cubePositionBuffer.unmap();
	}

private:

	unsigned m_numCubes;
	std::shared_ptr<InstancingProgram> m_instancingProgram;

	oplo::BufferObject m_unitCube;
	oplo::BufferObject m_cubePositionBuffer;

} g_app;


void resizeFunc(int w, int h) { g_app.resize(w, h); }

void mouseFunc(int button, int state, int x, int y) { g_app.mouseFunc(button, state, x, y); }

void passiveMotion(int x, int y) { g_app.passiveMotion(x, y); }

void motionFunc(int x, int y) { g_app.motionFunc(x, y); }

void keyEvent(unsigned char key, int x, int y) { g_app.keyEvent(key, x, y);  }

void drawFunc() { g_app.drawFunc(); }

int main(int argc, char* argv[])
{
	const char my_name[] = "Convex Instancing";

	glutInit(&argc, argv);
	glutInitContextFlags(GLUT_DEBUG);
	glutInitDisplayMode(GLUT_RGBA | GLUT_MULTISAMPLE | GLUT_ALPHA | GLUT_DOUBLE | GLUT_DEPTH);

	glutInitWindowPosition(10, 10);
	glutInitWindowSize(800, 600);

	glutCreateWindow(my_name);

	GLenum err = glewInit();
	if (GLEW_OK != err)
	{
		/* Problem: glewInit failed, something is seriously wrong. */
		fprintf(stdout, "Error: %s\n", glewGetErrorString(err));

	}
	fprintf(stdout, "Status: Using GLEW %s\n", glewGetString(GLEW_VERSION));

	g_app.initialize(true);
	glutReshapeFunc(resizeFunc);
	glutDisplayFunc(drawFunc);
	glutMouseFunc(mouseFunc);
	glutMotionFunc(motionFunc);
	glutPassiveMotionFunc(passiveMotion);
	glutKeyboardFunc(keyEvent);
	glutMainLoop();

	return 0;
}
