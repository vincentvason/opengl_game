#version 330
layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

//Output to Shader
out vec3 io_position;
out vec3 io_normal_vector;
out vec3 io_color;
out vec2 io_texture_coordinator;

in DATA
{
    vec3 normal_vector;
	vec3 color;
	vec2 texture_coordinator;
    mat4 projection_matrix;
} data_in[];


// Default main function

void main()
{
    // ("Normal" Tutorial)
    vec4 surface_normal = vec4(0.0f); //Simply remove surface_normal
    
    // ("Explosive" Tutorial)
    /* 
    vec3 vector0 = vec3(gl_in[0].gl_Position - gl_in[1].gl_Position);
    vec3 vector1 = vec3(gl_in[2].gl_Position - gl_in[1].gl_Position);
    vec4 surface_normal = vec4(normalize(cross(vector0, vector1)), 0.0f);
    */

    gl_Position = data_in[0].projection_matrix * gl_in[0].gl_Position + surface_normal;
    io_position = vec3(data_in[0].projection_matrix * gl_in[0].gl_Position + surface_normal);
    io_normal_vector = data_in[0].normal_vector;
    io_color = data_in[0].color;
    io_texture_coordinator = data_in[0].texture_coordinator;
    EmitVertex();

    gl_Position = data_in[1].projection_matrix * gl_in[1].gl_Position + surface_normal;
    io_position = vec3(data_in[1].projection_matrix * gl_in[1].gl_Position + surface_normal);
    io_normal_vector = data_in[1].normal_vector;
    io_color = data_in[1].color;
    io_texture_coordinator = data_in[1].texture_coordinator;
    EmitVertex();

    gl_Position = data_in[2].projection_matrix * gl_in[2].gl_Position + surface_normal;
    io_position = vec3(data_in[2].projection_matrix * gl_in[2].gl_Position + surface_normal);
    io_normal_vector = data_in[2].normal_vector;
    io_color = data_in[2].color;
    io_texture_coordinator = data_in[2].texture_coordinator;
    EmitVertex();

    EndPrimitive();
}