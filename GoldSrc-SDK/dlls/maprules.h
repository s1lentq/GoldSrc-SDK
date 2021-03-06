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

class CRuleEntity: public CBaseEntity
{
public:
	virtual void Spawn() = 0;
	virtual void KeyValue(KeyValueData *pkvd) = 0;
	virtual int Save(CSave &save) = 0;
	virtual int Restore(CRestore &restore) = 0;
public:
	void SetMaster(int iszMaster) { m_iszMaster = iszMaster; }

private:
	string_t m_iszMaster;
};

// Base class for all rule "point" entities (not brushes)
class CRulePointEntity: public CRuleEntity
{
public:
	virtual void Spawn() = 0;
};

// Base class for all rule "brush" entities (not brushes)
// Default behavior is to set up like a trigger, invisible, but keep the model for volume testing
class CRuleBrushEntity: public CRuleEntity
{
public:
	virtual void Spawn() = 0;
};

#define SF_SCORE_NEGATIVE BIT(0) // Allow negative scores
#define SF_SCORE_TEAM     BIT(1) // Award points to team in teamplay

// Award points to player / team
// Points +/- total
class CGameScore: public CRulePointEntity
{
public:
	virtual void Spawn() = 0;
	virtual void KeyValue(KeyValueData *pkvd) = 0;
	virtual void Use(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value) = 0;
public:
	int Points() const { return int(pev->frags); }
	bool AllowNegativeScore() { return (pev->spawnflags & SF_SCORE_NEGATIVE) == SF_SCORE_NEGATIVE; }
	bool AwardToTeam() const { return (pev->spawnflags & SF_SCORE_TEAM) == SF_SCORE_TEAM; }
	void SetPoints(int points) { pev->frags = points; }
};

// Ends the game in Multiplayer
class CGameEnd: public CRulePointEntity
{
public:
	virtual void Use(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value) = 0;
};

#define SF_ENVTEXT_ALLPLAYERS BIT(0) // Message will be displayed to all players instead of just the activator.

// NON-Localized HUD Message (use env_message to display a titles.txt message)
class CGameText: public CRulePointEntity
{
public:
	virtual void KeyValue(KeyValueData *pkvd) = 0;
	virtual int Save(CSave &save) = 0;
	virtual int Restore(CRestore &restore) = 0;
	virtual void Use(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value) = 0;

public:
	bool MessageToAll() const { return (pev->spawnflags & SF_ENVTEXT_ALLPLAYERS) == SF_ENVTEXT_ALLPLAYERS; }
	void MessageSet(const char *pMessage) { pev->message = ALLOC_STRING(pMessage); }
	const char *MessageGet() const { return STRING(pev->message); }

private:
	hudtextparms_t m_textParms;
};

#define SF_TEAMMASTER_FIREONCE BIT(0) // Remove on Fire
#define SF_TEAMMASTER_ANYTEAM  BIT(1) // Any team until set? -- Any team can use this until the team is set (otherwise no teams can use it)

// "Masters" like multisource, but based on the team of the activator
// Only allows mastered entity to fire if the team matches my team
//
// team index (pulled from server team list "mp_teamlist"
class CGameTeamMaster: public CRulePointEntity
{
public:
	virtual void KeyValue(KeyValueData *pkvd) = 0;
	virtual int ObjectCaps() = 0;
	virtual BOOL IsTriggered(CBaseEntity *pActivator) = 0;
	virtual const char *TeamID() = 0;
	virtual void Use(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value) = 0;
public:
	bool RemoveOnFire() const { return (pev->spawnflags & SF_TEAMMASTER_FIREONCE) == SF_TEAMMASTER_FIREONCE; }
	bool AnyTeam() const { return (pev->spawnflags & SF_TEAMMASTER_ANYTEAM) == SF_TEAMMASTER_ANYTEAM; }

public:
	int m_teamIndex;
	USE_TYPE m_triggerType;
};

#define SF_TEAMSET_FIREONCE  BIT(0) // Remove entity after firing.
#define SF_TEAMSET_CLEARTEAM BIT(1) // Clear team -- Sets the team to "NONE" instead of activator

// Changes the team of the entity it targets to the activator's team
class CGameTeamSet: public CRulePointEntity
{
public:
	virtual void Use(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value) = 0;
public:
	bool RemoveOnFire() const { return (pev->spawnflags & SF_TEAMSET_FIREONCE) == SF_TEAMSET_FIREONCE; }
	bool ShouldClearTeam() const { return (pev->spawnflags & SF_TEAMSET_CLEARTEAM) == SF_TEAMSET_CLEARTEAM; }
};

