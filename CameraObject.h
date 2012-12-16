#ifndef _CAMERA_OBJECT
#define _CAMERA_OBJECT

#include "SceneObject.h"

class CameraObject : public SceneObject
{
	private:
		bool forward, backward, up, down, left, right;
		bool xRotLock, planarLock; // lock x rotation up and down,
								   // and lock movement to a plane
		float moveSpeed, rotSpeed;
		vec3 velocity;
		float plane; // horizontal plane to lock to

	public:
		CameraObject(string objName, float moveSpeed, float rotSpeed);
		CameraObject(string objName, float moveSpeed, float rotSpeed,
				vec3 position, vec3 rotation);

		// update the camera's position based on its speed
		void update();

		// methods for setting which way the camera will move
		void setMoveForward(bool forward);
		void setMoveBackward(bool backward);
		void setMoveUp(bool up);
		void setMoveDown(bool down);
		void setMoveLeft(bool left);
		void setMoveRight(bool right);

		void translate(vec3 translation);
		void rotate(vec3 rotation);

		// flag for locking the x rotation to 90<->-90
		void setLockedXRot(bool lock);

		// lock camera to a specific height
		void lockToPlane(float height);
		void unlockPlane();

	protected:

		void createMatrix();
};

#endif
