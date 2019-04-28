#version 330 core

layout (location=0) in vec3 aPos;
layout (location=1) in vec3 aColor;
layout (location=2) in vec2 aTexCoord;

out vec3 ourColor;
out vec2 texCoord;
out vec3 ourPos;

uniform float offset;
uniform int up_or_down;

void main()
{
    gl_Position = vec4(aPos.x + offset, aPos.y * up_or_down, aPos.z, 1.0);
    ourColor = aColor;
    ourPos = aPos;
    texCoord = aTexCoord;
}
