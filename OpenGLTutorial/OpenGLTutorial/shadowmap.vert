#version 330 core
layout (location = 0) in vec3 l_position;

uniform mat4 u_light_projection_matrix;
uniform mat4 u_model_matrix;

void main()
{
	gl_Position = u_light_projection_matrix * u_model_matrix * vec4(l_position, 1.0);
}