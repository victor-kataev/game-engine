#include "stdafx.h"
#include "HashGrid2D.h"


namespace XYZ {
	HashGrid2D::HashGrid2D(int cellSize, int tableSize)
		: m_CellSize(cellSize), m_TableSize(tableSize)
	{
		m_Table.resize(m_TableSize);
	}
	void HashGrid2D::Insert(int element, const glm::vec2& pos, const glm::vec2& size)
	{
		for (int i = (int)pos.x; i < int(pos.x + size.x); ++i)
		{
			for (int j = (int)pos.y; j < int(pos.y + size.y); ++j)
			{
				size_t index = ((size_t)floor(i / m_CellSize) + (size_t)floor(j / m_CellSize)) % m_TableSize;
				m_Table[index].elements.push_back(element);
			}
		}

	}
	void HashGrid2D::Remove(int element, const glm::vec2& pos, const glm::vec2& size)
	{
		for (int i = (int)pos.x; i < int(pos.x + size.x); ++i)
		{
			for (int j = (int)pos.y; j < int(pos.y + size.y); ++j)
			{
				size_t index = ((size_t)floor(i / m_CellSize) + (size_t)floor(j / m_CellSize)) % m_TableSize;
				auto it = std::find(m_Table[index].elements.begin(), m_Table[index].elements.end(), element);
				if (it != m_Table[index].elements.end())
					m_Table[index].elements.erase(it);
			}
		}
	}
	size_t HashGrid2D::GetElements(int** buffer, const glm::vec2& pos, const glm::vec2& size)
	{
		std::vector<size_t> indices;
		size_t count = 0;
		for (int i = (int)pos.x; i < int(pos.x + size.x); ++i)
		{
			for (int j = (int)pos.y; j < int(pos.y + size.y); ++j)
			{
				size_t index = ((size_t)floor(i / m_CellSize) + (size_t)floor(j / m_CellSize)) % m_TableSize;
				count += m_Table[index].elements.size();
				indices.push_back(index);
			}
		}

		*buffer = new int[count * sizeof(int)];
		int* ptr = *buffer;
		for (auto it : indices)
		{
			size_t elementsSize = m_Table[it].elements.size();
			memcpy(ptr, m_Table[it].elements.data(), elementsSize * sizeof(int));
			ptr += elementsSize;
		}

		return count;
	}
}