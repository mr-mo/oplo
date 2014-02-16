#include "../GlutApp/GlutApp.h"
#include "Rendering/BasicStructures/BufferObject.h"
#include "Rendering/BasicStructures/Framebuffer.h"
#include "Rendering/Shapes/ShapeGenerator.h"
#include "Rendering/Shaders/Program.h"
#include "Rendering/Shaders/Shader.h"
#include "Rendering/Techniques/Rasterization/DeferredFramebuffer.h"
#include "Math/Matrix.h"

#include "PatternGenerator.h"
#include "SummedAreaTable.h"

class InstancingApp : public oplo::GlutApp
{
public:

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
			vtxShader->addFile("../data/shaders/InstancingTest.glsl");

			frgShader->addInline("#define FRAGMENT_SHADER");
			frgShader->addFile("../data/shaders/InstancingTest.glsl");

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


	class CompositionProgram : public oplo::Program
	{
	public:

		CompositionProgram() :
			Program("CompositionProgram")
		{}

		void create()
		{
			std::shared_ptr<oplo::Shader> vtxShader, frgShader;
			vtxShader = std::make_shared<oplo::Shader>(oplo::Shader::VTX_SHADER, "CompositionVertex");
			frgShader = std::make_shared<oplo::Shader>(oplo::Shader::FRAG_SHADER, "CompositionFragment");
			vtxShader->addInline("#define VERTEX_SHADER");
			vtxShader->addFile("../data/shaders/DeferredComposition.glsl");

			frgShader->addInline("#define FRAGMENT_SHADER");
			frgShader->addFile("../data/shaders/DeferredComposition.glsl");

			vtxShader->create();
			frgShader->create();

			addShader(vtxShader);
			addShader(frgShader);

			compile();
		}
	};

	class FinalPassProgram : public oplo::Program
	{
	public:

		FinalPassProgram() :
			Program("FinalPassProgram")
		{}

		void create()
		{
			std::shared_ptr<oplo::Shader> vtxShader, frgShader;
			vtxShader = std::make_shared<oplo::Shader>(oplo::Shader::VTX_SHADER, "CompositionVertex");
			frgShader = std::make_shared<oplo::Shader>(oplo::Shader::FRAG_SHADER, "CompositionFragment");
			vtxShader->addInline("#define VERTEX_SHADER");
			vtxShader->addFile("../data/shaders/DeferredComposition.glsl");

			frgShader->addInline("#define BLUR_TO_SCREEN");
			frgShader->addFile("../data/shaders/DeferredComposition.glsl");

			vtxShader->create();
			frgShader->create();

			addShader(vtxShader);
			addShader(frgShader);

			compile();
		}
	};


	InstancingApp()
	{}

	virtual void initialize(bool initializeDebug = true) override
	{
		oplo::GlutApp::initialize(initializeDebug);

		m_instancingProgram = std::make_shared<InstancingProgram>();
		m_instancingProgram->create();

		m_compositionProgram = std::make_shared<CompositionProgram>();
		m_compositionProgram->create();

		m_finalPassProgram = std::make_shared<FinalPassProgram>();
		m_finalPassProgram->create();

		m_uniforms.registerShader(m_instancingProgram);
		m_uniforms.registerShader(m_compositionProgram);
		m_uniforms.registerShader(m_finalPassProgram);

		allocateCubes();
		m_framebuffer.initialize(800, 600, 32);
		m_framebuffer.validate();
		m_sat.initialize(800, 600);
	}

	void shutdown()
	{
		m_cubePositionBuffer.deallocate();
		m_unitCube.deallocate();
		m_instancingProgram->destroy();
		m_compositionProgram->destroy();
		m_finalPassProgram->destroy();
		m_framebuffer.destroy();
	}

