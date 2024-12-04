#version 150

uniform sampler2DRect tex0;
uniform float aberrationAmount;
uniform float time;

in vec2 vTexCoord;
out vec4 fragColor;

void main() {
    vec2 offset = vec2(aberrationAmount, 0.0);

    vec2 new_tex_coord = vec2(vTexCoord.x, vTexCoord.y);

    new_tex_coord.x += sin(new_tex_coord.y + time) * 10.0f;

    vec3 color;
    color.r = texture(tex0, new_tex_coord + offset).r;
    color.g = texture(tex0, new_tex_coord).g;
    color.b = texture(tex0, new_tex_coord - offset).b;

    float alpha = 1.0;

    fragColor = vec4(color, alpha);
}