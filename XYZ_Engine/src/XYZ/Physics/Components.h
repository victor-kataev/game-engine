#pragma once
#include <glm/glm.hpp>
#include "XYZ/ECS/Component.h"

namespace XYZ {
	template<typename T>
	using vector2D = std::vector<std::vector<T>>;

	struct Transform2D : public Type<Transform2D>
	{
		Transform2D(const glm::vec3& Pos, const glm::vec2& Size = glm::vec2(1), float Rot = 0.0f)
			:
			position(Pos),size(Size),rotation(Rot)
		{}
		glm::vec3 position = glm::vec3(0);
		glm::vec2 size = glm::vec2(1);
		float rotation = 0.0f;
	};

	struct RigidBody2D : public Type<RigidBody2D>
	{
		RigidBody2D(const glm::vec2& Velocity)
			: velocity(Velocity),bounceValue(0)
		{}
		glm::vec2 velocity;
		int bounceValue;
	};
	
	struct GridBody : public Type<GridBody>
	{
		GridBody(int Row,int Col,int Width, int Height)
			: row(Row),col(Col),width(Width),height(Height),nextRow(0),nextCol(0)
		{}
		int row;
		int col;
		int width;
		int height;
		int nextRow;
		int nextCol;

		bool operator ==(const GridBody& other) const
		{
			return (col == other.col && row == other.row);
		}

		bool operator !=(const GridBody& other)
		{
			return (col != other.col || row != other.row);
		}
	};

	struct InterpolatedMovement : public Type<InterpolatedMovement>
	{
		InterpolatedMovement(const glm::vec2& Velocity)
			: distance(glm::vec2(0)), velocity(Velocity), inProgress(false)
		{}
		glm::vec2 distance;
		glm::vec2 velocity;
		bool inProgress;
	};

	struct CollisionComponent : public Type<CollisionComponent>
	{
		CollisionComponent(int32_t Layer, int32_t CollisionLayers)
			: layer(Layer), collisionLayers(CollisionLayers), currentCollisions(0)
		{}
		int32_t layer;

		// layers that might collide with layer
		int32_t collisionLayers;

		// current collisions
		int32_t currentCollisions;
	};


	struct RealGridBody : public Type<RealGridBody>
	{
		RealGridBody(float Left, float Right, float Bottom, float Top)
			: left(Left),right(Right),bottom(Bottom),top(Top)
		{}

		bool Collide(const RealGridBody& other)
		{
			if (left >= other.right || other.left >= right)
			{
				return false;
			}
			if (top <= other.bottom || other.top <= bottom)
			{
				return false;
			}
			return true;
		}

		void Move(const glm::vec2& pos)
		{
			left += pos.x;
			right += pos.x;
			top += pos.y;
			bottom += pos.y;
		}

		float left;
		float right;
		float bottom;
		float top;
	};

}


