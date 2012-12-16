#include "Train.h"

#include <iostream>

using std::cout;
using std::endl;

Train::Train(ModelObject *trainModel, ModelObject *doorModel1, 
		ModelObject *doorModel2, CameraObject *camera, 
		float acceleration, float maxVelocity, int waitTicks, int moveTicks,
		float trainBound):
	station(0), currTicks(waitTicks), movementTicks(moveTicks), 
	waitTicks(waitTicks), doorMoveTicks(105), 
	currAccel(0), maxAccel(acceleration), 
	currVel(0), maxVel(maxVelocity), trainBound(trainBound),  stopped(true), 
	trainModel(trainModel), camera(camera)
{
	this->trainModel->addChild(doorModel1);
	this->trainModel->addChild(doorModel2);

	door1 = doorModel1;
	door2 = doorModel2;
}

void Train::update()
{
	currTicks--;

	if (currTicks == 0 && stopped)
	{
		stopped = false;
		
		// forward if at first station, backward if at second
		if (station == 0)
		{
			currAccel = maxAccel;
		}
		else
		{
			currAccel = -maxAccel;
		}

		if (camera->getPosition().x > trainBound)
		{
			trainModel->addChild(camera);
		}

		currTicks = movementTicks;
	}

	else if (currTicks == 0 && !stopped)
	{
		stopped = true;

		if (station == 0)
		{
			station = 1;
		}
		else
		{
			station = 0;
		}

		camera->removeParent();

		currAccel = 0;
		currVel = 0;

		currTicks = waitTicks;
	}

	if (stopped)
	{
		if (waitTicks - currTicks < doorMoveTicks)
		{
			door1->translate(vec3(0.0, 0.0, -maxVel/10.0));
			door2->translate(vec3(0.0, 0.0, maxVel/10.0));
		}
		else if (currTicks < doorMoveTicks)
		{
			door1->translate(vec3(0.0, 0.0, maxVel/10.0));
			door2->translate(vec3(0.0, 0.0, -maxVel/10.0));
		}
	}

	// slow down if we're approaching the station
	if (!stopped && currTicks < maxVel/maxAccel)
	{
		if (station == 0)
		{
			currAccel = -maxAccel;
		}
		else
		{
			currAccel = maxAccel;
		}
	}

	currVel += currAccel;

	// make sure train doesn't go too fast
	if ((currVel >= maxVel && currAccel > 0) ||
			(currVel <= -maxVel && currAccel < 0))
	{
		if (currAccel > 0)
		{
			currVel = maxVel;
		}
		else if (currAccel < 0)
		{
			currVel = -maxVel;
		}

		currAccel = 0;
	}

	trainModel->translate(vec3(0.0, 0.0, currVel));
}

