#include <iostream>
#include <vector>
#include <fstream>

#include <GL/glew.h>
#include <GL/freeglut.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>


unsigned int progid;
unsigned int mvpid;

glm::mat4 model;
glm::mat4 view;
glm::mat4 proj;
glm::mat4 mvp;

float angle = 0.0f;
float scale = 0.0f;
float inc = 0.1f;

unsigned int vaoids[ 1 ];


void display()
{
  glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

  // L'origine du repère est déplacée à -5 suivant l'axe z.
  view = glm::translate( glm::mat4( 1.0f ) , glm::vec3( 0.0f, 0.0f, -5.0f ) );
  // Le repère subit une rotation suivant l'axe z.
  view = glm::rotate( view, glm::degrees( angle ), glm::vec3( 0.0f, 0.0f, 1.0f ) );
  //
  model = glm::scale( glm::mat4( 1.0f ), glm::vec3( scale, scale, scale ) );

  // Calcul de la matrice mvp.
  mvp = proj * view * model;

  // Passage de la matrice mvp au shader.
  glUniformMatrix4fv( mvpid , 1, GL_FALSE, &mvp[0][0]);

  // Dessin de 2 triangles à partir de 6 indices.
  glBindVertexArray( vaoids[ 0 ] );
  glDrawElements( GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0 );
  
  glutSwapBuffers();
}


void idle()
{
  angle += 0.001f;
  if( angle >= 360.0f )
  {
    angle = 0.0f;
  }

  if( scale <= 0.0f )
  {
    inc = 0.1f;
  }
  else if( scale > 2.0f )
  {
    inc = -0.1f;
  }

  scale += inc;
  
  glutPostRedisplay();
}


void reshape( int w, int h )
{
  glViewport(0, 0, w, h);
  // Modification de la matrice de projection à chaque redimensionnement de la fenêtre.
  proj = glm::perspective( 45.0f, w/static_cast< float >( h ), 0.1f, 100.0f );  
}


void initVAOs()
{
  unsigned int vboids[ 3 ];
  
  std::vector< float > vertices = {
    -0.5f,  0.0f,  0.0f,
     0.0f,  0.5f,  0.0f,
     0.5f,  0.0f,  0.0f,
     0.0f, -0.5f,  0.0f
  };

  std::vector< float > colors = {
    1.0f, 0.0f, 0.0f,
    1.0f, 1.0f, 0.0f,
    0.0f, 1.0f, 1.0f,
    1.0f, 0.0f, 1.0f
  };

  std::vector< unsigned short > indices = {
    0, 1, 2,
    2, 3, 0
  };

  // Génération d'un Vertex Array Object contenant 3 Vertex Buffer Objects.
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

  // VBO contenant les couleurs.
  
  glBindBuffer( GL_ARRAY_BUFFER, vboids[ 1 ] );
  glBufferData( GL_ARRAY_BUFFER, colors.size() * sizeof( float ), colors.data(), GL_STATIC_DRAW );

  // L'attribut in_color du vertex shader est associé aux données de ce VBO.
  auto color = glGetAttribLocation( progid, "in_color" );
  glVertexAttribPointer( color, 3, GL_FLOAT, GL_FALSE, 0, 0 );
  glEnableVertexAttribArray( color );

  // VBO contenant les indices.
  
  glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, vboids[ 2 ] );
  glBufferData( GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof( unsigned short ), indices.data(), GL_STATIC_DRAW );

  glBindVertexArray( 0 );
}


void initShaders()
{
  unsigned int vsid, fsid;
  int status;
  int logsize;
  std::string log;
  
  std::ifstream vs_ifs( "test.vs" );
  std::ifstream fs_ifs( "test.fs" );

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
  
  // Initialisation de la bibliothèque GLEW.
  glewInit();

  initShaders();
  initVAOs();

  glClearColor( 0.0f, 0.0f, 0.0f, 0.0f );
  
  glutMainLoop();
  
  return 0;
}
