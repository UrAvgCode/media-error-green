#version 430

layout(location = 0) in vec2 position;
layout(location = 1) in vec2 velocity;

void main() {
    gl_PointSize = 3.0;
    gl_Position = vec4((position / vec2(960.0, 540.0)) - 1.0, 0.0, 1.0);
}