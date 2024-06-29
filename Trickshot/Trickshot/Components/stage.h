#pragma once
#include "mesh.h"

/// <summary>
/// Stage's Constructor
/// </summary>
class Stage
{
public:
	glm::vec3 m_camera_start_position;
	glm::vec3 m_room_size;
	float m_camera_threshold;
	std::vector<Mesh> m_meshes;

public:
	Stage(glm::vec3 room_size, std::vector<Texture> texture_top, std::vector<Texture> textures_side, std::vector<Texture> textures_bottom, glm::vec3 camera_pos = glm::vec3(0.5f, 0.0f, 0.5f), float camera_thresold = 0.4);
	void drawStage(Shader& shader, Camera camera);
	
};