#pragma once
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <SDL.h>

class Camera {
public:
	Camera(GLfloat width, GLfloat height)
		: _cameraSpeed(0.01f)
		, _cameraPos(glm::vec3(0.0f, 0.0f, 3.0f))
		, _cameraFront(glm::vec3(0.0f, 0.0f, -1.0f))
		, _cameraUp(glm::vec3(0.0f, 1.0f, 0.0f))
		, _yaw(-90.0f)
		, _pitch(0.0f)
		, _lastX(width/2.0)
		, _lastY(height/2.0)
		, _fov(45.0)
		, _firstMouse(true)
	{

	}

	void SetFov(int scroll) {
		if (_fov >= 1.0f && _fov <= 45.0f)
			_fov -= scroll;
		if (_fov <= 1.0f)
			_fov = 1.0f;
		if (_fov >= 45.0f)
			_fov = 45.0f;
	}

	glm::vec3 Position() {
		return _cameraPos;
	}

	void Update(int xpos, int ypos, bool* keys) {
		if (_firstMouse) // this bool variable is initially set to true
		{
			_lastX = xpos;
			_lastY = ypos;
			_firstMouse = false;
		}

		GLfloat xoffset = xpos - _lastX;
		GLfloat yoffset = _lastY - ypos; // Reversed since y-coordinates range from bottom to top
		_lastX = xpos;
		_lastY = ypos;

		GLfloat sensitivity = 0.15f;
		xoffset *= sensitivity;
		yoffset *= sensitivity;
		_yaw += xoffset;
		_pitch += yoffset;

		if (_pitch > 89.0f)
			_pitch = 89.0f;
		if (_pitch < -89.0f)
			_pitch = -89.0f;

		glm::vec3 front;
		front.x = cos(glm::radians(_pitch)) * cos(glm::radians(_yaw));
		front.y = sin(glm::radians(_pitch));
		front.z = cos(glm::radians(_pitch)) * sin(glm::radians(_yaw));
		_cameraFront = glm::normalize(front);

		// Keyboard input handling
		if (keys[SDL_SCANCODE_W]) {
			_cameraPos += _cameraSpeed * _cameraFront;
		}
		if (keys[SDL_SCANCODE_S]) {
			_cameraPos -= _cameraSpeed * _cameraFront;
		}
		if (keys[SDL_SCANCODE_A]) {
			_cameraPos -= glm::normalize(glm::cross(_cameraFront, _cameraUp)) * _cameraSpeed;
		}
		if (keys[SDL_SCANCODE_D]) {
			_cameraPos += glm::normalize(glm::cross(_cameraFront, _cameraUp)) * _cameraSpeed;
		}
	}

	glm::mat4 GetViewMatrix() {
		return glm::lookAt(_cameraPos, _cameraPos + _cameraFront, _cameraUp);
	}

	GLfloat Zoom() {
		return _fov;
	}

private:
	GLfloat _cameraSpeed;
	glm::vec3 _cameraPos;
	glm::vec3 _cameraFront;
	glm::vec3 _cameraUp;

	GLfloat _yaw;	// Yaw is initialized to -90.0 degrees since a yaw of 0.0 results in a direction vector pointing to the right (due to how Eular angles work) so we initially rotate a bit to the left.
	GLfloat _pitch;
	GLfloat _lastX;
	GLfloat _lastY;
	GLfloat _fov;

	bool _firstMouse;
};