#version 150

in vec4 v_color;
in vec3 v_normal;
in vec3 v_frag_pos;

out vec4 frag_color;

void main() {
    if (v_color.a == 0) {
        discard;
    }

    // hardcoded light properties
    vec3 light_dir = normalize(vec3(0.5, 0.7, 1.0));
    vec3 light_color = vec3(1.0, 1.0, 0.9);
    
    // ambient component
    float ambient_strength = 0.3;
    vec3 ambient = ambient_strength * light_color;
    
    // diffuse component
    vec3 normal = normalize(v_normal);
    float diff = max(dot(normal, light_dir), 0.0);
    vec3 diffuse = diff * light_color;
    
    // combine lighting with base color
    vec3 result = (ambient + diffuse) * v_color.rgb;
    
    frag_color = vec4(result, v_color.a);
}