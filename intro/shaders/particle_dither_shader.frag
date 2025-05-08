#version 150

uniform sampler2DRect tex0;
uniform vec2 resolution;

in vec2 vTexCoord;
out vec4 fragColor;

// Bayer 4x4 Dither Matrix
int ditherMatrix4x4[16] = int[16](
     0,  8,  2, 10,
    12,  4, 14,  6,
     3, 11,  1,  9,
    15,  7, 13,  5
);

void main() {
    vec2 uv = vTexCoord;
    vec4 color = texture(tex0, uv);

    int x = int(mod(uv.x, 4.0));
    int y = int(mod(uv.y, 4.0));
    int index = y * 4 + x;

    float threshold = float(ditherMatrix4x4[index]) / 16.0;

    // Farb-Dithering pro Kanal
    vec3 ditheredColor;
    ditheredColor.r = color.r < threshold ? 0.0 : 1.0;
    ditheredColor.g = color.g < threshold ? 0.0 : 1.0;
    ditheredColor.b = color.b < threshold ? 0.0 : 1.0;

    fragColor = vec4(ditheredColor, 1.0);
}