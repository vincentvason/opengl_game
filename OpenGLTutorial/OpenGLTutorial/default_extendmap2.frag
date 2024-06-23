#version 330
//Output
layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 BloomColor;

//Input from vertex shader (Must be same name with variable in .vert)
in vec3 io_position;
in vec3 io_normal_vector;
in vec3 io_color;
in vec2 io_texture_coordinator;
in vec3 io_light_position;
in vec3 io_camera_position;

//Uniform
uniform sampler2D u_diffuse0;
uniform sampler2D u_specular0;
uniform sampler2D u_normal0;
uniform sampler2D u_displacement0;

uniform vec4 u_light_color;

vec4 pointLight()
{
	//Intensity Attenuation
	float a = 1.0f;
	float b = 0.7f;
	vec3 light_vector = io_light_position - io_position;
	float distance = length(light_vector);
	float intensity = 1.0f / ((a * distance * distance) + (b * distance) + 1.0f);

	//Ambient Lighting
	float ambient = 0.05f;

	//Parallax Occlusion Mapping
	vec3 view_direction = normalize(io_camera_position - io_position);
	
	// Variables that control parallax occlusion mapping quality
	float height_scale = 0.05f;
	const float min_layers = 8.0f;
    const float max_layers = 64.0f;
    float num_layers = mix(max_layers, min_layers, abs(dot(vec3(0.0f, 0.0f, 1.0f), view_direction)));
	float layer_depths = 1.0f / num_layers;
	float current_layer_depths = 0.0f;
	
	// Remove the z division if you want less aberated results
	vec2 s_vector = view_direction.xy / view_direction.z * height_scale; 
    vec2 delta_uvs = s_vector / num_layers;
	
	vec2 texture_coordinator = io_texture_coordinator;
	float current_depth_map_value = 1.0f - texture(u_displacement0, texture_coordinator).r;
	
	// Loop till the point on the heightmap is "hit"
	while(current_layer_depths < current_depth_map_value)
    {
        texture_coordinator -= delta_uvs;
        current_depth_map_value = 1.0f - texture(u_displacement0, texture_coordinator).r;
        current_layer_depths += layer_depths;
    }

	// Apply Occlusion (interpolation with prev value)
	vec2 prev_texture_coordinator = texture_coordinator + delta_uvs;
	float after_depth  = current_depth_map_value - current_layer_depths;
	float before_depth = 1.0f - texture(u_displacement0, prev_texture_coordinator).r - current_layer_depths + layer_depths;
	float weight = after_depth / (after_depth - before_depth);
	texture_coordinator = prev_texture_coordinator * weight + texture_coordinator * (1.0f - weight);

	// Get rid of anything outside the normal range
	if(texture_coordinator.x > 1.0 || texture_coordinator.y > 1.0 || texture_coordinator.x < 0.0 || texture_coordinator.y < 0.0)
	{
		discard;
	}	

	//Diffuse Lighting
	//vec3 normal = normalize(io_normal_vector);
	vec3 normal = normalize(texture(u_normal0, texture_coordinator).xyz * 2.0f - 1.0f);
	vec3 light_direction = normalize(light_vector);
	float diffuse = max(dot(normal, light_direction), 0.0f);
	
	//Specular Lighting
	float specular = 0.0f;

	//Blinn-Phong Lighting
	if(diffuse != 0.0f)
	{
		float specular_light = 0.50f;
		vec3 view_direction = normalize(io_camera_position - io_position);

		//Blinn-Phong Lighting
		vec3 half_way_vector = normalize(view_direction + light_direction);

		float specular_amount = pow(max(dot(normal, half_way_vector), 0.0f), 16);
		specular = specular_amount * specular_light;
	}
		
	return (texture(u_diffuse0, texture_coordinator) * (diffuse * intensity + ambient) + texture(u_specular0, texture_coordinator).r * specular * intensity) * u_light_color;
}

vec4 directLight()
{
	//Ambient Lighting
	float ambient = 0.2f;

	//Diffuse Lighting
	vec3 normal = normalize(io_normal_vector);
	vec3 light_direction = normalize(vec3(1.0f, 1.0f, 0.0f));
	float diffuse = max(dot(normal, light_direction), 0.0f);
	
	//Specular Lighting
	float specular = 0.0f;

	//Blinn-Phong Lighting
	if(diffuse != 0.0f)
	{
		float specular_light = 0.50f;
		vec3 view_direction = normalize(io_camera_position - io_position);
		vec3 reflection_direction = reflect(-light_direction, normal);

		//Blinn-Phong Lighting
		vec3 half_way_vector = normalize(view_direction + light_direction);

		float specular_amount = pow(max(dot(normal, half_way_vector), 0.0f), 16);
		specular = specular_amount * specular_light;
	}
	
	return (texture(u_diffuse0, io_texture_coordinator) * (diffuse + ambient) + texture(u_specular0, io_texture_coordinator).r * specular) * u_light_color;
}

//outerCone and innerCone use cos of angle.
vec4 spotLight(float outer_cone, float inner_cone)
{
	//Ambient Lighting
	float ambient = 0.2f;

	//Diffuse Lighting
	vec3 normal = normalize(io_normal_vector);
	vec3 light_direction = normalize(io_light_position - io_position);
	float diffuse = max(dot(normal, light_direction), 0.0f);
	
	//Specular Lighting
	float specular = 0.0f;

	//Blinn-Phong Lighting
	if(diffuse != 0.0f)
	{
		float specular_light = 0.50f;
		vec3 view_direction = normalize(io_camera_position - io_position);
		vec3 reflection_direction = reflect(-light_direction, normal);

		//Blinn-Phong Lighting
		vec3 half_way_vector = normalize(view_direction + light_direction);

		float specular_amount = pow(max(dot(normal, half_way_vector), 0.0f), 16);
		specular = specular_amount * specular_light;
	}

	//Spotlight Area
	float angle = dot(vec3(0.0f, -1.0f, 0.0f), -light_direction);
	float intensity = clamp((angle - outer_cone) / (inner_cone - outer_cone), 0.0f, 1.0f);
	
	return (texture(u_diffuse0, io_texture_coordinator) * (diffuse * intensity + ambient) + texture(u_specular0, io_texture_coordinator).r * specular * intensity) * u_light_color;
}


void main()
{
	//FragColor = vec4(io_color, 1.0f);
	//FragColor = texture(u_diffuse0, io_texture_coordinator);
	FragColor = pointLight();

	//Bloom Color Code
	if(FragColor.r > 0.05f)
	{
		FragColor.r *= 5.0f;
	}


	// Calculate brightness by adding up all the channels with different weights each
	float brightness = dot(FragColor.rgb, vec3(0.2126f, 0.7152f, 0.0722f));
    if(brightness > 0.15f)
        BloomColor = vec4(FragColor.rgb, 1.0f);
    else
        BloomColor = vec4(0.0f, 0.0f, 0.0f, 1.0f);
}
