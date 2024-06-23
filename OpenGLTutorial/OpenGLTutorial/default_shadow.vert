#version 330 core
//Layout
layout (location = 0) in vec3 l_position;
layout (location = 1) in vec3 l_normal_vector;
layout (location = 2) in vec3 l_color;
layout (location = 3) in vec2 l_texture_coordinator;


//Output to Shader
out vec3 io_position;
out vec3 io_normal_vector;
out vec3 io_color;
out vec2 io_texture_coordinator;
out vec4 io_fragment_light_position;

//Uniform variables
uniform mat4 u_camera_matrix;
uniform mat4 u_model_matrix;
uniform mat4 u_translation_matrix;
uniform mat4 u_rotation_matrix;
uniform mat4 u_scale_matrix;

uniform mat4 u_light_projection_matrix;

void main()
{
	//GLM may have difference CW, CCW from JSON accessors
	io_position = vec3(u_model_matrix * u_translation_matrix * u_rotation_matrix * u_scale_matrix * vec4(l_position, 1.0f));
	gl_Position = u_camera_matrix * vec4(io_position, 1.0);

	io_normal_vector = l_normal_vector;

	io_color = l_color;

	//GLM may have difference angle from JSON accessors
	io_texture_coordinator = mat2(0.0, -1.0, 1.0, 0.0) * l_texture_coordinator;

	io_fragment_light_position = u_light_projection_matrix * vec4(io_position, 1.0);
}