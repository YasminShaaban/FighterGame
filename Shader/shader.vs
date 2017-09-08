#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoord;

out vec2 TexCoords;
out vec3 positionAgain;
out vec3 normalAgain;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform float time ;

vec4 tempPosition;

void main()
{	
    tempPosition = projection * view * model * vec4(position, 1.0f);
	tempPosition.y = tempPosition.y + 0.0*sin(time);
	gl_Position = tempPosition;
	positionAgain = vec3(tempPosition.x , tempPosition.y , tempPosition.z);
    TexCoords = vec2(texCoord.x, 1.0 - texCoord.y);
	normalAgain = normal;
}