#include <iostream>
#include <vector>
#include <fstream>

#include <GL/glew.h>
#include <GL/freeglut.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <opencv4/opencv2/opencv.hpp>


unsigned int progid;
unsigned int mvpid;

glm::mat4 view;
glm::mat4 proj;
glm::mat4 mvp;

float angle = 0.0f;
float inc = 0.1f;

unsigned int vaoids[ 1 ];


void display()
{
  glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

  // L'origine du repère est déplacée à -5 suivant l'axe z.
  view = glm::translate( glm::mat4( 1.0f ) , glm::vec3( 0.0f, 0.0f, -5.0f ) );
  // Le repère subit une rotation suivant l'axe z.
  view = glm::rotate( view, glm::degrees( angle ), glm::vec3( 0.0f, 1.0f, 1.0f ) );
  //

  // Calcul de la matrice mvp.
  mvp = proj * view;

  // Passage de la matrice mvp au shader.
  glUniformMatrix4fv( mvpid , 1, GL_FALSE, &mvp[0][0]);

  // Dessin des triangles...
  glBindVertexArray( vaoids[ 0 ] );
  glDrawElements( GL_TRIANGLES, 18, GL_UNSIGNED_SHORT, 0 );
  
  glutSwapBuffers();
}


void idle()
{
  angle += 0.001f;
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


void initVAOs()
{
  unsigned int vboids[ 3 ];
  
  std::vector< float > vertices = {
    -0.5f, -0.5f,  0.5f,
    -0.5f,  0.5f,  0.5f,
     0.5f,  0.5f,  0.5f,
     0.5f, -0.5f,  0.5f,
    
    -0.5f, -0.5f, -0.5f,
    -0.5f, -0.5f,  0.5f,
     0.5f, -0.5f,  0.5f,
     0.5f, -0.5f, -0.5f,
    
    -0.5f, -0.5f, -0.5f,
    -0.5f, -0.5f,  0.5f,
    -0.5f,  0.5f,  0.5f,
    -0.5f,  0.5f, -0.5f

    // ajouter les 3 faces manquantes
  };

  std::vector< float > texcoords = {
     0.25f, 0.25f,
     0.25f, 0.00f,
     0.50f, 0.00f,
     0.50f, 0.25f,

     0.25f, 0.50f,
     0.25f, 0.25f,
     0.50f, 0.25f,
     0.50f, 0.50f,

     0.00f, 0.50f,
     0.25f, 0.50f,
     0.25f, 0.25f,
     0.00f, 0.25f

     // ajouter les coordonnées de texture pour les 3 faces manquantes
  };

  std::vector< unsigned short > indices = {
    0, 1, 2,
    2, 3, 0,
    4, 5, 6,
    6, 7, 4,
    8, 9, 10,
    10, 11, 8

    // ajouter les indices pour les 3 dernières faces
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
  
  // VBO contenant les coordonnées de texture.
  
  glBindBuffer( GL_ARRAY_BUFFER, vboids[ 1 ] );
  glBufferData( GL_ARRAY_BUFFER, texcoords.size() * sizeof( float ), texcoords.data(), GL_STATIC_DRAW );

  auto coord = glGetAttribLocation( progid, "in_coord" );
  glVertexAttribPointer( coord, 2, GL_FLOAT, GL_FALSE, 0, 0 );
  glEnableVertexAttribArray( coord );

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
  
  std::ifstream vs_ifs( "res/shaders/texture.vert.glsl" );
  std::ifstream fs_ifs( "res/shaders/texture.frag.glsl" );

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

void initTextures()
{
  cv::Mat img = cv::imread( "res/images/numbers.png", cv::IMREAD_UNCHANGED );

  auto rows = img.rows;
  auto cols = img.cols;

  auto data = img.data;
  
  unsigned int texture;

  glGenTextures( 1, &texture );

  glBindTexture( GL_TEXTURE_2D, texture );

  glActiveTexture( GL_TEXTURE0 );

  unsigned int tex = glGetUniformLocation(progid, "tex");
  glUniform1i(tex, 0);

  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );

  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, cols, rows, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

  glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
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
  
  glClearColor( 0.0f, 0.0f, 0.0f, 0.0f );

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE);

  initShaders();
  initVAOs();
  initTextures();

  glutMainLoop();
  
  return 0;
}
