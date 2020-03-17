#version 450
// version du langage GLSL utilisée, ici 4.5

// mvp est la variable contenant la matrice proj*view*model
// uniform indique que c'est la même matrice pour tous les points
uniform mat4 mvp;

// in indique que la variable est fournie en entrée pour chaque point
// chaque point possède une position 3D
in vec3 in_pos;

void main(void)
{
  // calcul de la position du point une fois toutes les transformations appliquées
  gl_Position = mvp * vec4( in_pos, 1.0 );
}
