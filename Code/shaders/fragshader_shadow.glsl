#version 330 core

in vec2 vert_uv;

uniform sampler2D material_diffuse;

void main() {
    vec4 tex_out = texture(material_diffuse, vert_uv);
    if (tex_out.a < 0.2) {
        discard;
    }
}
