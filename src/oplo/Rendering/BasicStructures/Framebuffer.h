#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include "Rendering/oploGL.h"
#include "Rendering/Debug/RenderingDebugOutput.h"
#include "Rendering/BasicStructures/TextureDescriptor.h"

#include <array>
#include <memory>

namespace oplo
{

	template<std::size_t NColorAttachments>
	class TextureFramebuffer
	{
	public:

		TextureFramebuffer(const char* label) : m_id(0), m_label(label)
		{}

		~TextureFramebuffer()
		{
			destroy();
		}

		void create()
		{
			if (m_id)
			{
				oplo::DebugMessage(GL_DEBUG_SOURCE_APPLICATION, GL_DEBUG_TYPE_ERROR, 0, GL_DEBUG_SEVERITY_LOW, "%s buffer allocated multiple times", m_label.c_str());
			}
			else
			{
				glGenFramebuffers(1, &m_id);
				oplo::ObjectLabel(GL_FRAMEBUFFER, m_id, 0, m_label.c_str());
			}
		}

		const TextureDescriptor& getDepthBuffer() const
		{
			return *m_depthAttachment;
		}

		const TextureDescriptor& getStencilBuffer() const
		{
			return *m_stencilAttachment;
		}

		const TextureDescriptor& getAttachment(int i) const
		{
			return *m_colorAttachment[i];
		}

		TextureDescriptor& getDepthBuffer()
		{
			return *m_depthAttachment;
		}

		TextureDescriptor& getStencilBuffer()
		{
			return *m_stencilAttachment;
		}

		TextureDescriptor& getAttachment(int i)
		{
			return *m_colorAttachment[i];
		}


		void destroy()
		{
			if (m_id)
			{
				glDeleteFramebuffers(1, &m_id);
			}
		}

		void enableBuffers( unsigned n, GLenum* buffers )
		{
			glFramebufferDrawBuffersEXT(m_id, n, buffers);
		}

		void bind( unsigned target = GL_FRAMEBUFFER)
		{
			glBindFramebuffer(target, m_id);
		}

		void clear(unsigned clearMask = GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT)
		{
			glClear(clearMask);
		}

		/// Specialized routines for array textures
		void attach(
			unsigned attachmentPoint,
			unsigned attachmentLevel,
			unsigned attachmentLayer,
			std::shared_ptr<TextureDescriptor> descriptor)
		{
			assignAttachment(attachmentPoint, descriptor);
			glNamedFramebufferTextureLayerEXT(m_id, attachmentPoint, descriptor->getId(), attachmentLevel, attachmentLayer);
		}

		//Specialized routine for cubemaps
		void attachCubemap(
			unsigned attachmentPoint,
			unsigned attachmentTarget,
			unsigned attachmentLevel,
			std::shared_ptr<TextureDescriptor> descriptor)
		{
			assignAttachment(attachmentPoint, descriptor);
			glNamedFramebufferTexture2DEXT(m_id, attachmentPoint, attachmentTarget, descriptor->getId(), attachmentLevel);
		}

		void attach(
			unsigned attachmentPoint,
			unsigned attachmentLevel,
			std::shared_ptr<TextureDescriptor> descriptor)
		{
			assignAttachment(attachmentPoint, descriptor);
			glNamedFramebufferTextureEXT(m_id, attachmentPoint, descriptor->getId(), attachmentLevel);
		}


		void validate()
		{
			glBindFramebuffer(GL_FRAMEBUFFER, m_id);
			GLenum errorCode = glCheckFramebufferStatus(GL_FRAMEBUFFER);
			switch (errorCode)
			{
			case GL_FRAMEBUFFER_UNDEFINED:
				oplo::DebugMessage(GL_DEBUG_SOURCE_APPLICATION, GL_DEBUG_TYPE_ERROR, 0, GL_DEBUG_SEVERITY_LOW,
					"%s: %s", m_label.c_str(), "GL_FRAMEBUFFER_UNDEFINED");
				break;
			case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
				oplo::DebugMessage(GL_DEBUG_SOURCE_APPLICATION, GL_DEBUG_TYPE_ERROR, 0, GL_DEBUG_SEVERITY_LOW,
					"%s: %s", m_label.c_str(), "GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT");
				break;
			case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
				oplo::DebugMessage(GL_DEBUG_SOURCE_APPLICATION, GL_DEBUG_TYPE_ERROR, 0, GL_DEBUG_SEVERITY_LOW,
					"%s: %s", m_label.c_str(), "GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT");
				break;
			case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:
				oplo::DebugMessage(GL_DEBUG_SOURCE_APPLICATION, GL_DEBUG_TYPE_ERROR, 0, GL_DEBUG_SEVERITY_LOW,
					"%s: %s", m_label.c_str(), "GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER");
				break;
			case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:
				oplo::DebugMessage(GL_DEBUG_SOURCE_APPLICATION, GL_DEBUG_TYPE_ERROR, 0, GL_DEBUG_SEVERITY_LOW,
					"%s: %s", m_label.c_str(), "GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER");
				break;
			case GL_FRAMEBUFFER_UNSUPPORTED:
				oplo::DebugMessage(GL_DEBUG_SOURCE_APPLICATION, GL_DEBUG_TYPE_ERROR, 0, GL_DEBUG_SEVERITY_LOW,
					"%s: %s", m_label.c_str(), "GL_FRAMEBUFFER_UNSUPPORTED");
				break;
			case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE:
				oplo::DebugMessage(GL_DEBUG_SOURCE_APPLICATION, GL_DEBUG_TYPE_ERROR, 0, GL_DEBUG_SEVERITY_LOW,
					"%s: %s", m_label.c_str(), "GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE");
				break;
			case GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS:
				oplo::DebugMessage(GL_DEBUG_SOURCE_APPLICATION, GL_DEBUG_TYPE_ERROR, 0, GL_DEBUG_SEVERITY_LOW,
					"%s: %s", m_label.c_str(), "GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS");
				break;
			default:
				break;
			}
		}

	private:

		void assignAttachment(unsigned attachmentPoint, std::shared_ptr<TextureDescriptor> descriptor)
		{
			switch (attachmentPoint)
			{
			case GL_DEPTH_ATTACHMENT:
				m_depthAttachment = descriptor;
				break;
			case GL_STENCIL_ATTACHMENT:
				m_stencilAttachment = descriptor;
				break;
			default:
				int idx = attachmentPoint - GL_COLOR_ATTACHMENT0;
				m_colorAttachment[idx] = descriptor;
				break;
			}
		}

	protected:

		std::string m_label;
		std::shared_ptr<TextureDescriptor> m_depthAttachment;
		std::shared_ptr<TextureDescriptor> m_stencilAttachment;
		std::array<std::shared_ptr<TextureDescriptor>, NColorAttachments> m_colorAttachment;
		unsigned m_id;
	};

}

#endif
