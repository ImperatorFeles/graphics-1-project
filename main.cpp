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
#include <cstdio>

#define MOUSE_SENSITIVITY 0.1
#define MOVEMENT_SPEED 0.1

float delta;

// current transformation matrix
mat4 transformation;
GLuint camMatLoc;
GLuint perspectiveMatLoc;

//Get the world ready
World world = World();

// object that stores camera data
CameraObject camera = CameraObject("cam1", MOVEMENT_SPEED, MOUSE_SENSITIVITY,
		vec3(1.0, -3.0, 0.0), vec3(0));

LightObject Light[8];

using namespace std;

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
	LightInfo li;
	int i;
	char str[10];

	li.values.ambient  = vec4(1.0, 1.0, 1.0, 1.0);
	li.values.diffuse  = vec4(1.0, 1.0, 1.0, 1.0);
	li.values.specular = vec4(1.0, 1.0, 1.0, 1.0);

	MatProp.shinyid = glGetUniformLocation(program, "shinyness");
	MatProp.values.shinyness = 0;
	MatProp.values.diff = vec4(0.0, 0.2, 0.0, 1.0);
	MatProp.values.ambi = vec4(1.0, 1.0, 1.0, 1.0);
	MatProp.values.spec = vec4(0.0, 0.0, 0.0, 1.0);

	li.values.ambient  *= MatProp.values.ambi;
	li.values.diffuse  *= MatProp.values.diff;
	li.values.specular *= MatProp.values.spec;

	li.isSpotLight = false;

	for (i = 0; i < 8; i++) {
		snprintf(str, 10, "Light%d", i);
		Light[i] = LightObject(str, vec3(2.05, -7.3, -1.1), li);
		Light[i].setLightIndex(0);
	}
}

void init( void )
{
	int i;
	//Fixes GlutMouseWarpPointer on Mac, thanks to John Huston and Chris Compton
#ifdef __APPLE__
	CGSetLocalEventsSuppressionInterval( 0.0 );
#endif

	glEnable(GL_DEPTH_TEST);

	// set up camera
	camera.setLockedXRot(true);
	//camera.lockToPlane(-1.6);

	// load objects
	OBJParser::load_obj("models/subwaycar-done.obj", world);
	OBJParser::load_obj("models/stations.obj", world);
	OBJParser::load_obj("models/art.obj", world);

	transformation = *new mat4();

	GLuint program = InitShader( "vshader.glsl", "fshader.glsl" );
	glUseProgram( program );

	for (i = 0; i < 8; i++) {
		init_lights(program);
		Light[i].bindLight(program);
	}
	 
	camMatLoc = glGetUniformLocation( program, "modelview");
	perspectiveMatLoc = glGetUniformLocation(program, "perspective");

	world.getActors()->at(0)->generateBuffers();
	world.getActors()->at(1)->generateBuffers();
	world.getActors()->at(2)->generateBuffers();
	world.getActors()->at(0)->loadTexture("img/subwaycar.png");
	world.getActors()->at(1)->loadTexture("img/stations.png");
	world.getActors()->at(2)->loadTexture("img/art.png");
	world.getActors()->at(0)->setRotation(vec3(0.0, 90.0, 0.0));
	world.getActors()->at(0)->setPosition(vec3(-0.3, 1.3, -2.0));
	world.getActors()->at(1)->setScale(vec3(0.4));
	world.getActors()->at(0)->addChild(&camera);
	world.getActors()->at(0)->addChild(world.getActors()->at(2));

	glEnable( GL_DEPTH_TEST );

	glClearColor( 0.0, 0.0, 0.0, 1.0 ); /* white background */
}

void display( void )
{
	int i;
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	world.getActors()->at(0)->translate(vec3(0.0, 0.0, 0.1));

	glUniformMatrix4fv(perspectiveMatLoc, 1, true, Perspective(60, 1.0, 0.01, 100));
	glUniformMatrix4fv( camMatLoc, 1, true, camera.getTransformationMatrix() );
	

	for (i = 0; i < 8; i++) 
		Light[i].setValues();
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
		case 'l':
			camera.removeParent();
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
		case 'p':
			cout << camera.getPosition();
			break;
	}
}

// handles glut mouse movement events
void passiveMotion(int x, int y)
{
	// make sure we're not in the center
	if (x != 255 || y != 255)
	{
		vec3 rotation(0);
		rotation.x = -(255 - y);
		rotation.y = -(255 - x);
		camera.rotate(rotation);

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
