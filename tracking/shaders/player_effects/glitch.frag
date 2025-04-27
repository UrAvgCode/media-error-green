#version 150

uniform sampler2DRect tex0;
uniform float rand1;
uniform float rand2;

in vec2 vTexCoord;
out vec4 fragColor;

void main() {
    vec2 offset = vec2(0.0, 0.0);

    if (vTexCoord.y < rand1 + 5 && vTexCoord.y > rand1 - 5) {
        offset.x += 30.0f;
    }

    if (vTexCoord.y < rand2 + 5 && vTexCoord.y > rand2 - 5) {
        offset.x -= 30.0f;
    }

    vec4 color = texture(tex0, vTexCoord + offset);

    fragColor = vec4(color);
}