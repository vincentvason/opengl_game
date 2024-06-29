#version 330 core
out vec4 FragColor;

in vec2 io_texture_coordinator;
uniform sampler2D u_screen_texture;
uniform float u_gamma;

const float offset_x = 1.0f / 800.0f;
const float offset_y = 1.0f / 800.0f;

void main()
{
	//Gamma + Tone Mapping (HDR)
	vec3 fragment = texture(u_screen_texture, io_texture_coordinator).rgb;
	
	float exposure = 1.0f;
	vec3 tone_mapping = vec3(1.0f) - exp(-fragment * exposure);

	FragColor.rgb = pow(tone_mapping.rgb, vec3(1.0f / u_gamma));
	
	
}