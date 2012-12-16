#ifndef _TRAIN_H
#define _TRAIN_H

#include "ModelObject.h"
#include "CameraObject.h"

class Train
{
	private:
		int station;
		int currTicks, movementTicks, waitTicks, doorMoveTicks;
		float currAccel, maxAccel;
		float currVel, maxVel;
		float trainBound;
		bool stopped;
		ModelObject *trainModel;
		ModelObject *door1;
		ModelObject *door2;
		CameraObject *camera;

	public:
		Train(ModelObject *trainModel, ModelObject *doorModel1, ModelObject *doorModel2, CameraObject *camera, float acceleration,
				float maxVelocity, int waitTicks, int moveTicks, float trainBound);

		void update();

};

#endif
