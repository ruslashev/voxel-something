#version 120

uniform mat4 mvp;

attribute vec3 vposition;
attribute vec3 vcolor;

varying vec3 position;
varying vec3 color;

void main()
{
	position = vposition;
	color = vcolor;
	gl_Position = mvp * vec4(vposition, 1.0);
}
