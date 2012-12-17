#include "World.h"

World::World()
{
}
void World::bufferActors()
{
  int index = 0;
  GLuint vaos[actors.size()];

  glGenVertexArrays( actors.size(), vaos );

  vector<ModelObject*>::iterator iter;
  for( iter = actors.begin(); iter != actors.end(); iter++ ) {
    actors.at(index)->setVao( vaos[index] );
    index++;
    (*iter)->generateBuffers();
  }
}
void World::drawActors()
{
  vector<ModelObject*>::iterator iter;
  for( iter = actors.begin(); iter != actors.end(); iter++ ) {
    (*iter)->draw();
  }
}
vector<ModelObject*>* World::getActors()
{
  return &actors;
}
void World::addActor( ModelObject* newActor )
{
  actors.push_back( newActor );
}
