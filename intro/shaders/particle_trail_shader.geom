#version 330 core

layout(lines) in;
layout(line_strip, max_vertices = 2) out;

in vec4 vColor[];
out vec4 gColor;

uniform float uMaxLength;

void main()
{
    vec3 p0 = gl_in[0].gl_Position.xyz;
    vec3 p1 = gl_in[1].gl_Position.xyz;

    float length = distance(p0, p1);

    if (length <= uMaxLength)
    {
        gl_Position = gl_in[0].gl_Position;
        gColor = vColor[0];
        EmitVertex();

        gl_Position = gl_in[1].gl_Position;
        gColor = vColor[1];
        EmitVertex();

        EndPrimitive();
    }
}