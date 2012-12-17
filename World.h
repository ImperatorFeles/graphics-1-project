#ifndef _WORLD
#define _WORLD

#include <vector>
#include "ModelObject.h"

using std::vector;

class World
{
 private:
  vector<ModelObject*> actors;
  
 public:
  World();
  void drawActors();
  void bufferActors();
  vector<ModelObject*>* getActors();
  void addActor( ModelObject *newActor );

};

#endif
