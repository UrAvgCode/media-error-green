#version 150

uniform sampler2DRect tex0;
uniform float corner;
uniform float time;

in vec2 vTexCoord;
out vec4 fragColor;

void main() {

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
    vec3 invertedColor = vec3(1.0 - r, 1.0 - g, 1.0 - b);

    fragColor = vec4(invertedColor,1.0);
}