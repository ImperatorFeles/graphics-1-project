#include "World.h"

World::World()
{
}
void World::drawActors()
{
  vector<ModelObject>::iterator iter;
  for( iter = actors.begin(); iter != actors.end(); iter++ ) {
    (*iter).draw();
  }
}
vector<ModelObject>* World::getActors()
{
  return &actors;
}
void World::addActor( ModelObject* newActor )
{
  actors.push_back( *newActor );
}
