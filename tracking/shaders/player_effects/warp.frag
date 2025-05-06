#version 150

uniform sampler2DRect tex0;
uniform float time;

in vec2 vTexCoord;
out vec4 fragColor;

void main() {
    vec2 offset = vec2(0.0, 0.0);

    float amplitude = 10.0;

    offset.x = sin(vTexCoord.y * 0.1 + time / 100.0) * amplitude;

    vec4 color = texture(tex0, vTexCoord + offset);

    fragColor = vec4(color);
}