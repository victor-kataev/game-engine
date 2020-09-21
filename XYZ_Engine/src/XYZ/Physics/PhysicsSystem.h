#pragma once
#include "XYZ/ECS/ECSManager.h"
#include "Components.h"

#include "XYZ/Renderer/Renderable2D.h"


namespace XYZ {
	/**
	* @class PhysicsSytem
	* @brief Represents system for handling physics
	*/
	class PhysicsSystem : public System
	{
	public:
		/**
		* Construct
		*/
		PhysicsSystem();
		~PhysicsSystem();

		virtual void Update(float dt);
		virtual void Add(Entity entity) override;
		virtual void Remove(Entity entity) override;
		virtual bool Contains(Entity entity) override;
	
	private:
		struct Component : public System::Component
		{
			int bodyIndex;
			int interpolIndex;
		};


	private:
		std::vector<Component> m_Components;
		std::shared_ptr<ComponentStorage<RigidBody2D>> m_BodyStorage;
		std::shared_ptr<ComponentStorage<InterpolatedMovement>> m_InterpolStorage;

	};

}