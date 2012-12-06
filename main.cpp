#include "Angel.h"
#include "vec.h"
#include "mat.h"
#include "ModelObject.h"
#include "OBJParser.h"

#include <iostream>
#include <vector>
#include <sstream>
#include <fstream>
#include <string>

#define MOUSE_SENSITIVITY 0.1
#define MOVEMENT_SPEED 0.04;

vec4 cameraPos; // position of camera
vec3 cameraRot; // rotation of camera
vec4 cameraVel; // velocity of camera

// light position
vec4 lightPos0;
vec4 lightPos1;

float lightSpeed = 0.02; //Speed of light0 orbit
float lightOrbitRad = 3.0; //Radius of light0 orbit
float i = 0; //Light0 orbit frame

// current transformation matrix
mat4 transformation;

// pointer to matrix in shader
GLuint matLoc;

int numVertices = 0;

// camera movement directions
bool forward_, backward;
bool strafeL, strafeR;
bool up, down;

using namespace std;

/*
   Initialize lights for the scene
   Defines initial positions and colors
 */
void initLights( void ) {

	//Initial positions
	lightPos0 = vec4( 0.0, 5.0, 0.0, 1.0 );
	lightPos1 = vec4( 0.0, -1.0, 0.5, 1.0 );

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHT1);

	//Put things into the right format for the shader
	GLfloat light_pos0[] = {lightPos0.x, lightPos0.y, lightPos0.z, lightPos0.w};
	GLfloat light_pos1[] = {lightPos1.x, lightPos1.y, lightPos1.z, lightPos1.w};
	GLfloat light_Kd[]  = {0.1f, 0.6f, 0.1f, 1.0f};
	GLfloat light_Kd1[] = {0.5f, 0.5f, 1.0f, 1.0f};

	//Send lighting information to the GPU
	glLightfv(GL_LIGHT0, GL_POSITION, light_pos0);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_Kd);
	glLightfv(GL_LIGHT1, GL_POSITION, light_pos1);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, light_Kd1);

}

void init( void )
{
	vector<vec3> normals;
	vector<vec4> vertices;
	vector<vec3> colors;
	vector<ModelObject> *objects;

	//Fixes GlutMouseWarpPointer on Mac, thanks to John Huston and Chris Compton
#ifdef __APPLE__
	CGSetLocalEventsSuppressionInterval( 0.0 );
#endif

	objects = OBJParser::load_obj("models/art.obj");

	vertices = objects->at(0).getVertices();
	normals = objects->at(0).getNormals();
	cout << "Number of UVs: " << objects->at(0).getTextureUVs().size() << endl;

	numVertices = vertices.size();

	initLights();

	//Set color of all vertices to be white
	colors.resize(vertices.size());
	for (unsigned int i = 0; i < vertices.size(); i++)
	{
		colors[i] = vec3(1.0, 1.0, 1.0);
	}

	forward_ = backward = false;
	strafeL = strafeR = false;
	up = down = false;

	//Set initial camera position backwards from the (very large) model so it's all visible from the get-go
	cameraPos = 0;
	cameraPos.z = -10;
	cameraRot = 0;

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

	glClearColor( 0.0, 0.0, 0.0, 1.0 ); /* white background */
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

	// pass matrix to shader
	glUniformMatrix4fv(matLoc, 1, true, transformation);
	glDrawArrays( GL_TRIANGLES, 0, numVertices);
	glFlush();
}

//----------------------------------------------------------------------------

// handles glut keyboard events
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

// handles glut keyboardUp events
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

// handles glut mouse movement events
void passiveMotion(int x, int y)
{
	// make sure we're not in the center
	if (x != 255 || y != 255)
	{
		cameraRot.y -= (255 - x) * MOUSE_SENSITIVITY;

		// lock the up and down look to no more than 90 degrees
		if (cameraRot.x - (255 - y) * MOUSE_SENSITIVITY <= 90 &&
				cameraRot.x - (255 - y) * MOUSE_SENSITIVITY >= -90)
		{
			cameraRot.x -= (255 - y) * MOUSE_SENSITIVITY;
		}

		glutWarpPointer(255, 255);
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

	//Make light 0 orbit the center point
	//Thanks to Nick St.Pierre
	if ( i > 360 )
		i = 0;
	i += lightSpeed;
	lightPos0.z = sin(i)*lightOrbitRad;
	lightPos0.x = cos(i)*lightOrbitRad;
	GLfloat light_pos[] = {lightPos0.x, lightPos0.y, lightPos0.z, lightPos0.w};
	glLightfv(GL_LIGHT0, GL_POSITION, light_pos);

	glutPostRedisplay(); 
}
//----------------------------------------------------------------------------

int main( int argc, char **argv )
{
	glutInit( &argc, argv );
	glutInitDisplayMode( GLUT_RGBA | GLUT_DEPTH | GLUT_SINGLE );
	glutInitWindowSize( 800, 800 );
	glutCreateWindow( "Monkey Robot" );
	init();

	glutDisplayFunc( display );
	glutKeyboardFunc( keyboard );
	glutKeyboardUpFunc(keyboardUp);
	glutIdleFunc(idle);
	glutMotionFunc(motion);
	glutPassiveMotionFunc(passiveMotion);

	glutSetCursor(GLUT_CURSOR_NONE);

	glutMainLoop();
	return 0;
}
