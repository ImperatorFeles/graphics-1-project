#ifndef _MODEL_OBJECT
#define _MODEL_OBJECT

#include "SceneObject.h"
#include "vec.h"

#include <vector>

using std::vector;

class ModelObject : public SceneObject
{
	private:
		vector<vec4> vertices; // stores all vertices of the object
		vector<vec3> normals; // stores normals of the faces of the object
		vector<vec2> textureUVs; // stores UV coordinates for faces
		GLuint samplerLoc; // texture sampler location in shader
		GLuint matLoc; // matrix location in shader
		GLuint texture; // handle to loaded texture
		GLuint vao; // handle to the array object

	public:
		// create model object without position, rotation, scale
		ModelObject(string objName, vector<vec4> vertices, vector<vec3> normals, vector<vec2> textureUVs);
		ModelObject(string objName, vector<vec4> vertices, vector<vec3> normals, vector<vec2> textureUVs, 
					vec3 position, vec3 rotation, vec3 scale);
		
		// accessors for vector objects
		vector<vec4> getVertices();
		vector<vec3> getNormals();
		vector<vec2> getTextureUVs();
		
		// methods for buffering and for drawing
		void generateBuffers();
		void draw();
		void loadTexture(string filename);
};

#endif
