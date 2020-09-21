#pragma once
#include "XYZ/ECS/ECSManager.h"
#include "XYZ/Physics/Components.h"
#include "Renderable2D.h"
#include "SortingLayer.h"


#include <queue>

namespace XYZ {
	

	/**
	* @class RendererBatchSystem2D
	* @brief represents system , that groups ( batches ) renderables dependent on the material and z distance from the camera
	*/
	class RendererBatchSystem2D : public System
	{
	public:
		/**
		* Construct a batch system
		*/
		RendererBatchSystem2D();

		/**
		* Add entity to the Opaque/Transparent group dependent on the sort key in the material of the entity
		* @param[in] enity 
		*/
		virtual void Add(Entity entity) override;

		/**
		* Remove entity
		* @param[in] entity
		*/
		virtual void Remove(Entity entity) override;

		/**
		* Check if system contains entity
		* @param[in] entity
		*/
		virtual bool Contains(Entity entity) override;

		/**
		* If entity is updated, if the sort key of the entity has changed, remove it from the current group,
		* and insert it to the new group dependent on the key
		* @param[in] entity
		*/
		virtual void EntityUpdated(Entity entity) override;

		/**
		* Build commands and submit them to the Renderer2D
		*/
		void SubmitToRenderer();
	


	private:
	

		struct Component : public System::Component
		{
			int renderableIndex;
			int transformIndex;
		};

		std::vector<Component> m_OpaqueComponents;
		std::vector<Component> m_TransparentComponents;


		std::shared_ptr<ComponentStorage<Renderable2D>> m_RenderableStorage;
		std::shared_ptr<ComponentStorage<Transform2D>> m_TransformStorage;

		struct OpaqueComparator
		{
			OpaqueComparator()
				:
				m_TransformStorage(ECSManager::Get().GetComponentStorage<Transform2D>()),
				m_RenderableStorage(ECSManager::Get().GetComponentStorage<Renderable2D>())
			{
			}
			std::shared_ptr<ComponentStorage<Transform2D>> m_TransformStorage;
			std::shared_ptr<ComponentStorage<Renderable2D>> m_RenderableStorage;

			bool operator()(const Component& a, const Component& b) const
			{
				auto& transformA = (*m_TransformStorage)[a.transformIndex];
				auto& transformB = (*m_TransformStorage)[b.transformIndex];
				auto& renderableA = (*m_RenderableStorage)[a.renderableIndex];
				auto& renderableB = (*m_RenderableStorage)[b.renderableIndex];

				int sortLayerA = renderableA.sortLayerID;
				int sortLayerB = renderableB.sortLayerID;

				float valueA = (float)SortingLayer::Get().GetOrderValueByID(sortLayerA) + transformA.position.z;
				float valueB = (float)SortingLayer::Get().GetOrderValueByID(sortLayerB) + transformB.position.z;


				if (renderableA.material->GetSortKey() == renderableB.material->GetSortKey())
				{
					return valueA < valueB;
				}
				return renderableA.material->GetSortKey() < renderableB.material->GetSortKey();		
			}
		};

		struct TransparentComparator
		{
			TransparentComparator()
				:
				m_TransformStorage(ECSManager::Get().GetComponentStorage<Transform2D>()),
				m_RenderableStorage(ECSManager::Get().GetComponentStorage<Renderable2D>())
			{
			}
			std::shared_ptr<ComponentStorage<Transform2D>> m_TransformStorage;
			std::shared_ptr<ComponentStorage<Renderable2D>> m_RenderableStorage;

			bool operator()(const Component& a, const Component& b) const
			{
				auto& transformA = (*m_TransformStorage)[a.transformIndex];
				auto& transformB = (*m_TransformStorage)[b.transformIndex];
				auto& renderableA = (*m_RenderableStorage)[a.renderableIndex];
				auto& renderableB = (*m_RenderableStorage)[b.renderableIndex];

				int sortLayerA = renderableA.sortLayerID;
				int sortLayerB = renderableB.sortLayerID;

				float valueA = (float)SortingLayer::Get().GetOrderValueByID(sortLayerA) + transformA.position.z;
				float valueB = (float)SortingLayer::Get().GetOrderValueByID(sortLayerB) + transformB.position.z;

				if (fabs(valueA - valueB) <= std::numeric_limits<float>::epsilon())
				{
					return renderableA.material->GetSortKey() < renderableB.material->GetSortKey();
				}
				return valueA > valueB;
			}
		};

	private:	

		// Maybe useless
		template <typename Comparator>
		void InsertionSort(std::vector<Component>& vec)
		{
			Comparator cmp;
			for (int i = 1; i < vec.size(); ++i)
			{
				auto el = vec[i];
				int j = i - 1;

				while (j >= 0 && cmp(el, vec[j]))
				{
					vec[j + 1] = vec[j];
					j = j - 1;
				}
				vec[j + 1] = el;
			}
		}
	};
}