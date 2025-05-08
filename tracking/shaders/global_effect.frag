#version 150

uniform sampler2DRect tex0;
uniform vec2 effect_position;

uniform int duration;
uniform int elapsed_time;

uniform float time;
uniform vec2 aspect;

uniform int line_position;

in vec2 vTexCoord;
out vec4 fragColor;

vec2 barrel_distortion(vec2 coord) {
    vec2 texture_size = textureSize(tex0);
    coord /= texture_size;

    coord = coord * 2.0 - 1.0;
    float radial = dot(coord, coord);
    float strength = 0.4;
    coord *= 1.0 + strength * radial;
    coord = (coord + 1.0) * 0.5;

    coord *= texture_size;
    return coord; 
}

vec4 scanlines(vec4 color) {
    vec2 distorted_coords = barrel_distortion(vTexCoord);
    color.g += 0.05 * sin((distorted_coords.y - line_position) * 0.5);
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
    blend = clamp(blend, 0.0, 1.0) * 0.5;

    return mix(color, vec4(0, 0, 0, 1), blend);
}

float blend_area(float radius, float amplitude, float thickness) {
    float horizontal_distance = abs(effect_position.x - vTexCoord.x);
    float vertical_distance = abs(effect_position.y - vTexCoord.y);
    float hyperbole_distance = horizontal_distance * vertical_distance;

    float distortion_x = sin(vTexCoord.x * 0.05 + time * 3.0) * amplitude;
    float distortion_y = sin(vTexCoord.y * 0.05 + time * 3.0) * amplitude;
    float distortion = distortion_x * distortion_y;
    float distorted_distance = hyperbole_distance + distortion;

    const float max_scale = 100;
    float progress = (float(elapsed_time) / float(duration));
    float scale = (1.0 - pow(2.0 * progress - 1.0, 2.0)) * max_scale;

    float scaled_distance = distorted_distance / scale;

    float fade_start = radius * 0.0;
    float blend = 1.0 - smoothstep(fade_start, radius, scaled_distance);
    blend = clamp(blend, 0.0, 1.0);

    float discrete_blend = step(0.1, sin(blend * 6.2831));

    if (blend == 1.0) {
        return 1.0;
    }

    float index = floor(pow(blend, 0.5) / thickness);

    return mod(index, 2.0);
}

vec4 apply_corner_effect(vec4 color) {
    if (elapsed_time < duration) {
        float green_blend = blend_area(200.0, 40.0, 0.01);
        float red_blend = blend_area(400.0, 80.0, 0.02);
        float blue_blend = blend_area(600.0, 60, 0.03);

        color = mix(color, vec4(0.0, 0.0, 1.0, 1.0), blue_blend);
        color = mix(color, vec4(1.0, 0.0, 0.0, 1.0), red_blend);
        color = mix(color, vec4(0.0, 1.0, 0.0, 1.0), green_blend);
    }

    return color;
}

void main() {
    vec4 color = texture(tex0, vTexCoord);
    color.a = 1.0;

    color = scanlines(color);
    color = vignette(color);
    color = apply_corner_effect(color);
    
    fragColor = color;
}