#version 330 core

// Define constants
#define M_PI 3.141593

// Specify the input locations of attributes
layout (location = 0) in vec3 vert_coordinates_in;
layout (location = 1) in vec3 vert_normal_in;
layout (location = 2) in vec2 vert_uv_in;

// Specify the Uniforms of the vertex shader
uniform mat4x4 model, view, projection;

uniform mat4x4 light_view, light_projection;

uniform mat3x3 normal_matrix;

uniform float[6] amplitude;
uniform float[6] frequency;
uniform float[6] phase;
uniform float time;

uniform bool is_water;

uniform sampler2D wave_mask;

// Light properties
uniform vec3 light_position;

// Specify the output of the vertex stage
out vec3 vert_position;
out vec3 vert_normal;
out vec2 vert_uv;
out float wave_height;
out vec3 light_view_position;
out vec4 light_space_frag_position;

float waveHeight(int idx, float x) {
    return amplitude[idx] * sin(2.0 * M_PI * frequency[idx] * x + phase[idx] + time);
}

float waveHeight2(int idx, vec2 uv) {
    return amplitude[idx] * (sin(2.0 * M_PI * frequency[idx] * uv.y + phase[idx] + time))
            - amplitude[idx + 3] * (abs(sin(2.0 * M_PI * frequency[idx + 3] * uv.x + phase[idx + 3] + time)));
}

vec2 waveDeriv(int idx, vec2 uv) {
     float dy = 2.0 * M_PI * frequency[idx] * amplitude[idx] * cos(2.0 * M_PI * frequency[idx] * uv.y + phase[idx] + time);
     float dx = -(M_PI * amplitude[idx] * amplitude[idx + 3] * frequency[idx + 3] * sin(2 * (2 * M_PI * frequency[idx + 3] * uv.x + phase[idx + 3] + time)))
             / abs(sin(phase[idx + 3] + time + 2 * M_PI * frequency[idx + 3] * uv.x));
     return vec2(dx, dy);
}

void main()
{
    wave_height = 0.0;
    vec2 deriv = vec2(0.0, 0.0);
    float mask = texture(wave_mask, vert_uv_in).r;
    for (int i = 0; i < 3; ++i) {
        if (is_water) {
            wave_height += mask * waveHeight2(i, vert_uv_in);
            deriv += mask * waveDeriv(i, vert_uv_in);
        } else {
            wave_height += mask * waveHeight(i, vert_uv_in.y);
        }
    }

    vec3 world_position = vert_coordinates_in;
    vert_normal = vert_normal_in;
    if (is_water) {
        world_position += vert_normal_in * wave_height;
        vert_normal = normalize(vec3(-deriv.x, 1.0, -deriv.y));
    } else {
        world_position += wave_height;
    }

    // Note: all calculations are in view space!
    vert_position = vec3(view * model * vec4(world_position, 1.0));
    gl_Position = projection * vec4(vert_position, 1.0);

    vert_normal = normalize(normal_matrix * vert_normal); // Normal vector
    vert_uv = vert_uv_in;

    light_view_position = vec3(view * vec4(light_position, 1.0));
    light_space_frag_position = light_projection * light_view * model * vec4(world_position, 1.0);
}
