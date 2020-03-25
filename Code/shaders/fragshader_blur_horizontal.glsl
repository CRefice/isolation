#version 330 core

out vec4 color;

in vec2 vert_uv;

uniform sampler2D screen_texture;
uniform float weight[5] = float[] (0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216);

void main()
{
    vec2 tex_offset = 1.0 / textureSize(screen_texture, 0); // gets size of single texel
    vec3 result = texture(screen_texture, vert_uv).rgb * weight[0]; // current fragment's contribution

    for(int i = 1; i < 5; ++i)
    {
        result += texture(screen_texture, vert_uv + vec2(tex_offset.x * i, 0.0)).rgb * weight[i];
        result += texture(screen_texture, vert_uv - vec2(tex_offset.x * i, 0.0)).rgb * weight[i];
    }

    color = vec4(result, 1.0);
}
