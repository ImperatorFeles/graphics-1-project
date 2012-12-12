#include "ModelObject.h"
#include "vec.h"

#include <vector>
#include <iostream>

using std::cout;
using std::endl;
using std::vector;

ModelObject::ModelObject(string objName, vector<vec4> vertices, vector<vec3> normals, vector<vec2> textureUVs): 
		SceneObject(objName), vertices(vertices),
		normals(normals), textureUVs(textureUVs)
{
}

ModelObject::ModelObject(string objName, vector<vec4> vertices, vector<vec3> normals, vector<vec2> textureUVs, vec3 position, vec3 rotation, vec3 scale): 
		SceneObject(objName, position, rotation, scale), vertices(vertices),
		normals(normals), textureUVs(textureUVs)
{
}

vector<vec4> ModelObject::getVertices()
{
	return vertices;
}

vector<vec3> ModelObject::getNormals()
{
	return normals;
}

vector<vec2> ModelObject::getTextureUVs()
{
	return textureUVs;
}

void ModelObject::generateBuffers()
{
  //Set all vertices to have white color. We may want to either abstract this into a class member variable or ditch it entirely
  vector<vec3> colors;
  colors.resize( vertices.size() );
  for( unsigned int i = 0; i < vertices.size(); i++ ) {
    colors[i] = vec3( 1.0, 1.0, 1.0 );
  }

  //Initialize the shaders to use
  GLuint program = InitShader( "vshader.glsl", "fshader.glsl" );
  glUseProgram( program );
  
  //Create VAO
  GLuint vao;
  glGenVertexArrays( 1, &vao );
  glBindVertexArray( vao );
  
  //Create VBOs
  GLuint vbos[4];
  glGenBuffers( 4, vbos );
  
  //Buffer data for vertices, and send data to the shader
  glBindBuffer( GL_ARRAY_BUFFER, vbos[0] ); //set current active buffer
  glBufferData( GL_ARRAY_BUFFER, sizeof( vec4 ) * vertices.size(), &vertices[0], GL_STATIC_DRAW );
  GLuint vPosition = glGetAttribLocation( program, "vPosition" );
  glEnableVertexAttribArray( vPosition );
  glVertexAttribPointer( vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0) );
  
  //Buffer data for color, and send data to the shader
  glBindBuffer( GL_ARRAY_BUFFER, vbos[1] ); //set current active buffer
  glBufferData( GL_ARRAY_BUFFER, sizeof( vec3 ) * colors.size(), &colors[0], GL_STATIC_DRAW );
  GLuint vColor = glGetAttribLocation( program, "vColor" );
  glEnableVertexAttribArray( vColor );
  glVertexAttribPointer( vColor, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0) );

  //Buffer data for normals, and send data to the shader
  glBindBuffer( GL_ARRAY_BUFFER, vbos[2] ); //set current active buffer
  glBufferData( GL_ARRAY_BUFFER, sizeof( vec3 ) * normals.size(), &normals[0], GL_STATIC_DRAW );
  GLuint vNormal = glGetAttribLocation( program, "vNormal" );
  glEnableVertexAttribArray( vNormal );
  glVertexAttribPointer( vNormal, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0) );

  //Buffer data for UVs, and send data to the shader
  glBindBuffer( GL_ARRAY_BUFFER, vbos[3] ); //set current active buffer
  glBufferData( GL_ARRAY_BUFFER, sizeof( vec2 ) * textureUVs.size(), &textureUVs[0], GL_STATIC_DRAW );
  GLuint vTexture = glGetAttribLocation( program, "vTexture" );
  glEnableVertexAttribArray( vTexture );
  glVertexAttribPointer( vTexture, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0) );

  //Send additional data to the shader
  matLoc = glGetUniformLocation(program, "m");
  samplerLoc = glGetUniformLocation(program, "gSampler");

}

void ModelObject::draw()
{

  glUniformMatrix4fv( matLoc, 1, true, getTransformationMatrix() );
  glUniform1i( samplerLoc, 0 );
  glDrawArrays( GL_TRIANGLES, 0, vertices.size() );

}
