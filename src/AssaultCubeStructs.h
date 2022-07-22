#pragma once
#include "pch.h"
#include "framework.h"
#include "geomatry.h"

enum GameTypes {
	TEAMDEATHMATCH = 0,
	COOPEDIT,
	DEATHMATCH,
	SURVIVOR,
	TEAMSURVIVOR,
	CTF,
	PISTOLFRENZY,
	BOTTEAMDEATHMATCH,
	BOTDEATHMATCH,
	LASTSWISSSTANDING,
	ONESHOTONEKILL,
	TEAMONESHOTONEKILL,
	BOTONESHOTONEKILL,
	HUNTTHEFLAG,
	TEAMKEEPTHEFLAG,
	KEEPTHEFLAG,
	TEAMPF,
	TEAMLSS,
	BOTPISTOLFRENZY,
	BOtlSS,
	BOTTEAMSURVIVOR,
	BOTTEAMONESHOTONEKILL,
	NUM
};

struct traceresult_s {
	Vector3 end;
	bool collided;
};

class Entity {
public:
	uint32_t VTable; //0x0000
	Vector3 PositionHead; //0x0004
	char pad_0010[24]; //0x0010
	Vector3 Position; //0x0028
	Vector3 Angle; //0x0034
	char pad_003C[40]; //0x003C
	uint8_t N0000021C; //0x0064
	uint8_t N0000021E; //0x0065
	uint8_t Scoping; //0x0066
	char pad_0067[133]; //0x0067
	int32_t Health; //0x00EC
	char pad_00F0[24]; //0x00F0
	int32_t AmmoPistol2; //0x0108
	int32_t AmmoRifle2; //0x010C
	char pad_0110[28]; //0x0110
	int32_t AmmoPistol1; //0x012C
	int32_t AmmoRifle1; //0x0130
	char pad_0134[28]; //0x0134
	int32_t WaitPistol; //0x0150
	int32_t WaitRifle; //0x0154
	char pad_0158[28]; //0x0158
	int32_t ShotsFiredPistol; //0x0174
	int32_t ShotsFiredRifle; //0x0178
	char pad_017C[108]; //0x017C
	int32_t N000000C7; //0x01E8
	char pad_01EC[24]; //0x01EC
	uint8_t Dummy; //0x0204
	char Name[15]; //0x0205
	char pad_0214[248]; //0x0214
	int32_t Team; //0x030C
	char pad_0310[84]; //0x0310
	class Weapon* CurrentWeapon; //0x0364
	char pad_0368[472]; //0x0368
}; //Size: 0x0540

class Base {
public:
	class EnitityList* EntityListPointer; //0x0000
}; //Size: 0x0004

class EnitityList {
public:
	class Entity* Entities[32]; //0x0000
}; //Size: 0x0080

class Weapon {
public:
	char pad_0000[8]; //0x0000
	class Entity* WeaponOwner; //0x0008
	class WeaponInitializer* WeaponInitValues; //0x000C
	char pad_0010[80]; //0x0010
}; //Size: 0x0060

class WeaponInitializer {
public:
	char pad_0000[64]; //0x0000
	int16_t N00000E42; //0x0040
	int16_t N00000F46; //0x0042
	int16_t N00000E43; //0x0044
	int16_t N00000F48; //0x0046
	int16_t N00000E44; //0x0048
	int16_t N00000F4A; //0x004A
	int16_t N00000E45; //0x004C
	int16_t N00000F4C; //0x004E
	int16_t N00000E46; //0x0050
	int16_t N00000F4E; //0x0052
	int16_t N00000E47; //0x0054
	int16_t N00000F50; //0x0056
	int16_t N00000E48; //0x0058
	int16_t N00000F52; //0x005A
	int16_t N00000E49; //0x005C
	int16_t N00000F54; //0x005E
	int16_t recoil1; //0x0060
	int16_t recoil2; //0x0062
	int16_t N00000E4B; //0x0064
	int16_t N00000F58; //0x0066
}; //Size: 0x0068