#pragma once
#include "Shader.h"
#include "Texture.h"
#include "RenderFlags.h"

#include <queue>
#include <bitset>
#include <unordered_set>

namespace XYZ {

	class Material
	{
		friend class MaterialInstance;
	public:
		/**
		* Constructor ,allocates memory for the size of the shader uniforms and add reload callback to the shader,
		* and stores shared_ptr to the Shader
		* @param[in] shader 
		*/
		Material(const std::shared_ptr<Shader>& shader);

		/**
		* Destructor, removes itself from MaterialManager,
		* delete storage for shader uniforms
		*/
		~Material();


		/**
		* Set the value in the buffer corresponding to the shader uniform, update value for it's instances
		* @param[in] name	Name of the uniform
		* @arg[in] val		Value for the uniform
		*/
		template<typename T>
		void Set(const std::string& name, const T& val)
		{
			auto uni = m_Shader->FindUniform(name);
			XYZ_ASSERT(uni, "Material uniform does not exist ", name.c_str());
			XYZ_ASSERT(uni->offset + uni->size <= m_Shader->GetUniformSize(), "Material uniform buffer out of range");
			memcpy(m_Buffer + uni->offset, (unsigned char*)& val, uni->size);

			for (auto& it : m_MaterialInstances)
				it->UpdateMaterialValue(uni);
		}

		/**
		* Set the array in the buffer corresponding to the shader array uniform, update value for it's instances
		* @param[in] name	Name of the uniform
		* @arg[in] val		Value for the uniform
		* @param[in] size	Size of the val
		* @param[in] offset Offset in the shader array uniform
		*/
		template<typename T>
		void Set(const std::string& name, const T& val, int size, int offset)
		{
			auto uni = m_Shader->FindUniform(name);
			XYZ_ASSERT(uni, "Material uniform does not exist ", name.c_str());
			XYZ_ASSERT(uni->offset + uni->size <= m_Shader->GetUniformSize(), "Material uniform buffer out of range");
			XYZ_ASSERT(size + offset < uni->size, "Material uniform out of range");
			memcpy(m_Buffer + uni->offset + offset, (unsigned char*)& val, size);

			for (auto& it : m_MaterialInstances)
				it->UpdateMaterialValue(uni);
		}

		/**
		* Set the texture corresponding to the shader texture
		* @param[in] name		Name of the texture
		* @param[in] texture	shared_ptr to the Texture2D
		* @param[in] index		Binding index of the texture
		*/
		void Set(const std::string& name, std::shared_ptr<Texture2D> texture, uint32_t index = 0)
		{
			auto tex = m_Shader->FindTexture(name);
			XYZ_ASSERT(tex, "Material texture does not exist ", name.c_str());

			if ((int)m_Textures.size() <= tex->slot + index)
				m_Textures.resize((size_t)tex->slot + 1 + index);


			m_Textures[size_t(tex->slot) + size_t(index)] = texture;
		}

		/**
		* Set routine in the shader
		* @param[in] name	Name of the routine
		*/
		void SetRoutine(const std::string& name)
		{
			m_Shader->SetSubRoutine(name);
		}

		/**
		* Set material sorting key to the render flags
		* @param[in] renderFlags	
		*/
		void SetFlags(RenderFlags renderFlags)
		{
			m_Key |= renderFlags;
		}
		
		/**
		* Bind the shader of the material, set the uniforms in the shader, bind textures
		*/
		void Bind();

		/**
		* @return sort key
		*/
		const int64_t GetSortKey() const { return m_Key; }


		//TODO TEMPORARY
		/**
		* @return shared_ptr to the Shader
		*/
		std::shared_ptr<Shader> GetShader() { return m_Shader; }
		//////////////////////


		/**
		* @param[in] shader
		* @return shared_ptr to the Material
		*/
		static std::shared_ptr<Material> Create(const std::shared_ptr<Shader>& shader);

		//TODO TEMPORARY
		void ReloadShader() { m_Shader->Reload(); };
	private:
		/**
		* It is called when shader is reloaded.
		* Reallocate storage for uniforms and call OnShaderReload for MaterialInstances
		*/
		void OnShaderReload();

	private:
		std::shared_ptr<Shader>	   m_Shader;
		std::unordered_set<MaterialInstance*> m_MaterialInstances;
		std::vector<std::shared_ptr<Texture>> m_Textures;

		unsigned char* m_Buffer;
		int64_t m_Key = 0;
	};


	class MaterialInstance
	{
		friend class Material;
	public:
		/**
		* Constructor ,allocates memory for the size of the shader uniforms of the Material ,stores shared_ptr
		* to the Material and insert itself to the material instances
		* @param[in] material
		*/
		MaterialInstance(const std::shared_ptr<Material>& material);

		/**
		* Destructor, deletes memory for the shader uniforms and erase itself from the Material's instances
		*/
		~MaterialInstance();


		/**
		* Set the value in the buffer corresponding to the shader uniform
		* @param[in] name	Name of the uniform
		* @arg[in] val		Value for the uniform
		*/
		template<typename T>
		void Set(const std::string& name, const T& val)
		{
			auto uni = m_Material->m_Shader->FindUniform(name);
			XYZ_ASSERT(uni, "Material uniform does not exist");
			XYZ_ASSERT(uni->offset + uni->size <= m_Material->m_Shader->GetUniformSize(), "Material uniform buffer out of range");
			memcpy(m_Buffer + uni->offset, (unsigned char*)& val, uni->size);
			m_UpdatedValues.insert(name);
		}


		/**
		* Set the array in the buffer corresponding to the shader array uniform
		* @param[in] name	Name of the uniform
		* @arg[in] val		Value for the uniform
		* @param[in] size	Size of the val
		* @param[in] offset Offset in the shader array uniform
		*/
		template<typename T>
		void Set(const std::string& name, const T& val, int size, int offset)
		{
			auto uni = m_Material->m_Shader->FindUniform(name);
			XYZ_ASSERT(uni, "Material uniform does not exist ", name.c_str());
			XYZ_ASSERT(uni->offset + uni->size <= m_Material->m_Shader->GetUniformSize(), "Material uniform buffer out of range");
			XYZ_ASSERT(size + offset < uni->size, "Material uniform out of range");
			memcpy(m_Buffer + uni->offset + offset, (unsigned char*)& val, size);
			m_UpdatedValues.insert(name);
		}

		/**
		* Set the uniforms in the shader
		*/
		void Bind();
	
		/**
		* @return shared_ptr to the Material used in constructor
		*/
		int64_t GetSortKey() const { return m_Material->m_Key; }
		const std::shared_ptr<Material> GetParentMaterial() { return m_Material; }

		/**
		* @param[in] material
		* @return shared_ptr to the MaterialInstance
		*/
		static std::shared_ptr<MaterialInstance> Create(const std::shared_ptr<Material>& material);

	private:
		/**
		* It is called when shader is reloaded.
		* Reallocate storage for uniforms
		*/
		void OnShaderReload();

		/**
		* Set uniform in the buffer if the uniform is not in set of updated values
		*/
		void UpdateMaterialValue(const Uniform* uni);

	private:
		std::shared_ptr<Material> m_Material;


		unsigned char* m_Buffer;
		std::unordered_set<std::string> m_UpdatedValues;
	};

}