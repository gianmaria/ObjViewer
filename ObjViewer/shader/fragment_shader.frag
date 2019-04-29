#version 330 core

in vec3 ourColor;
in vec2 texCoord;
in vec3 ourPos;

out vec4 FragColor;

uniform sampler2D texture_container;
uniform sampler2D texture_awesomeface;
uniform float mix_value;

void main()
{
    FragColor = mix(texture(texture_container, texCoord),
					texture(texture_awesomeface, vec2(1.0-texCoord.x, texCoord.y)),
					mix_value);
}
