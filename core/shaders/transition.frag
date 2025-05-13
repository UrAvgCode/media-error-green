#version 150

uniform sampler2DRect tex0;
uniform sampler2DRect transition_tex;

uniform float progress;
uniform float random_offset_one;
uniform float random_offset_two;

in vec2 v_texcoord;
out vec4 frag_color;

void main() {
    vec4 current_color = texture(tex0, v_texcoord);

    const float glitch_width = 50;
    const float glitch_height = 8;

    vec2 offset = vec2(0, 0);

    if (v_texcoord.y < random_offset_one + glitch_height && v_texcoord.y > random_offset_one - glitch_height) {
        offset.x += glitch_width;
    }

    if (v_texcoord.y < random_offset_two + glitch_height && v_texcoord.y > random_offset_two - glitch_height) {
        offset.x -= glitch_width;
    }

    vec4 transition_color = texture(transition_tex, v_texcoord + offset);

    float fade = pow(progress, 10.0);
    vec4 color = mix(transition_color, current_color, fade);

    frag_color = color;
}