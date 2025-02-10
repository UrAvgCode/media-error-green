#version 150

uniform sampler2DRect tex0;
uniform float block_size;
uniform float quality;

in vec2 vTexCoord;
out vec4 fragColor;

void main() {
    vec2 texSize = textureSize(tex0);

    float block_size_addition = (floor(abs(((1920 / 2) - vTexCoord.x)) / 50) * 50) / 50;

    vec2 blockPos = floor(vTexCoord / (block_size + block_size_addition)) * (block_size + block_size_addition);

    vec4 color = texture(tex0, blockPos);

    color.r += (1.0 - quality) * 0.1 * sin(vTexCoord.y * 0.5);
    color.g += (1.0 - quality) * 0.1 * cos(vTexCoord.y * 0.5);

    color.rgb = floor(color.rgb * ( quality * 256.0)) / (quality * 256.0);

    fragColor = color;
}