#version 450
// version du langage GLSL utilisée, ici 4.5

// mvp est la variable contenant la matrice proj*view*model
// uniform indique que c'est la même matrice pour tous les points
uniform mat4 mvp;
uniform mat4 mv;
uniform mat3 norm;

// in indique que la variable est fournie en entrée pour chaque point
// chaque point possède une position 3D
in vec3 in_pos;
in vec3 in_normal;

// On passe le point et sa normale au fragment shader
out vec3 N;
out vec3 V;


void main(void)
{
  // Position 3D du point dans le référentiel du monde 3D
  V = vec3( mv * vec4(in_pos, 1.0) );
  // Normale au point dans le référentiel du monde 3D
  // Les normales envoyées à la carte sont définies dans le référentiel du modèle 3D.
  N = normalize( norm * in_normal );

  gl_Position = mvp * vec4( in_pos, 1.0 );
}
