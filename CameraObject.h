#ifndef _CAMERA_OBJECT
#define _CAMERA_OBJECT

#include "SceneObject.h"

class CameraObject : public SceneObject
{
	private:
		bool forward, backward, up, down, left, right;
		bool xRotLock;
		float moveSpeed, rotSpeed;
		vec3 velocity;

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

		// methods for updating rotation
		void addXRot(float theta);
		void addYRot(float theta);
		void addZRot(float theta);

		// flag for locking the x rotation to 90<->-90
		void setLockedXRot(bool lock);

	protected:

		void createMatrix();
};

#endif
