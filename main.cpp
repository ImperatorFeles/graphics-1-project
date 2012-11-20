#include "Angel.h"
#include "vec.h"
#include "mat.h"

#include <iostream>
#include <vector>
#include <sstream>
#include <fstream>
#include <string>

#define MOUSE_SENSITIVITY 0.1
#define MOVEMENT_SPEED 0.01;

vec4 cameraPos;
vec3 cameraRot;
vec4 cameraVel;

vec4 lightPos;
int lightDir = 0; //0 is left, 1 is right
float lightSpeed = 0.005;

vec2 mouseCenter;

mat4 transformation;

GLuint matLoc;

int numVertices = 0;

float sensitivity = 0.1;

bool forward_, backward;
bool strafeL, strafeR;
bool up, down;

using namespace std;

vector<string> split(string str, char delim)
{
	vector<string> *elements = new vector<string>();

	while (str.find_first_of(delim) != string::npos)
	{
		int pos = str.find_first_of(delim);

		elements->push_back(str.substr(0, pos));

		str = str.substr(pos + 1);
	}

	elements->push_back(str);

	return *elements;
}

void initLights( void ) {

  lightPos = vec4( 0.0, -0.5, 0.5, 1.0 );

  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);
  glEnable(GL_NORMALIZE);

  // -------------------------------------------                                                
  // Lighting parameters:                                                                       

  GLfloat light_pos[] = {lightPos.x, lightPos.y, lightPos.z, lightPos.w};
  GLfloat light_Ka[]  = {0.2f, 0.2f, 0.2f, 1.0f};
  GLfloat light_Kd[]  = {1.0f, 1.0f, 1.0f, 1.0f};
  GLfloat light_Ks[]  = {1.0f, 1.0f, 1.0f, 1.0f};

  glLightfv(GL_LIGHT0, GL_POSITION, light_pos);
  glLightfv(GL_LIGHT0, GL_AMBIENT, light_Ka);
  glLightfv(GL_LIGHT0, GL_DIFFUSE, light_Kd);
  glLightfv(GL_LIGHT0, GL_SPECULAR, light_Ks);

}

void load_obj(const char* filename, vector<vec4> &vertices,
			  vector<vec3> &normals, vector<GLushort> &elements)
{
	ifstream in(filename, ios::in);

	if (!in)
	{
		cerr << "Cannot open " << filename << endl;
		exit(1);
	}

	string line;

	while (getline(in, line))
	{
		if (line.substr(0, 2) == "v ")
		{
		  istringstream s(line.substr(2));
		  vec4 v;
		  s >> v.x;
		  s >> v.y;
		  s >> v.z;
		  v.w = 1.0;
		  
		  vertices.push_back(v);
		}
		else if (line.substr(0, 2) == "f ")
		{
		  istringstream s(line.substr(2));
		  string a, b, c;
		  s >> a;
		  s >> b;
		  s >> c;

		  elements.push_back(atoi(split(a, '/')[0].c_str()));
		  elements.push_back(atoi(split(b, '/')[0].c_str()));
		  elements.push_back(atoi(split(c, '/')[0].c_str()));
		}
		else if (line.substr(0, 3) == "vn ") {
		  istringstream s(line.substr(3));
		  vec3 vn;
		  s >> vn.x;
		  s >> vn.y;
		  s >> vn.z;
		  
		  normals.push_back(vn);
		}
		else if (line[0] == '#') { /* ignore comments */ }
		else { /* blank/junk */ }
	}

}

