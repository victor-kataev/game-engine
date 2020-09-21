#include "stdafx.h"
#include "RendererBatchSystem2D.h"
#include "RenderCommandQueue.h"
#include "Renderer2D.h"
#include "RenderCommand.h"


#include "XYZ/Timer.h"

namespace XYZ {
	struct Vertex2D
	{
		glm::vec4 color;
		glm::vec3 position;
		glm::vec2 texCoord;
		float	  textureID;
	};

	struct MaterialComparator
	{
		bool operator()(const std::shared_ptr<Material>& a, const std::shared_ptr<Material>& b) const
		{
			return (int16_t)a->GetSortKey() < (int16_t)b->GetSortKey();
		}
	};

	struct Renderer2DData
	{
		Renderer2DData() { Reset(); }
		void Submit(const Renderable2D& renderable,const Transform2D& transform);
		void Reset();

		const uint32_t MaxQuads = 10000;
		const uint32_t MaxVertices = MaxQuads * 4;
		const uint32_t MaxIndices = MaxQuads * 6;

		std::shared_ptr<Material> Material;
		std::shared_ptr<VertexArray> QuadVertexArray;
		std::shared_ptr<VertexBuffer> QuadVertexBuffer;

		uint32_t IndexCount = 0;
		Vertex2D* BufferBase = nullptr;
		Vertex2D* BufferPtr = nullptr;

	};
	static std::map<std::shared_ptr<Material>, Renderer2DData, MaterialComparator> s_OpaqueBuckets;
	static std::map<std::shared_ptr<Material>, Renderer2DData, MaterialComparator> s_TransparentBuckets;

	static void SubmitCommand(const Renderer2DData& data)
	{
		uint32_t dataSize = (uint8_t*)data.BufferPtr - (uint8_t*)data.BufferBase;
		if (dataSize != 0)
		{
			data.QuadVertexBuffer->Update(data.BufferBase, dataSize);
			data.Material->Bind();
			data.QuadVertexArray->Bind();
			RenderCommand::DrawIndexed(data.QuadVertexArray, data.IndexCount);
		}
	}
	static void SubmitMaterial(std::shared_ptr<Material> material)
	{
		material->Bind();
	}
	RendererBatchSystem2D::RendererBatchSystem2D()
	{
		m_Signature.set(ECSManager::Get().GetComponentType<Renderable2D>());
		m_Signature.set(ECSManager::Get().GetComponentType<Transform2D>());
		m_RenderableStorage = ECSManager::Get().GetComponentStorage<Renderable2D>();
		m_TransformStorage = ECSManager::Get().GetComponentStorage<Transform2D>();

	}


	void RendererBatchSystem2D::Add(Entity entity)
	{		
		auto renderable = ECSManager::Get().GetComponent<Renderable2D>(entity);
		auto key = renderable->material->GetSortKey();

		if (!(key & RenderFlags::InstancedFlag))
		{
			XYZ_LOG_INFO("Entity with id ", entity, " added");
			Component component;
			component.entity = entity;
			component.renderableIndex = ECSManager::Get().GetComponentIndex<Renderable2D>(entity);
			component.transformIndex = ECSManager::Get().GetComponentIndex<Transform2D>(entity);
			component.activeIndex = ECSManager::Get().GetComponentIndex<ActiveComponent>(entity);

			if (renderable->material->GetSortKey() & RenderFlags::TransparentFlag)
			{
				m_TransparentComponents.push_back(component);
				std::push_heap(m_TransparentComponents.begin(), m_TransparentComponents.end(),TransparentComparator());
			}
			else
			{
				m_OpaqueComponents.push_back(component);
				std::push_heap(m_OpaqueComponents.begin(), m_OpaqueComponents.end(),OpaqueComparator());
			}
		}
	}

	void RendererBatchSystem2D::Remove(Entity entity)
	{
		if (ECSManager::Get().Contains<Renderable2D>(entity))
		{
			auto renderable = ECSManager::Get().GetComponent<Renderable2D>(entity);
			auto key = renderable->material->GetSortKey();
			if (key & RenderFlags::TransparentFlag)
			{
				auto it = std::find(m_TransparentComponents.begin(), m_TransparentComponents.end(), entity);
				if (it != m_TransparentComponents.end())
				{
					XYZ_LOG_INFO("Entity with id ", entity, " removed");
					*it = std::move(m_TransparentComponents.back());
					m_TransparentComponents.pop_back();
				}
			}
			else
			{
				auto it = std::find(m_OpaqueComponents.begin(), m_OpaqueComponents.end(), entity);
				if (it != m_OpaqueComponents.end())
				{
					XYZ_LOG_INFO("Entity with id ", entity, " removed");
					*it = std::move(m_OpaqueComponents.back());
					m_OpaqueComponents.pop_back();
				}
			}
		}
	}
	bool RendererBatchSystem2D::Contains(Entity entity)
	{
		if (ECSManager::Get().Contains<Renderable2D>(entity))
		{
			auto renderable = ECSManager::Get().GetComponent<Renderable2D>(entity);
			auto key = renderable->material->GetSortKey();
			if (key & RenderFlags::TransparentFlag)
			{
				auto it = std::find(m_TransparentComponents.begin(), m_TransparentComponents.end(), entity);
				if (it != m_TransparentComponents.end())
					return true;
			}
			else
			{
				auto it = std::find(m_OpaqueComponents.begin(), m_OpaqueComponents.end(), entity);
				if (it != m_OpaqueComponents.end())
					return true;
			}
		}
		return false;
	}

