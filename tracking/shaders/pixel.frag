#version 150

uniform sampler2DRect tex0;
uniform float block_size;
uniform float quality;

in vec2 vTexCoord;
out vec4 fragColor;

void main() {

    float block_size_distortion = 0;
    block_size_distortion += (floor(abs(((1920 / 2) - vTexCoord.x)) / 50) * 50) / 50;
    block_size_distortion += (floor(abs(((1080 / 2) - vTexCoord.y)) / 50) * 50) / 50;
    float distorted_block_size = block_size + block_size_distortion;

    if(distorted_block_size < 1) {
        distorted_block_size = 1;
    }

    vec2 block_pos = floor(vTexCoord / distorted_block_size) * distorted_block_size;
    vec4 color = texture(tex0, block_pos);


    fragColor = color;
}