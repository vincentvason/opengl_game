#version 330 core
//Output
out vec4 FragColor;

//Input from vertex shader (Must be same name with variable in .vert)
in vec3 io_position;
in vec3 io_normal_vector;
in vec3 io_color;
in vec2 io_texture_coordinator;
in vec4 io_fragment_light_position;

//Uniform
uniform sampler2D u_diffuse0;
uniform sampler2D u_specular0;
uniform sampler2D u_shadowmap;
uniform samplerCube u_shadowcubemap;

uniform vec4 u_light_color;
uniform vec3 u_light_position;
uniform vec3 u_camera_position;
uniform float u_far_plane;

vec4 pointLight()
{
	//Intensity Attenuation
	vec3 light_vector = u_light_position - io_position;
	float a = 0.0003f;
	float b = 0.00002f;
	float distances = length(light_vector);
	float intensity = 1.0f / ((a * distances * distances) + (b * distances) + 1.0f);

	//Ambient Lighting
	float ambient = 0.2f;

	//Diffuse Lighting
	vec3 normal = normalize(io_normal_vector);
	vec3 light_direction = normalize(light_vector);
	float diffuse = max(dot(normal, light_direction), 0.0f);
	
	//Specular Lighting
	float specular = 0.0f;
	if(diffuse != 0.0f)
	{
		float specular_light = 0.50f;
		vec3 view_direction = normalize(u_camera_position - io_position);
		vec3 halfway_vector = normalize(view_direction + light_direction);
		float specular_amount = pow(max(dot(normal, halfway_vector), 0.0f), 16);
		specular = specular_amount * specular_light;
	}

	//Shadow
	float shadow = 0.0f;
	vec3 fragment_to_light = io_position - u_light_position;
	float current_depth = length(fragment_to_light);
	float bias_min = 0.0005f;
	float bias_max = 0.5f;
	float bias = max(bias_max * (1.0f - dot(normal, light_direction)), bias_min); 

	// Smoothens out the shadows using radius (not only 1..0)
	int sample_radius = 2;
	float offset = 0.02f;
	for(int z = -sample_radius; z <= sample_radius; z++)
	{
		for(int y = -sample_radius; y <= sample_radius; y++)
		{
			for(int x = -sample_radius; x <= sample_radius; x++)
			{
				float closest_depth = texture(u_shadowcubemap, fragment_to_light + vec3(x, y, z) * offset).r;
				closest_depth *= u_far_plane;
				if(current_depth > closest_depth + bias)
				{
					shadow += 1.0f;
				}
			}
		}
	}
	shadow /= pow((sample_radius * 2 + 1), 3);

	return (texture(u_diffuse0, io_texture_coordinator) * (diffuse * (1.0f - shadow) * intensity + ambient) + texture(u_specular0, io_texture_coordinator).r * specular * (1.0f - shadow) * intensity) * u_light_color;
}

