#version 310 es
precision highp float;

layout(location=0) uniform mat4 mvp;
layout(location=1) uniform vec2 framecoord;
layout(location=2) uniform vec2 framesize;

layout(location=0) in vec3 vpos;
layout(location=1) out vec3 color;
layout(location=2) out vec2 texcoord;

void main()
{
  gl_Position = mvp * vec4(vpos, 1.0);
  vec2 texpos = (vec2(vpos) + 1.0) / 2.0;
  texcoord = (framecoord * framesize) + (texpos * framesize);
}
