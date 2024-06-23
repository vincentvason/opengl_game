#version 330
out vec4 FragColor;

in vec3 io_texture_coordinator;
uniform samplerCube u_skybox_texture;

void main()
{
	FragColor = texture(u_skybox_texture, io_texture_coordinator);
}
