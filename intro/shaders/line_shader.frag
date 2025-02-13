#version 330 core

uniform sampler2DRect logo_texture;

in vec4 gColor;
out vec4 FragColor;

void main()
{
    if (texture(logo_texture, gl_FragCoord.xy).r == 0) {
        FragColor = mix(gColor, vec4(0, 0, 0, 1), 0.5);
    } else {
        FragColor = gColor;
    }
}