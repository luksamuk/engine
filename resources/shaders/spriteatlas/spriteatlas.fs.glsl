#version 310 es
precision mediump float;

uniform sampler2D tex;

in vec2 texcoord;

out vec4 fragColor;

void main()
{
  fragColor = texture(tex, texcoord) * vec4(1.0, 1.0, 1.0, 1.0);
}
