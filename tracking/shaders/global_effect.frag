#version 150

uniform sampler2DRect tex0;
uniform vec2 effect_position;
uniform vec2 texture_size;

uniform int duration;
uniform int elapsed_time;

uniform float time;
uniform vec2 aspect;

in vec2 vTexCoord;
out vec4 fragColor;

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

void main() {
    vec4 color = texture(tex0, vTexCoord);

    float green_blend = blend_area(200.0, 40.0, 0.01);
    float red_blend = blend_area(400.0, 80.0, 0.02);
    float blue_blend = blend_area(600.0, 60, 0.03);

    color = mix(color, vec4(0.0, 0.0, 1.0, 1.0), blue_blend);
    color = mix(color, vec4(1.0, 0.0, 0.0, 1.0), red_blend);
    color = mix(color, vec4(0.0, 1.0, 0.0, 1.0), green_blend);

    fragColor = color;
}