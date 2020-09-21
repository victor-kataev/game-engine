#pragma once

#include <glm/glm.hpp>


namespace XYZ {
	/**
	* @class OrthoCamera
	* @brief represents camera for 2D scene, calculates projection and view matrix 
	*/
	class OrthoCamera
	{
		glm::mat4 m_ProjectionMatrix;
		glm::mat4 m_ViewMatrix;
		glm::mat4 m_ViewProjectionMatrix;

		glm::vec3 m_Position;
		float m_Rotation = 0.0f;
	private:
		/**
		* Recalculate view matrix of the camera dependent on it's position and rotation
		*/
		void RecalculateViewMatrix();
	public:
		/**
		* Constructor, takes bounds of view frustrum of the camera
		* @param[in] left
		* @param[in] right
		* @param[in] bottom
		* @param[in] top
		*/
		OrthoCamera(float left, float right, float bottom, float top);

		/**
		* @return position of the camera
		*/
		const glm::vec3& GetPosition() const { return m_Position; }

		/**
		* Set position of the camera and recalculate the view matrix
		* @param[in] pos 
		*/
		inline void SetPosition(const glm::vec3& pos) { m_Position = pos; RecalculateViewMatrix(); }

		/**
		* @return rotation of the camera
		*/
		float GetRotation() const { return m_Rotation; }

		/**
		* Set the rotation of the camera and recalculate the view matrix
		* @param[in] rot
		*/
		void SetRotation(float rot) { m_Rotation = rot; RecalculateViewMatrix(); }

		/**
		* Set projection matrix dependent on boundaries
		* @param[in] left
		* @param[in] right
		* @param[in] bottom
		* @param[in] top
		*/
		void SetProjection(float left, float right, float bottom, float top);

		/**
		* @return projection matrix of the camera
		*/
		inline const glm::mat4& GetProjectionMatrix() const { return m_ProjectionMatrix; }

		/**
		* @return view matrix of the camera
		*/
		inline const glm::mat4& GetViewMatrix() const { return m_ViewMatrix; }

		/**
		* @return projectionview matrix of the camera
		*/
		inline const glm::mat4& GetViewProjectionMatrix() const { return m_ViewProjectionMatrix; }
	};

}