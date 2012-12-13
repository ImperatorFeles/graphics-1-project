#include <vector>
#include <string>
#include <fstream>
#include <sstream>

#include "OBJParser.h"
#include "vec.h"

using namespace std;

 /*
   Basic function to split strings over a delimiter
   used in model parsing
	ex: splitting foo&bar over '&' will return a vector {foo, bar}
		splitting foo over '&' will return a vector {foo}
		splitting f..oo over '.' will return a vector {f, , ,oo}
 */
vector<string>* OBJParser::split(string str, char delim)
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

	return elements;
}

/*
   .obj model loader
   models must be triangulated, and have normals included to be
   correctly parsed
   vertices, normals, vertex elements, and normal elements will be returned to pointers to vectors
 */
void OBJParser::load_obj(const char* filename, World& theWorld)
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
	vector<vec2> raw_textureUVs;
	vector<vec2> textureUVs;

      	string line;
	string objName;

	// parse the .obj file for its data
	while (getline(in, line))
	{
		// lines beginning with 'v ' have vertices
		if (line.substr(0, 2) == "v ")
		{
			raw_vertices.push_back(*parseVertex(line));
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

			theWorld.addActor(new ModelObject(objName, vertices, normals, textureUVs));

			vertices.clear();
			normals.clear();
			textureUVs.clear();
		}

		// lines beginning with 'f ' have elements
		else if (line.substr(0, 2) == "f ")
		{
			// parse the elements from the .obj line
			vector< vector<int> > faceElements = *parseFaceElements(line);

			// put the vertices described by the element vector into the main vertex vector 
			for (uint i = 0; i < faceElements.at(0).size(); i++)
			{
				vertices.push_back(raw_vertices.at(faceElements.at(0).at(i) - 1));
			}

			// do the same for texture elements
			for (uint i = 0; i < faceElements.at(1).size(); i++)
			{
				textureUVs.push_back(raw_textureUVs.at(faceElements.at(1).at(i) - 1));
			}

			// and finally for normal elements
			for (uint i = 0; i < faceElements.at(2).size(); i++)
			{
				normals.push_back(raw_normals.at(faceElements.at(2).at(i) - 1));
			}
		}

		// lines beginning with 'vt ' will have UV coords
		else if (line.substr(0, 3) == "vt ")
		{
			raw_textureUVs.push_back(*parseTextureUV(line));
		}

		// lines beginning with 'vn ' will have normals
		else if (line.substr(0, 3) == "vn ") 
		{
			raw_normals.push_back(*parseNormal(line));
		}

		else if (line[0] == '#') { /* ignore comments */ }
		else { /* blank/junk */ }
	}

	theWorld.addActor(new ModelObject(objName, vertices, normals, textureUVs));

}

vec4* OBJParser::parseVertex(string line)
{
	vec4* v = new vec4();

	// read in vertex coords
	istringstream s(line.substr(2));
	s >> v->x;
	s >> v->y;
	s >> v->z;
	v->w = 1.0;

	return v;
}

vec2* OBJParser::parseTextureUV(string line)
{
	vec2* uv = new vec2();

	// read in uv coords
	istringstream s(line.substr(3));
	s >> uv->x;
	s >> uv->y;

	return uv;
}

vec3* OBJParser::parseNormal(string line)
{
	vec3 *vn = new vec3();

	istringstream s(line.substr(3));
	s >> vn->x;
	s >> vn->y;
	s >> vn->z;

	return vn;
}

vector< vector<int> >* OBJParser::parseFaceElements(string line)
{
	vector< vector<int> >* elements = new vector< vector<int> >();
	// vectors for vertex, texture, and normal elements
	vector<int> *v_elements = new vector<int>();
	vector<int> *uv_elements = new vector<int>();
	vector<int> *n_elements = new vector<int>();

	// vector for elements returned from split
	vector<string> raw_elements;

	// get the 3 triples of elements from the line
	istringstream s(line.substr(2));
	string a, b, c;
	s >> a;
	s >> b;
	s >> c;

	// parse the 3 triples into 3 vectors
	parseElementTriple(a, v_elements, uv_elements, n_elements);
	parseElementTriple(b, v_elements, uv_elements, n_elements);
	parseElementTriple(c, v_elements, uv_elements, n_elements);

	// add the vectors of different elements to single vector
	elements->push_back(*v_elements);
	elements->push_back(*uv_elements);
	elements->push_back(*n_elements);

	return elements;
}

// parses a line of elements and adds the elements to the vectors passed in
// if a line has less than 3 kinds of elements, the vectors of the types
// of elements that are not included in the file will not be updated
void OBJParser::parseElementTriple(string triple, vector<int> *v_elements,
	vector<int> *uv_elements, vector<int> *n_elements)
{
	vector<string> raw_elements = *split(triple, '/');

	switch (raw_elements.size())
	{
		// if we have at least 3 types, add in normals
		case 3:
			n_elements->push_back(atoi(raw_elements[2].c_str()));	
		// if we have at least 2, add in uv elements if they exist
		case 2:
			if (raw_elements[1].compare("") != 0)
			{
				uv_elements->push_back(atoi(raw_elements[1].c_str()));
			}
		// add in vertex elements
		case 1:
			v_elements->push_back(atoi(raw_elements[0].c_str()));
	}
}

