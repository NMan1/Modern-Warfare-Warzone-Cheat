#include "Camera.h"

bool WorldToScreen(Vector vOrigin, Camera* refdef, Vector2D* vOut)
{
	Vector vLocal, vTrans;

	vLocal = vOrigin - refdef->ViewLocation;

	vTrans.x = vLocal.Dot(refdef->Matrix[1]);
	vTrans.y = vLocal.Dot(refdef->Matrix[2]);
	vTrans.z = vLocal.Dot(refdef->Matrix[0]);

	if (vTrans.z < 0.01f)
		return false;

	vOut->x = ((refdef->Width / 2) * (1 - (vTrans.x / refdef->FovX / vTrans.z)));
	vOut->y = ((refdef->Height / 2) * (1 - (vTrans.y / refdef->FovY / vTrans.z)));

	return true;
}