#version 300 es
precision mediump float;

uniform float alpha;

in vec3 color;

out vec4 fragColor;

void main()
{
  vec3 interm_color = mix(color, vec3(1.0, 1.0, 1.0), alpha);
  fragColor = vec4(interm_color, 1.0);
}
