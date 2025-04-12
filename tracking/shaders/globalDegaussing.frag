#version 150

uniform sampler2DRect tex0;
uniform float block_size;
uniform float corner;
uniform float time;

in vec2 vTexCoord;
out vec4 fragColor;

void main() {

    vec2 texSize = textureSize(tex0);



    vec2 block_pos = floor(vTexCoord);
    vec4 color = texture(tex0, block_pos);

    color.rgb = vec3(1.0) - color.rgb;

    fragColor = color;
}