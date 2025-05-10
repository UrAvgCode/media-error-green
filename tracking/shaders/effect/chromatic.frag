#version 150

uniform sampler2DRect tex0;

uniform float aberration_amount;

in vec2 vTexCoord;
out vec4 fragColor;

void main() {
    vec2 offset = vec2(aberration_amount, 0);

    float green = texture(tex0, vTexCoord).g;
    float red = max(texture(tex0, vTexCoord + offset).g - green, 0.0);
    float blue = max(texture(tex0, vTexCoord - offset).g - green, 0.0);
    green = min(red + green + blue, 1.0);

    float green_alpha = texture(tex0, vTexCoord).a;
    float red_alpha = texture(tex0, vTexCoord + offset).a;
    float blue_alpha = texture(tex0, vTexCoord - offset).a;
    float alpha = min(green_alpha + red_alpha + blue_alpha, 1);

    vec4 color;
    color.r = red;
    color.g = green;
    color.b = blue;
    color.a = alpha;

    fragColor = color;
}
