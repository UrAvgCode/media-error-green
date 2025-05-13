#version 150

uniform sampler2DRect tex0;
uniform sampler2DRect matrix_texture;

uniform int time;
uniform float speed;

in vec2 vTexCoord;
out vec4 fragColor;

void main() {
    vec2 matrix_size = textureSize(matrix_texture);

    int horizontal_coord = int(vTexCoord.x) % int(matrix_size.x);
    int vertical_coord = int(vTexCoord.y - time * speed) % int(matrix_size.y);

    vec3 color = texture(matrix_texture, vec2(horizontal_coord, vertical_coord)).rgb;
    float alpha = texture(tex0, vTexCoord).a;

    fragColor = vec4(color, alpha);
}