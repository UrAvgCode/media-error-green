#version 150

uniform sampler2DRect tex0;
uniform sampler2DRect transition_tex;

uniform float progress;

in vec2 vTexCoord;
out vec4 fragColor;

void main() {
    vec3 current_color = texture(tex0, vTexCoord).rgb;
    vec3 transition_color = texture(transition_tex, vTexCoord).rgb;

    vec3 color = (progress * current_color) + ((1.0 - progress) * transition_color);

    float alpha = 1.0;
    fragColor = vec4(color, alpha);
}