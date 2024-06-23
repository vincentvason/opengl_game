#version 330 core
in vec4 io_fragment_position;

uniform vec3 u_light_position;
uniform float u_far_plane;

void main()
{
	gl_FragDepth = length(io_fragment_position.xyz - u_light_position) / u_far_plane;

}