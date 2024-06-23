#version 330 core

layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

out vec3 io_normal_vector;
out vec3 io_color;
out vec2 io_texture_coordinator;
out vec3 io_position;
out vec3 io_light_position;
out vec3 io_camera_position;

in DATA
{
    vec3 normal_vector;
	vec3 color;
	vec2 texture_coordinator;
    mat4 projection_matrix;
    mat4 model_matrix;
    vec3 light_position;
	vec3 camera_position;
} data_in[];



// Default main function
void main()
{
    //https://ogldev.org/www/tutorial26/tutorial26.html

    // Edges of the triangle
    // edge0 = E1(x,y,z), edge1 = E2(x,y,z)
    vec3 edge0 = gl_in[1].gl_Position.xyz - gl_in[0].gl_Position.xyz;
    vec3 edge1 = gl_in[2].gl_Position.xyz - gl_in[0].gl_Position.xyz;

    // Lengths of UV differences
    // delta_uv1 = (deltaU1,deltaV1), delta_uv2 = (deltaU2,deltaV2)
    vec2 delta_uv0 = data_in[1].texture_coordinator - data_in[0].texture_coordinator;
    vec2 delta_uv1 = data_in[2].texture_coordinator - data_in[0].texture_coordinator;

    // [ E1x, E1y, E1z ] = [ deltaU1 deltaV1 ][Tx Ty Tz]
    // [ E2x, E2y, E2z ]   [ dettaU2 deltaV2 ][Bx By Bz]

    // [ deltaU1 deltaV1 ]^-1 [ E1x, E1y, E1z ] = [ deltaU1 deltaV1 ]^-1[ deltaU1 deltaV1 ][Tx Ty Tz]
    // [ deltaU2 deltaV2 ]    [ E2x, E2y, E2z ]   [ deltaU2 deltaV2 ]   [ dettaU2 deltaV2 ][Bx By Bz]

    // [ deltaU1 deltaV1 ]^-1 [ E1x, E1y, E1z ] = [Tx Ty Tz]
    // [ deltaU2 deltaV2 ]    [ E2x, E2y, E2z ]   [Bx By Bz]

    // 1 / det([ deltaU1 deltaV1 ])[  deltaV2 -deltaV1 ][ E1x, E1y, E1z ] = [Tx Ty Tz]
    //        ([ deltaU2 deltaV2 ])[ -deltaU2  deltaU1 ][ E2x, E2y, E2z ]   [Bx By Bz]

    // one over the determinant
    float det_delta_matrix = (delta_uv0.x * delta_uv1.y - delta_uv1.x * delta_uv0.y);
    vec3 tangent = vec3((1.0f / det_delta_matrix) * (delta_uv1.y * edge0 - delta_uv0.y * edge1));
    vec3 bitangent = vec3((1.0f / det_delta_matrix) * (-delta_uv1.x * edge0 + delta_uv0.x * edge1));

    vec3 t_normal = normalize(vec3(data_in[0].model_matrix * vec4(tangent, 0.0f)));
    vec3 b_normal = normalize(vec3(data_in[0].model_matrix * vec4(bitangent, 0.0f)));
    vec3 n_normal = normalize(vec3(data_in[0].model_matrix * vec4(cross(edge1, edge0), 0.0f)));

    mat3 tbn_matrix = mat3(t_normal, b_normal, n_normal);
    
    // TBN is an orthogonal matrix and so its inverse is equal to its transpose
    mat3 inv_tbn_matrix = transpose(tbn_matrix);

    gl_Position = data_in[0].projection_matrix * gl_in[0].gl_Position;
    io_normal_vector = data_in[0].normal_vector;
    io_color = data_in[0].color;
    io_texture_coordinator = data_in[0].texture_coordinator;
    
    // Change all lighting variables to TBN space
    io_position = inv_tbn_matrix * gl_in[0].gl_Position.xyz;
    io_light_position = inv_tbn_matrix * data_in[0].light_position;
    io_camera_position = inv_tbn_matrix * data_in[0].camera_position;
    EmitVertex();

    gl_Position = data_in[1].projection_matrix * gl_in[1].gl_Position;
    io_normal_vector = data_in[1].normal_vector;
    io_color = data_in[1].color;
    io_texture_coordinator = data_in[1].texture_coordinator;
    // Change all lighting variables to TBN space
    io_position = inv_tbn_matrix * gl_in[1].gl_Position.xyz;
    io_light_position = inv_tbn_matrix * data_in[1].light_position;
    io_camera_position = inv_tbn_matrix * data_in[1].camera_position;
    EmitVertex();

    gl_Position = data_in[2].projection_matrix * gl_in[2].gl_Position;
    io_normal_vector = data_in[2].normal_vector;
    io_color = data_in[2].color;
    io_texture_coordinator = data_in[2].texture_coordinator;
    // Change all lighting variables to TBN space
    io_position = inv_tbn_matrix * gl_in[2].gl_Position.xyz;
    io_light_position = inv_tbn_matrix * data_in[2].light_position;
    io_camera_position = inv_tbn_matrix * data_in[2].camera_position;
    EmitVertex();

    EndPrimitive();
}