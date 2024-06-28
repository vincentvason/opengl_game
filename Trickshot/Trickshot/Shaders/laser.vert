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
uniform mat4 u_camera_matrix;
uniform mat4 u_model_matrix;

void main()
{
	io_position = vec3(u_model_matrix * vec4(l_position, 1.0f));
	gl_Position = u_camera_matrix * vec4(l_position, 1.0f);

	io_color = l_color;
}
