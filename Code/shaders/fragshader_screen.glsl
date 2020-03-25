#version 330 core

out vec4 color;

in vec2 vert_uv;

uniform sampler2D screen_texture;
uniform sampler2D bloom_texture;

void main()
{
    const float exposure = 0.2;
    const float gamma = 2.2;
    vec3 hdr = texture(screen_texture, vert_uv).rgb + texture(bloom_texture, vert_uv).rgb;

    vec3 mapped = vec3(1.0) - exp(-hdr * exposure);
    // gamma correction
    mapped = pow(mapped, vec3(1.0 / gamma));

    color = vec4(mapped, 1.0);
}
