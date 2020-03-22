#include <iostream>
#include <vector>
#include <array>
#include <fstream>

#include <unistd.h>


#include <GL/glew.h>
#include <GL/freeglut.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

// Identifiant du programme shader.
unsigned int progid;

// Identifiant de la variable du shader contenant la matrice mvp.
unsigned int mvpid;
unsigned int mvid;
unsigned int normid;

// Matrices 4x4 contenant les transformations.
glm::mat4 model;
glm::mat4 view;
glm::mat4 proj;
glm::mat4 mv;
glm::mat4 mvp;
glm::mat3 norm;

// Variables pour la rotation.
float angle = 0.0f;
float inc = 0.001f;

// Identifiant pour le VAO.
unsigned int vaoids[ 1 ];

// Données pour le modèle.
unsigned int nbtriangles;
float scale;
float x, y, z;


// Position de la caméra.
std::array< float, 3 > eye = { 0.0f, 0.0f, 1.0f };


void display()
{
  glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

  // Positionnement de la caméra en ( 0.0f, 0.0f, 5.0f ),
  // on regarde en direction du point ( 0.0f, 0.0f, 0.0f ),
  // la tête est orienté suivant vers le haut l'axe y ( 0.0f, 1.0f, 0.0f ).
  view = glm::lookAt( glm::vec3( eye[ 0 ], eye[ 1 ], eye[ 2 ] ), glm::vec3( eye[ 0 ], eye[ 1 ], eye[ 2 ]-1.0f ), glm::vec3( 0.0f, 1.0f, 0.0f ) );

  // Le repère subit une rotation suivant l'axe z.
  view = glm::rotate( view, glm::degrees( angle ), glm::vec3( 0.0f, 1.0f, 0.0f ) );

  // Le modèle est mis à l'échelle et recentré.
  model = glm::scale( model, glm::vec3( scale ) );
  model = glm::translate( glm::mat4( 1.0f ), glm::vec3( -x, -y, -z ) );
  
  // Calcul des matrices mv, mvp et norm.
  mv = view * model;
  mvp = proj * mv;
  glm::mat4 tmp = glm::transpose( glm::inverse( mv ) );
  for( std::size_t j = 0 ; j < 3 ; ++j )
  {
    for( std::size_t i = 0 ; i < 3 ; ++i )
    {
      norm[ j ][ i ] = tmp[ j ][ i ];
    }
  }

  // Passage des matrices au shader.
  glUniformMatrix4fv( mvid , 1, GL_FALSE, &mv[0][0]);
  glUniformMatrix4fv( mvpid , 1, GL_FALSE, &mvp[0][0]);
  glUniformMatrix3fv( normid , 1, GL_FALSE, &norm[0][0]);

  glBindVertexArray( vaoids[ 0 ] );
  glDrawElements( GL_TRIANGLES, 3 * nbtriangles, GL_UNSIGNED_INT, 0 );
  
  glutSwapBuffers();
}


void idle()
{
  angle += inc;
  if( angle >= 360.0f )
  {
    angle = 0.0f;
  }
  
  glutPostRedisplay();
}


void reshape( int w, int h )
{
  glViewport(0, 0, w, h);
  // Modification de la matrice de projection à chaque redimensionnement de la fenêtre.
  proj = glm::perspective( 45.0f, w/static_cast< float >( h ), 0.1f, 100.0f );  
}


void special( int key, int x, int y )
{
  switch( key )
  {
  case GLUT_KEY_LEFT:
    eye[ 0 ] -= 0.1f;
    break;
  case GLUT_KEY_RIGHT:
    eye[ 0 ] += 0.1f;
    break;
  case GLUT_KEY_UP:
    eye[ 2 ] -= 0.1f;
    break;
  case GLUT_KEY_DOWN:
    eye[ 2 ] += 0.1f;
    break;
  }
  glutPostRedisplay();
}


