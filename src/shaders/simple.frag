#version 420 core
out vec4 fragColor;
uniform vec4 vertexColor;

void main()
{
    fragColor = vertexColor;
}
