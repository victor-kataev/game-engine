#pragma once

#include <glm/glm.hpp>

namespace XYZ {
/*! @class HashGrid2D
*	@brief 2D HashGrid for storing elements
*/
	class HashGrid2D
	{
	public:
		HashGrid2D(int cellSize, int tableSize);
		
		/** Inserts an element into the hashgrid */
		void Insert(int element, const glm::vec2& pos, const glm::vec2& size);

		/** Removes an element from the hashgrid */
		void Remove(int element, const glm::vec2& pos, const glm::vec2& size);
		
		/** Returns the element count */
		size_t GetElements(int** buffer, const glm::vec2& pos, const glm::vec2& size);

	private:
		struct Cell
		{
			std::vector<int> elements;
		};


		int m_CellSize;
		int m_TableSize;

		std::vector<Cell> m_Table;
	};
}