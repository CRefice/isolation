#version 330 core

// Define constants
#define M_PI 3.141593

out vec4 color;

in vec2 vert_uv;

uniform sampler2D screen_texture;

void main()
{
    color = texture(screen_texture, vert_uv);
    float brightness = dot(color.rgb, vec3(0.2126, 0.7152, 0.0722));
    if (brightness <= 3.0) {
        color = vec4(0.0, 0.0, 0.0, 1.0);
    }
}
