#include "Camera.hpp"
#include <iostream>

namespace gps {

	//Camera constructor
	Camera::Camera(glm::vec3 cameraPosition, glm::vec3 cameraFrontDirection, glm::vec3 cameraUp) {
		this->cameraPosition = cameraPosition;
		this->cameraUpDirection = cameraUp;
		this->cameraFrontDirection = cameraFrontDirection;
		this->cameraRightDirection = glm::normalize(glm::cross(this->cameraFrontDirection, this->cameraUpDirection));
		//TODO - Update the rest of camera parameters

	}

	//return the view matrix, using the glm::lookAt() function
	glm::mat4 Camera::getViewMatrix() {
		return glm::lookAt(cameraPosition, cameraPosition + cameraFrontDirection, cameraUpDirection);
	}

	//update the camera internal parameters following a camera move event
	void Camera::move(MOVE_DIRECTION direction, float speed) {
		switch (direction)
		{
		case MOVE_FORWARD:
			if (cameraPosition.y > 1.5f)
				this->cameraPosition += this->cameraFrontDirection * speed;
			break;
		case MOVE_BACKWARD:
			if (cameraPosition.y > 1.5f)
				this->cameraPosition -= this->cameraFrontDirection * speed;
			break;
		case MOVE_RIGHT:
			this->cameraPosition += this->cameraRightDirection * speed;
			break;
		case MOVE_LEFT:
			this->cameraPosition -= this->cameraRightDirection * speed;
			break;
		case MOVE_UP_TARGET:
			this->cameraFrontDirection += glm::vec3(0.0f, speed, 0.0f);
			break;
		case MOVE_DOWN_TARGET:
			this->cameraFrontDirection -= glm::vec3(0.0f, speed, 0.0f);
			break;
		case MOVE_UP:
			this->cameraPosition += glm::vec3(0.0f, speed, 0.0f);
			break;
		case MOVE_DOWN:
			if(cameraPosition.y > 2.5f)
				this->cameraPosition -= glm::vec3(0.0f, speed, 0.0f);
			break;
		}
	}

	void Camera::setCameraDirection(glm::vec3 cameraDirection)
	{
		this->cameraFrontDirection = cameraDirection;
		this->cameraRightDirection = glm::normalize(glm::cross(this->cameraFrontDirection, glm::vec3(0.0f, 1.0f, 0.0f)));
	}

	void Camera::rotate(float pitch, float yaw) {

		glm::vec3 front;
		front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
		front.y = sin(glm::radians(pitch));
		front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
		this->cameraFrontDirection = glm::normalize(front);
		setCameraDirection(cameraFrontDirection);
	}
}