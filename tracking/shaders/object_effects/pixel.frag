#version 150

uniform sampler2DRect tex0;
uniform float block_size;
uniform float quality;

in vec2 vTexCoord;
out vec4 fragColor;

void main() {
    vec2 texSize = textureSize(tex0);
    vec2 block_pos = floor(vTexCoord / block_size) * block_size;

    vec4 colorSum = vec4(0.0);
    int sampleCount = 0;

    // Iterate over the pixels within the block size
    for (float y = 0.0; y < block_size; ++y) {
        for (float x = 0.0; x < block_size; ++x) {
            vec2 samplePos = block_pos + vec2(x, y);
            if (samplePos.x < texSize.x && samplePos.y < texSize.y) {
                colorSum += texture(tex0, samplePos);
                sampleCount++;
            }
        }
    }

    // Compute the average color
    vec4 avgColor = colorSum / float(sampleCount);

    // Apply quality quantization
    avgColor.rgb = floor(avgColor.rgb * (quality * 256.0)) / (quality * 256.0);

    fragColor = avgColor;
}