void initVAOs()
{
  unsigned int vboids[ 3 ];

  std::ifstream ifs( "res/models/rabbit.off" );

  std::string off;

  unsigned int nbpoints, tmp;
  
  ifs >> off;
  ifs >> nbpoints;
  ifs >> nbtriangles;
  ifs >> tmp;

  std::vector< float > vertices( nbpoints * 3 );
  std::vector< unsigned int > indices( nbtriangles *3 );
  std::vector< float > normals( vertices.size() );

  //std::fill( std::begin( normals ), std::end( normals ), 0.0f );
  
  for( unsigned int i = 0 ; i < vertices.size() ; ++i) {
    ifs >> vertices[ i ];
  }

  for( unsigned int i = 0 ; i < nbtriangles ; ++i) {
    ifs >> tmp;
    ifs >> indices[ i * 3 ];
    ifs >> indices[ i * 3 + 1 ];
    ifs >> indices[ i * 3 + 2 ];
  }

  /**
   * Calcul de la boîte englobante du modèle
   */
  float dx, dy, dz;
  float xmin, xmax, ymin, ymax, zmin, zmax;

  xmin = xmax = vertices[0];
  ymin = ymax = vertices[1];
  zmin = zmax = vertices[2];
  for(unsigned int i = 1 ; i < nbpoints ; ++i) {
    if(xmin > vertices[i*3]) xmin = vertices[i*3];
    if(xmax < vertices[i*3]) xmax = vertices[i*3];
    if(ymin > vertices[i*3+1]) ymin = vertices[i*3+1];
    if(ymax < vertices[i*3+1]) ymax = vertices[i*3+1];
    if(zmin > vertices[i*3+2]) zmin = vertices[i*3+2];
    if(zmax < vertices[i*3+2]) zmax = vertices[i*3+2];
  }

  // calcul du centre de la boîte englobante

  x = (xmax + xmin)/2.0f;
  y = (ymax + ymin)/2.0f;
  z = (zmax + zmin)/2.0f;

  // calcul des dimensions de la boîte englobante

  dx = xmax - xmin;
  dy = ymax - ymin;
  dz = zmax - zmin;

  // calcul du coefficient de mise à l'échelle

  scale = 1.0f/fmax(dx, fmax(dy, dz));

  /**
   * Calcul des normales.
   */
  for( std::size_t i = 0 ; i < indices.size() ; i+=3 )
  {
    auto x0 = vertices[ 3 * indices [ i ]     ] - vertices[ 3 * indices [ i+1 ]     ];
    auto y0 = vertices[ 3 * indices [ i ] + 1 ] - vertices[ 3 * indices [ i+1 ] + 1 ];
    auto z0 = vertices[ 3 * indices [ i ] + 2 ] - vertices[ 3 * indices [ i+1 ] + 2 ];

    auto x1 = vertices[ 3 * indices [ i ]     ] - vertices[ 3 * indices [ i+2 ]     ];
    auto y1 = vertices[ 3 * indices [ i ] + 1 ] - vertices[ 3 * indices [ i+2 ] + 1 ];
    auto z1 = vertices[ 3 * indices [ i ] + 2 ] - vertices[ 3 * indices [ i+2 ] + 2 ];

    auto x01 = y0 * z1 - y1 * z0;
    auto y01 = z0 * x1 - z1 * x0;
    auto z01 = x0 * y1 - x1 * y0;
    
    auto norminv = 1.0f / std::sqrt( x01 * x01 + y01 * y01 + z01 * z01 );
    x01 *= norminv;
    y01 *= norminv;
    z01 *= norminv;

    normals[ 3 * indices[ i ]     ] += x01;
    normals[ 3 * indices[ i ] + 1 ] += y01;
    normals[ 3 * indices[ i ] + 2 ] += z01;

    normals[ 3 * indices[ i + 1 ]     ] += x01;
    normals[ 3 * indices[ i + 1 ] + 1 ] += y01;
    normals[ 3 * indices[ i + 1 ] + 2 ] += z01;

    normals[ 3 * indices[ i + 2 ]     ] += x01;
    normals[ 3 * indices[ i + 2 ] + 1 ] += y01;
    normals[ 3 * indices[ i + 2 ] + 2 ] += z01;
  }

  for( std::size_t i = 0 ; i < normals.size() ; i+=3 )
  {
    auto & x = normals[ i     ];
    auto & y = normals[ i + 1 ];
    auto & z = normals[ i + 2 ];

    auto norminv = 1.0f / std::sqrt( x * x + y * y + z * z );

    x *= norminv;
    y *= norminv;
    z *= norminv; 
  }

  
  /**
   * Génération d'un Vertex Array Object contenant 3 Vertex Buffer Objects.
   */
  
  glGenVertexArrays( 1, &vaoids[ 0 ] );
  glBindVertexArray( vaoids[ 0 ] );

  // Génération de 3 VBO.
  glGenBuffers( 3, vboids );

  // VBO contenant les sommets.
  
  glBindBuffer( GL_ARRAY_BUFFER, vboids[ 0 ] );
  glBufferData( GL_ARRAY_BUFFER, vertices.size() * sizeof( float ), vertices.data(), GL_STATIC_DRAW );

  // L'attribut in_pos du vertex shader est associé aux données de ce VBO.
  auto pos = glGetAttribLocation( progid, "in_pos" );
  glVertexAttribPointer( pos, 3, GL_FLOAT, GL_FALSE, 0, 0 );
  glEnableVertexAttribArray( pos );

  // VBO contenant les indices.
  
  glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, vboids[ 1 ] );
  glBufferData( GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof( unsigned int ), indices.data(), GL_STATIC_DRAW );

  // VBO contenant les normales.

  glBindBuffer( GL_ARRAY_BUFFER, vboids[ 2 ] );
  glBufferData( GL_ARRAY_BUFFER, normals.size() * sizeof( float ), normals.data(), GL_STATIC_DRAW );

  auto normal = glGetAttribLocation( progid, "in_normal" );
  glVertexAttribPointer( normal, 3, GL_FLOAT, GL_TRUE, 0, 0 );
  glEnableVertexAttribArray( normal );
  
  glBindVertexArray( 0 );
}


