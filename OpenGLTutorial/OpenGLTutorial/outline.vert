//Layout
layout (location = 0) in vec3 l_position;
layout (location = 1) in vec3 l_normal_vector;

//Uniform variables
uniform mat4 u_camera_matrix;
uniform mat4 u_model_matrix;
uniform mat4 u_translation_matrix;
uniform mat4 u_rotation_matrix;
uniform mat4 u_scale_matrix;

//Have this uniform if using the first or second methods.
uniform float u_outline_scale;

void main()
{

	// First Method
	// Problem - it will not center at normal vector
	//vec3 position = vec3(u_model_matrix * u_translation_matrix * u_rotation_matrix * u_scale_matrix * u_outline_scale * vec4(l_position, 1.0f));
	//gl_Position = camMatrix * vec4(position, 1.0);

	// Second Method
	// Problem - it may have crisp border
	//vec3 position = vec3(u_model_matrix * u_translation_matrix * u_rotation_matrix * u_scale_matrix * vec4(l_position + l_normal_vector * (u_outline_scale - 1.0f), 1.0f));
	//gl_Position = camMatrix * vec4(position, 1.0);

	// Third Method
	// Simply have another outliner model, but it cost double memory size
	vec3 position = vec3(u_model_matrix * u_translation_matrix * u_rotation_matrix * u_scale_matrix * vec4(l_position, 1.0f));
	gl_Position = u_camera_matrix * vec4(position, 1.0);
}