void init( void )
{
  vector<vec4> raw_vertices;
  vector<vec4> vertices;
  vector<vec3> normals;
  vector<GLushort> elements;
  vector<vec3> colors;
  
  load_obj("models/suzanne.obj", raw_vertices, normals, elements);
  //  load_obj("models/flashlight.obj", raw_vertices, normals, elements);

  initLights();

  for (unsigned int i = 0; i < elements.size(); i++)
    {
      vertices.push_back(raw_vertices[elements[i]]);
    }
  
  numVertices = vertices.size();
  
  colors.resize(vertices.size());
  
  for (unsigned int i = 0; i < vertices.size(); i++)
    {
      colors[i] = vec3(1.0, 0.5, 0.5);
    }
  

  forward_ = backward = false;
  strafeL = strafeR = false;
  up = down = false;
  
  cameraPos = 0;
  cameraPos.z = -4;
  cameraRot = 0;
  
  mouseCenter = -1;
  
  transformation = *new mat4();
  
  // Create a vertex array object
  GLuint vao;
  glGenVertexArrays( 1, &vao );
  glBindVertexArray( vao );
  
  // Create and initialize a buffer object
  GLuint buffer;
  glGenBuffers( 1, &buffer );
  glBindBuffer( GL_ARRAY_BUFFER, buffer );
  
  // First, we create an empty buffer of the size we need by passing
  //   a NULL pointer for the data values
  glBufferData( GL_ARRAY_BUFFER, sizeof(vec4) * vertices.size() + sizeof(vec3) * colors.size() 
		+ sizeof(vec3) * normals.size(), NULL, GL_STATIC_DRAW );
  
  // Next, we load the real data in parts.  We need to specify the
  //   correct byte offset for placing the color data after the point
  //   data in the buffer.  Conveniently, the byte offset we need is
  //   the same as the size (in bytes) of the points array, which is
  //   returned from "sizeof(points)".
  glBufferSubData( GL_ARRAY_BUFFER, 0, sizeof(vec4) * vertices.size(), &vertices[0]);
  glBufferSubData( GL_ARRAY_BUFFER, sizeof(vec4) * vertices.size(), 
		   sizeof(vec3) * colors.size(), &colors[0]);
  glBufferSubData( GL_ARRAY_BUFFER, sizeof(vec4) * vertices.size() + sizeof(vec3) * colors.size(),
		   sizeof(vec3) * normals.size(), &normals[0]);
  
  // Load shaders and use the resulting shader program
  GLuint program = InitShader( "vshader.glsl", "fshader.glsl" );
  glUseProgram(program);
  
  GLuint vPosition = glGetAttribLocation(program, "vPosition");
  glEnableVertexAttribArray( vPosition );
  glVertexAttribPointer( vPosition, 4, GL_FLOAT, GL_FALSE, 0,
			 BUFFER_OFFSET(0) );
  
  // Likewise, initialize the vertex color attribute.  Once again, we
  //    need to specify the starting offset (in bytes) for the color
  //    data.  Just like loading the array, we use "sizeof(points)"
  //    to determine the correct value.
  GLuint vColor = glGetAttribLocation( program, "vColor" );
  glEnableVertexAttribArray( vColor );
  glVertexAttribPointer( vColor, 3, GL_FLOAT, GL_FALSE, 0,
			 BUFFER_OFFSET(sizeof(vec4) * vertices.size()) );

  GLuint vNormal = glGetAttribLocation( program, "vNormal" );
  glEnableVertexAttribArray( vNormal );
  glVertexAttribPointer( vNormal, 3, GL_FLOAT, GL_FALSE, 0,
			 BUFFER_OFFSET(sizeof(vec4) * vertices.size() + sizeof(vec3) * colors.size()) );
  
  matLoc = glGetUniformLocation(program, "m");
  
  glEnable( GL_DEPTH_TEST );
  
  glClearColor( 1.0, 1.0, 1.0, 1.0 ); /* white background */
}

void
display( void )
{
  glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
  
  // transform the camera position based on the rotation and the velocity
  transformation = Angel::identity();
  transformation = transformation  * RotateY(-cameraRot.y) * RotateX(-cameraRot.x);
  cameraPos = cameraPos + transformation * cameraVel;
  
  // build the transformation matrix
  transformation = Angel::identity();

  transformation = transformation * Perspective(60, 1.0, 0.01, 100);      
  transformation = transformation * RotateX(cameraRot.x) * RotateY(cameraRot.y);
  transformation = transformation * Translate(cameraPos.x, cameraPos.y, cameraPos.z);

  glUniformMatrix4fv(matLoc, 1, true, transformation);
  cout << "test" << endl;
  glDrawArrays( GL_TRIANGLES, 0, numVertices);
  cout << "test2" << endl;
  glFlush();
}

