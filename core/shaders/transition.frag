#version 150

uniform sampler2DRect tex0;
uniform sampler2DRect transition_tex;

uniform float progress;
uniform int random_offset_one;
uniform int random_offset_two;

in vec2 vTexCoord;
out vec4 fragColor;

void main() {
    vec3 current_color = texture(tex0, vTexCoord).rgb;

    const float glitch_width = 50;
    const float glitch_height = 8;

    vec2 offset = vec2(0, 0);

    if (vTexCoord.y < random_offset_one + glitch_height && vTexCoord.y > random_offset_one - glitch_height) {
        offset.x += glitch_width;
    }

    if (vTexCoord.y < random_offset_two + glitch_height && vTexCoord.y > random_offset_two - glitch_height) {
        offset.x -= glitch_width;
    }

    vec3 transition_color = texture(transition_tex, vTexCoord + offset).rgb;

    float fade = pow(progress, 10.0);
    vec3 color = mix(transition_color, current_color, fade);

    float alpha = 1.0;
    fragColor = vec4(color, alpha);
}