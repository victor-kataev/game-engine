#include "stdafx.h"
#include "SpriteSystem.h"


namespace XYZ {
	SpriteSystem::SpriteSystem()
	{
		m_Signature.set(ECSManager::Get().GetComponentType<SpriteAnimation>());
		m_Signature.set(ECSManager::Get().GetComponentType<Renderable2D>());

		m_SpriteStorage = ECSManager::Get().GetComponentStorage<SpriteAnimation>();
		m_RenderableStorage = ECSManager::Get().GetComponentStorage<Renderable2D>();
	}
	void SpriteSystem::Update(float dt)
	{
		for (auto it : m_Components)
		{
			(*m_SpriteStorage)[it.animIndex].Update(dt, &(*m_RenderableStorage)[it.rendIndex]);
		}
	}
	void SpriteSystem::Add(Entity entity)
	{
		XYZ_LOG_INFO("Entity with ID ", entity, " added");
	
		Component component;
		component.animIndex = ECSManager::Get().GetComponentIndex<SpriteAnimation>(entity);
		component.rendIndex = ECSManager::Get().GetComponentIndex<Renderable2D>(entity);

		component.entity = entity;

		m_Components.push_back(component);
	}
	void SpriteSystem::Remove(Entity entity)
	{
		auto it = std::find(m_Components.begin(), m_Components.end(), entity);
		if (it != m_Components.end())
		{
			XYZ_LOG_INFO("Entity with id ", entity, " removed");
			*it = std::move(m_Components.back());
			m_Components.pop_back();
		}
	}
	bool SpriteSystem::Contains(Entity entity)
	{
		auto it = std::find(m_Components.begin(), m_Components.end(), entity);
		if (it != m_Components.end())
			return true;
		return false;
	}
}