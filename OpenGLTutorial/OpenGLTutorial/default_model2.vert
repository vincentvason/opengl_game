#version 330
//Layout
layout (location = 0) in vec3 l_position;
layout (location = 1) in vec3 l_normal_vector;
layout (location = 2) in vec3 l_color;
layout (location = 3) in vec2 l_texture_coordinator;


//Output to Shader
out DATA
{
    vec3 normal_vector;
	vec3 color;
	vec2 texture_coordinator;
    mat4 projection_matrix;
} data_out;

//Uniform variables
uniform mat4 u_camera_matrix;
uniform mat4 u_model_matrix;
uniform mat4 u_translation_matrix;
uniform mat4 u_rotation_matrix;
uniform mat4 u_scale_matrix;

void main()
{
	//GLM may have difference CW, CCW from JSON accessors
	gl_Position = u_camera_matrix * vec4(io_position, 1.0);

	data_out.normal_vector = l_normal_vector;
	data_out.color = l_color;

	//GLM may have difference angle from JSON accessors
	data_out.texture_coordinator = l_texture_coordinator;

	data_out.projection_matrix = u_camera_matrix;
}