void initShaders()
{
  unsigned int vsid, fsid;
  int status;
  int logsize;
  std::string log;
  
  std::ifstream vs_ifs( "res/shaders/basic.vert.glsl" );
  std::ifstream fs_ifs( "res/shaders/basic.frag.glsl" );

  auto begin = vs_ifs.tellg();
  vs_ifs.seekg( 0, std::ios::end );
  auto end = vs_ifs.tellg();
  vs_ifs.seekg( 0, std::ios::beg );
  auto size = end - begin;
  
  std::string vs;
  vs.resize( size );
  vs_ifs.read( &vs[ 0 ], size );

  begin = fs_ifs.tellg();
  fs_ifs.seekg( 0, std::ios::end );
  end = fs_ifs.tellg();
  fs_ifs.seekg( 0, std::ios::beg );
  size = end - begin;
  
  std::string fs;
  fs.resize( size );
  fs_ifs.read( &fs[0], size );

  vsid = glCreateShader( GL_VERTEX_SHADER );
  char const * vs_char = vs.c_str();
  glShaderSource( vsid, 1, &vs_char, nullptr );
  glCompileShader( vsid );

  // Get shader compilation status.
  glGetShaderiv( vsid, GL_COMPILE_STATUS, &status );

  if( !status )
  {
    std::cerr << "Error: vertex shader compilation failed.\n";
    glGetShaderiv( vsid, GL_INFO_LOG_LENGTH, &logsize );
    log.resize( logsize );
    glGetShaderInfoLog( vsid, log.size(), &logsize, &log[0] );
    std::cerr << log << std::endl;
  }

  fsid = glCreateShader( GL_FRAGMENT_SHADER );
  char const * fs_char = fs.c_str();
  glShaderSource( fsid, 1, &fs_char, nullptr );
  glCompileShader( fsid );

  // Get shader compilation status.
  glGetShaderiv( fsid, GL_COMPILE_STATUS, &status );

  if( !status )
  {
    std::cerr << "Error: fragment shader compilation failed.\n";
    glGetShaderiv( fsid, GL_INFO_LOG_LENGTH, &logsize );
    log.resize( logsize );
    glGetShaderInfoLog( fsid, log.size(), &logsize, &log[0] );
    std::cerr << log << std::endl;
  }
  
  progid = glCreateProgram();

  glAttachShader( progid, vsid );
  glAttachShader( progid, fsid );

  glLinkProgram( progid );

  glUseProgram( progid );

  mvpid = glGetUniformLocation( progid, "mvp" );
  mvid = glGetUniformLocation( progid, "mv" );
  normid = glGetUniformLocation( progid, "norm" );
}


int main( int argc, char * argv[] )
{
  glutInit( &argc, argv );
  glutInitContextVersion( 4, 5 );

  glutInitWindowSize( 640, 480 );
  glutInitDisplayMode( GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA );

  glutCreateWindow( argv[ 0 ]  );

  glutDisplayFunc( display );
  glutReshapeFunc( reshape );
  glutIdleFunc( idle );
  glutSpecialFunc( special );
  
  // Initialisation de la bibliothèque GLEW.
  glewInit();

  glEnable(GL_DEPTH_TEST);
  
  initShaders();
  initVAOs();

  glClearColor( 0.0f, 0.0f, 0.0f, 0.0f );
  
  glutMainLoop();
  
  return 0;
}
