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

#include "shake.h"
#include "activity.h"
#include "enginecallback.h"
#include "tier1/utlvector.h"

extern globalvars_t *gpGlobals;
extern const Vector g_vecZero;

#define STRING(offset)			((const char *)(gpGlobals->pStringBase + (unsigned int)(offset)))
#define MAKE_STRING(str)		((uint64_t)(str) - (uint64_t)(STRING(0)))

#define GROUP_OP_AND	0
#define GROUP_OP_NAND	1

// Dot products for view cone checking
#define VIEW_FIELD_FULL			-1.0	// +-180 degrees
#define VIEW_FIELD_WIDE			-0.7	// +-135 degrees 0.1 // +-85 degrees, used for full FOV checks
#define VIEW_FIELD_NARROW		0.7		// +-45 degrees, more narrow check used to set up ranged attacks
#define VIEW_FIELD_ULTRA_NARROW	0.9		// +-25 degrees, more narrow check used to set up ranged attacks

#define SND_STOP				BIT(5)	// duplicated in protocol.h stop sound
#define SND_CHANGE_VOL			BIT(6)	// duplicated in protocol.h change sound vol
#define SND_CHANGE_PITCH		BIT(7)	// duplicated in protocol.h change sound pitch
#define SND_SPAWNING			BIT(8)	// duplicated in protocol.h we're spawing, used in some cases for ambients

// All monsters need this data
#define DONT_BLEED			-1
#define BLOOD_COLOR_DARKRED	(byte)223
#define BLOOD_COLOR_RED		(byte)247
#define BLOOD_COLOR_YELLOW	(byte)195
#define BLOOD_COLOR_GREEN	BLOOD_COLOR_YELLOW

#define GERMAN_GIB_COUNT	4
#define HUMAN_GIB_COUNT		6
#define ALIEN_GIB_COUNT		4

#define LANGUAGE_ENGLISH	0
#define LANGUAGE_GERMAN		1
#define LANGUAGE_FRENCH		2
#define LANGUAGE_BRITISH	3

#define SVC_NOP				1
#define SVC_SOUND			6
#define SVC_TEMPENTITY		23
#define SVC_INTERMISSION	30
#define SVC_CDTRACK			32
#define SVC_WEAPONANIM		35
#define SVC_ROOMTYPE		37
#define SVC_DIRECTOR		51

#define VEC_HULL_MIN_Z		Vector(0, 0, -36)
#define VEC_DUCK_HULL_MIN_Z	Vector(0, 0, -18)

#define VEC_HULL_MIN		Vector(-16, -16, -36)
#define VEC_HULL_MAX		Vector(16, 16, 36)

#define VEC_VIEW			Vector(0, 0, 17)

#define VEC_DUCK_HULL_MIN	Vector(-16, -16, -18)
#define VEC_DUCK_HULL_MAX	Vector(16, 16, 32)
#define VEC_DUCK_VIEW		Vector(0, 0, 12)

#define PRECACHE_SOUND_ARRAY(a) \
	{ for (int i = 0; i < ARRAYSIZE(a); ++i) PRECACHE_SOUND((char *)a[i]); }

#define PLAYBACK_EVENT(flags, who, index)\
		PLAYBACK_EVENT_FULL(flags, who, index, 0, (float *)&g_vecZero, (float *)&g_vecZero, 0.0, 0.0, 0, 0, 0, 0)

#define PLAYBACK_EVENT_DELAY(flags, who, index, delay)\
		PLAYBACK_EVENT_FULL(flags, who, index, delay, (float *)&g_vecZero, (float *)&g_vecZero, 0.0, 0.0, 0, 0, 0, 0)

#define LINK_ENTITY_TO_CLASS(mapClassName, DLLClassName)\
	C_DLLEXPORT void EXT_FUNC mapClassName(entvars_t *pev);\
	void mapClassName(entvars_t *pev)\
	{\
		GetClassPtr((DLLClassName *)pev);\
	}

const EOFFSET eoNullEntity = (EOFFSET)0;	// Testing the three types of "entity" for nullity
const string_t iStringNull = (string_t)0;	// Testing strings for nullity

// Inlines
inline edict_t *FIND_ENTITY_BY_CLASSNAME(edict_t *entStart, const char *pszName) { return (*g_engfuncs.pfnFindEntityByString)(entStart, "classname", pszName); }
inline edict_t *FIND_ENTITY_BY_TARGETNAME(edict_t *entStart, const char *pszName) { return (*g_engfuncs.pfnFindEntityByString)(entStart, "targetname", pszName); }

inline edict_t *ENT(const entvars_t *pev) { return pev->pContainingEntity; }
inline edict_t *ENT(EOFFSET eoffset) { return (*g_engfuncs.pfnPEntityOfEntOffset)(eoffset); }
inline EOFFSET OFFSET(const edict_t *pent) { return (*g_engfuncs.pfnEntOffsetOfPEntity)(pent); }
inline EOFFSET OFFSET(const entvars_t *pev) { return OFFSET(ENT(pev)); }

inline entvars_t *VARS(edict_t *pent)
{
	if (!pent)
		return nullptr;

	return &pent->v;
}

inline entvars_t *VARS(EOFFSET eoffset)
{
	return VARS(ENT(eoffset));
}

#ifndef ENTINDEX
inline int ENTINDEX(const edict_t *pEdict) { return (*g_engfuncs.pfnIndexOfEdict)(pEdict); }
inline int ENTINDEX(const entvars_t *pev) { return (*g_engfuncs.pfnIndexOfEdict)(ENT(pev)); }
#endif // ENTINDEX

