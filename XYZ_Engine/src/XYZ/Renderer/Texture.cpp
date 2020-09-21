#include "stdafx.h"
#include "Texture.h"

#include "Renderer.h"
#include "API/OpenGL/OpenGLTexture.h"

namespace XYZ {
	std::shared_ptr<Texture2D> Texture2D::Create(TextureFormat format, TextureWrap wrap, uint32_t width, uint32_t height)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None: XYZ_ASSERT(false, "Renderer::GetAPI() = RendererAPI::None");
		case RendererAPI::API::OpenGL: return std::make_shared<OpenGLTexture2D>(format, wrap, width, height);
		}

		XYZ_ASSERT(false, "Renderer::GetAPI() = RendererAPI::None");
		return NULL;
	}
	std::shared_ptr<Texture2D> Texture2D::Create(TextureWrap wrap, const std::string& path)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None: XYZ_ASSERT(false, "Renderer::GetAPI() = RendererAPI::None");
		case RendererAPI::API::OpenGL: return std::make_shared<OpenGLTexture2D>(wrap, path);
		}

		XYZ_ASSERT(false, "Renderer::GetAPI() = RendererAPI::None");
		return NULL;
	}
}