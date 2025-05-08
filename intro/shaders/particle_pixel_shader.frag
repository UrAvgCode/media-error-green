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

    return offset;
}

vec4 scanlines(vec4 color) {
    color.rgb *= 1 - 0.8 * 0.5 + sin((vTexCoord.y - line_position)) * 0.8 * 0.5;
    return color;
}

void main() {
    vec2 texSize = textureSize(tex0);
    vec2 block_pos = floor((vTexCoord + offset()) / block_size) * block_size;

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
    vec4 color = colorSum / float(sampleCount);

    // Apply quality quantization
    color.rgb = floor(color.rgb * (quality * 256.0)) / (quality * 256.0);

    {
        float index = floor((vTexCoord.y - line_position) / 20.0);
        float value = mod(index, 2.0);
        if (value == 1.0) {
            //avgColor.g -= 0.3;
        }
    }
    
    {
        float index = floor((vTexCoord.y - line_position) / 200.0);
        float value = mod(index, 2.0);
        if (value == 1.0) {
            //avgColor.g /= 2;
        }
    }

    color = scanlines(color);

    fragColor = color;
}