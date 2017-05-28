precision mediump float;

uniform vec2 resolution;
uniform float time;

void main()
{
  vec2 q = gl_FragCoord.xy / resolution.xy;
  vec2 uv = -1.0 + 2.0 * q;
  vec3 colour = vec3(uv.xy, abs(sin(time)));

  gl_FragColor = vec4(colour, 1.0);
}
