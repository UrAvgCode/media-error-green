#version 150

in vec4 gColor;
out vec4 outputColor;

void main()
{
    outputColor = vec4(gColor.rgb, gColor.a);
}