#version 100
precision mediump float;
uniform mat4 mvp;
attribute vec3 vcol;
attribute vec2 vpos;
varying vec3 color;

void main()
{
	gl_Position = mvp * vec4(vpos, 0.0, 1.0);
	color = vcol;
}

