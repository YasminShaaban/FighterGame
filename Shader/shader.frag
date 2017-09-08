#version 330 core


uniform sampler2D texture_ambient1;
uniform sampler2D texture_ambient2;
uniform sampler2D texture_ambient3;

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_diffuse2;
uniform sampler2D texture_diffuse3;
uniform sampler2D texture_specular1;
uniform sampler2D texture_specular2;



in vec2 TexCoords;
in vec3 positionAgain ;
in vec3 normalAgain;

out vec4 color;
vec4 color2;
void main()
{
	  color =  mix(texture(texture_diffuse1, TexCoords), texture(texture_diffuse2, TexCoords), 0.5);
	  color = mix (color , texture(texture_ambient1,TexCoords),.5);
	  color = mix (color , texture(texture_ambient2,TexCoords),.5);
	  if(color.x == 0 && color.y == 0 && color.z ==0)
	  {
	  float value1 = 1 + .1 * TexCoords.x* (normalAgain.x+.1)*(positionAgain.x+positionAgain.y+positionAgain.z+.1) ;
	  float value2 = .3*(normalAgain.y+.2) * (positionAgain.y+positionAgain.x+positionAgain.z) ;
	  float value3 = .3*(positionAgain.z+ positionAgain.x+positionAgain.y+.1)*(normalAgain.z+.1) ;
    color2 = vec4( .5 * ( 1+ cos (value1)*sin(value2)) ,
	.5 * (1+cos(value2)* sin(value3)),
	.5 *(1+sin(value3)* cos(value3)),
	0);
	if(color2.x == 0 && color2.y == 0 && color2.z ==0)
	{color2 = vec4(0,0,1,0);}
	color = color2;
	}
	}