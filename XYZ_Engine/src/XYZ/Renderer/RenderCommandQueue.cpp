#include "stdafx.h"
#include "RenderCommandQueue.h"

namespace XYZ {
	RenderCommandQueue::RenderCommandQueue()
	{
		m_CommandBuffer = new unsigned char[10 * 1024 * 1024]; // 10mb buffer
		m_CommandBufferPtr = m_CommandBuffer;
		memset(m_CommandBuffer, 0, 10 * 1024 * 1024);
	}

	RenderCommandQueue::~RenderCommandQueue()
	{
		delete[] m_CommandBuffer;
	}

	void RenderCommandQueue::Allocate(CommandI* cmd, unsigned int size)
	{
		*(int*)m_CommandBufferPtr = size;
		m_CommandBufferPtr += sizeof(unsigned int);

		//  copy command
		memcpy(m_CommandBufferPtr, cmd, size);
		m_CommandBufferPtr += size;

		m_CommandCount++;
	}
	void RenderCommandQueue::Execute()
	{
		unsigned char* buffer = m_CommandBuffer;
		for (unsigned int i = 0; i < m_CommandCount; i++)
		{
			unsigned int size = *(int*)buffer;
			buffer += sizeof(unsigned int);
			auto func = reinterpret_cast<CommandI*>(buffer);
			func->Execute();
			buffer += size;
		}

		m_CommandBufferPtr = m_CommandBuffer;
		m_CommandCount = 0;
	}

	void RenderCommandQueue::Clear()
	{
		memset(m_CommandBuffer, 0, 10 * 1024 * 1024);
		m_CommandBufferPtr = m_CommandBuffer;
		memset(m_CommandBuffer, 0, 10 * 1024 * 1024);
	}


}