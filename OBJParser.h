#ifndef _OBJ_PARSER
#define _OBJ_PARSER

#include <vector>
#include <string>
#include <exception>

#include "vec.h"
#include "ModelObject.h"

using namespace std;

namespace OBJParser
{
	// splits a string over a delimiter and returns tokens
	vector<string>* split(string str, char delim);

	// loads the .obj file into a ModelObject object
	vector<ModelObject>* load_obj(const char* filename);


	// parses a vertex's coordinates from an .obj line
	vec4* parseVertex(string line);

	// parses a vertex's UV coordinates from an .obj line
	vec2* parseTextureUV(string line);

	// parsers a normal vector from an .obj line
	vec3* parseNormal(string line);

	// parses face elements into a vector of 3 vectors of 3 ints
	// from an .obj line
	vector< vector<int> >* parseFaceElements(string line);

	// parses individual elements from a triple of elements and
	// adds them to the vectors passed into the function
	void parseElementTriple(string triple, vector<int> *v_elements,
			vector<int> *uv_elements, vector<int> *n_elements);
}

#endif
