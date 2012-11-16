/* sierpinski gasket with vertex arrays */

#include "Angel.h"
#include "vec.h"
#include "mat.h"

#define MOUSE_SENSITIVITY 0.1
#define MOVEMENT_SPEED 0.01;

vec3 points[6];
vec3 colors[6];

vec4 cameraPos;
vec3 cameraRot;
vec4 cameraVel;

vec2 mouseCenter;

mat4 transformation;

GLuint matLoc;

float sensitivity = 0.1;

bool forward, backward;
bool left, right;
bool up, down;

void
init( void )
{
    forward = backward = false;
    left = right = false;
    up = down = false;

    cameraPos = 0;
    cameraPos.z = -4;
    cameraRot = 0;

    mouseCenter = -1;

    transformation = *new mat4();

    // add in plane
    points[0] = vec3(1.0, 1.0, 0.3335);
    points[1] = vec3(1.0, -1.0, 0.3335);
    points[2] = vec3(-1.0, -1.0, 0.3335);
    points[3] = points[2];
    points[4] = vec3(-1.0, 1.0, 0.3335);
    points[5] = points[0];

    for (int i = 0; i < 6; i++)
    {
      colors[i] = vec3(0.0, 0.5, 0.0);
    }

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
    glBufferData( GL_ARRAY_BUFFER, sizeof(points) + sizeof(colors),
		  NULL, GL_STATIC_DRAW );

    // Next, we load the real data in parts.  We need to specify the
    //   correct byte offset for placing the color data after the point
    //   data in the buffer.  Conveniently, the byte offset we need is
    //   the same as the size (in bytes) of the points array, which is
    //   returned from "sizeof(points)".
    glBufferSubData( GL_ARRAY_BUFFER, 0, sizeof(points), points );
    glBufferSubData( GL_ARRAY_BUFFER, sizeof(points), sizeof(colors), colors );

    // Load shaders and use the resulting shader program
    GLuint program = InitShader( "vshader.glsl", "fshader.glsl" );
    glUseProgram( program );

    // Initialize the vertex position attribute from the vertex shader    
    GLuint vPosition = glGetAttribLocation( program, "vPosition" );
    glEnableVertexAttribArray( vPosition );
    glVertexAttribPointer( vPosition, 3, GL_FLOAT, GL_FALSE, 0,
                           BUFFER_OFFSET(0) );

    // Likewise, initialize the vertex color attribute.  Once again, we
    //    need to specify the starting offset (in bytes) for the color
    //    data.  Just like loading the array, we use "sizeof(points)"
    //    to determine the correct value.
    GLuint vColor = glGetAttribLocation( program, "vColor" );
    glEnableVertexAttribArray( vColor );
    glVertexAttribPointer( vColor, 3, GL_FLOAT, GL_FALSE, 0,
                           BUFFER_OFFSET(sizeof(points)) );

    matLoc = glGetUniformLocation(program, "m");

    glEnable( GL_DEPTH_TEST );

    glClearColor( 0.0, 0.0, 0.0, 1.0 ); /* white background */
}
//----------------------------------------------------------------------------

void
display( void )
{
  glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

  // transform the camera position based on the rotation and the velocity
  transformation = identity();
  transformation = transformation  * RotateY(-cameraRot.y) * RotateX(-cameraRot.x);
  cameraPos = cameraPos + transformation * cameraVel;

  glutSolidTeapot(1);

  // build the transformation matrix
  transformation = identity();

  transformation = transformation * Perspective(60, 1.0, 0.01, 100);      
  transformation = transformation * RotateX(cameraRot.x) * RotateY(cameraRot.y);
  transformation = transformation * Translate(cameraPos.x, cameraPos.y, cameraPos.z);
  transformation = transformation * RotateX(90);
  
  glUniformMatrix4fv(matLoc, 1, true, transformation);
  glDrawArrays( GL_TRIANGLES, 0, 6);
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
    forward = true;
    break;
  case 's':
    backward = true;
    break;
  case 'd':
    right = true;
    break;
  case 'a':
    left = true;
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
    forward = false;
    break;
  case 's':
    backward = false;
    break;
  case 'd':
    right = false;
    break;
  case 'a':
    left = false;
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
    forward = button == GLUT_DOWN;
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
  if (forward)
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
  if (left)
  {
    cameraVel.x = MOVEMENT_SPEED;
  }
  else if (right)
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

  glutPostRedisplay(); 
}
//----------------------------------------------------------------------------

int
main( int argc, char **argv )
{
    glutInit( &argc, argv );
    glutInitDisplayMode( GLUT_RGBA | GLUT_DEPTH | GLUT_SINGLE);
    glEnable(GL_DEPTH_TEST);
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
