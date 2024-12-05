#version 150

uniform sampler2DRect tex0;
uniform int kernel_size;
uniform float sigma;

in vec2 vTexCoord;
out vec4 frag_color;

float gaussian(float x, float sigma) {
    return exp(-(x * x) / (2.0 * sigma * sigma)) / (2.0 * 3.141592653589793 * sigma * sigma);
}

void main() {
    vec3 color = vec3(0.0);
    float total_weight = 0.0;
    int half_size = kernel_size / 2;

    for (int x = -half_size; x <= half_size; ++x) {
        for (int y = -half_size; y <= half_size; ++y) {
            vec2 sample_coord = vTexCoord + vec2(x, y);
            float weight = gaussian(length(vec2(x, y)), sigma);
            color += texture(tex0, sample_coord).rgb * weight;
            total_weight += weight;
        }
    }

    color /= total_weight;

    frag_color = vec4(color, 1.0);
}