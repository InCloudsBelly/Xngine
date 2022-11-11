#type vertex
#version 450 core

layout(location = 0) in vec2 aPos;

void main()
{
	gl_Position = vec4(aPos.x, aPos.y, 0.0, 1.0);
}

#type fragment
#version 450 core

out vec4 FragColor;

uniform sampler2D ComTexture;

void main()
{
	FragColor = texture(ComTexture, vec2(gl_FragCoord.xy) / vec2(textureSize(ComTexture, 0)));
}
