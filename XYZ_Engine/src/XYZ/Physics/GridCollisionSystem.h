#pragma once
#include "XYZ/ECS/ECSManager.h"
#include "Components.h"


namespace XYZ {

	/*! @class GridCollisionSystem
	*	@brief Implements and handles grid collisions, objects must be entirely in cells
	*/
	class GridCollisionSystem : public System
	{
	public:
		GridCollisionSystem();

		/**
		* Clear all cells in the grid
		*/
		void ClearGrid();

		/** Resizes the grid to the set dimensions and moves the grid */
		void ResizeGrid(int numRows, int numCols, int cellSize, int positionX, int positionY);

		/** Moves the grid to the set position */
		void MoveGrid(int positionX, int positionY);

		
		int GetCellSize() const { return m_CellSize; }

		virtual void Update(float dt);
		virtual void Add(Entity entity) override;
		virtual void Remove(Entity entity) override;
		virtual bool Contains(Entity entity) override;
	
	private:
		bool Insert(const GridBody& body, int32_t collisionMask = 1, int32_t layerMask = 1);
		int32_t Move(const GridBody& oldBody,int32_t collisionMask = 1, int32_t layerMask = 1);
		void Remove(const GridBody& body, int32_t layerMask = 1);
		

	private:
		/** Grid cell */
		struct Cell
		{
			int32_t mask = 0;
		};

		int m_PositionX;
		int m_PositionY;
		int m_NumRows;
		int m_NumCols;
		int m_CellSize;

		vector2D<Cell> m_Cells;
	
	
		struct Component : public System::Component
		{
			int gridBodyIndex;
			int collisionIndex;
		};
		std::vector<Component> m_Components;
		std::shared_ptr<ComponentStorage<GridBody>> m_GridBodyStorage;
		std::shared_ptr<ComponentStorage<CollisionComponent>> m_CollisionStorage;
	};

}