//----------------------------------------------------------------------------

void
keyboard( unsigned char key, int x, int y )
{
  switch ( key ) {
  case 033:
    exit( EXIT_SUCCESS );
    break;
  case 'w':
    forward_ = true;
    break;
  case 's':
    backward = true;
    break;
  case 'd':
    strafeR = true;
    break;
  case 'a':
    strafeL = true;
    break;
  case 'r':
    up = true;
    break;
  case 'f':
    down = true;
    break;
  }
}

void keyboardUp(unsigned char key, int x, int y)
{
  switch (key)
  {
  case 'w':
    forward_ = false;
    break;
  case 's':
    backward = false;
    break;
  case 'd':
    strafeR = false;
    break;
  case 'a':
    strafeL = false;
    break;
  case 'r':
    up = false;
    break;
  case 'f':
    down = false;
    break;
  }
}

void mouse(int button, int state, int x, int y)
{
  if (button == GLUT_LEFT_BUTTON)
  {
    forward_ = button == GLUT_DOWN;
  }
  else if (button == GLUT_RIGHT_BUTTON)
  {
    backward = button == GLUT_DOWN;
  }
      
}

void passiveMotion(int x, int y)
{
  if (x != 255 || y != 254)
  {
    cameraRot.y -= (255 - x) * MOUSE_SENSITIVITY;

    // lock the up and down look to no more than 90 degrees
    if (cameraRot.x - (254 - y) * MOUSE_SENSITIVITY <= 90 &&
	cameraRot.x - (254 - y) * MOUSE_SENSITIVITY >= -90)
    {
      cameraRot.x -= (254 - y) * MOUSE_SENSITIVITY;
    }
 
    glutWarpPointer(255, 254);
  }
}

void motion(int x, int y)
{
}

void idle()
{
  // set the velocity based on which direction we're going
  if (forward_)
  {
    cameraVel.z = MOVEMENT_SPEED;
  }
  else if (backward)
  {
    cameraVel.z = -MOVEMENT_SPEED;
  }
  else
  {
    cameraVel.z = 0;
  }
  if (strafeL)
  {
    cameraVel.x = MOVEMENT_SPEED;
  }
  else if (strafeR)
  {
    cameraVel.x = -MOVEMENT_SPEED;
  }
  else
  {
    cameraVel.x = 0;
  }
  if (up)
  {
    cameraVel.y = -MOVEMENT_SPEED;
  }
  else if (down)
  {
    cameraVel.y = MOVEMENT_SPEED;
  }
  else
  {
    cameraVel.y = 0;
  }

  // make sure rotation does not grow infinitely
  if (cameraRot.y > 360)
  {
    cameraRot.y -= 360;
  }

  if( lightDir == 0 ) {
    lightPos.x -= lightSpeed;
  } else {
    lightPos.x += lightSpeed;
  }
  if( lightPos.x < -3.0 ) {
    lightDir = 1;
  } else if( lightPos.x > 3.0 ) {
    lightDir = 0;
  }
  GLfloat light_pos[] = {lightPos.x, lightPos.y, lightPos.z, lightPos.w};
  glLightfv(GL_LIGHT0, GL_POSITION, light_pos);

  glutPostRedisplay(); 
}
//----------------------------------------------------------------------------

int
main( int argc, char **argv )
{
    glutInit( &argc, argv );
    glutInitDisplayMode( GLUT_RGBA | GLUT_DEPTH | GLUT_SINGLE );
    glutInitWindowSize( 512, 512 );
    glutCreateWindow( "Flyby" );
    init();

    glutDisplayFunc( display );
    glutKeyboardFunc( keyboard );
    glutKeyboardUpFunc(keyboardUp);
    glutMouseFunc(mouse);
    glutIdleFunc(idle);
    glutMotionFunc(motion);
    glutPassiveMotionFunc(passiveMotion);

    glutSetCursor(GLUT_CURSOR_NONE);

    glutMainLoop();
    return 0;
}