#ifndef INDEXENT
inline edict_t *INDEXENT(int iEdictNum) { return (*g_engfuncs.pfnPEntityOfEntIndex)(iEdictNum); }
#endif // INDEXENT

class CBaseEntity;
class CBasePlayer;
class CBasePlayerItem;

inline void MESSAGE_BEGIN(int msg_dest, int msg_type, const float *pOrigin, entvars_t *ent) { MESSAGE_BEGIN(msg_dest, msg_type, pOrigin, ENT(ent)); }
inline bool FNullEnt(EOFFSET eoffset) { return (eoffset == 0); }
inline bool FNullEnt(entvars_t *pev) { return (pev == nullptr || FNullEnt(OFFSET(pev))); }
inline bool FNullEnt(const edict_t *pent) { return (pent == nullptr || pent->free || FNullEnt(OFFSET(pent))); }
inline bool FStringNull(string_t iString) { return (iString == iStringNull); }
inline bool FStrEq(const char *sz1, const char *sz2) { return (Q_strcmp(sz1, sz2) == 0); }
inline bool FClassnameIs(entvars_t *pev, const char *szClassname) { return FStrEq(STRING(pev->classname), szClassname); }
inline bool FClassnameIs(edict_t *pent, const char *szClassname) { return FStrEq(STRING(VARS(pent)->classname), szClassname); }
inline void UTIL_MakeVectorsPrivate(Vector vecAngles, float *p_vForward, float *p_vRight, float *p_vUp) { g_engfuncs.pfnAngleVectors(vecAngles, p_vForward, p_vRight, p_vUp); }

extern void EMIT_SOUND_DYN(edict_t *entity, int channel, const char *sample, float volume, float attenuation, int flags, int pitch);

// NOTE: use EMIT_SOUND_DYN to set the pitch of a sound. Pitch of 100
// is no pitch shift.  Pitch > 100 up to 255 is a higher pitch, pitch < 100
// down to 1 is a lower pitch.   150 to 70 is the realistic range.
// EMIT_SOUND_DYN with pitch != 100 should be used sparingly, as it's not quite as
// fast as EMIT_SOUND (the pitchshift mixer is not native coded).
inline void EMIT_SOUND(edict_t *entity, int channel, const char *sample, float volume, float attenuation)
{
	EMIT_SOUND_DYN(entity, channel, sample, volume, attenuation, 0, PITCH_NORM);
}

inline void STOP_SOUND(edict_t *entity, int channel, const char *sample)
{
	EMIT_SOUND_DYN(entity, channel, sample, 0, 0, SND_STOP, PITCH_NORM);
}

inline void EMIT_SOUND_MSG(edict_t *entity, int msg_type, int channel, const char *sample, float volume, float attenuation, int flags, int pitch = PITCH_NORM, Vector vecOrigin = Vector(0, 0, 0), edict_t *player = nullptr)
{
	(*g_engfuncs.pfnBuildSoundMsg)(entity, channel, sample, (volume * 255.0f), attenuation, flags, pitch, msg_type, SVC_NOP, vecOrigin, player);
}

inline void STOP_SOUND_MSG(edict_t *entity, int msg_type, int channel, const char *sample, edict_t *player)
{
	(*g_engfuncs.pfnBuildSoundMsg)(entity, channel, sample, 0, 0, SND_STOP, PITCH_NORM, msg_type, SVC_NOP, Vector(0, 0, 0), player);
}

extern char *UTIL_VarArgs(char *format, ...);
extern void UTIL_LogPrintf(const char *fmt, ...);
extern void UTIL_ServerPrint(const char *fmt, ...);

extern void NORETURN Sys_Error(const char *error, ...);

extern void UTIL_MakeVectors(const Vector &vecAngles);
extern float UTIL_SharedRandomFloat(unsigned int seed, float low, float high);
extern float UTIL_WeaponTimeBase();

extern BOOL UTIL_GetNextBestWeapon(CBasePlayer *pPlayer, CBasePlayerItem *pCurrentWeapon);
extern void UTIL_TraceLine(const Vector &vecStart, const Vector &vecEnd, IGNORE_MONSTERS igmon, edict_t *pentIgnore, TraceResult *ptr);
extern void UTIL_TraceLine(const Vector &vecStart, const Vector &vecEnd, IGNORE_MONSTERS igmon, IGNORE_GLASS ignoreGlass, edict_t *pentIgnore, TraceResult *ptr);
extern void UTIL_TraceHull(const Vector &vecStart, const Vector &vecEnd, IGNORE_MONSTERS igmon, int hullNumber, edict_t *pentIgnore, TraceResult *ptr);
extern void UTIL_TraceModel(const Vector &vecStart, const Vector &vecEnd, int hullNumber, edict_t *pentModel, TraceResult *ptr);
extern TraceResult UTIL_GetGlobalTrace();

extern CBaseEntity *UTIL_FindEntityByClassname(CBaseEntity *pStartEntity, const char *szName);

// Inlines
template <typename T = CBaseEntity>
inline T *UTIL_FindEntityByClassname(T *pStartEntity, const char *szName)
{
#ifdef CLIENT_DLL
	return nullptr;
#else
	return (T *)UTIL_FindEntityByString(pStartEntity, "classname", szName);
#endif
}
