#version 150

uniform sampler2DRect tex0;
uniform float block_size;
uniform float quality;

in vec2 vTexCoord;
out vec4 fragColor;

void main() {
    float flipped_x_coord = 1920 - vTexCoord.x;
    vec2 flipped_tex_coord = vec2(flipped_x_coord, vTexCoord.y);

    vec2 texSize = textureSize(tex0);

    float block_size_distortion = 0;
    block_size_distortion += (floor(abs(((1920 / 2) - flipped_tex_coord.x)) / 50) * 50) / 50;
    block_size_distortion += (floor(abs(((1080 / 2) - flipped_tex_coord.y)) / 50) * 50) / 50;
    float distorted_block_size = block_size + block_size_distortion;

    if(distorted_block_size < 1) {
        distorted_block_size = 1;
    }

    vec2 block_pos = floor(flipped_tex_coord / distorted_block_size) * distorted_block_size;
    vec4 color = texture(tex0, block_pos);

    color.r += (1.0 - quality) * 0.1 * sin(flipped_tex_coord.y * 0.5);
    color.g += (1.0 - quality) * 0.1 * cos(flipped_tex_coord.y * 0.5);

    color.rgb = floor(color.rgb * ( quality * 256.0)) / (quality * 256.0);

    fragColor = color;
}