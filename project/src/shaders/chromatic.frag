#version 150

uniform sampler2DRect tex0;
uniform float aberrationAmount;

in vec2 vTexCoord;
out vec4 fragColor;

void main() {
    vec2 offset = vec2(aberrationAmount, 0.0);

    vec3 color;
    color.r = texture(tex0, vTexCoord + offset).r;
    color.g = texture(tex0, vTexCoord).g;
    color.b = texture(tex0, vTexCoord - offset).b;

    float alpha = 1.0;

    fragColor = vec4(color, alpha);
}