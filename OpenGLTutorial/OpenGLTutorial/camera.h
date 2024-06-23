#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/vector_angle.hpp>
#include "shader.h"

/// <summary>
/// Camera's Class
/// </summary>
class Camera
{
private:
	float m_camera_speed_default = 0.1f;
	float m_mouse_sensitivity_default = 250.0f;

public:
	//Store main vector of camera
	glm::vec3 m_position;
	glm::vec3 m_orientation = glm::vec3(0.0f, 0.0f, -1.0f);
	glm::vec3 m_up = glm::vec3(0.0, 1.0f, 0.0f);

	glm::mat4 m_camera_matrix = glm::mat4(1.0f);

	//Prevent the camera from jumping around when first clicking left click
	bool m_first_click = true;

	int m_width;
	int m_height;

	//Adjust speed of movement and sensitivity of mouse
	float m_speed = m_camera_speed_default;
	float m_sensitivity = m_mouse_sensitivity_default;

	Camera(int width, int height, glm::vec3 position, float camera_speed_default = 0.1f, float mouse_sensitivity_default = 250.0f);
	void setReferenceMatrices(float fov_deg, float near_plane, float far_plane);
	void updateReferenceMatricesToShader(Shader& shader, const char* uniform);
	void handleInput(GLFWwindow* window);
};
