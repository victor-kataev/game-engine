#pragma once
#include "XYZ/ECS/Component.h"

#include "SortingLayer.h"
#include "Material.h"

#include <glm/glm.hpp>

namespace XYZ {
	/**
	* @struct Renderable2D
	* @brief represents 2D renderable object.
	*/
	struct Renderable2D : public Type<Renderable2D>
	{
		/**
		* Construct a 2D renderable, from given parameters
		* @param[in] Material	Material of the renderable
		* @param[in] Color		Color of the renderable
		* @param[in] TexCoord	Bottom left and top right texture coordinates of renderable
		* @param[in] Position   Position of the renderable
		* @param[in] Size		Size of the renderable
		* @param[in] Rotation   Rotation of the renderable
		* @param[in] Visible	Specify if the renderable is visible and should be rendered
		* @param[in] TextureID  Specify the ID of the texture
		*/
		Renderable2D(
			std::shared_ptr<Material> Material,
			const glm::vec4& Color,
			const glm::vec4& TexCoord,
			bool Visible,
			int TextureID
			)
		:
			material(Material),
			color(Color),
			texCoord(TexCoord),
			visible(Visible),
			textureID(TextureID),
			sortLayerID(SortingLayer::Get().GetOrderValueByName("default"))
		{}

		~Renderable2D()
		{
		}

		std::shared_ptr<Material> material;
		glm::vec4 color;
		glm::vec4 texCoord; // (x = left, y = bottom, z = right, w = top)
		bool visible;
		int textureID;
		SortingLayerID sortLayerID;
	};
}