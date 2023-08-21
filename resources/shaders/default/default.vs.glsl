#version 300 es
precision mediump float;

uniform mat4 mvp;

layout(location = 0) in vec3 vcol;
layout(location = 1) in vec3 vpos;

out vec3 color;

void main()
{
  gl_Position = mvp * vec4(vpos, 1.0);
  color = vcol;
}