	// If renderable is updated and keys do not match, reinsert it and update key
	void RendererBatchSystem2D::EntityUpdated(Entity entity)
	{
		// TODO
	}

	void RendererBatchSystem2D::SubmitToRenderer()
	{	
		if (!std::is_heap(m_OpaqueComponents.begin(), m_OpaqueComponents.end(),  OpaqueComparator()))
			std::make_heap(m_OpaqueComponents.begin(), m_OpaqueComponents.end(), OpaqueComparator());
		for (auto it : m_OpaqueComponents)
		{
			auto material = (*m_RenderableStorage)[it.renderableIndex].material;
			auto& bucket = s_OpaqueBuckets[material];
			if (bucket.IndexCount < bucket.IndexCount)
			{
				bucket.Submit((*m_RenderableStorage)[it.renderableIndex],(*m_TransformStorage)[it.transformIndex]);
			}
			else
			{
				bucket.Material = material;
				SubmitCommand(bucket);
				bucket.Reset();
			}
		}
		for (auto& it : s_OpaqueBuckets)
		{
			it.second.Material = it.first;
			SubmitCommand(it.second);
			it.second.Reset();
		}
		
		if (!std::is_heap(m_TransparentComponents.begin(), m_TransparentComponents.end(), TransparentComparator()))
			std::make_heap(m_TransparentComponents.begin(), m_TransparentComponents.end(), TransparentComparator());
		for (auto it : m_TransparentComponents)
		{
			
			auto material = (*m_RenderableStorage)[it.renderableIndex].material;
			auto& bucket = s_TransparentBuckets[material];
			
			if (bucket.IndexCount < bucket.MaxIndices)
			{
				bucket.Submit((*m_RenderableStorage)[it.renderableIndex],(*m_TransformStorage)[it.transformIndex]);
			}
			else
			{
				bucket.Material = material;
				SubmitCommand(bucket);
				bucket.Reset();
			}
		}
		//std::cout << "Draw" << std::endl;
		for (auto& it : s_TransparentBuckets)
		{
			it.second.Material = it.first;
			SubmitCommand(it.second);
			it.second.Reset();
		}
	}


	void Renderer2DData::Submit(const Renderable2D& renderable,const Transform2D& transform)
	{
		BufferPtr->position = { transform.position.x - transform.size.x / 2.0f,transform.position.y - transform.size.y / 2.0f,transform.position.z };
		BufferPtr->color = renderable.color;
		BufferPtr->texCoord.x = renderable.texCoord.x;
		BufferPtr->texCoord.y = renderable.texCoord.y;
		BufferPtr->textureID = (float)renderable.textureID;
		BufferPtr++;

		BufferPtr->position = { transform.position.x + transform.size.x / 2.0f,transform.position.y - transform.size.y / 2.0f,transform.position.z };
		BufferPtr->color = renderable.color;
		BufferPtr->texCoord.x = renderable.texCoord.z;
		BufferPtr->texCoord.y = renderable.texCoord.y;
		BufferPtr->textureID = (float)renderable.textureID;
		BufferPtr++;

		BufferPtr->position = { transform.position.x + transform.size.x / 2.0f,transform.position.y + transform.size.y / 2.0f,transform.position.z };
		BufferPtr->color = renderable.color;
		BufferPtr->texCoord.x = renderable.texCoord.z;
		BufferPtr->texCoord.y = renderable.texCoord.w;
		BufferPtr->textureID = (float)renderable.textureID;
		BufferPtr++;

		BufferPtr->position = { transform.position.x - transform.size.x / 2.0f,transform.position.y + transform.size.y / 2.0f,transform.position.z };
		BufferPtr->color = renderable.color;
		BufferPtr->texCoord.x = renderable.texCoord.x;
		BufferPtr->texCoord.y = renderable.texCoord.w;
		BufferPtr->textureID = (float)renderable.textureID;
		BufferPtr++;

		IndexCount += 6;
	}

	void Renderer2DData::Reset()
	{
		if (!BufferBase)
		{
			BufferBase = new Vertex2D[MaxVertices];
			QuadVertexArray = VertexArray::Create();
			QuadVertexBuffer = VertexBuffer::Create(MaxVertices * sizeof(Vertex2D));
			QuadVertexBuffer->SetLayout(BufferLayout{
			{0, XYZ::ShaderDataType::Float4, "a_Color" },
			{1, XYZ::ShaderDataType::Float3, "a_Position" },
			{2, XYZ::ShaderDataType::Float2, "a_TexCoord" },
			{3, XYZ::ShaderDataType::Float,  "a_TextureID" },
				});
			QuadVertexArray->AddVertexBuffer(QuadVertexBuffer);


			uint32_t* quadIndices = new uint32_t[MaxIndices];
			uint32_t offset = 0;
			for (uint32_t i = 0; i < MaxIndices; i += 6)
			{
				quadIndices[i + 0] = offset + 0;
				quadIndices[i + 1] = offset + 1;
				quadIndices[i + 2] = offset + 2;

				quadIndices[i + 3] = offset + 2;
				quadIndices[i + 4] = offset + 3;
				quadIndices[i + 5] = offset + 0;

				offset += 4;
			}
			std::shared_ptr<IndexBuffer> quadIB = IndexBuffer::Create(quadIndices, MaxIndices);
			QuadVertexArray->SetIndexBuffer(quadIB);
			delete[] quadIndices;
		}
		BufferPtr = BufferBase;
		IndexCount = 0;
	}

}