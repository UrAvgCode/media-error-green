#version 150

uniform sampler2DRect tex0;
uniform sampler2DRect matrix_texture;

uniform int time;

in vec2 vTexCoord;
out vec4 fragColor;

void main() {
    int height = textureSize(matrix_texture).y;

    int horizontal_coord = int(vTexCoord.x);
    int vertical_coord = (int(vTexCoord.y) - time / 8) % height;

    vec3 color = texture(matrix_texture, vec2(horizontal_coord, vertical_coord)).rgb;
    float alpha = texture(tex0, vTexCoord).a;

    fragColor = vec4(color, alpha);
}