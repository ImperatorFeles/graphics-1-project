#include "Angel.h"
#include "vec.h"
#include "mat.h"
#include "ModelObject.h"
#include "LightObject.h"
#include "OBJParser.h"
#include "World.h"
#include "SOIL.h"
#include "CameraObject.h"

#include <iostream>
#include <vector>
#include <sstream>
#include <fstream>
#include <string>
#include <ctime>

#define MOUSE_SENSITIVITY 0.1
#define MOVEMENT_SPEED 0.05

float delta;

// current transformation matrix
mat4 transformation;
GLuint camMatLoc;
GLuint perspectiveMatLoc;

//Get the world ready
World world = World();

// object that stores camera data
CameraObject camera = CameraObject("cam1", MOVEMENT_SPEED, MOUSE_SENSITIVITY,
		vec3(0.0, 0.0, -5.0), vec3(0));

using namespace std;

struct Light_Info {
	struct {
		GLint diffid,
			  ambiid,
			  specid,
			  posid;
	} id;
	struct {
		vec4 diff,
			 ambi,
			 spec,
			 pos;
	} values;
} test_light;
struct Material_Properties {
	GLint shinyid;
	struct {	
		vec4 diff,
			 ambi,
			 spec;
		GLfloat shinyness;
	} values;
} MatProp;

void init_lights(GLuint program) {
	test_light.id.diffid = glGetUniformLocation(program, "inLight.diff");
	test_light.id.ambiid = glGetUniformLocation(program, "inLight.ambi");
	test_light.id.specid = glGetUniformLocation(program, "inLight.spec");
	test_light.id.posid  = glGetUniformLocation(program, "inLight.position"); 

	test_light.values.ambi = vec4(0.2, 0.2, 0.3, 1.0);
	test_light.values.diff = vec4(1.0, 1.0, 1.0, 1.0);
	test_light.values.spec = vec4(1.0, 0.8, 1.0, 1.0);
	test_light.values.pos  = vec4(0.0, 0.0, 5.0, 1.0);

	MatProp.shinyid = glGetUniformLocation(program, "shinyness");
	MatProp.values.shinyness = 100;
	MatProp.values.diff = vec4(1.0, 0.0, 1.0, 1.0);
	MatProp.values.ambi = vec4(1.0, 0.0, 0.0, 1.0);
	MatProp.values.spec = vec4(1.0, 0.8, 0.0, 1.0);

	test_light.values.ambi *= MatProp.values.ambi;
	test_light.values.diff *= MatProp.values.diff;
	test_light.values.spec *= MatProp.values.spec;
}

void init( void )
{
	//Fixes GlutMouseWarpPointer on Mac, thanks to John Huston and Chris Compton
#ifdef __APPLE__
	CGSetLocalEventsSuppressionInterval( 0.0 );
#endif

	glEnable(GL_DEPTH_TEST);

	// set up camera
	camera.setLockedXRot(true);

	// load objects
	OBJParser::load_obj("models/subwaycar-done.obj", world);
	OBJParser::load_obj("models/art.obj", world);
	OBJParser::load_obj("models/stations.obj", world);

	transformation = *new mat4();

	GLuint program = InitShader( "vshader.glsl", "fshader.glsl" );
	glUseProgram( program );

	init_lights(program);
	 
	camMatLoc = glGetUniformLocation( program, "modelview");
	perspectiveMatLoc = glGetUniformLocation(program, "perspective");

	world.getActors()->at(0)->generateBuffers();
	world.getActors()->at(1)->generateBuffers();
	world.getActors()->at(2)->generateBuffers();
	world.getActors()->at(0)->loadTexture("img/subwaycar.png");
	world.getActors()->at(1)->loadTexture("img/art.png");
	world.getActors()->at(2)->loadTexture("img/stations.png");
	//world.getActors()->at(0)->addChild(&(world.getActors()->at(1)));

	glEnable( GL_DEPTH_TEST );

	glClearColor( 1.0, 1.0, 1.0, 1.0 ); /* white background */
}

void display( void )
{
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	glUniformMatrix4fv(perspectiveMatLoc, 1, true, Perspective(60, 1.0, 0.01, 100));
	glUniformMatrix4fv( camMatLoc, 1, true, camera.getTransformationMatrix() );

	glUniform4fv(test_light.id.diffid, 1, test_light.values.diff);
	glUniform4fv(test_light.id.ambiid, 1, test_light.values.ambi);
	glUniform4fv(test_light.id.specid, 1, test_light.values.spec);
	glUniform4fv(test_light.id.posid,  1, test_light.values.pos);
	
	glUniform1f(MatProp.shinyid, MatProp.values.shinyness);

	world.drawActors();

	glFlush();
}

void keyboard( unsigned char key, int x, int y )
{
	switch ( key ) {
		case 033: case 'q': case 'Q':
			exit( EXIT_SUCCESS );
			break;
		case 'w':
			camera.setMoveForward(true);
			break;
		case 's':
			camera.setMoveBackward(true);
			break;
		case 'd':
			camera.setMoveRight(true);
			break;
		case 'a':
			camera.setMoveLeft(true);
			break;
		case 'r':
			camera.setMoveUp(true);
			break;
		case 'f':
			camera.setMoveDown(true);
			break;
	}
}

void keyboardUp(unsigned char key, int x, int y)
{
	switch (key)
	{
		case 'w':
			camera.setMoveForward(false);
			break;
		case 's':
			camera.setMoveBackward(false);
			break;
		case 'd':
			camera.setMoveRight(false);
			break;
		case 'a':
			camera.setMoveLeft(false);
			break;
		case 'r':
			camera.setMoveUp(false);
			break;
		case 'f':
			camera.setMoveDown(false);
			break;
	}
}

// handles glut mouse movement events
void passiveMotion(int x, int y)
{
	// make sure we're not in the center
	if (x != 255 || y != 255)
	{
		camera.addYRot(-(255 - x));
		camera.addXRot(-(255 - y));

		glutWarpPointer(255, 255);
	}
}

void motion(int x, int y)
{
}

void idle()
{
	camera.update();

	delta += 0.2;

	glutPostRedisplay(); 
}

int main( int argc, char **argv )
{
	glutInit( &argc, argv );
	glutInitDisplayMode( GLUT_RGBA | GLUT_DEPTH | GLUT_SINGLE );
	glutInitWindowSize( 800, 800 );
	glutCreateWindow( "Graphics" );
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
