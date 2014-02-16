#include "SummedAreaTable.h"
#include "Rendering/BasicStructures/TextureDescriptor.h"
#include "Rendering/Shaders/Program.h"
#include "Rendering/Shaders/Shader.h"
#include "Math/Vector.h"

namespace oplo
{

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
