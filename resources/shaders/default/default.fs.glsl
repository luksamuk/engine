#version 100
precision mediump float;
uniform float alpha;
varying vec3 color;

void main()
{
     vec3 interm_color = mix(color, vec3(1.0, 1.0, 0.0), alpha);
     gl_FragColor = vec4(interm_color, 1.0);
}
