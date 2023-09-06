#version 310 es
precision highp float;

uniform sampler2D tex;

layout(location=2) in vec2 texcoord;
layout(location=0) out vec4 fragColor;

void main()
{
     vec4 texcolor = texture(tex, texcoord);
     if(texcolor.a < 0.001)
	  discard;
     fragColor = texcolor * vec4(1.0, 1.0, 1.0, 1.0);
}
