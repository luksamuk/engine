#version 310 es
precision mediump float;

layout(location = 1) uniform float alpha;

layout(location = 2) in vec3 color;
layout(location = 0) out vec4 fragColor;

void main()
{
  vec3 interm_color = mix(color, vec3(1.0, 1.0, 1.0), alpha);
  fragColor = vec4(interm_color, 1.0);
}
