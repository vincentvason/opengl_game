#include "Camera.h"

/// <summary>
/// Camera's Constructor
/// </summary>
/// <param name="width">Width of Screen</param>
/// <param name="height">Height of Screen</param>
/// <param name="position">Position of Camera</param>
Camera::Camera(int width, int height, glm::vec3 position, float camera_speed_default, float mouse_sensitivity_default)
{
	m_width = width;
	m_height = height;
	m_position = position;
	m_camera_speed_default = camera_speed_default;
	m_mouse_sensitivity_default = mouse_sensitivity_default;
}

/// <summary>
/// Set Reference Matrices
/// </summary>
/// <param name="fov_deg">Degree of view point.</param>
/// <param name="near_plane">Nearest distance can see.</param>
/// <param name="far_plane">Furthest distance can see.</param>
void Camera::setReferenceMatrices(float fov_deg, float near_plane, float far_plane)
{
	//Store matrics
	glm::mat4 view_matrix = glm::mat4(1.0f);
	glm::mat4 projection_matrix = glm::mat4(1.0f);
	
	//Make camera look in the right direction from the right position
	view_matrix = glm::lookAt(m_position, m_position + m_orientation, m_up);
	//Add perspective to the scene
	projection_matrix = glm::perspective(glm::radians(fov_deg), (float)m_width / m_height, near_plane, far_plane);

	//Exports te camera matrix to the matrix shader
	m_camera_matrix = projection_matrix * view_matrix;
}

/// <summary>
/// Update all reference matrices to shader
/// </summary>
/// <param name="shader">Shader Program</param>
/// <param name="uniform">Uniform variable (see .frag)</param>
void Camera::updateReferenceMatricesToShader(Shader& shader, const char* uniform)
{
	glUniformMatrix4fv(glGetUniformLocation(shader.m_id, uniform), 1, GL_FALSE, glm::value_ptr(m_camera_matrix));
}



void Camera::handleInput(GLFWwindow* window, glm::vec3 world_space, float thresould)
{
	//Forward, Backward, Left, Right
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
	{
		m_position += m_speed * m_orientation;
	}
	if(glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
	{
		m_position += m_speed * -glm::normalize(glm::cross(m_orientation, m_up));
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
	{
		m_position += m_speed * -m_orientation;
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
	{
		m_position += m_speed * glm::normalize(glm::cross(m_orientation, m_up));
	}
	//Up, Down
	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
	{
		m_position += m_speed * m_up;
	}
	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
	{
		m_position += m_speed * -m_up;
	}

	//World Limitation
	if (world_space != glm::vec3{ 0.0f })
	{
		for (int i = 0; i < 3; i++)
		{
			m_position[i] = std::min(m_position[i], world_space[i] - thresould);
			m_position[i] = std::max(m_position[i], thresould);
		}
	}

	std::cout << "Pos:" << m_position.x << "," << m_position.y << "," << m_position.z << " Orn:" << m_orientation.x << "," << m_orientation.y << "," << m_orientation.z << std::endl;

	//Mouse
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
	{
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

		//Prevent camera from jumping on the first click
		if (m_first_click)
		{
			glfwSetCursorPos(window, (m_width / 2), (m_height / 2));
			m_first_click = false;
		}

		//get cursor position the store it
		double mouse_x;
		double mouse_y;
		glfwGetCursorPos(window, &mouse_x, &mouse_y);

		// Normalizes and shifts the coordinates of the cursor such that they begin in the middle of the screen
		// and then "transforms" them into degrees
		float rot_x = m_sensitivity * (float)(mouse_y - ((float)m_height / 2)) / m_height;
		float rot_y = m_sensitivity * (float)(mouse_x - ((float)m_width / 2)) / m_width;

		
		// Calculates upcoming vertical change in the Orientation
		glm::vec3 new_orientation = glm::rotate(m_orientation, glm::radians(-rot_x), glm::normalize(glm::cross(m_orientation, m_up)));

		// Decides whether or not the next vertical Orientation is legal or not
		// This prevent from continous rotate when move too much
		if(abs(glm::angle(new_orientation, m_up) - glm::radians(90.0f)) <= glm::radians(85.0f))
		{
			m_orientation = new_orientation;
		}

		// Rotates the Orientation left and right
		m_orientation - glm::rotate(m_orientation, glm::radians(-rot_y), m_up);

		// Set cursor position always in circle
		// This prevent cursor to roaming around the screen
		m_orientation = glm::rotate(m_orientation, glm::radians(-rot_y), m_up);
		glfwSetCursorPos(window, (m_width/2), (m_height/2));
	}
	else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE)
	{
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		m_first_click = true;
	}
}


