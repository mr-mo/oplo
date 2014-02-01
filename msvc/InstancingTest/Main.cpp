#include "GlutApp.h"
#include "Rendering/BasicStructures/BufferObject.h"
#include "Rendering/Shapes/ShapeGenerator.h"
#include "Rendering/Shaders/Program.h"
#include "Rendering/Shaders/Shader.h"

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
			m_scaleAndTranslationLoc = getInputLoc("scaleAndTranslation");
		}

		//Vertex attributes
		int m_vertexLoc;
		int m_scaleAndTranslationLoc;
		int m_rotationLoc;
		int m_colorLoc;
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

		glBindBuffer(GL_ARRAY_BUFFER, m_unitCube.getId());
		glVertexAttribPointer(m_instancingProgram->m_vertexLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
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

		m_instancingProgram->unbind();
	}

	void allocateCubes(unsigned numCubes = 7500)
	{
		m_numCubes = numCubes;

		m_unitCube.deallocate();
		m_unitCube.setImmutable(true);
		m_unitCube.allocate(GL_ARRAY_BUFFER, 1, 72 * sizeof(float), GL_MAP_WRITE_BIT, "CubeBuffer");

		m_cubePositionBuffer.deallocate();
		m_cubePositionBuffer.setImmutable(true);
		m_cubePositionBuffer.allocate(GL_ARRAY_BUFFER, 1, numCubes * sizeof(CubeData), GL_MAP_WRITE_BIT, "CubePositionBuffer");

		m_unitCube.map(GL_WRITE_ONLY);
		oplo::MakeCube(oplo::Vec3F(-1, -1, -1), oplo::Vec3F(1, 1, 1), m_unitCube.getMappedPointer<float>());
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

			for (int i = 0; i < numCubes; ++i)
			{
				oplo::Vec3F axis(axisRange(twister), axisRange(twister), axisRange(twister));
				oplo::Anglef angle = oplo::Anglef::fromDeg(rotation(twister));

				cubes[i].m_translation[0] = translation(twister);
				cubes[i].m_translation[1] = translation(twister);
				cubes[i].m_translation[2] = translation(twister);
				cubes[i].m_scale = scale(twister);
				cubes[i].m_color[0] = color(twister);
				cubes[i].m_color[1] = color(twister);
				cubes[i].m_color[2] = color(twister);
				cubes[i].m_color[3] = color(twister);
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
