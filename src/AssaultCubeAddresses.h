#pragma once
#include <vector>

#define ADDR_LOCAL_PLAYER_ENTITY								0x0018AC00
#define ADDR_ENTITY_LIST										0x0018AC04

#define ADDR_FIRST_WEAPON_AMMO									0x0017E0A8
const std::vector<unsigned int>OFF_FIRST_WEAPON_AMMO			= { 0x364, 0x14, 0x0 };

#define ADDR_GAME_MODE											0x0018ABF8
#define ADDR_NUM_PLAYERS										0x0018AC0C
#define ADDR_MATRIX												0x0057DFD0

#define ADDR_NORECOIL_FUNCTION									0x000C8BA0
#define VAL_ENTITY_VTABLE_COMPARE								0x0054D07C