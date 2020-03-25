#version 330 core

// Define constants
#define M_PI 3.141593

// Specify the inputs to the fragment shader
in vec3 vert_position;
in vec3 vert_normal;
in vec2 vert_uv;
in vec3 light_view_position;

in float wave_height;

// Material properties
uniform sampler2D material_diffuse;
uniform vec4 material_properties;

// Light properties
uniform vec3 light_color;

// Specify the output of the fragment shader
out vec4 color;

void main()
{
     // Note: all calculations are in view space!
    vec3 L = normalize(light_view_position - vert_position); // Light vector
    vec3 V = normalize(-vert_position);
    vec3 R = reflect(-L, vert_normal);

    float step = smoothstep(-0.1, 0.2, wave_height);
    vec3 diffuse_color = mix(vec3(0.1, 0.1, 0.9), vec3(1.0, 1.0, 1.0), step);

    vec3 ambient = light_color * diffuse_color * material_properties.x;
    vec3 diffuse = light_color * diffuse_color * max(0.0, dot(vert_normal, L)) * material_properties.y;
    vec3 specular = light_color * pow(max(0.0, dot(R, V)), material_properties.w) * material_properties.z;

    color = vec4(ambient + diffuse + specular, 1.0);
}