	virtual void draw() override
	{
		m_framebuffer.enableAll();
		m_framebuffer.bind();
		m_framebuffer.clear();

		m_instancingProgram->bind();

		glEnable(GL_DEPTH_TEST);

		glEnableVertexAttribArray(m_instancingProgram->m_vertexLoc);
		glEnableVertexAttribArray(m_instancingProgram->m_colorLoc);
		glEnableVertexAttribArray(m_instancingProgram->m_rotationLoc);
		glEnableVertexAttribArray(m_instancingProgram->m_scaleAndTranslationLoc);
		glEnableVertexAttribArray(m_instancingProgram->m_normalLoc);

		glBindBuffer(GL_ARRAY_BUFFER, m_unitCube.getId());
		glVertexAttribPointer(m_instancingProgram->m_vertexLoc, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), 0);
		glVertexAttribPointer(m_instancingProgram->m_normalLoc, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (char*)0 + 3 * sizeof(float));
		glBindBuffer(GL_ARRAY_BUFFER, m_cubePositionBuffer.getId());
		glVertexAttribPointer(m_instancingProgram->m_colorLoc, 4, GL_FLOAT, GL_FALSE, sizeof(PatternGenerator::CubeData), (GLchar*)0 + offsetof(PatternGenerator::CubeData, m_color));
		glVertexAttribPointer(m_instancingProgram->m_scaleAndTranslationLoc, 4, GL_FLOAT, GL_FALSE, sizeof(PatternGenerator::CubeData), (GLchar*)0 + offsetof(PatternGenerator::CubeData, m_scale));
		glVertexAttribPointer(m_instancingProgram->m_rotationLoc, 4, GL_FLOAT, GL_FALSE, sizeof(PatternGenerator::CubeData), (GLchar*)0 + offsetof(PatternGenerator::CubeData, m_rotation));
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
		
		GLenum lastBuffer[] = { GL_COLOR_ATTACHMENT2 };
		m_framebuffer.enableBuffers(1, lastBuffer);

		glBindTexture(GL_TEXTURE_2D_ARRAY, m_framebuffer.getAttachment(0).getId());

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, oplo::MakeScreenQuad());

		glDisable(GL_DEPTH_TEST);

		m_compositionProgram->bind();
		
		glDrawArrays(GL_QUADS, 0, 4);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		m_sat.generateFor(m_framebuffer.getAttachment(0));
		glBindMultiTextureEXT(GL_TEXTURE2, GL_TEXTURE_2D, m_sat.get().getId());
		m_finalPassProgram->bind();

		glDrawArrays(GL_QUADS, 0, 4);

		glDisableVertexAttribArray(0);
		glUseProgram(0);
	}

	void allocateCubes(unsigned numCubes = 7500)
	{
		m_numCubes = m_patterns.getTotalAllocations(PatternGenerator::CUBE_JUNGLE, numCubes);

		m_unitCube.deallocate();
		m_unitCube.create("CubeBuffer");
		m_unitCube.allocate(GL_ARRAY_BUFFER, 72 * 2 * sizeof(float), GL_MAP_WRITE_BIT, 0, true );

		m_cubePositionBuffer.deallocate();
		m_cubePositionBuffer.create("CubePositionBuffer");
		m_cubePositionBuffer.allocate(GL_ARRAY_BUFFER, m_numCubes *  sizeof(PatternGenerator::CubeData), GL_MAP_WRITE_BIT, 0, true);

		m_unitCube.map(GL_WRITE_ONLY);
		oplo::MakeCube(oplo::Vec3F(-1, -1, -1), oplo::Vec3F(1, 1, 1), m_unitCube.getMappedPointer<float>(), 6);
		oplo::InjectNormalsIntoPrimitive(GL_QUADS, m_unitCube.getMappedPointer<float>(), 6 * 4, 6);
		m_unitCube.unmap();

		m_cubePositionBuffer.map(GL_WRITE_ONLY);

		{
			m_patterns.fillBuffer(m_cubePositionBuffer.getMappedPointer<PatternGenerator::CubeData>(), PatternGenerator::CUBE_JUNGLE, numCubes);
		}

		m_cubePositionBuffer.unmap();
	}

	virtual void resize(int w, int h)
	{
		oplo::GlutApp::resize(w, h);
		m_framebuffer.resize(w, h);
		m_sat.resize(w, h);
	}

	virtual void keyEvent(unsigned char key, int x, int y)
	{
		bool overriden = false;

		switch (key)
		{
		case 's':
			break;
		default:
			break;
		}

		if (!overriden)
			oplo::GlutApp::keyEvent(key, x, y);
	}


private:

	unsigned m_numCubes;
	std::shared_ptr<InstancingProgram> m_instancingProgram;
	std::shared_ptr<CompositionProgram> m_compositionProgram;
	std::shared_ptr<FinalPassProgram> m_finalPassProgram;

	PatternGenerator m_patterns;
	oplo::SummedAreaTable m_sat;

	oplo::BufferObject m_unitCube;
	oplo::BufferObject m_cubePositionBuffer;

	oplo::DeferredFramebuffer m_framebuffer;
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
