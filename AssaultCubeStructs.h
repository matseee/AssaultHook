#pragma once
#include "pch.h"
#include "framework.h"

class Entity
{
public:
	uint32_t VTable; //0x0000
	Vector3 PositionHead; //0x0004
	char pad_0010[24]; //0x0010
	Vector3 Position; //0x0028
	Vector2 Angle; //0x0034
	char pad_003C[176]; //0x003C
	int32_t Health; //0x00EC
	char pad_00F0[24]; //0x00F0
	int32_t AmmoPistol2; //0x0108
	char pad_010C[16]; //0x010C
	int32_t AmmoRifle2; //0x011C
	char pad_0120[12]; //0x0120
	int32_t AmmoPistol1; //0x012C
	char pad_0130[16]; //0x0130
	int32_t AmmoRifle1; //0x0140
	char pad_0144[12]; //0x0144
	int32_t WaitPistol; //0x0150
	char pad_0154[16]; //0x0154
	int32_t WaitRifle; //0x0164
	char pad_0168[12]; //0x0168
	int32_t ShotsFiredPistol; //0x0174
	char pad_0178[16]; //0x0178
	int32_t ShotsFiredRifle; //0x0188
	char pad_018C[120]; //0x018C
	uint8_t N000000D0; //0x0204
	char Name[16]; //0x0205
	char pad_0215[1]; //0x0215
}; //Size: 0x0216

class Base
{
public:
	class EnitityList* EntityListPointer; //0x0000
}; //Size: 0x0004

class EnitityList
{
public:
	class Entity* Entities[32]; //0x0000
}; //Size: 0x0080