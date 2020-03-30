#version 450

in vec2 texcoord;

out vec4 frag_color;

uniform sampler2D tex;

void main(void)
{
  frag_color = texture( tex, texcoord );//+ vec4( 0.1, 0.1, 0.1, 0.1 );
}
