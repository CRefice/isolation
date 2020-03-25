#version 330 core

// Define constants
#define M_PI 3.141593
#define NUMBER_OF_WAVES 3

// Specify the input locations of attributes
layout (location = 0) in vec3 vert_coordinates_in;
layout (location = 1) in vec3 vert_normal_in;
layout (location = 2) in vec2 vert_uv_in;


// Specify the Uniforms of the vertex shader
uniform mat4x4 model, view, projection;
uniform mat3x3 normal_matrix;

// Light properties
uniform vec3 light_position;

// Wave properties
uniform float amplitude[NUMBER_OF_WAVES];
uniform float frequency[NUMBER_OF_WAVES];
uniform float phase[NUMBER_OF_WAVES];
uniform float time;

// Specify the output of the vertex stage
out vec3 vert_position;
out vec3 vert_normal;
out vec2 vert_uv;
out vec3 light_view_position;

out float wave_height;

float waveHeight(int idx, vec2 uv) {
    float r = length(uv - vec2(0.5, 0.5));
    return amplitude[idx] * sin(2.0 * M_PI * frequency[idx] * r + phase[idx] + time);
}

vec2 waveDeriv(int idx, vec2 uv) {
    vec2 pos = uv - vec2(0.5, 0.5);
    float r = length(pos);
    float angle = atan(pos.y, pos.x);
    float dr = 2.0 * M_PI * amplitude[idx] * frequency[idx] * cos(2.0 * M_PI * frequency[idx] * r + phase[idx] + time);
    return vec2(dr * cos(angle), dr * sin(angle));
}

void main() {
    vec3 pos = vert_coordinates_in;

    float offset = 0.0;
    vec2 deriv = vec2(0.0, 0.0);
    for (int i = 0; i < NUMBER_OF_WAVES; ++i) {
        offset += waveHeight(i, vert_uv_in);
        deriv += waveDeriv(i, vert_uv_in);
    }

    pos += vert_normal_in * offset;
    wave_height = offset;
    vec3 normal = normalize(vec3(-deriv, 1.0));

    // Note: all calculations are in view space!
    vert_position = vec3(view * model * vec4(pos, 1.0));
    vert_normal = normalize(normal_matrix * normal); // Normal vector

    vert_uv = vert_uv_in;
    light_view_position = vec3(view * vec4(light_position, 1.0));

    gl_Position = projection * vec4(vert_position, 1.0);
}
