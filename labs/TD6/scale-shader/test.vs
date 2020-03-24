#version 450

in vec3 in_pos;
in vec3 in_color;

uniform mat4 mvp;
uniform float scale;

out vec3 color;

void main(void)
{
  gl_Position = mvp * vec4( in_pos * scale, 1.0 );
  color = in_color;
}
