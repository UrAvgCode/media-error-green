#version 150

uniform sampler2DRect tex0;
uniform float intensity;

in vec2 vTexCoord;
out vec4 fragColor;

void main() {

    vec4 color = texture(tex0, vTexCoord);

    color.g += (intensity) * 0.1 * sin(vTexCoord.y * 0.05);
    color.b += (intensity) * 0.1 * cos(vTexCoord.y * 0.05);
    color.r += (intensity) * 0.1 * cos((vTexCoord.y * 0.05) + 3.14);

    color.rgb = floor(color.rgb * (256.0)) / (256.0);

    fragColor = color;
}