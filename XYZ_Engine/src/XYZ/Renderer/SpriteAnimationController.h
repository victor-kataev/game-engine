#pragma once
#include "SpriteAnimation.h"

#include <unordered_map>

namespace XYZ {
	/**
	* @class SpriteAnimationController
	* @brief Stores and updates animations
	*/
	class SpriteAnimationController
	{
	public:
		/**
		* Add animation to unordered_map of animations
		* @param[in] name   Name of the animation
		* @param[in] anim	Animation
		*/
		void AddAnimation(const std::string& name, Animation anim) { m_Animations.insert(std::pair<std::string, Animation>(name, anim)); };


		/**
		* Set animation with name to current
		* @param[in] name	Name of the Animation
		*/
		void StartAnimation(const std::string& name) { m_Current = &m_Animations[name]; };

		/**
		* Updates sprite animation with currently set animation
		* @param[in] anim	Pointer to the SpriteAnimation
		*/
		void UpdateSpriteAnimation(SpriteAnimation* anim);
	private:
		std::unordered_map<std::string, Animation> m_Animations;
		Animation* m_Current;
	};
}