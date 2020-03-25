#version 330 core

// Define constants
#define M_PI 3.141593

// Specify the inputs to the fragment shader
in vec3 vert_position;
in vec3 vert_normal;
in vec2 vert_uv;
in float wave_height;
in vec3 light_view_position;
in vec4 light_space_frag_position;

// Material properties
uniform sampler2D material_diffuse;
uniform vec4 material_properties;

uniform sampler2DShadow	shadow_map;

// Light properties
uniform vec3 light_color;

uniform bool is_water;

out vec4 color;

float shadow_test() {
    float bias = max(0.05 * (1.0 - dot(vert_normal, light_view_position)), 0.005);
    vec3 proj_coords = light_space_frag_position.xyz / light_space_frag_position.w;
    proj_coords = proj_coords * 0.5 + 0.5;
    proj_coords.z -= bias;
    return texture(shadow_map, proj_coords);
}

void main()
{
     // Note: all calculations are in view space!
    vec3 L = normalize(light_view_position - vert_position); // Light vector
    vec3 V = normalize(-vert_position);
    vec3 R = reflect(-L, vert_normal);

    vec4 tex_out = texture(material_diffuse, vert_uv);
    if (tex_out.a < 0.2) {
        discard;
    }
    vec3 diffuse_tex = tex_out.rgb;

    if (is_water) {
        float step = smoothstep(0.1, 0.25, wave_height + (1.0 - diffuse_tex.r));
        diffuse_tex = mix(vec3(0, 0.467, 0.745) * 0.33, vec3(1.0, 1.0, 1.0), step);
    }

    float direct_light = shadow_test();

    vec3 ambient = light_color * diffuse_tex * material_properties.x;
    vec3 diffuse = light_color * diffuse_tex * max(0.0, dot(vert_normal, L)) * material_properties.y;
    vec3 specular = light_color * pow(max(0.0, dot(R, V)), material_properties.w) * material_properties.z;

    color = vec4(ambient + direct_light * (diffuse + specular), 1.0);
}
