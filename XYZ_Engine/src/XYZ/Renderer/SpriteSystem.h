#pragma once
#include "XYZ/ECS/ECSManager.h"
#include "SpriteAnimation.h"

namespace XYZ {

	/**
	* @class SpriteSystem
	* @brief Updates entities with components Renderable2D and SpriteAnimation
	*/
	class SpriteSystem : public System
	{
	public:
		/**
		* Construct a sprite system, set signature
		*/
		SpriteSystem();

		/**
		* Update stored animations
		* @param dt		delta time
		*/
		virtual void Update(float dt);

		/**
		* Add entity to the vector of components
		* @param[in] entity
		*/
		virtual void Add(Entity entity) override;
		/**
		* Remove entity from the vector of components
		*/
		virtual void Remove(Entity entity) override;
		/**
		* @return true if the vector of components contains entity
		*/
		virtual bool Contains(Entity entity) override;

	private:
		struct Component : public System::Component
		{
			int animIndex;
			int rendIndex;
			int interIndex;
		};

		std::vector<Component> m_Components;
		std::shared_ptr<ComponentStorage<SpriteAnimation>> m_SpriteStorage;
		std::shared_ptr<ComponentStorage<Renderable2D>> m_RenderableStorage;
	};
}