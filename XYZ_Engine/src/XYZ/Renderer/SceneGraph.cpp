#include "stdafx.h"
#include "SceneGraph.h"

namespace XYZ {
	SceneGraph::SceneGraph()
	{
		m_TransformStorage = ECSManager::Get().GetComponentStorage<Transform2D>();
		m_RelationStorage = ECSManager::Get().GetComponentStorage<Relationship>();
	}

	void SceneGraph::Insert(Entity entity)
	{
		Component component;
		component.entity = entity;
		component.activeIndex = ECSManager::Get().GetComponentIndex<ActiveComponent>(entity);
		component.transformIndex = ECSManager::Get().GetComponentIndex<Transform2D>(entity);
		component.relationIndex = ECSManager::Get().GetComponentIndex<Relationship>(entity);

		m_Components.push_back(component);
	}

	void SceneGraph::Remove(Entity entity)
	{
		auto activeComponent = ECSManager::Get().GetComponent<ActiveComponent>(entity);
		activeComponent->activeComponents = 0;
	}

	void SceneGraph::AttachChild(Entity parent, Entity children)
	{
		if (!ECSManager::Get().Contains<Relationship>(parent))
			ECSManager::Get().AddComponent(parent, Relationship());
		
		if (!ECSManager::Get().Contains<Relationship>(children))
			ECSManager::Get().AddComponent(children, Relationship());

		auto parentRelation = ECSManager::Get().GetComponent<Relationship>(parent);
		auto childrenRelation = ECSManager::Get().GetComponent<Relationship>(children);

		Entity next = parentRelation->next;
		Entity previous = parent;
		while (next != NULL_ENTITY)
		{
			previous = next;
			next = ECSManager::Get().GetComponent<Relationship>(next)->next;
		}

		parentRelation->numChildren++;
		ECSManager::Get().GetComponent<Relationship>(previous)->next = children;
		ECSManager::Get().GetComponent<Relationship>(previous)->previous = previous;
		

		Component component;
		component.entity = children;
		component.activeIndex = ECSManager::Get().GetComponentIndex<ActiveComponent>(children);
		component.transformIndex = ECSManager::Get().GetComponentIndex<Transform2D>(children);
		component.relationIndex = ECSManager::Get().GetComponentIndex<Relationship>(children);

		for (size_t i = 0; i < m_Components.size(); ++i)
		{
			if (m_Components[i].relationIndex != -1
			 && m_Components[i].entity == parent)
			{		
				m_Components.insert(m_Components.begin() + (i + 1), component);
				break;		
			}
		}
	}

	void SceneGraph::DetachChild(Entity parent, Entity children)
	{
		XYZ_ASSERT(ECSManager::Get().Contains<Relationship>(parent), "parent does not have relationship component");
		XYZ_ASSERT(ECSManager::Get().Contains<Relationship>(children), "child does not have relationship component");

		auto parentRelation = ECSManager::Get().GetComponent<Relationship>(parent);
		auto childrenRelation = ECSManager::Get().GetComponent<Relationship>(children);

		Entity next = parentRelation->next;
		Entity previous = parent;
		while (next != NULL_ENTITY || next != children)
		{
			previous = next;
			next = ECSManager::Get().GetComponent<Relationship>(next)->next;
		}
		if (next == NULL_ENTITY)
			ECSManager::Get().GetComponent<Relationship>(previous)->next = NULL_ENTITY;
		else
			ECSManager::Get().GetComponent<Relationship>(previous)->next = ECSManager::Get().GetComponent<Relationship>(next)->next;
	}

	void SceneGraph::Print()
	{
		for (auto it : m_Components)
			std::cout << it.entity << std::endl;
	}


}