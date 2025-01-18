#version 150

uniform sampler2DRect tex0;
uniform sampler2DRect transition_tex;

uniform float progress;
uniform float rand1;
uniform float rand2;

in vec2 vTexCoord;
out vec4 fragColor;

void main() {
    vec3 current_color = texture(tex0, vTexCoord).rgb;

    vec2 offset = vec2(0, 0);

    float glitch_width = 50;
    float glitch_height = 8;

    if (vTexCoord.y < rand1 + glitch_height && vTexCoord.y > rand1 - glitch_height) {
        offset.x += glitch_width * (1.0 - progress);
    }

    if (vTexCoord.y < rand2 + glitch_height && vTexCoord.y > rand2 - glitch_height) {
        offset.x -= glitch_width * (1.0 - progress);
    }

    vec3 transition_color = texture(transition_tex, vTexCoord + offset).rgb;

    vec3 color = (progress * current_color) + ((1.0 - progress) * transition_color);

    float alpha = 1.0;
    fragColor = vec4(color, alpha);
}