// Players in the zone fire my target when I'm fired
// Needs master?
class CGamePlayerZone: public CRuleBrushEntity
{
public:
	virtual void KeyValue(KeyValueData *pkvd) = 0;
	virtual int Save(CSave &save) = 0;
	virtual int Restore(CRestore &restore) = 0;
	virtual void Use(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value) = 0;
private:
	string_t m_iszInTarget;
	string_t m_iszOutTarget;
	string_t m_iszInCount;
	string_t m_iszOutCount;
};

#define SF_PKILL_FIREONCE BIT(0) // Remove entity after firing.

// Damages the player who fires it
class CGamePlayerHurt: public CRulePointEntity
{
public:
	virtual void Use(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value) = 0;
public:
	bool RemoveOnFire() const { return (pev->spawnflags & SF_PKILL_FIREONCE) == SF_PKILL_FIREONCE; }
};

#define SF_GAMECOUNT_FIREONCE   BIT(0) // Remove entity after firing.
#define SF_GAMECOUNT_RESET      BIT(1) // Reset entity Initial value after fired.
#define SF_GAMECOUNT_OVER_LIMIT BIT(2) // Fire a target when initial value is higher than limit value.

// Counts events and fires target
class CGameCounter: public CRulePointEntity
{
public:
	virtual void Spawn() = 0;
	virtual void Use(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value) = 0;
public:
	bool RemoveOnFire() const { return (pev->spawnflags & SF_GAMECOUNT_FIREONCE) == SF_GAMECOUNT_FIREONCE; }
	bool ResetOnFire() const { return (pev->spawnflags & SF_GAMECOUNT_RESET) == SF_GAMECOUNT_RESET; }

	void CountUp() { pev->frags++; }
	void CountDown() { pev->frags--; }
	void ResetCount() { pev->frags = pev->dmg; }

	int CountValue() const { return int(pev->frags); }
	int LimitValue() const { return int(pev->health); }
	bool HitLimit() const  { return ((pev->spawnflags & SF_GAMECOUNT_OVER_LIMIT) == SF_GAMECOUNT_OVER_LIMIT) ? (CountValue() >= LimitValue()) : (CountValue() == LimitValue()); }

private:
	void SetCountValue(int value) { pev->frags = value; }
	void SetInitialValue(int value) { pev->dmg = value; }
};

#define SF_GAMECOUNTSET_FIREONCE BIT(0) // Remove entity after firing.

// Sets the counter's value
class CGameCounterSet: public CRulePointEntity
{
public:
	virtual void Use(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value) = 0;
public:
	bool RemoveOnFire() const { return (pev->spawnflags & SF_GAMECOUNTSET_FIREONCE) == SF_GAMECOUNTSET_FIREONCE; }
};

#define MAX_EQUIP 32
#define SF_PLAYEREQUIP_USEONLY BIT(0) // If set, the game_player_equip entity will not equip respawning players,
                                      // but only react to direct triggering, equipping its activator. This makes its master obsolete.

// Sets the default player equipment
class CGamePlayerEquip: public CRulePointEntity
{
public:
	virtual void KeyValue(KeyValueData *pkvd) = 0;
	virtual void Touch(CBaseEntity *pOther) = 0;
	virtual void Use(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value) = 0;
public:
	bool UseOnly() const { return (pev->spawnflags & SF_PLAYEREQUIP_USEONLY) == SF_PLAYEREQUIP_USEONLY; }
public:
	string_t m_weaponNames[MAX_EQUIP];
	int m_weaponCount[MAX_EQUIP];
};

#define SF_PTEAM_FIREONCE BIT(0) // Remove entity after firing.
#define SF_PTEAM_KILL     BIT(1) // Kill Player.
#define SF_PTEAM_GIB      BIT(2) // Gib Player.

// Changes the team of the player who fired it
class CGamePlayerTeam: public CRulePointEntity
{
public:
	virtual void Use(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value) = 0;
private:
	bool RemoveOnFire() const { return (pev->spawnflags & SF_PTEAM_FIREONCE) == SF_PTEAM_FIREONCE; }
	bool ShouldKillPlayer() const { return (pev->spawnflags & SF_PTEAM_KILL) == SF_PTEAM_KILL; }
	bool ShouldGibPlayer() const { return (pev->spawnflags & SF_PTEAM_GIB) == SF_PTEAM_GIB; }
};
