#version 120

uniform mat4 mvp;

attribute vec3 vposition;
attribute vec3 vnormal;

varying vec3 position;
varying vec3 normal;

void main()
{
	position = vposition;
	normal = vnormal;
	gl_Position = mvp * vec4(vposition, 1.0);
}
