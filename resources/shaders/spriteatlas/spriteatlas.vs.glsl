#version 310 es
precision mediump float;

uniform mat4 mvp;
uniform vec2 framecoord;
uniform vec2 framesize;

in vec3 vpos;

out vec2 texcoord;
out vec3 color;

void main()
{
  gl_Position = mvp * vec4(vpos, 1.0);
  vec2 texpos = (vec2(vpos) + 1.0) / 2.0;
  texcoord = (framecoord * framesize) + (texpos * framesize);
}
