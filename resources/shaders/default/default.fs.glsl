#version 100
precision mediump float;
uniform float alpha;
varying vec3 color;

void main()
{
     gl_FragColor = vec4(color, alpha);
}
