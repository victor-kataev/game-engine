#pragma once
#include "Types.h"
#include "XYZ/Utils/DataStructures/FreeList.h"

#include <unordered_map>
#include <array>
#include <assert.h>

namespace XYZ {
	class ComponentManager;
	/*! @class IComponentStorage
	* @brief interface of storage for components
	*/
	class IComponentStorage
	{
		friend class ComponentManager;
	public:
		/**
		* virtual destructor
		*/
		virtual ~IComponentStorage() = default;
		virtual void EntityDestroyed(Entity entity) = 0;

	};

	/*! @class ComponentStorage
	* @brief storage for new type of components
	*/
	template<typename T>
	class ComponentStorage : public IComponentStorage
	{
	public:
		virtual ~ComponentStorage()
		{}

		/**
		* Check if contains entity
		* @param[in] entity
		* @return
		*/
		bool Contains(Entity entity)
		{
			return m_Lookup.find(entity) != m_Lookup.end();
		}

		/**
		* Add new component to entity
		* @param[in] entity
		* @tparam[in] component
		*/
		void AddComponent(Entity entity,const T& component)
		{
			XYZ_ASSERT(m_Lookup.find(entity) == m_Lookup.end(), "Entity ",entity," already contains component");
			m_Lookup[entity] = m_Components.Insert(component);
		}

		/**
		* Remove component from the entity
		* @param[in] entity
		*/
		void RemoveComponent(Entity entity)
		{
			XYZ_ASSERT(m_Lookup.find(entity) != m_Lookup.end(), "Removing non-existent component");
			int removeIndex = m_Lookup[entity];	
			m_Components.Erase(removeIndex);
			m_Lookup.erase(entity);
		}

		/**
		* @param[in] entity
		* @return index of component in the storage
		*/
		int GetComponentIndex(Entity entity)
		{
			XYZ_ASSERT(m_Lookup.find(entity) != m_Lookup.end(), "Retrieving non-existent component.");
			return m_Lookup[entity];
		}

		/**
		* @param[in] entity
		* @return pointer to the component
		*/
		T* GetComponent(Entity entity)
		{
			XYZ_ASSERT(m_Lookup.find(entity) != m_Lookup.end(), "Retrieving non-existent component.");
			return &m_Components[m_Lookup[entity]];
		}

		/**
		* Remove entity from storage if entity is destroyed
		* @param[in] entity
		*/
		virtual void EntityDestroyed(Entity entity) override
		{
			if (Contains(entity))
				RemoveComponent(entity);
		}


		T& operator [](int index)
		{
			XYZ_ASSERT(index < m_Components.Range() && index >= 0, "Index out of range");
			return m_Components[index];
		}


	private:
		FreeList<T> m_Components;
		std::unordered_map<Entity, int> m_Lookup;
	};

}