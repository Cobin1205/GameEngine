#version 330 core

out vec4 FragColor;

in vec2 texCoord;

uniform sampler2D ourTexture;
uniform vec4 ourColor;
uniform bool useTexture;

void main()
{
	if (useTexture){
		FragColor = texture(ourTexture, texCoord);
	}
	else{
		FragColor = ourColor;
	}
};