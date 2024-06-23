#version 330

layout (triangles) in;
layout (line_strip, max_vertices = 6) out;

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
    gl_Position = data_in[0].projection_matrix * gl_in[0].gl_Position;
    EmitVertex();
    gl_Position = data_in[0].projection_matrix * (gl_in[0].gl_Position + 0.01f * (vec4(data_in[0].normal_vector, 0.0f)));
    EmitVertex();
    EndPrimitive();

    gl_Position = data_in[1].projection_matrix * gl_in[1].gl_Position;
    EmitVertex();
    gl_Position = data_in[1].projection_matrix * (gl_in[1].gl_Position + 0.01f * (vec4(data_in[1].normal_vector, 0.0f)));
    EmitVertex();
    EndPrimitive();

    gl_Position = data_in[2].projection_matrix * gl_in[2].gl_Position;
    EmitVertex();
    gl_Position = data_in[2].projection_matrix * (gl_in[2].gl_Position + 0.01f * (vec4(data_in[2].normal_vector, 0.0f)));
    EmitVertex();
    EndPrimitive();
}