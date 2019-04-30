#version 330 core

in vec2 texCoord;

out vec4 FragColor;

uniform sampler2D texture_container;
uniform sampler2D texture_awesomeface;

void main()
{
    FragColor = mix(texture(texture_container, texCoord),
					texture(texture_awesomeface, texCoord),
					0.2);
}
