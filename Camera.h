#pragma once
#include "Vector.h"
#include "Vector2D.h"

class RefDef
{
public:
	class Camera* m_pCamera; //0x0000
}; //Size: 0x0008

class Camera
{
public:
	char pad_0000[8]; //0x0000
	uint32_t Width; //0x0008
	uint32_t Height; //0x000C
	float FovX; //0x0010
	float FovY; //0x0014
	float Unk; //0x0018
	char pad_001C[8]; //0x001C
	Vector Matrix[3]; //0x0024
	char pad_0048[52]; //0x0048
	Vector ViewLocationDelayed0; //0x007C
	Vector ViewLocationDelayed1; //0x0088
	char pad_0094[2808]; //0x0094
	Vector ViewMatrixTest[3]; //0x0B8C
	Vector ViewLocation; //0x0BB0
}; //Size: 0x0BBC

bool WorldToScreen(Vector vOrigin, Camera* refdef, Vector2D* vOut);