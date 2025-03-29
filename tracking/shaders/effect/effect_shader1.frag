#version 150

uniform sampler2DRect tex0;
uniform float block_size;
uniform float quality;

in vec2 vTexCoord;
out vec4 fragColor;

void main() {
    fragColor = texture(tex0, vTexCoord);
}