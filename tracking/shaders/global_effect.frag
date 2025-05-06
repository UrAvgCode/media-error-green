#version 150

uniform sampler2DRect tex0;
uniform vec2 effect_position;

uniform float time;
uniform vec2 aspect;

in vec2 vTexCoord;
out vec4 fragColor;


bool is_in_range(float radius, float min_radius, float amplitude) {
    float mask_offset_x = sin(vTexCoord.x * 0.05 + time * 3.0) * amplitude;
    float mask_offset_y = sin(vTexCoord.y * 0.05 + time * 3.0) * amplitude;

    float mask_distance = distance(effect_position, vTexCoord + vec2(mask_offset_x, mask_offset_y));

    return mask_distance < radius && mask_distance > min_radius;
}


void main() {

    vec2 uv = vTexCoord / aspect;
    vec4 color = texture(tex0, uv);

    float dist = distance(uv, effect_position);

     // Create horizontal offset based on vertical position and time
    float offsetR = sin(vTexCoord.y * 0.05 + time * 3.0) * 10.0; // 10 = amplitude
    float offsetG = sin(vTexCoord.y * 0.07 + time * 2.5) * 10.0; // 10 = amplitude
    float offsetB = sin(vTexCoord.y * 0.09 + time * 2.0) * 10.0; // 10 = amplitude

    // Shift texture coordinate
    vec2 shiftedCoordR = vec2(vTexCoord.x + offsetR, vTexCoord.y);
    vec2 shiftedCoordG = vec2(vTexCoord.x + offsetG, vTexCoord.y);
    vec2 shiftedCoordB = vec2(vTexCoord.x + offsetB, vTexCoord.y);

    float r = texture(tex0, shiftedCoordR).r;
    float g = texture(tex0, shiftedCoordG).g;
    float b = texture(tex0, shiftedCoordB).b;

    // Invert each channel
    vec4 invertedColor = vec4(1.0 - r, 1.0 - g, 1.0 - b, 1);

    float mask_offset_x = sin(vTexCoord.x * 0.05 + time * 3.0) * 10.0;
    float mask_offset_y = sin(vTexCoord.y * 0.05 + time * 3.0) * 10.0;
    float mask_size = 600;

    float mask_distance = distance(effect_position, vTexCoord + vec2(mask_offset_x, mask_offset_y));

    if (is_in_range(200, 0, 10)) {
        color = mix(invertedColor, vec4(1, 0, 0, 1), 0.1);
    }

    if (is_in_range(200, 0, 20)) {
        color = mix(invertedColor, vec4(0, 0, 1, 1), 0.1);
    }
     if (is_in_range(200, 0, 50)) {
        color = mix(invertedColor, vec4(0, 1, 0, 1), 0.1);
    }

    fragColor = color;
}