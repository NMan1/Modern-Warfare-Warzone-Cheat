#pragma once
#include <Windows.h>
#include "Vector.h"
#include <vector>

bool GetPid();

class cl_entity
{
public:
	char pad_0000[56]; //0x0000
	int64_t xor1; //0x0038
	int64_t xor2; //0x0040
	int64_t xorVar; //0x0048
	char pad_0050[8]; //0x0050
	uint64_t isAlive; //0x0058
	char pad_0060[280]; //0x0060
	Vector location; //0x0178
	char pad_0184[24]; //0x0184
	Vector rotation; //0x019C
	char pad_01A8[56]; //0x01A8
	uint16_t type; //0x01E0
	char pad_01E2[2]; //0x01E2
	uint8_t stance; //0x01E4
	char pad_01E5[263]; //0x01E5
	uint8_t valid; //0x02EC
	char pad_02ED[43]; //0x02ED
}; //Size: 0x0318

namespace Entity
{
	cl_entity* GetEntityByID(unsigned long i);
	int GetTeamID(unsigned long i);
	unsigned long GetLocalIndex();
	uint64_t GetCGEntitys();
	std::vector<cl_entity*> GetMaxEntitys();
}