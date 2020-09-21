#pragma once
#include "XYZ/Renderer/Buffer.h"


namespace XYZ {
	class OpenGLVertexBuffer : public VertexBuffer
	{
	public:
		OpenGLVertexBuffer(float* vertices, uint32_t size, BufferUsage usage);
		OpenGLVertexBuffer(uint32_t size);
		~OpenGLVertexBuffer();

		virtual void Bind() const override;
		virtual void UnBind() const override;
		virtual void Update(void* vertices, uint32_t size, uint32_t offset = 0) override;
		virtual void Resize(float* vertices, uint32_t size) override;

		virtual void SetLayout(const BufferLayout& layout) override { m_Layout = layout; };

		virtual const BufferLayout& GetLayout() const override { return m_Layout; };
	private:
		BufferUsage m_Usage;
		uint32_t m_Size;
		uint32_t m_VBO;
		BufferLayout m_Layout;

	};

	class OpenGLIndexBuffer : public IndexBuffer
	{
	public:
		OpenGLIndexBuffer(uint32_t* indices, uint32_t count);
		~OpenGLIndexBuffer();

		virtual void Bind() const override;
		virtual void UnBind() const override;

		virtual uint32_t GetCount() const override { return m_Count; }
	private:
		uint32_t m_IBO;
		uint32_t m_Count;
	};

	class OpenGLShaderStorageBuffer : public ShaderStorageBuffer
	{
	public:
		OpenGLShaderStorageBuffer(float* data, uint32_t size, BufferUsage usage);
		~OpenGLShaderStorageBuffer();

		virtual void BindRange(uint32_t offset, uint32_t size, uint32_t index) override;
		virtual void Bind() override;
		virtual void Update(const void* data, uint32_t size, uint32_t offset = 0) override;
		virtual void Resize(const void* data, uint32_t size) override;
		virtual void GetSubData(void* buffer, uint32_t size, uint32_t offset = 0) override;

		virtual void SetLayout(const BufferLayout& layout) override { m_Layout = layout; };;
		virtual const BufferLayout& GetLayout() const override { return m_Layout; };

	private:
		BufferUsage m_Usage;
		uint32_t m_Size;
		uint32_t m_SSBO;
		BufferLayout m_Layout;
	};

}