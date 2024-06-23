#version 330 core
out vec4 FragColor;

in vec2 io_texture_coordinator;
uniform sampler2D u_screen_texture;
uniform float u_gamma;

const float offset_x = 1.0f / 800.0f;
const float offset_y = 1.0f / 800.0f;

/*
vec2 offsets[9] = vec2[](
	vec2(-offset_x, offset_y),	vec2(0.0f, offset_y),	vec2(offset_x, offset_y),
	vec2(-offset_x, 0.0f),		vec2(0.0f, 0.0f),		vec2(offset_x, 0.0f),
	vec2(-offset_x, -offset_y), vec2(0.0f, -offset_y),	vec2(offset_x, -offset_y)
);


float kernel[9] = float[9](
	0, 0, 0,
	0, 0, 0,
	0, 0, 0
);
*/

void main()
{
	//Nothing
	//FragColor = texture(u_screen_texture, io_screen_texture_coordinator);

	//Inverse Color
	//FragColor = vec4(1.0f) - texture(u_screen_texture, io_screen_texture_coordinator); 

	//Grayscale
	/*
	vec4 tex = texture(u_screen_texture, io_screen_texture_coordinator);
	float avg = (tex.x + tex.y + tex.z) / 3.0f;
	FragColor = vec4(avg, avg, avg, avg); 
	*/

	//Kernel Post-processing
	
	/*
	vec3 color = vec3(0.0f);
	for(int i = 0; i < 9; i++)
	{
		 color += vec3(texture(u_screen_texture, io_texture_coordinator.st + offsets[i])) * kernel[i];
	}
	FragColor = vec4(color, 1.0f); 
	*/
	
	//Gamma Correction
	//vec4 fragment = texture(u_screen_texture, io_texture_coordinator);
	//FragColor.rgb = pow(fragment.rgb, vec3(1.0f / u_gamma));

	//Gamma + Tone Mapping (HDR)
	vec3 fragment = texture(u_screen_texture, io_texture_coordinator).rgb;
	
	float exposure = 1.0f;
	vec3 tone_mapping = vec3(1.0f) - exp(-fragment * exposure);

	FragColor.rgb = pow(tone_mapping.rgb, vec3(1.0f / u_gamma));
}