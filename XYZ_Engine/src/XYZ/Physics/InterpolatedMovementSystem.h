#pragma once
#include "XYZ/ECS/ECSManager.h"
#include "Components.h"


namespace XYZ {
	/*! @class InterpolatedMovementSystem
	* @brief InterpolatedMovementystem takes care for interpolated movement of entities with interpolated movement component
	*/
	class InterpolatedMovementSystem : public System
	{
	public:
		InterpolatedMovementSystem();
		virtual void Update(float dt);
		virtual void Add(Entity entity) override;
		virtual void Remove(Entity entity) override;
		virtual bool Contains(Entity entity) override;

	private:
		struct Component : public System::Component
		{
			int interpolIndex;
			int gridBodyIndex;
			int transformIndex;
		};

		std::shared_ptr<ComponentStorage<InterpolatedMovement>> m_InterpolStorage;
		std::shared_ptr<ComponentStorage<GridBody>> m_GridBodyStorage;
		std::shared_ptr<ComponentStorage<Transform2D>> m_TransformStorage;
		std::vector<Component> m_Components;
	};

}