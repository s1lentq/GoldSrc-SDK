/*
*
*    Copyright (c) 1996-2002, Valve LLC. All rights reserved.
*
*    This product contains software technology licensed from Id
*    Software, Inc. ("Id Technology").  Id Technology (c) 1996 Id Software, Inc.
*    All Rights Reserved.
*
*    Use, distribution, and modification of this source code and/or resulting
*    object code is restricted to non-commercial enhancements to products from
*    Valve LLC.  All other use, distribution, or modification is prohibited
*    without written permission from Valve LLC.
*
*/

#pragma once

enum TANKBULLET
{
	TANK_BULLET_NONE = 0,	// Custom damage
	TANK_BULLET_9MM,		// env_laser (duration is 0.5 rate of fire)
	TANK_BULLET_MP5,		// rockets
	TANK_BULLET_12MM,		// explosion?
};

#define SF_TANK_ACTIVE      BIT(0)
#define SF_TANK_PLAYER      BIT(1)
#define SF_TANK_HUMANS      BIT(2)
#define SF_TANK_ALIENS      BIT(3)
#define SF_TANK_LINEOFSIGHT BIT(4)
#define SF_TANK_CANCONTROL  BIT(5)

#define SF_TANK_SOUNDON     BIT(15)

class CFuncTank: public CBaseEntity
{
public:
	virtual void Spawn() = 0;
	virtual void Precache() = 0;
	virtual void KeyValue(KeyValueData *pkvd) = 0;
	virtual int Save(CSave &save) = 0;
	virtual int Restore(CRestore &restore) = 0;

	// Bmodels don't go across transitions
	virtual int ObjectCaps() = 0;
	virtual BOOL OnControls(entvars_t *pevTest) = 0;
	virtual void Think() = 0;
	virtual void Use(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value) = 0;
	virtual void Fire(const Vector &barrelEnd, const Vector &forward, entvars_t *pevAttacker) = 0;
	virtual Vector UpdateTargetPosition(CBaseEntity *pTarget) = 0;
public:
	BOOL IsActive() const { return (pev->spawnflags & SF_TANK_ACTIVE) == SF_TANK_ACTIVE; }
	void TankActivate()
	{
		pev->spawnflags |= SF_TANK_ACTIVE;
		pev->nextthink = pev->ltime + 0.1f;
		m_fireLast = 0.0f;
	}
	void TankDeactivate()
	{
		pev->spawnflags &= ~SF_TANK_ACTIVE;
		m_fireLast = 0.0f;
		StopRotSound();
	}

	BOOL CanFire() const { return (gpGlobals->time - m_lastSightTime) < m_persist; }
	Vector BarrelPosition()
	{
		Vector forward, right, up;
		UTIL_MakeVectorsPrivate(pev->angles, forward, right, up);
		return pev->origin + (forward * m_barrelPos.x) + (right * m_barrelPos.y) + (up * m_barrelPos.z);
	}
protected:
	CBasePlayer *m_pController;
	float m_flNextAttack;
	Vector m_vecControllerUsePos;

	float m_yawCenter;		// "Center" yaw
	float m_yawRate;		// Max turn rate to track targets
	float m_yawRange;		// Range of turning motion (one-sided: 30 is +/- 30 degress from center)
							// Zero is full rotation

	float m_yawTolerance;	// Tolerance angle

	float m_pitchCenter;	// "Center" pitch
	float m_pitchRate;		// Max turn rate on pitch
	float m_pitchRange;		// Range of pitch motion as above
	float m_pitchTolerance;	// Tolerance angle

	float m_fireLast;		// Last time I fired
	float m_fireRate;		// How many rounds/second
	float m_lastSightTime;	// Last time I saw target
	float m_persist;		// Persistence of firing (how long do I shoot when I can't see)
	float m_minRange;		// Minimum range to aim/track
	float m_maxRange;		// Max range to aim/track

	Vector m_barrelPos;		// Length of the freakin barrel
	float m_spriteScale;	// Scale of any sprites we shoot
	int m_iszSpriteSmoke;
	int m_iszSpriteFlash;
	TANKBULLET m_bulletType;// Bullet type
	int m_iBulletDamage;	// 0 means use Bullet type's default damage

	Vector m_sightOrigin;	// Last sight of target
	int m_spread;			// firing spread
	int m_iszMaster;		// Master entity (game_team_master or multisource)
};

class CFuncTankGun: public CFuncTank
{
public:
	virtual void Fire(const Vector &barrelEnd, const Vector &forward, entvars_t *pevAttacker) = 0;
};

class CFuncTankLaser: public CFuncTank
{
public:
	virtual void KeyValue(KeyValueData *pkvd) = 0;
	virtual int Save(CSave &save) = 0;
	virtual int Restore(CRestore &restore) = 0;
	virtual void Activate() = 0;
	virtual void Think() = 0;
	virtual void Fire(const Vector &barrelEnd, const Vector &forward, entvars_t *pevAttacker) = 0;
public:
	CLaser *m_pLaser;
	float m_laserTime;
};

class CFuncTankRocket: public CFuncTank
{
public:
	virtual void Precache() = 0;
	virtual void Fire(const Vector &barrelEnd, const Vector &forward, entvars_t *pevAttacker) = 0;
};

class CFuncTankMortar: public CFuncTank
{
public:
	virtual void KeyValue(KeyValueData *pkvd) = 0;
	virtual void Fire(const Vector &barrelEnd, const Vector &forward, entvars_t *pevAttacker) = 0;
};

class CFuncTankControls: public CBaseEntity
{
public:
	virtual void Spawn() = 0;
	virtual int Save(CSave &save) = 0;
	virtual int Restore(CRestore &restore) = 0;
	virtual int ObjectCaps() = 0;
	virtual void Think() = 0;
	virtual void Use(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value) = 0;
public:
	CFuncTank *m_pTank;
};
