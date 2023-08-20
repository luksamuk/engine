#version 100
precision mediump float;
uniform mat4 mvp;
attribute vec3 vcol;
attribute vec3 vpos;
varying vec3 color;

void main()
{
	gl_Position = mvp * vec4(vpos, 1.0);
	color = vcol;
}

