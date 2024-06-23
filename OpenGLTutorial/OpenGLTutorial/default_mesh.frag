#version 330
//Output
out vec4 FragColor;

//Input from vertex shader (Must be same name with variable in .vert)
in vec3 io_position;
in vec3 io_normal_vector;
in vec3 io_color;
in vec2 io_texture_coordinator;

//Uniform
uniform sampler2D u_diffuse0;
uniform sampler2D u_specular0;

uniform vec4 u_light_color;
uniform vec3 u_light_position;
uniform vec3 u_camera_position;

vec4 pointLight()
{
	//Intensity Attenuation
	float a = 3.0f;
	float b = 0.7f;
	vec3 light_vector = u_light_position - io_position;
	float distance = length(light_vector);
	float intensity = 1.0f / ((a * distance * distance) + (b * distance) + 1.0f);

	//Ambient Lighting
	float ambient = 0.2f;

	//Diffuse Lighting
	vec3 normal = normalize(io_normal_vector);
	vec3 light_direction = normalize(light_vector);
	float diffuse = max(dot(normal, light_direction), 0.0f);
	
	//Specular Lighting
	float specular = 0.0f;

	//Blinn-Phong Lighting
	if(diffuse != 0.0f)
	{
		float specular_light = 0.50f;
		vec3 view_direction = normalize(u_camera_position - io_position);
		vec3 reflection_direction = reflect(-light_direction, normal);

		//Blinn-Phong Lighting
		vec3 half_way_vector = normalize(view_direction + light_direction);

		float specular_amount = pow(max(dot(normal, half_way_vector), 0.0f), 16);
		specular = specular_amount * specular_light;
	}
		
	return (texture(u_diffuse0, io_texture_coordinator) * (diffuse * intensity + ambient) + texture(u_specular0, io_texture_coordinator).r * specular * intensity) * u_light_color;
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
	float specular_light = 0.50f;
	vec3 view_direction = normalize(u_camera_position - io_position);
	vec3 reflection_direction = reflect(-light_direction, normal);
	float specular_amount = pow(max(dot(view_direction, reflection_direction), 0.0f), 16);
	float specular = specular_amount * specular_light;
	
	return (texture(u_diffuse0, io_texture_coordinator) * (diffuse + ambient) + texture(u_specular0, io_texture_coordinator).r * specular) * u_light_color;
}

//outerCone and innerCone use cos of angle.
vec4 spotLight(float outer_cone, float inner_cone)
{
	//Ambient Lighting
	float ambient = 0.2f;

	//Diffuse Lighting
	vec3 normal = normalize(io_normal_vector);
	vec3 light_direction = normalize(u_light_position - io_position);
	float diffuse = max(dot(normal, light_direction), 0.0f);
	
	//Specular Lighting
	float specular_light = 0.50f;
	vec3 view_direction = normalize(u_camera_position - io_position);
	vec3 reflection_direction = reflect(-light_direction, normal);
	float specular_amount = pow(max(dot(view_direction, reflection_direction), 0.0f), 16);
	float specular = specular_amount * specular_light;

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
}