vec4 directLight()
{
	//Ambient Lighting
	float ambient = 0.2f;

	//Diffuse Lighting
	vec3 normal = normalize(io_normal_vector);
	vec3 light_direction = normalize(u_light_position);
	float diffuse = max(dot(normal, light_direction), 0.0f);
	
	//Specular Lighting
	float specular = 0.0f;
	if(diffuse != 0.0f)
	{
		float specular_light = 0.50f;
		vec3 view_direction = normalize(u_camera_position - io_position);
		vec3 halfway_vector = normalize(view_direction + light_direction);
		float specular_amount = pow(max(dot(normal, halfway_vector), 0.0f), 16);
		specular = specular_amount * specular_light;
	}

	// Shadow value
	float shadow = 0.0f;
	// Sets lightCoords to cull space
	vec3 light_coordination = io_fragment_light_position.xyz / io_fragment_light_position.w;
	if(light_coordination.z <= 1.0f)
	{
		// Get from [-1, 1] range to [0, 1] range just like the shadow map
		light_coordination = (light_coordination + 1.0f) / 2.0f;
		float current_depth = light_coordination.z;

		// Bias to put down some mapping to can prevents shadow acne
		float bias_min = 0.0005f;
		float bias_max = 0.025f;
		float bias = max(bias_max * (1.0f - dot(normal, light_direction)), bias_min);

		// Smoothens out the shadows using radius (not only 1..0)
		int sample_radius = 2;
		vec2 pixel_size = 1.0 / textureSize(u_shadowmap, 0);
		for(int y = -sample_radius; y <= sample_radius; y++)
		{
			for(int x = -sample_radius; x <= sample_radius; x++)
			{
				float closest_depth = texture(u_shadowmap, light_coordination.xy + vec2(x, y) * pixel_size).r;
				if(current_depth > closest_depth + bias)
				{
					shadow += 1.0f;
				}
			}
		}
		// Get average shadow from sample radius
		shadow /= pow((sample_radius * 2 + 1), 2);
	}
	
	return (texture(u_diffuse0, io_texture_coordinator) * (diffuse * (1.0f - shadow) + ambient) + texture(u_specular0, io_texture_coordinator).r * specular * (1.0f - shadow)) * u_light_color;
}

//outerCone and innerCone use cos of angle.
vec4 spotLight()
{
	float outer_cone = 0.90f;
	float inner_cone = 0.95f;

	//Ambient Lighting
	float ambient = 0.2f;

	//Diffuse Lighting
	vec3 normal = normalize(io_normal_vector);
	vec3 light_direction = normalize(u_light_position - io_position);
	float diffuse = max(dot(normal, light_direction), 0.0f);
	
	//Specular Lighting
	float specular = 0.0f;
	if(diffuse != 0.0f)
	{
		float specular_light = 0.50f;
		vec3 view_direction = normalize(u_camera_position - io_position);
		vec3 halfway_vector = normalize(view_direction + light_direction);
		float specular_amount = pow(max(dot(normal, halfway_vector), 0.0f), 16);
		specular = specular_amount * specular_light;
	}

	//Spotlight Area
	float angle = dot(vec3(0.0f, -1.0f, 0.0f), -light_direction);
	float intensity = clamp((angle - outer_cone) / (inner_cone - outer_cone), 0.0f, 1.0f);

	// Shadow value
	float shadow = 0.0f;
	// Sets lightCoords to cull space
	vec3 light_coordination = io_fragment_light_position.xyz / io_fragment_light_position.w;
	if(light_coordination.z <= 1.0f)
	{
		// Get from [-1, 1] range to [0, 1] range just like the shadow map
		light_coordination = (light_coordination + 1.0f) / 2.0f;
		float current_depth = light_coordination.z;

		// Bias to put down some mapping to can prevents shadow acne
		float bias_min = 0.000005f;
		float bias_max = 0.00025f;
		float bias = max(bias_max * (1.0f - dot(normal, light_direction)), bias_min);

		// Smoothens out the shadows using radius (not only 1..0)
		int sample_radius = 2;
		vec2 pixel_size = 1.0 / textureSize(u_shadowmap, 0);
		for(int y = -sample_radius; y <= sample_radius; y++)
		{
			for(int x = -sample_radius; x <= sample_radius; x++)
			{
				float closest_depth = texture(u_shadowmap, light_coordination.xy + vec2(x, y) * pixel_size).r;
				if(current_depth > closest_depth + bias)
				{
					shadow += 1.0f;
				}
			}
		}
		// Get average shadow from sample radius
		shadow /= pow((sample_radius * 2 + 1), 2);
	}
	
	return (texture(u_diffuse0, io_texture_coordinator) * (diffuse * (1.0f - shadow) * intensity + ambient) + texture(u_specular0, io_texture_coordinator).r * specular * (1.0f - shadow) * intensity) * u_light_color;
}

void main()
{
	//FragColor = vec4(io_color, 1.0f);
	//FragColor = texture(u_diffuse0, io_texture_coordinator);
	FragColor = pointLight();
}
