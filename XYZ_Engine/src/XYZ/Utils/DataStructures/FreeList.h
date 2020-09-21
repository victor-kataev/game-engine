#pragma once

#include <algorithm>
#include <utility>

/* Copied from
https://stackoverflow.com/questions/41946007/efficient-and-well-explained-implementation-of-a-quadtree-for-2d-collision-det
*/

namespace XYZ {

	/// Provides an indexed free list with constant-time removals from anywhere
	/// in the list without invalidating indices. T must be trivially constructible 
	/// and destructible.
	template <typename T>
	class FreeList
	{
	public:
		/// Creates a new free list.
		FreeList()
			: m_FirstFree(-1)
		{}

		FreeList(const FreeList<T>& other)
		{
			m_FirstFree = other.m_FirstFree;
			m_Data = other.m_Data;
		}
		
		FreeList<T>& operator=(const FreeList<T>& other)
		{
			m_FirstFree = other.m_FirstFree;
			m_Data = other.m_Data;
			return *this;
		}
		
		/// Inserts an element to the free list and returns an index to it.
		int Insert(const T& elem)
		{
			if (m_FirstFree != -1)
			{
				int index = m_FirstFree;
				m_FirstFree = m_Data[m_FirstFree].next;
				m_Data[index].element = elem;
				return index;
			}
			else
			{
				m_Data.emplace_back(elem);
				return static_cast<int>(m_Data.size() - 1);
			}
		}


		/// Erases the nth element
		void Erase(int index)
		{
			m_Data[index].next = m_FirstFree;
			m_FirstFree = index;
		}
		
		/// Shrinks the list to the given size
		void Shrink(int size)
		{
			if (size <= m_FirstFree)
				m_FirstFree = -1;
			
			m_Data.resize(static_cast<size_t>(size));
		}

		/// Removes all elements from the free list.
		void Clear()
		{
			m_Data.clear();
			m_FirstFree = -1;
		}

		/// Returns the range of valid indices.
		int Range() const
		{
			return static_cast<int>(m_Data.size());
		}

		/// Returns the nth element.
		T& operator[](int index)
		{
			return m_Data[index].element;
		}

		/// Returns the nth element.
		const T& operator[](int index) const
		{
			return m_Data[index].element;
		}

	private:
		union FreeElement
		{
			FreeElement()
			{
				memset(this, 0, sizeof(FreeElement));
			}
			FreeElement(const T& el)
				: element(el)
			{}
			FreeElement(const FreeElement& other)
				: element(other.element)
			{}
			~FreeElement()
			{}

			T element;
			int next;
		};
		std::vector<FreeElement> m_Data;
		int m_FirstFree;
	};
}