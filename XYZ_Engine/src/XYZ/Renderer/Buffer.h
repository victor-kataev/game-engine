#pragma once
#include <memory>

namespace XYZ {

	/**
	* Represents data types in shader program
	*/
	enum class ShaderDataType
	{
		None = 0, Float, Float2, Float3, Float4, Mat3, Mat4, Int, Int2, Int3, Int4, Bool
	};

	/**
	* Return size of specified shader data type
	* @param[in] type   ShaderDataType
	* @return a size of the type
	*/
	static unsigned int ShaderDataTypeSize(ShaderDataType type)
	{
		switch (type)
		{
		case ShaderDataType::Float:		return 4;
		case ShaderDataType::Float2:	return 4 * 2;
		case ShaderDataType::Float3:	return 4 * 3;
		case ShaderDataType::Float4:	return 4 * 4;
		case ShaderDataType::Mat3:		return 4 * 3 * 3;
		case ShaderDataType::Mat4:		return 4 * 4 * 4;
		case ShaderDataType::Int:		return 4;
		case ShaderDataType::Int2:		return 4 * 2;
		case ShaderDataType::Int3:		return 4 * 3;
		case ShaderDataType::Int4:		return 4 * 4;
		case ShaderDataType::Bool:		return 1;
		}
		XYZ_ASSERT(false, "Buffer: Unknown ShaderDataType");
		return 0;
	}

	/**
	* @struct BufferElement
	* Store information about buffer element
	* 
	* Each element contains information about it's size in vertex buffer.
	* The vertex buffers can store only raw data, the buffer element let us use
	* custom ShaderDataType values in the vertex buffers.
	*/
	struct BufferElement
	{
		/**
		* Constructor
		* @param[in] index		Index of element in buffer
		* @param[in] type		Shader data type
		* @param[in] name		Name of element represented in shader
		* @param[in] divisior	Specify how is data split between instances, default 0
		*/
		BufferElement(unsigned int index, ShaderDataType type, const std::string& name, unsigned int divisor = 0)
			: Index(index), Type(type), Divisor(divisor), Size(ShaderDataTypeSize(type)), Offset(0)
		{}

		/**
		* Split ShaderDataType to four byte values and return count
		* @return a count of four byte values after split
		*/
		uint32_t GetComponentCount() const
		{
			switch (Type)
			{
			case ShaderDataType::Bool:   return 1;
			case ShaderDataType::Float:  return 1;
			case ShaderDataType::Float2: return 2;
			case ShaderDataType::Float3: return 3;
			case ShaderDataType::Float4: return 4;
			case ShaderDataType::Int:    return 1;
			case ShaderDataType::Int2:   return 2;
			case ShaderDataType::Int3:   return 3;
			case ShaderDataType::Int4:   return 4;
			case ShaderDataType::Mat3:   return 9;
			case ShaderDataType::Mat4:   return 16;
			}
			XYZ_ASSERT(false, "ShaderDataTypeSize(ShaderDataType::None)");
			return 0;
		}

		ShaderDataType Type;
		unsigned int   Size;
		unsigned int   Offset;
		unsigned int   Index;
		unsigned int   Divisor;
	};


	/**
	* @class BufferLayout
	* Represents layout of data in buffer. Consists of multiple BufferElements, stored in vector,
	* let us structure data in the vertex buffer.
	*/
	class BufferLayout
	{
	public:
		/**
		* default Constructor
		*/
		BufferLayout() {};

		/**
		* Takes initializer_list of BufferElements.
		* Generates special Mat4 buffer elements, calcultes offsets and strides in elements
		* @param[in] elements initializer_list of BufferElements 
		*/
		BufferLayout(const std::initializer_list<BufferElement>& elements)
			: m_Elements(elements)
		{
			CreateMat4();
			CalculateOffsetsAndStride();
		}

		/**
		* @return a size of the BufferElements in bytes
		*/
		inline const uint32_t& GetStride() const { return m_Stride; }

		/**
		* @return a vector of stored BufferElements
		*/
		inline const std::vector<BufferElement>& GetElements() const { return m_Elements; }

		auto begin() { return m_Elements.begin(); }
		auto end() { return m_Elements.end(); }
		auto begin() const { return m_Elements.begin(); }
		auto end() const { return m_Elements.end(); }
	private:

		/**
		* Calculate offsets and strides
		*
		* Calculate offset in the vertex buffer for each buffer element,
		* and size of the BufferElements in bytes
		*/
		inline void CalculateOffsetsAndStride()
		{
			unsigned int offset = 0;
			m_Stride = 0;
			for (auto& element : m_Elements)
			{
				element.Offset = offset;
				offset += element.Size;
				m_Stride += element.Size;
			}
		};

