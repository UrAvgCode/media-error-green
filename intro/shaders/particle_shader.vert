#version 430

struct Particle {
    vec2 position;
    vec2 velocity;
};

layout(std430, binding = 0) buffer ParticleBuffer {
    Particle particles[];
};

uniform mat4 modelViewProjectionMatrix;
in uint particleID;

void main() {
    vec2 position = particles[particleID].position;
    gl_Position = modelViewProjectionMatrix * vec4(position, 0.0, 1.0);
}