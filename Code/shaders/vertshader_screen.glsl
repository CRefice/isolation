#version 330 core

layout (location = 0) in vec3 vert_coordinates_in;
layout (location = 1) in vec3 vert_normal_in;
layout (location = 2) in vec2 vert_uv_in;

out vec2 vert_uv;

void main()
{
    gl_Position = vec4(vert_coordinates_in, 1.0);
    vert_uv = vert_uv_in;
}
