#include "acFunctions.h"

// Wrapper for AssaultCubes original "intersectgeometry" function - this function changes the "to"-vector to the location
// of the first intersection with a world object. We can use this function to check if a player is behind a wall (or other object) or not.
// int __fastcall IntersectGeometry(float* a1, int WorldPos) <= at 0x004CCA80
void AcIntersectGeometry(const vec& from, vec& to) {
	DWORD fIntersectGeometry = AcState::Get()->IntersectGeometry;

	void* pFrom = (void*)&from;
	void* pTo = (void*)&to;

	__asm {
		mov ecx, pFrom
		mov edx, pTo
		call fIntersectGeometry
	}
}

// Wrapper for AssaultCubes original "intersectclosest" function (at = LocalPlayer) - I thought that I can use this function as my TraceLine-
// Function but It only searches for the first player in the "line". It only recognizes PhysEnts and none other game objects...
// int __usercall IntersectClosest@<eax>(int to@<edx>, int at, float* bestdistsquared, _DWORD* hitzone, char aAiming) <= at 0x004CA250
AcEntity* AcIntersectClosest(const vec& from, const vec& to, const AcEntity* at, float& bestdistsquared, int& hitzone, bool aiming) {
	DWORD fIntersectClosest = AcState::Get()->IntersectGeometry;
	AcEntity* pResultEntity = nullptr;

	void* pHitzone = &hitzone;
	void* pBestDist = &bestdistsquared;
	void* pAt = (void*)at;
	void* pTo = (void*)&to;

	// Compiler magic happens here. This function gets called from four different places, but everytime the "at"-Entity is the LocalPlayer
	// and also the "from"-argument is the position of the LocalPlayer - SO the compiler decided to throw away the from-argument and only use
	// the "at" entity (which is the same address)...
	__asm
	{
		push [aiming]
		push pHitzone
		push pBestDist
		push pAt
		mov edx, pTo
		call fIntersectClosest
		mov pResultEntity, eax
		add esp, 10h
	}

	return pResultEntity;
}
