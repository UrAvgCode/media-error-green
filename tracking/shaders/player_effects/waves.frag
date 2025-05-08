#version 150

uniform sampler2DRect tex0;
uniform float time;

in vec2 vTexCoord;
out vec4 fragColor;

vec2 barrel_distortion(vec2 coord) {
    vec2 texture_size = textureSize(tex0);
    coord /= texture_size;

    coord = coord * 2.0 - 1.0;
    float radial = dot(coord, coord);
    float strength = 0.5 + sin(time) * 0.5;
    coord *= 1.0 + strength * radial;
    coord = (coord + 1.0) * 0.5;

    coord *= texture_size;
    return coord;
}

void main() {
    vec2 texture_size = textureSize(tex0);

    vec2 uv = vTexCoord / texture_size;
    vec2 center = vec2(0.5, 0.5);
    vec2 toCenter = uv - center;

    float dist = length(toCenter);

    // Wave parameters
    float frequency = 20.0;  // Number of waves
    float speed = 0.5;       // Speed of wave animation
    float amplitude = 0.015; // Maximum distortion amount

    // Compute wave offset using a sine function
    float wave = sin(dist * frequency - time * speed) * amplitude;

    // Apply distortion in the direction away from the center
    vec2 distortedUV = uv + (toCenter / dist) * wave;

    distortedUV *= texture_size;

    // Sample the texture
    vec4 color = texture(tex0, distortedUV);

    fragColor = color;
}