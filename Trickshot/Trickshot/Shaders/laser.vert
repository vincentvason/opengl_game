#version 330 core
//Layout
layout (location = 0) in vec3 l_position;
layout (location = 1) in vec3 l_normal_vector;
layout (location = 2) in vec3 l_color;
layout (location = 3) in vec2 l_texture_coordinator;


//Output to Shader
out vec3 io_position;
out vec3 io_color;

//Uniform variables
uniform vec3 u_camera_positon;
uniform vec3 u_camera_orientation;
uniform mat4 u_camera_matrix;
uniform mat4 u_model_matrix;
uniform mat4 u_camera_view_matrix;

void main()
{
	vec4 position = inverse(u_camera_view_matrix) * vec4((l_position * vec3(-1.0f)), 1.0f);

	io_position = vec3(u_model_matrix * position);
	gl_Position = u_camera_matrix * position;

	io_color = l_color;
}
