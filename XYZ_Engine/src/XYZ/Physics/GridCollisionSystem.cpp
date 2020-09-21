#include "stdafx.h"
#include "GridCollisionSystem.h"

#define OUT_OF_GRID (1 << 31)
namespace XYZ {
	GridCollisionSystem::GridCollisionSystem()
		: m_NumRows(1), m_NumCols(1), m_CellSize(1), m_PositionX(0), m_PositionY(0)
	{
		m_Signature.set(XYZ::ECSManager::Get().GetComponentType<GridBody>());
		m_Signature.set(XYZ::ECSManager::Get().GetComponentType<CollisionComponent>());

		m_GridBodyStorage = ECSManager::Get().GetComponentStorage<GridBody>();
		m_CollisionStorage = ECSManager::Get().GetComponentStorage<CollisionComponent>();
	}
	void GridCollisionSystem::ClearGrid()
	{
		for (int i = 0; i < m_NumRows; ++i)
			for (int j = 0; j < m_NumCols; ++j)
				m_Cells[i][j].mask = 0;
	}
	void GridCollisionSystem::ResizeGrid(int numRows, int numCols, int cellSize, int positionX, int positionY)
	{
		m_NumRows = numRows;
		m_NumCols = numCols;
		m_CellSize = cellSize; 
		m_PositionX = positionX; 
		m_PositionY = positionY;

		m_Cells.resize(m_NumRows);
		for (int i = 0; i < m_NumRows; ++i)
			m_Cells[i].resize(m_NumCols);
	}
	void GridCollisionSystem::MoveGrid(int positionX, int positionY)
	{
		m_PositionX = positionX;
		m_PositionY = positionY;
	}
	void GridCollisionSystem::Update(float dt)
	{
		for (auto& it : m_Components)
		{
			if (((*m_ActiveStorage)[it.activeIndex].activeComponents & m_Signature) == m_Signature)
			{
				auto mask = (*m_CollisionStorage)[it.collisionIndex].layer;
				int32_t result = Move(
					(*m_GridBodyStorage)[it.gridBodyIndex],
					(*m_CollisionStorage)[it.collisionIndex].collisionLayers,
					(*m_CollisionStorage)[it.collisionIndex].layer);

				// Store mask of layer it collides with
				(*m_CollisionStorage)[it.collisionIndex].currentCollisions = result;
				
				// No collisions , free to move
				if (!result && result != OUT_OF_GRID)
				{
					(*m_GridBodyStorage)[it.gridBodyIndex].col += (*m_GridBodyStorage)[it.gridBodyIndex].nextCol;
					(*m_GridBodyStorage)[it.gridBodyIndex].row += (*m_GridBodyStorage)[it.gridBodyIndex].nextRow;			
				}
				else
				{
					//(*m_GridBodyStorage)[it.gridBodyIndex].nextCol = 0;
					//(*m_GridBodyStorage)[it.gridBodyIndex].nextRow = 0;
				}		
			}
		}

	}
	void GridCollisionSystem::Add(Entity entity)
	{
		Component component;
		component.entity = entity;
		component.collisionIndex = ECSManager::Get().GetComponentIndex<CollisionComponent>(entity);
		component.gridBodyIndex = ECSManager::Get().GetComponentIndex<GridBody>(entity);
		component.activeIndex = ECSManager::Get().GetComponentIndex<ActiveComponent>(entity);

		auto layer = (*m_CollisionStorage)[component.collisionIndex].layer;
		auto collisionLayer = (*m_CollisionStorage)[component.collisionIndex].collisionLayers;
		if (Insert((*m_GridBodyStorage)[component.gridBodyIndex],collisionLayer, layer))
		{
			m_Components.push_back(component);
			XYZ_LOG_INFO("Entity with ID ", entity, " added");
		}
		else
			XYZ_LOG_WARN("Entity with ID ", entity, " not added");
	}
	void GridCollisionSystem::Remove(Entity entity)
	{
		auto it = std::find(m_Components.begin(), m_Components.end(), entity);
		if (it != m_Components.end())
		{
			XYZ_LOG_INFO("Entity with id ", entity, " removed");
			auto mask = (*m_CollisionStorage)[it->collisionIndex].layer;
			Remove((*m_GridBodyStorage)[it->gridBodyIndex], mask);
			*it = std::move(m_Components.back());
			m_Components.pop_back();
		}
	}
	bool GridCollisionSystem::Contains(Entity entity)
	{
		auto it = std::find(m_Components.begin(), m_Components.end(), entity);
		if (it != m_Components.end())
			return true;
		return false;
	}
	bool GridCollisionSystem::Insert(const GridBody& body, int32_t collisionMask, int32_t layerMask)
	{
		if (body.row + body.height < m_NumRows && body.col + body.width < m_NumCols
			&& body.row >= m_PositionY && body.col >= m_PositionX)
		{
			for (int i = body.row; i < body.row + body.height; ++i)
				for (int j = body.col; j < body.col + body.width; ++j)
					if (m_Cells[i][j].mask & collisionMask)
						return false;

			for (int i = body.row; i < body.row + body.height; ++i)
				for (int j = body.col; j < body.col + body.width; ++j)
					m_Cells[i][j].mask |= layerMask;


			return true;
		}
		return false;
	}
	int32_t GridCollisionSystem::Move(const GridBody& oldBody,int32_t collisionMask, int32_t layerMask)
	{
		GridBody newBody = oldBody;
		newBody.col += newBody.nextCol;
		newBody.row += newBody.nextRow;

		if (newBody.row + newBody.height < m_NumRows && newBody.col + newBody.width < m_NumCols
			&& newBody.row >= m_PositionY && newBody.col >= m_PositionX)
		{
			// No changes to the body
			if (oldBody == newBody)
				return 0;

			if (newBody.col < oldBody.col)
			{
				// Handle diagonal movement
				if (newBody.row != oldBody.row)
				{
					if (m_Cells[oldBody.row][oldBody.col - 1].mask & collisionMask)
					{
						return m_Cells[oldBody.row][oldBody.col - 1].mask & collisionMask;
					}
					else if (m_Cells[newBody.row][newBody.col + newBody.width].mask & collisionMask)
					{
						return m_Cells[newBody.row][newBody.col + newBody.width].mask & collisionMask;
					}
				}
				else
				{
					// Check all cells to the top of the body
					for (int i = newBody.row; i < newBody.row + newBody.height; ++i)
						if (m_Cells[i][newBody.col].mask & collisionMask)
							return m_Cells[i][newBody.col].mask & collisionMask;
				}
			}
			else if (newBody.col > oldBody.col)
			{
				// Handle diagonal movement
				if (newBody.row != oldBody.row)
				{
					if (m_Cells[oldBody.row][oldBody.col + oldBody.width].mask & collisionMask)
					{
						return m_Cells[oldBody.row][oldBody.col + oldBody.width].mask & collisionMask;
					}
					else if (m_Cells[newBody.row][newBody.col - 1].mask & collisionMask)
					{
						return m_Cells[newBody.row][newBody.col - 1].mask & collisionMask;
					}
				}
				else
				{
					// Check all cells to the top of the body and one cell in the width of the new body
					for (int i = newBody.row; i < newBody.row + newBody.height; ++i)
						if (m_Cells[i][newBody.col + newBody.width - 1].mask & collisionMask)
							return m_Cells[i][newBody.col + newBody.width - 1].mask & collisionMask;
				}
			}
			else if (newBody.row < oldBody.row)
			{
				// Check all cells to the width of the body
				for (int i = newBody.col; i < newBody.col + newBody.width; ++i)
					if (m_Cells[newBody.row][i].mask & collisionMask)
						return m_Cells[newBody.row][i].mask & collisionMask;
			}
			else
			{
				// Check all cells to the width of the body and one to the top
				for (int i = newBody.col; i < newBody.col + newBody.width; ++i)
					if (m_Cells[newBody.row + newBody.height - 1][i].mask & collisionMask)
						return m_Cells[newBody.row + newBody.height - 1][i].mask & collisionMask;
			}

			// Clear all the cells occupied by the oldBody
			for (int i = oldBody.row; i < oldBody.row + oldBody.height; ++i)
				for (int j = oldBody.col; j < oldBody.col + oldBody.width; ++j)
					m_Cells[i][j].mask &= (~layerMask);

			// Set all the cells occupied by the newBody
			for (int i = newBody.row; i < newBody.row + newBody.height; ++i)
				for (int j = newBody.col; j < newBody.col + newBody.width; ++j)
					m_Cells[i][j].mask |= layerMask;


			// Movement was successful return empty collision mask
			return 0;
		}
		return 1 << 31;
	}
	void GridCollisionSystem::Remove(const GridBody& body, int32_t layerMask)
	{
		if (body.row + body.height < m_NumRows && body.col + body.width < m_NumCols
			&& body.row >= m_PositionY && body.col >= m_PositionX)
		{
			// Clear all cells occupied by the body
			for (int i = body.row; i < body.row + body.height; ++i)
				for (int j = body.col; j < body.col + body.width; ++j)
					m_Cells[i][j].mask &= (~layerMask);
		}
	}
}