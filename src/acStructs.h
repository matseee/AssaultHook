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

#define WEAPON_BELOW_EYES .2f;

// vector struct from assualtcube
#define RAD   (0.01745329252f) /* PI / 180 */
struct vec
{
    union
    {
        struct { float x, y, z; };
        float v[3];
        int i[3];
    };

    vec() { x = y = z = 0; }
    vec(float a, float b, float c) : x(a), y(b), z(c) {}
    vec(float* v) : x(v[0]), y(v[1]), z(v[2]) {}

    float& operator[](int i) { return v[i]; }
    float  operator[](int i) const { return v[i]; }

    bool iszero() const { return x == 0 && y == 0 && z == 0; }

    bool operator==(const vec& o) const { return x == o.x && y == o.y && z == o.z; }
    bool operator!=(const vec& o) const { return x != o.x || y != o.y || z != o.z; }
    vec operator-() const { return vec(-x, -y, -z); }

    vec& mul(float f) { x *= f; y *= f; z *= f; return *this; }
    vec& div(float f) { x /= f; y /= f; z /= f; return *this; }
    vec& add(float f) { x += f; y += f; z += f; return *this; }
    vec& sub(float f) { x -= f; y -= f; z -= f; return *this; }

    vec& add(const vec& o) { x += o.x; y += o.y; z += o.z; return *this; }
    vec& sub(const vec& o) { x -= o.x; y -= o.y; z -= o.z; return *this; }

    float squaredlen() const { return x * x + y * y + z * z; }
    float sqrxy() const { return x * x + y * y; }

    float dot(const vec& o) const { return x * o.x + y * o.y + z * o.z; }
    float dotxy(const vec& o) const { return x * o.x + y * o.y; }

    float magnitude() const { return sqrtf(squaredlen()); }
    vec& normalize() { div(magnitude()); return *this; }

    float dist(const vec& e) const { vec t; return dist(e, t); }
    float dist(const vec& e, vec& t) const { t = *this; t.sub(e); return t.magnitude(); }
    float squareddist(const vec& e) const { vec t = *this; t.sub(e); return t.squaredlen(); }

    float distxy(const vec& e) const { float dx = e.x - x, dy = e.y - y; return sqrtf(dx * dx + dy * dy); }
    float magnitudexy() const { return sqrtf(x * x + y * y); }
    float anglexy() const { return atan2f(x, y) / RAD; }

    bool reject(const vec& o, float max) const { return x > o.x + max || x<o.x - max || y>o.y + max || y < o.y - max; }

    vec& cross(const vec& a, const vec& b) { x = a.y * b.z - a.z * b.y; y = a.z * b.x - a.x * b.z; z = a.x * b.y - a.y * b.x; return *this; }
    float cxy(const vec& a) { return x * a.y - y * a.x; }

    void rotate_around_z(float angle) { *this = vec(cosf(angle) * x - sinf(angle) * y, cosf(angle) * y + sinf(angle) * x, z); }
    void rotate_around_x(float angle) { *this = vec(x, cosf(angle) * y - sinf(angle) * z, cosf(angle) * z + sinf(angle) * y); }
    void rotate_around_y(float angle) { *this = vec(cosf(angle) * x - sinf(angle) * z, y, cosf(angle) * z + sinf(angle) * x); }

    vec& rotate(float angle, const vec& d)
    {
        float c = cosf(angle), s = sinf(angle);
        return rotate(c, s, d);
    }

    vec& rotate(float c, float s, const vec& d)
    {
        *this = vec(x * (d.x * d.x * (1 - c) + c) + y * (d.x * d.y * (1 - c) - d.z * s) + z * (d.x * d.z * (1 - c) + d.y * s),
            x * (d.y * d.x * (1 - c) + d.z * s) + y * (d.y * d.y * (1 - c) + c) + z * (d.y * d.z * (1 - c) - d.x * s),
            x * (d.x * d.z * (1 - c) - d.y * s) + y * (d.y * d.z * (1 - c) + d.x * s) + z * (d.z * d.z * (1 - c) + c));
        return *this;
    }

    void orthogonal(const vec& d)
    {
        int i = fabs(d.x) > fabs(d.y) ? (fabs(d.x) > fabs(d.z) ? 0 : 2) : (fabs(d.y) > fabs(d.z) ? 1 : 2);
        v[i] = d[(i + 1) % 3];
        v[(i + 1) % 3] = -d[i];
        v[(i + 2) % 3] = 0;
    }
};

// Created with ReClass.NET 1.2 by KN4CK3R

class AcAnimateState {
public:
	int32_t Anim; //0x0000
	int32_t Frame; //0x0004
	int32_t Range; //0x0008
	int32_t BaseTime; //0x000C
	float Speed; //0x0010
}; //Size: 0x0014

