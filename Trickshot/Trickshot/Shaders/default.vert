#version 330 core

// Positions/Coordinates
layout (location = 0) in vec3 l_position;
// Normals (not necessarily normalized)
layout (location = 1) in vec3 l_normal_vector;
// Colors
layout (location = 2) in vec3 l_color;
// Texture Coordinates
layout (location = 3) in vec2 l_texture_coordinator;


out DATA
{
    vec3 normal_vector;
	vec3 color;
	vec2 texture_coordinator;
    mat4 projection_matrix;
	mat4 model_matrix;
	vec3 light_position;
	vec3 camera_position;

} data_out;



// Imports the camera matrix
uniform mat4 u_camera_matrix;
// Imports the transformation matrices
uniform mat4 u_model_matrix;
uniform mat4 u_translation_matrix;
uniform mat4 u_rotation_matrix;
uniform mat4 u_scale_matrix;
// Imports Light/Camera Position
uniform vec3 u_light_position;
uniform vec3 u_camera_position;


void main()
{
	gl_Position = u_model_matrix * u_translation_matrix * u_rotation_matrix * u_scale_matrix * vec4(l_position, 1.0f);
	data_out.normal_vector = l_normal_vector;
	data_out.color = l_color;
	data_out.texture_coordinator = l_texture_coordinator;
	data_out.projection_matrix = u_camera_matrix;
	data_out.model_matrix = u_model_matrix * u_translation_matrix * u_rotation_matrix * u_scale_matrix;
	data_out.light_position = u_light_position;
	data_out.camera_position = u_camera_position;
}