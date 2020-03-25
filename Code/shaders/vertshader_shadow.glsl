#version 330 core

// Define constants
#define M_PI 3.141593

// Specify the input locations of attributes
layout (location = 0) in vec3 vert_coordinates_in;
layout (location = 1) in vec3 vert_normal_in;
layout (location = 2) in vec2 vert_uv_in;

// Specify the Uniforms of the vertex shader
uniform mat4x4 model, view, projection;

uniform float[3] amplitude;
uniform float[3] frequency;
uniform float[3] phase;
uniform float time;

uniform sampler2D wave_mask;

out vec2 vert_uv;

float waveHeight(int idx, float x) {
    return amplitude[idx] * sin(2.0 * M_PI * frequency[idx] * x + phase[idx] + time);
}

void main() {
    vec3 world_position = vert_coordinates_in;
    float mask = texture(wave_mask, vert_uv_in).r;
    for (int i = 0; i < 3; ++i) {
            world_position += mask * waveHeight(i, world_position.y);
    }
    gl_Position = projection * view * model * vec4(world_position, 1.0);
    vert_uv = vert_uv_in;
}