		/**
		* If element Type equals ShaderDataType::Mat4 ,it must create three additional elements,
		* and set for the element and the three additional elements ShaderDataType::Float4.
		*/
		inline void CreateMat4()
		{
			for (auto& element : m_Elements)
			{
				if (element.Type == ShaderDataType::Mat4)
				{
					element.Type = ShaderDataType::Float4;
					element.Size = 4 * 4;

					BufferElement tmpElement = element;
					m_Elements.push_back(BufferElement(tmpElement.Index + 1, tmpElement.Type, "", tmpElement.Divisor));
					m_Elements.push_back(BufferElement(tmpElement.Index + 2, tmpElement.Type, "", tmpElement.Divisor));
					m_Elements.push_back(BufferElement(tmpElement.Index + 3, tmpElement.Type, "", tmpElement.Divisor));
				}
			}
		}

	private:
		std::vector<BufferElement> m_Elements;
		unsigned int m_Stride = 0;
	};

	/**
	* Represents usage of the vertex buffer
	* If Static, the data is not expected to be updated,
	* if Dynamic, the data is expected to be updated
	*/
	enum class BufferUsage
	{
		None = 0, Static = 1, Dynamic = 2
	};

	/**
	* @interface VertexBuffer
	* pure virtual (interface) class.
	* Storage of data for rendering. Send the data to the GPU for further processing. Vertices are rendered by shader program.
	* VertexArray stores VertexBuffers, must have BufferLayout set before being stored.
	*/
	class VertexBuffer
	{
	public:
		virtual ~VertexBuffer() = default;
		virtual void Bind() const = 0;
		virtual void UnBind() const = 0;
		virtual void Update(void* vertices, uint32_t size, uint32_t offset = 0) = 0;
		virtual void Resize(float* vertices, uint32_t size) = 0;
		virtual void SetLayout(const BufferLayout& layout) = 0;
		virtual const BufferLayout& GetLayout() const = 0;
		
		/**
		* Create empty VertexBuffer, Buffer usage is Dynamic
		* @param size	Size of the buffer in bytes
		* @return shared_ptr to VertexBuffer
		*/
		static std::shared_ptr<VertexBuffer> Create(uint32_t size);

		/**
		* Create VertexBuffer
		* @param[in] vertices  Pointer to the vertices
		* @param[in] size	Size of the buffer in bytes
		* @param[in] usage  Data in the buffer will be static or dynamic , default Static
		* @return shared_ptr to VertexBuffer
		*/
		static std::shared_ptr<VertexBuffer> Create(float* vertices, uint32_t size, BufferUsage usage = BufferUsage::Static);
	};

	/**
	* @interface IndexBuffer
	* pure virtual (interface) class.
	* Storage of the indices, send them to the GPU for further processing. 
	* The GPU use stored indices for indexing the vertices in the vertex buffer
	*/
	class IndexBuffer
	{
	public:
		virtual ~IndexBuffer() = default;
		virtual void Bind() const = 0;
		virtual void UnBind() const = 0;

		virtual unsigned int GetCount() const = 0;

		/**
		* Create IndexBuffer
		* @param[in] indices	Pointer to the indices
		* @param[in] count		Count of the indices
		* @return shared_ptr to IndexBuffer
		*/
		static std::shared_ptr<IndexBuffer> Create(uint32_t* indices, unsigned int count);
	};


	/**
	* @interface ShaderStorageBuffer
	* pure virtual (interface) class.
	* Storage of the data, can be processed by compute shaders.
	*/
	class ShaderStorageBuffer
	{
	public:
		virtual ~ShaderStorageBuffer() = default;

		virtual void BindRange(uint32_t offset, uint32_t size, uint32_t index) = 0;
		virtual void Bind() = 0;
		
		virtual void Update(const void* vertices, uint32_t size, uint32_t offset = 0) = 0;
		virtual void Resize(const void* vertices, uint32_t size) = 0;
		virtual void GetSubData(void* buffer, uint32_t size, uint32_t offset = 0) = 0;
		virtual void SetLayout(const BufferLayout& layout) = 0;
		virtual const BufferLayout& GetLayout() const = 0;

		/**
		* Create empty ShaderStorageBuffer, Buffer usage is Dynamic
		* @param[in] size	Size of the buffer in bytes
		* @return shared_ptr to ShaderStorageBuffer
		*/
		static std::shared_ptr<ShaderStorageBuffer> Create(uint32_t size);


		/**
		* Create ShaderStorageBuffer
		* @param[in] vertices  Pointer to the vertices
		* @param[in] size	Size of the buffer in bytes
		* @param[in] usage  Data in the buffer will be static or dynamic , default Dynamic
		* @return shared_ptr to ShaderStoageBuffer
		*/
		static std::shared_ptr<ShaderStorageBuffer> Create(float* vertices, uint32_t size, BufferUsage usage = BufferUsage::Dynamic);
	};

}