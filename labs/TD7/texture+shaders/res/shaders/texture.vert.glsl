#version 450

uniform mat4 mvp;

in vec3 in_pos;
in vec2 in_coord;

out vec2 texcoord;

void main(void)
{
  gl_Position = mvp * vec4( in_pos, 1.0 );
  texcoord = in_coord;
}
