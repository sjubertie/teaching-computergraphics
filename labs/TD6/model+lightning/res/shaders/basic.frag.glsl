#version 450

in vec3 N;
in vec3 V;

// couleur émise pour le pixel
out vec4 frag_color;

void main(void)
{
  vec4 ambient = vec4( 0.0, 0.1, 0.0, 1.0 ); // lumière ambiante
  vec4 diffuse;
  vec4 spec;
  
  // L est le vecteur unitaire depuis le point en direction de la lumière
  vec3 L = normalize( /*position de la lumière*/vec3( 1.0, 0.0, 0.0 ) - V );
  
  // Le produit scalaire en N et L indique l'orientation de la face par rapport à la lumière
  // N et L sont normalisés donc le max est à 1 le min à -1.
  // Si 1 alors la face est perpendiculaire à la lumière et lui fait face donc éclairage diffus max.
  // Si valeur négative, alors la face n'est pas éclairée.
  diffuse = clamp( /*couleur diffuse*/vec4( 1.0, 1.0, 0.0, 1.0 ) * max( dot( N, L ), 0.0 ), 0.0, 1.0 );

  //vec3 E = normalize( -V );
  //vec3 R = normalize( reflect( -L, N ) );
  //spec = clamp( /*couleur spéculaire*/vec4( 0.7, 0.7, 0.7, 1.0 ) * pow( max( dot( R, E ), 0.0 ), 0.2 ), 0.0, 1.0 );

  // on assemble toutes les composantes pour obtenir la couleur finale
  frag_color = ambient + diffuse;// + spec;
}

