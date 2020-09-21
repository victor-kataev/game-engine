#pragma once
#include "XYZ/ECS/Component.h"
#include "Renderable2D.h"

namespace XYZ {
	/**
	* @struct Animation
	* @brief encapsulation for animation data
	*/
	struct Animation
	{
		std::pair<int, int> frameInterval;
		float frameLen;
		int currentFrame = 0;
	};


	/**
	* @class SpriteAnimation
	* @brief handles sprite animation, updates Renderable2D texture coordinates,
	* texture is split to single sized quads, each quad representing single frame.
	*/
	class SpriteAnimation : public Type<SpriteAnimation>
	{
	public:
		/**
		* Construct a sprite animation
		* @param[in] numRows	Number of rows in texture atlas
		* @param[in] numCols    Number of columns in texture atlas
		* @param[in] width		Width of texture atlas
		* @param[in] height		Height of texture atlas
		*/
		SpriteAnimation(int numRows, int numCols, uint32_t width, uint32_t height);

		/**
		* Set duration between specified frames
		* @param[in] first		First frame 
		* @param[in] last		Last frame
		* @param[in] timeFrame  duration
		*/
		void SetFrameInterval(int first, int last, float timeFrame);


		/**
		* Updates renderable texture coordinates
		* @param[in] dt		delta time
		* @param[in] sprite	Pointer to the Renderable2D
		*/
		void Update(float dt, Renderable2D* sprite);

	private:
		/**
		* Calculate texture coordinates dependent on current animation frame
		* @return texture coordinates
		*/
		const glm::vec4 calcTexCoords();
	private:
		Animation m_Animation;
		uint32_t m_Width;
		uint32_t m_Height;

		int m_Rows;
		int m_Columns;
		int m_NumFrames;

		float m_AnimationLen;
		float m_CurrentTime;
	};

}