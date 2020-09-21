#pragma once
#include "XYZ/Renderer/VertexArray.h"
#include "XYZ/Renderer/Renderable2D.h"
#include "XYZ/Renderer/Material.h"
#include "XYZ/Utils/DataStructures/FreeList.h"

#include <glm/glm.hpp>
#include <set>
#include <vector>

namespace XYZ {
	
	struct ParticleQuad
	{
		ParticleQuad(glm::vec2 pos, glm::vec2 size)
			: Size(size)
		{
			squareVert[0] += pos.x - size.x;
			squareVert[5] += pos.x + size.x;
			squareVert[10] += pos.x + size.x;
			squareVert[15] += pos.x - size.x;

			squareVert[1] += pos.y - size.y;
			squareVert[6] += pos.y - size.y;
			squareVert[11] += pos.y + size.y;
			squareVert[16] += pos.y + size.y;
		}

		float squareVert[20] = {
			-0.5f, -0.5f, 0.0f, 0.0f, 0.0f,
			 0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
			 0.5f,  0.5f, 0.0f, 1.0f, 1.0f,
			-0.5f,  0.5f, 0.0f, 0.0f, 1.0f
		};
		glm::vec2 Size;
	};

	struct ParticleEmitter : public Type<ParticleEmitter>
	{
		std::shared_ptr<MaterialInstance> material;
		std::shared_ptr<Material> renderMaterial;
		glm::vec2 position		= glm::vec2(0);
		glm::vec2 size			= glm::vec2(0);
		int	 numParticles		= 0;
		bool play				= false;
		bool loop				= false;
	};

	struct ParticleVertex
	{
		glm::vec4 color			 = glm::vec4(0);
		glm::vec4 position		 = glm::vec4(0);
		glm::vec2 texCoordOffset = glm::vec2(0);
		glm::vec2 size		     = glm::vec2(0);
		float	  rotation		 = 0.0f;

	private:
		float	  alignment		 = 0.0f;
		float	  alignment2	 = 0.0f;
		float	  alignment3	 = 0.0f;
	};
	struct ParticleInformation
	{
		glm::vec4 colorBegin		= glm::vec4(0);
		glm::vec4 colorEnd			= glm::vec4(0);
		glm::vec2 startVelocity		= glm::vec2(0);
		glm::vec2 endVelocity		= glm::vec2(0);
		glm::vec2 defaultPosition	= glm::vec2(0);
		float sizeBegin				= 0.0f;
		float sizeEnd				= 0.0f;
		float rotation				= 0.0f;
		float lifeTime				= 0.0f;
	private:
		float timeAlive				= 0.0f;

	private:
		float alignment				= 0.0f;
	};


	
	/* !@class ParticleEffect2D 
	* @brief gpu based particle effect
	*/
	class ParticleEffect2D : public Type<ParticleEffect2D>
	{
	public:
		/**
		* Construct
		* @param[in] maxParticles
		* @param[in] material
		* @param[in] renderMaterial
		*/
		ParticleEffect2D(uint32_t maxParticles, std::shared_ptr<Material> material, std::shared_ptr<Material> renderMaterial);
		~ParticleEffect2D();

		/**
		* Rendering function
		*/
		void Render();

		/**
		* Bind data in the gpu and compute
		* @param[in] emitter
		*/
		void Bind(const ParticleEmitter& emitter);

		std::shared_ptr<Material> GetMaterial() { return m_Material; }
		size_t GetNumExistingParticles() { return m_ParticlesInExistence; }
		

		/**
		* Set max number of particles
		* @param[in] vertexBuffer
		* @param[in] particleInfo
		*/
		void SetParticles(ParticleVertex* vertexBuffer,ParticleInformation* particleInfo);
		
		/**
		* Set particles in range
		* @param[in] vertexBuffer
		* @param[in] particleInfo
		*/
		void SetParticlesRange(ParticleVertex* vertexBuffer, ParticleInformation* particleInfo, uint32_t offset, uint32_t count);
		
		/**
		* @param[out] vertexBuffer
		* @param[out] particleInfo
		* @return number of particles
		*/
		uint32_t GetParticles(ParticleVertex* vertexBuffer, ParticleInformation* particleInfo);
		
		/**
		* @param[out] vertexBuffer
		* @param[out] particleInfo
		* @param[in] offset
		* @param[in] count
		* @return number of particles
		*/
		uint32_t GetParticlesRange(ParticleVertex* vertexBuffer, ParticleInformation* particleInfo, uint32_t offset, uint32_t count);
		
	private:
		std::shared_ptr<VertexArray> m_VertexArray;
		std::shared_ptr<Material> m_Material;
		std::shared_ptr<Material> m_RenderMaterial;

		uint32_t m_ParticlesInExistence = 0;
		uint32_t m_MaxParticles;

		std::vector<ParticleVertex> m_Vertices;
		std::vector<ParticleInformation> m_Data;
		std::shared_ptr<ShaderStorageBuffer> m_VertexStorage;
		std::shared_ptr<ShaderStorageBuffer> m_PropsStorage;

		static constexpr size_t sc_MaxParticlesPerEffect = 10000;
	};

}