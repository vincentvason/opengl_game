#version 330
layout (location = 0) in vec3 l_position;

out vec3 io_texture_coordinator;

uniform mat4 u_projection_matrix;
uniform mat4 u_view_matrix;

void main()
{
	vec4 position = u_projection_matrix * u_view_matrix * vec4(l_position, 1.0f);

	//gl_Position.z will have depth of 1
	gl_Position = vec4(position.x, position.y, position.w, position.w);
	//flip the z axis due to the different coordinate systems (left hand vs right hand)
	io_texture_coordinator = vec3(l_position.x, l_position.y, -l_position.z);
}