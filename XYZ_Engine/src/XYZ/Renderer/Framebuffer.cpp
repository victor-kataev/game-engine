#include "stdafx.h"
#include "FrameBuffer.h"
#include "API/OpenGL/OpenGLFrameBuffer.h"
#include "Renderer.h"


namespace XYZ {
	std::shared_ptr<FrameBuffer> FrameBuffer::Create(uint32_t width, uint32_t height, FrameBufferFormat format)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None:    XYZ_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
		case RendererAPI::API::OpenGL:  return std::make_shared<OpenGLFrameBuffer>(width, height, format);
		}

		XYZ_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}
}