#version 450

in vec3 color;

// couleur émise pour le pixel
out vec4 frag_color;

void main(void)
{
  //frag_color = vec4( color, 1.0 );
  //frag_color = vec4( mod(ceil(gl_FragCoord.x/10)+ceil(gl_FragCoord.y/10), 2), 0.0, 0.0, 1.0 );
  frag_color = vec4( sin(gl_FragCoord.x/5), 0.0, 0.0, 1.0 );
}

