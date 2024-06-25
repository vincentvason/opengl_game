#version 330 core

layout (location = 0) in vec2 l_position;
layout (location = 1) in vec2 l_texture_coordinator;

out vec2 io_texture_coordinator;

void main()
{
    gl_Position = vec4(l_position.x, l_position.y, 0.0, 1.0); 
    io_texture_coordinator = l_texture_coordinator;
}  