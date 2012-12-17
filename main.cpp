#include "Angel.h"
#include "vec.h"
#include "mat.h"
#include "ModelObject.h"
#include "LightObject.h"
#include "OBJParser.h"
#include "World.h"
#include "SOIL.h"
#include "CameraObject.h"
#include "Train.h"

#include <iostream>
#include <vector>
#include <sstream>
#include <fstream>
#include <string>
#include <ctime>
#include <cstdio>

#define MOUSE_SENSITIVITY 0.1
#define MOVEMENT_SPEED 0.02

#define NUM_LIGHTS 4

float delta;

// current transformation matrix
mat4 transformation;
GLuint camMatLoc;
GLuint perspectiveMatLoc;

//Get the world ready
World world = World();

// train logic
Train *train;

// object that stores camera data
CameraObject camera = CameraObject("cam1", MOVEMENT_SPEED, MOUSE_SENSITIVITY,
		vec3(-4.0, -3.0, 4.0), vec3(0.0, -90.0, 0.0));

LightObject Light[8];

using namespace std;

struct Material_Properties {
	GLint shinyid;
	struct {	
		vec4 diff,
			 ambi,
			 spec;
		GLfloat shininess;
	} values;
} MatProp;

void init_lights(GLuint program) {
	LightInfo li;
	int i;
	char str[10];
	vec3 vals[NUM_LIGHTS];
	vals[0] = vec3(-16.5, -8.0, 9.4);
	vals[1] = vec3(-16.5, -7.0, -9.4);
	vals[2] = vec3(.43, -8.9, -.7);
	vals[3] = vec3(.6, 1.0, -3.3);
	//vals[4] = vec3(-9.1, -4.2, -45);

	li.values.ambient  = vec4(1.0, 1.0, 1.0, 1.0);
	li.values.diffuse  = vec4(1.0, 1.0, 1.0, 1.0);
	li.values.specular = vec4(1.0, 1.0, 1.0, 1.0);

	MatProp.shinyid = glGetUniformLocation(program, "shininess");
	MatProp.values.shininess = 1;
	MatProp.values.ambi = vec4(0.75, 0.75, 0.75, 1.0);
	MatProp.values.diff = vec4(0.5, 0.5, 0.5, 1.0);
	MatProp.values.spec = vec4(0.03, 0.03, 0.03, 1.0);

	li.values.ambient  *= MatProp.values.ambi;
	li.values.diffuse  *= MatProp.values.diff;
	li.values.specular *= MatProp.values.spec;

	li.isSpotLight = false;

	
	for (i = 0; i < NUM_LIGHTS; i++) {
		snprintf(str, 10, "Light%d", i);
		Light[i] = LightObject(str, vals[i], li);
		Light[i].setLightIndex(i);
		Light[i].enable();
	}
	Light[3].enable();
	world.getActors()->at(0)->addChild(&Light[3]);
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
	camera.lockToPlane(-1.7);

	// load objects
	OBJParser::load_obj("models/subwaycar-done.obj", world);
	OBJParser::load_obj("models/stations.obj", world);
	OBJParser::load_obj("models/cardoor.obj", world);
	OBJParser::load_obj("models/cardoor.obj", world);

	transformation = *new mat4();

	GLuint program = InitShader( "vshader.glsl", "fshader.glsl" );
	glUseProgram( program );

	for (i = 0; i < NUM_LIGHTS; i++) {
		init_lights(program);
		Light[i].bindLight(program);
	}
	 
	camMatLoc = glGetUniformLocation( program, "modelview");
	perspectiveMatLoc = glGetUniformLocation(program, "perspective");

	world.bufferActors();
	world.getActors()->at(0)->loadTexture("img/subwaycar.png");
	world.getActors()->at(1)->loadTexture("img/stations.png");
	world.getActors()->at(2)->loadTexture("img/cardoor.png");
	world.getActors()->at(3)->loadTexture("img/cardoor.png");
	world.getActors()->at(0)->setRotation(vec3(0.0, 90.0, 0.0));
	world.getActors()->at(2)->setRotation(vec3(0.0, 90.0, 0.0));
	world.getActors()->at(3)->setRotation(vec3(0.0, 90.0, 0.0));
	world.getActors()->at(0)->setPosition(vec3(-0.3, 1.3, -5.5));

	train = new Train(world.getActors()->at(0), world.getActors()->at(2), world.getActors()->at(3), &camera, 0.0002, 0.1, 600, 1050, -1.3);

	world.getActors()->at(2)->setPosition(vec3(1.22, 1.3, 3.15));
	world.getActors()->at(3)->setPosition(vec3(1.22, 1.3, -14.2));

	world.getActors()->at(0)->translate(vec3(0.0, 0.0, 6.0));

			
	world.getActors()->at(1)->setScale(vec3(0.4));

	glEnable( GL_DEPTH_TEST );

	glClearColor( 0.0, 0.0, 0.0, 1.0 ); /* white background */
}

void display( void )
{
	int i;
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	glUniformMatrix4fv(perspectiveMatLoc, 1, true, Perspective(60, 1.0, 0.01, 100));
	glUniformMatrix4fv( camMatLoc, 1, true, camera.getTransformationMatrix() );

	for (i = 0; i < NUM_LIGHTS; i++) {
		Light[i].setValues();
	}
	glUniform1f(MatProp.shinyid, MatProp.values.shininess);

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
			camera.unlockPlane();
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
		case 'p': case 'P':
			cerr << camera.getPosition() << endl;
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
	train->update();

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
