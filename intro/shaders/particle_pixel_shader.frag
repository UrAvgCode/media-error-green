#version 150

uniform sampler2DRect tex0;

uniform float block_size;
uniform float quality;

uniform float random;
uniform float random_width;
uniform float random_height;

uniform int line_position;

in vec2 vTexCoord;
out vec4 fragColor;

vec2 barrel_distortion(vec2 coord) {
    vec2 texture_size = textureSize(tex0);
    coord /= texture_size;

    coord = coord * 2.0 - 1.0;
    float radial = dot(coord, coord);
    float strength = 0.6;
    coord *= 1.0 + strength * radial;
    coord = (coord + 1.0) * 0.5;

    coord *= texture_size;
    return coord; 
}

vec2 offset() {
    vec2 offset = vec2(0, 0);
    if (random < 0.2) {
        
        float thickness = 14;
        if (vTexCoord.y > random_height - thickness && vTexCoord.y < random_height + thickness) {

            float wideness = 200;
            if (vTexCoord.x > random_width - wideness && vTexCoord.x < random_width + wideness) {
                offset.x += 20;
            }
        }
    }

    if (random > 0.9) {
        float thickness = 8;
        if (vTexCoord.y > random_height - thickness && vTexCoord.y < random_height + thickness) {
            offset.x += 20;
        }
    }

    const float thickness = 8;
    const int number_of_lines = 6;
    for (int i = 0; i < number_of_lines; ++i) {
        int current_pos = (line_position + i * (1080 / number_of_lines)) % 1080;
        if (vTexCoord.y > current_pos - thickness && vTexCoord.y < current_pos + thickness) {
            offset.x -= 8;
        }
    }

    return offset;
}

vec4 scanlines(vec4 color) {
    vec2 distorted_coords = barrel_distortion(vTexCoord);
    color.rgb *= 1 - 0.8 * 0.5 + sin((distorted_coords.y - line_position)) * 0.8 * 0.5;
    return color;
}

vec4 vignette(vec4 color) {
    vec2 texture_size = textureSize(tex0);
    float radius = 400.0;
    float scale = 32;

    float horizontal_distance = min(vTexCoord.x, texture_size.x - vTexCoord.x);
    float vertical_distance = min(vTexCoord.y, texture_size.y - vTexCoord.y);
    float hyperbole_distance = (horizontal_distance * vertical_distance) / scale;

    float blend = 1.0 - smoothstep(0, radius, hyperbole_distance);
    blend = clamp(blend, 0.0, 1.0);

    return mix(color, vec4(0, 0, 0, 1), blend);
}

void main() {
    vec2 texture_size = textureSize(tex0);
    vec2 block_pos = floor((vTexCoord + offset()) / block_size) * block_size;

    int sample_count = 0;
    vec4 color_sum = vec4(0.0);
    for (float y = 0.0; y < block_size; ++y) {
        for (float x = 0.0; x < block_size; ++x) {
            vec2 samplePos = block_pos + vec2(x, y);
            if (samplePos.x < texture_size.x && samplePos.y < texture_size.y) {
                color_sum += texture(tex0, samplePos);
                sample_count++;
            }
        }
    }

    // compute the average color
    vec4 color = color_sum / float(sample_count);

    // apply quality quantization
    color.rgb = floor(color.rgb * (quality * 256.0)) / (quality * 256.0);

    color = scanlines(color);
    color = vignette(color);

    fragColor = color;
}
