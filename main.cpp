#include "Angel.h"
#include "vec.h"
#include "mat.h"
#include "ModelObject.h"

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
   Basic function to split strings over a delimiter
   used in model parsing
 */
vector<string> split(string str, char delim)
{
	vector<string> *elements = new vector<string>();

	// loop through and break string up by the deliminator
	while (str.find_first_of(delim) != string::npos)
	{
		// position of deliminator in current string
		int pos = str.find_first_of(delim);

		elements->push_back(str.substr(0, pos));

		str = str.substr(pos + 1);
	}

	// str will have the final element remaining in it
	elements->push_back(str);

	return *elements;
}

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

/*
   .obj model loader
   models must be triangulated, and have normals included to be
   correctly parsed
   vertices, normals, vertex elements, and normal elements will be returned to pointers to vectors
 */
vector<ModelObject>* load_obj(const char* filename)
{
		// file input stream
	ifstream in(filename, ios::in);

	if (!in)
	{
		cerr << "Cannot open " << filename << endl;
		exit(1);
	}
	
	bool firstObject = true;

	vector<vec4> raw_vertices;
	vector<vec4> vertices;
	vector<vec3> raw_normals;
	vector<vec3> normals;
	vector<vec2> textureUVs;
	vector<int> v_elements;
	vector<int> n_elements;

	vector<ModelObject> *objects = new vector<ModelObject>;

	string line;
	string objName;

	// parse the .obj file for its data
	while (getline(in, line))
	{
		// lines beginning with 'v ' have vertices
		if (line.substr(0, 2) == "v ")
		{
			// read in vertex coords
			istringstream s(line.substr(2));
			vec4 v;
			s >> v.x;
			s >> v.y;
			s >> v.z;
			v.w = 1.0;

			raw_vertices.push_back(v);
		}

		// 'o ' or 'g ' signify a new object is starting
		else if (line.substr(0, 1) == "o" || line.substr(0, 1) == "g")
		{
			if (line.substr(1).compare("") == 0)
			{
				objName = "Unnamed Object";
			}
			else
			{
				objName = line.substr(2);
			}

			// skip writing data to an object if we don't have data yet
			if (firstObject)
			{
				firstObject = false;
				continue;
			}

			// put the raw vertices from the file and put them into a vector based on elements
			for (int i = 0; i < v_elements.size(); i++)
			{
				vertices.push_back(raw_vertices[v_elements[i] - 1]);
			}

			// do the same for the normals
			for (int i = 0; i < n_elements.size(); i++)
			{
				normals.push_back(raw_normals[n_elements[i] - 1]);
			}

			objects->push_back(*new ModelObject(objName, vertices, normals, textureUVs));

			vertices.clear();
			normals.clear();
			textureUVs.clear();
			n_elements.clear();
			v_elements.clear();
		}

		// lines beginning with 'f ' have elements
		else if (line.substr(0, 2) == "f ")
		{
			istringstream s(line.substr(2));
			string a, b, c;
			s >> a;
			s >> b;
			s >> c;

			// split elements by '/'
			v_elements.push_back(atoi(split(a, '/')[0].c_str()));
			v_elements.push_back(atoi(split(b, '/')[0].c_str()));
			v_elements.push_back(atoi(split(c, '/')[0].c_str()));

			n_elements.push_back(atoi(split(a, '/')[2].c_str()));
			n_elements.push_back(atoi(split(b, '/')[2].c_str()));
			n_elements.push_back(atoi(split(c, '/')[2].c_str()));
		}
		// lines beginning with 'vn ' will have normals
		else if (line.substr(0, 3) == "vn ") {
			istringstream s(line.substr(3));
			vec3 vn;
			s >> vn.x;
			s >> vn.y;
			s >> vn.z;

			raw_normals.push_back(vn);
		}
		else if (line[0] == '#') { /* ignore comments */ }
		else { /* blank/junk */ }
	}

	// put the raw vertices from the file and put them into a vector based on elements
	for (int i = 0; i < v_elements.size(); i++)
	{
		vertices.push_back(raw_vertices[v_elements[i] - 1]);
	}

	// do the same for the normals
	for (int i = 0; i < n_elements.size(); i++)
	{
		normals.push_back(raw_normals[n_elements[i] - 1]);
	}

	objects->push_back(*new ModelObject(objName, vertices, normals, textureUVs));

	return objects;
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

	objects = load_obj("models/test.obj");

	vertices = objects->at(1).getVertices();
	normals = objects->at(1).getNormals();

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

	int
main( int argc, char **argv )
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
