#version 150

uniform sampler2DRect tex0;
uniform float aberration_amount;
uniform float time;
uniform int random_offset_one;
uniform int random_offset_two;

in vec2 vTexCoord;
out vec4 fragColor;

void main() {
    vec2 offset = vec2(aberration_amount, 0.0);

    vec2 new_tex_coord = vec2(vTexCoord.x, vTexCoord.y);
    new_tex_coord.x += sin(new_tex_coord.y + time) * 10.0f;

    if (vTexCoord.y < random_offset_one + 5 && vTexCoord.y > random_offset_one - 5) {
        new_tex_coord.x += 30.0f;
    }

    if (vTexCoord.y < random_offset_two + 5 && vTexCoord.y > random_offset_two - 5) {
        new_tex_coord.x -= 30.0f;
    }

    vec3 color;
    color.r = texture(tex0, new_tex_coord + offset).r;
    color.g = texture(tex0, new_tex_coord).g;
    color.b = texture(tex0, new_tex_coord - offset).b;

    float alpha = 1.0;

    fragColor = vec4(color, alpha);
}