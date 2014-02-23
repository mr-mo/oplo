#include "SummedAreaTable.h"
#include "Rendering/BasicStructures/TextureDescriptor.h"
#include "Rendering/Shaders/Program.h"
#include "Rendering/Shaders/Shader.h"
#include "Math/Vector.h"

namespace oplo
{

	RectangleBoundaries::RectangleBoundaries()
	{

	}

	void RectangleBoundaries::initialize(int w, int h)
	{
		m_firstOrder = std::make_shared<TextureDescriptor>();
		m_firstOrder->createObject();
		m_firstOrder->setup2d(GL_TEXTURE_2D, 0, GL_RGBA32F, w, h, GL_RGBA, TextureDescriptor::F32);
		m_firstOrder->setParameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		m_firstOrder->setParameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		m_firstOrder->setParameter(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		m_firstOrder->setParameter(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		m_firstOrder->createStorage(false);

		//Todo: separable objects...
		std::shared_ptr<Shader> cs = std::make_shared<Shader>(Shader::COMPUTE_SHADER, "Rectangle Boundaries");
		cs->addInline("#define RECTANGLE_BOUNDARIES");
		cs->addFile("../data/shaders/ComputeSAT.glsl");
		cs->create();

		m_boundaries = std::make_shared<Program>("RectangleBoundaries");
		m_boundaries->addShader(cs);
		m_boundaries->compile();

		cs = std::make_shared<Shader>(Shader::COMPUTE_SHADER, "Rectangle BoundariesVSAT");
		cs->addInline("#define RECTANGLE_SAT_VERT");
		cs->addFile("../data/shaders/ComputeSAT.glsl");
		cs->create();

		m_boundariesVSAT = std::make_shared<Program>("RectangleBoundariesVSAT");
		m_boundariesVSAT->addShader(cs);
		m_boundariesVSAT->compile();

		cs = std::make_shared<Shader>(Shader::COMPUTE_SHADER, "Rectangle BoundariesHSAT");
		cs->addInline("#define RECTANGLE_SAT_HORIZ");
		cs->addFile("../data/shaders/ComputeSAT.glsl");
		cs->create();

		m_boundariesHSAT = std::make_shared<Program>("RectangleBoundariesHSAT");
		m_boundariesHSAT->addShader(cs);
		m_boundariesHSAT->compile();
		glGenBuffers(1, &m_shaderBuffer);
	}

	void RectangleBoundaries::resize(unsigned w, unsigned h)
	{
		m_firstOrder->resize(w, h);
		glNamedBufferDataEXT(m_shaderBuffer, w * h * 4 * sizeof(int), 0, GL_DYNAMIC_DRAW);

	}

	void RectangleBoundaries::generate(
					float cocScale, float cocBias,
					const TextureDescriptor& depthTexture,
					const TextureDescriptor& sourceTexture)
	{
		GLint clearedData[] = { 0, 0, 0, 0 };
		glClearNamedBufferDataEXT(m_shaderBuffer, GL_RGBA32I, GL_RGBA, GL_INT, clearedData);

		glBindBufferRange(GL_SHADER_STORAGE_BUFFER, 0, m_shaderBuffer, 0, depthTexture.getHeight() * depthTexture.getWidth() * 4 * 4 );
		glBindImageTexture(1, depthTexture.getId(), 0, GL_FALSE, 0, GL_READ_ONLY, GL_R32F);
		glBindImageTexture(2, sourceTexture.getId(), 0, GL_FALSE, 2, GL_READ_ONLY, GL_RGBA32F);
		m_boundaries->bind();

		glUniform2i(0, depthTexture.getWidth(), depthTexture.getHeight());
		glUniform2f(1, cocScale, cocBias);
		glDispatchCompute(GLuint(sourceTexture.getWidth() / 16.) + 1, GLuint(sourceTexture.getHeight() / 16.) + 1, 1);

		GLsync syncA = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);

//		struct i4 { int x, y, z, w;  };

//		i4* tmpA = (i4*)glMapNamedBufferEXT(m_shaderBuffer, GL_READ_ONLY);

//		glUnmapNamedBufferEXT(m_shaderBuffer);
//		oplo::Vec4F* tmp = new oplo::Vec4F[800 * 600];
//		glBindBufferRange(GL_SHADER_STORAGE_BUFFER, 0, m_shaderBuffer, 0, depthTexture.getHeight() * depthTexture.getWidth() * 4 * 4);

//		glBindImageTexture(1, m_firstOrder->getId(), 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);
		glBindImageTexture(1, m_firstOrder->getId(), 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
	
		m_boundariesVSAT->bind();
		glUniform2i(0, sourceTexture.getWidth(), sourceTexture.getHeight());
		glDispatchCompute(GLuint(sourceTexture.getWidth() / 128.) + 1, 1, 1);

		//fixme
		GLsync syncB = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);

		m_boundariesHSAT->bind();
		glUniform2i(0, sourceTexture.getWidth(), sourceTexture.getHeight());
		glDispatchCompute(1, GLuint(sourceTexture.getHeight() / 128.) + 1, 1);

//		get().fetchTexture<oplo::Vec4F>(tmp, 0);
//		delete[] tmp;

		glDeleteSync(syncA);
		glDeleteSync(syncB);

	}

	TextureDescriptor& RectangleBoundaries::get()
	{
		return *m_firstOrder;
	}


	SummedAreaTable::SummedAreaTable()
	{}

	void SummedAreaTable::initialize(int w, int h)
	{
		m_firstOrder = std::make_shared<TextureDescriptor>();
		m_firstOrder->createObject();
		m_firstOrder->setup2d(GL_TEXTURE_2D, 0, GL_RGBA32F, w, h, GL_RGBA, TextureDescriptor::F32);
		m_firstOrder->setParameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		m_firstOrder->setParameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		m_firstOrder->setParameter(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		m_firstOrder->setParameter(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		m_firstOrder->createStorage(false);

		//Todo: separable objects...
		std::shared_ptr<Shader> cs = std::make_shared<Shader>(Shader::COMPUTE_SHADER, "Vertical SAT");
		cs->addInline("#define VERTICAL_SLICE");
		cs->addFile("../data/shaders/ComputeSAT.glsl");
		cs->create();

		m_satV = std::make_shared<Program>("VerticalSAT");
		m_satV->addShader(cs);

		m_satV->compile();

		cs = std::make_shared<Shader>(Shader::COMPUTE_SHADER, "Horizontal SAT");
		cs->addInline("#define HORIZONTAL_SLICE");
		cs->addFile("../data/shaders/ComputeSAT.glsl");
		cs->create();

		m_satH = std::make_shared<Program>("HorizontalSAT");
		m_satH->addShader(cs);
		m_satH->compile();
	}

	void SummedAreaTable::resize(unsigned w, unsigned h)
	{
		m_firstOrder->resize(w, h);
	}

	void SummedAreaTable::generateFor(const TextureDescriptor& sourceTexture)
	{
		glBindImageTexture(0, sourceTexture.getId(), 0, GL_FALSE, 2, GL_READ_ONLY, GL_RGBA32F);
		glBindImageTexture(1, m_firstOrder->getId(), 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);

		m_satV->bind();
		glUniform2i(0, sourceTexture.getWidth(), sourceTexture.getHeight());
		glDispatchCompute(GLuint(sourceTexture.getWidth() / 128.) + 1, 1, 1);

		//fixme
		GLsync sync = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);

		m_satH->bind();
		glUniform2i(0, sourceTexture.getWidth(), sourceTexture.getHeight());
		glDispatchCompute(1, GLuint(sourceTexture.getHeight() / 128.) + 1, 1);
	
		glDeleteSync(sync);
	}

	TextureDescriptor& SummedAreaTable::get()
	{
		return *m_firstOrder;
	}

}
