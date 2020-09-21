#pragma once
#include "XYZ/ECS/ECSManager.h"
#include "Components.h"

#include "XYZ/Utils/DataStructures/FreeList.h"

namespace XYZ {
	/*! @class RealGridCollisionSystem
	*	@brief Real grid collisions , allows free movement of objects
	*/
	class RealGridCollisionSystem : public System
	{
	public:
		RealGridCollisionSystem();
		~RealGridCollisionSystem();

		void CreateGrid(int width, int height, int cellSize);
		virtual void Update(float dt);
		virtual void Add(Entity entity) override;
		virtual void Remove(Entity entity) override;
		virtual bool Contains(Entity entity) override;


	private:
		struct Component : public System::Component
		{
			int realGridBody;
			int rigidBody;
		};
	
		struct Cell
		{
			// Change to raw array on heap int*
			std::vector<int> indices;
		};

		int m_NumRows;
		int m_NumCols;
		int m_CellSize;

		vector2D<Cell> m_Cells;
		FreeList<Component> m_Components;
		
		std::shared_ptr<ComponentStorage<RealGridBody>> m_RealGridStorage;
		std::shared_ptr<ComponentStorage<RigidBody2D>> m_RigidStorage;

	};
}