class AcPositionHistory {
public:
	int32_t NextUpdate; //0x0000
	int32_t CurrentPos; //0x0004
	int32_t NumPos; //0x0008
	Vector3 Positions[7]; //0x000C
}; //Size: 0x0060

class AcEntity {
public:
	int32_t VTable; //0x0000
	Vector3 Origin; //0x0004
	Vector3 Velocity; //0x0010
	Vector3 DeltaPosition; //0x001C
	Vector3 NewPosition; //0x0028
	Vector3 Angle; //0x0034
	float PitchVelocity; //0x0040
	float MaxSpeed; //0x0044
	int32_t TimeInAir; //0x0048
	float Radius; //0x004C
	float EyeHeight; //0x0050
	float MaxEyeHeight; //0x0054
	float AboveEye; //0x0058
	bool InWater; //0x005C
	bool OnFloor; //0x005D
	bool OnLadder; //0x005E
	bool JumpNext; //0x005F
	bool JumpDone; //0x0060
	bool IsCrouching; //0x0061
	bool CrouchedInAir; //0x0062
	bool TryCrouch; //0x0063
	bool CanCollide; //0x0064
	bool Stuck; //0x0065
	bool Scoping; //0x0066
	int32_t LastJumpTimestamp; //0x0067
	float LastJumpHeight; //0x006B
	int32_t LastSplash; //0x006F
	int8_t N000000E8; //0x0073
	int8_t Move; //0x0074
	int8_t Strafe; //0x0075
	uint8_t State; //0x0076
	uint8_t Type; //0x0077
	float EyeHeightValue; //0x0078
	int32_t LastPosition; //0x007C
	bool KeyLeft; //0x0080
	bool KeyRight; //0x0081
	bool KeyUp; //0x0082
	bool KeyDown; //0x0083
	class AcAnimateState PrevAnimation[2]; //0x0084
	class AcAnimateState CurrentAnimation[2]; //0x00AC
	int32_t LastAnimationSwitchTime[2]; //0x00D4
	int32_t LastModel[2]; //0x00DC
	int32_t LastRendered; //0x00E4
	int32_t Health; //0x00E8
	int32_t Armour; //0x00EC
	int32_t Primary; //0x00F0
	int32_t NextPrimary; //0x00F4
	int32_t Akimbo; //0x00F8
	int32_t AmmoOfGuns[9]; //0x00FC
	int32_t MagazineOfGuns[9]; //0x0120
	int32_t WaittimeOfGuns[9]; //0x0144
	int32_t ShotsOfGuns[9]; //0x0168
	int32_t DamageOfGuns[9]; //0x018C
	char pad_01B0[40]; //0x01B0
	int32_t Frags; //0x01D8
	int32_t FlagScore; //0x01DC
	int32_t Deaths; //0x01E0
	int32_t Teamkills; //0x01E4
	int32_t LastAction; //0x01E8
	int32_t LastMove; //0x01EC
	int32_t LastPain; //0x01F0
	int32_t LastVoiceCom; //0x01F4
	int32_t LastDeath; //0x01F8
	int32_t ClientRole; //0x01FC
	bool Attacking; //0x0200
	char Name[260]; //0x0201
	int32_t Team; //0x0305
	int32_t WeaponChanging; //0x0309
	int32_t SwitchWeaponTo; //0x030D
	int32_t SpectateMode; //0x0311
	int32_t FollowPlayerCam; //0x0315
	int32_t EarDamageMillis; //0x0319
	float MaxRoll; //0x031D
	float MaxRollEffect; //0x0321
	float MovRoll; //0x0325
	float EffRoll; //0x0329
	int32_t FFov; //0x032D
	int32_t ScopeFov; //0x0331
	int32_t Weapons[9]; //0x0335
	int32_t PrevWeapon; //0x0359
	int32_t Weapon; //0x035D
	int32_t NextWeapon; //0x0361
	int32_t PrimaryWeapon; //0x0365
	int32_t NextPrimaryWeapon; //0x0369
	int32_t LastAttackWeapon; //0x036D
	class AcPositionHistory HistoryPosition; //0x0371
	uint32_t SkinNoTeam; //0x03D1
	uint32_t SkinCla; //0x03D5
	uint32_t SkinRvsf; //0x03D9
	float DeltaYaw; //0x03DD
	float DeltaPitch; //0x03E1
	float NewYaw; //0x03E5
	float NewPitch; //0x03E9
	int32_t SmoothMillis; //0x03ED
	Vector3 Head; //0x03F1
	bool Ignored; //0x03FD
	bool Muted; //0x03FE
	bool NoCorpse; //0x03FF
}; //Size: 0x0400

class AcEntityList {
public:
	class AcEntity* Entities[32]; //0x0000
}; //Size: 0x0080
