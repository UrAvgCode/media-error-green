#version 150

uniform sampler2DRect tex0;
uniform int time;
uniform float min_grid_size;
uniform float max_grid_size;
uniform float speed;
uniform float tolerance;

in vec2 vTexCoord;
out vec4 fragColor;

void main() {
    float amplitude = (max_grid_size - min_grid_size) / 2.0;
    float offset = (max_grid_size + min_grid_size) / 2.0;
    float grid_size = amplitude * sin(time * speed) + offset;

    vec2 grid_coord;
    grid_coord.x = floor(vTexCoord.x / grid_size) * grid_size;
    grid_coord.y = floor(vTexCoord.y / grid_size) * grid_size;

    bool on_horizontal_grid = grid_coord.x > vTexCoord.x - tolerance && grid_coord.x < vTexCoord.x + tolerance;
    bool on_vertical_grid = grid_coord.y > vTexCoord.y - tolerance && grid_coord.y < vTexCoord.y + tolerance;

    if (on_horizontal_grid && on_vertical_grid) {
        vec4 color = texture(tex0, vTexCoord);
        fragColor = vec4(color);
    } else {
        fragColor = vec4(0);
    }
}