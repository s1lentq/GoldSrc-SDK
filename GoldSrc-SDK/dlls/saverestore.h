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

#define IMPLEMENT_SAVERESTORE(derivedClass, baseClass)\
	int derivedClass::Save(CSave &save)\
	{\
		if (!baseClass::Save(save))\
			return 0;\
		return save.WriteFields(#derivedClass, this, m_SaveData, ARRAYSIZE(m_SaveData));\
	}\
	int derivedClass::Restore(CRestore &restore)\
	{\
		if (!baseClass::Restore(restore))\
			return 0;\
		return restore.ReadFields(#derivedClass, this, m_SaveData, ARRAYSIZE(m_SaveData));\
	}

enum GLOBALESTATE
{
	GLOBAL_OFF,
	GLOBAL_ON,
	GLOBAL_DEAD
};

typedef struct globalentity_s
{
	char name[64];
	char levelName[MAX_MAPNAME_LENGHT];
	GLOBALESTATE state;
	struct globalentity_s *pNext;

} globalentity_t;

struct HEADER
{
	unsigned short size;
	unsigned short token;
	char *pData;
};

const int MAX_ENTITY_ARRAY = 64;

class CBaseEntity;
class CSaveRestoreBuffer
{
public:
	CSaveRestoreBuffer();
	CSaveRestoreBuffer(SAVERESTOREDATA *pdata);
	~CSaveRestoreBuffer();

	int EntityIndex(entvars_t *pevLookup);
	int EntityIndex(edict_t *pentLookup);
	int EntityIndex(EOFFSET eoLookup);
	int EntityIndex(CBaseEntity *pEntity);
	int EntityFlags(int entityIndex, int flags);
	int EntityFlagsSet(int entityIndex, int flags);
	edict_t *EntityFromIndex(int entityIndex);
	unsigned short TokenHash(const char *pszToken);

protected:
	static constexpr int m_Sizes[] = {
		sizeof(float),     // FIELD_FLOAT
		sizeof(int),       // FIELD_STRING
		sizeof(int),       // FIELD_ENTITY
		sizeof(int),       // FIELD_CLASSPTR
		sizeof(int),       // FIELD_EHANDLE
		sizeof(int),       // FIELD_entvars_t
		sizeof(int),       // FIELD_EDICT
		sizeof(float) * 3, // FIELD_VECTOR
		sizeof(float) * 3, // FIELD_POSITION_VECTOR
		sizeof(int *),     // FIELD_POINTER
		sizeof(int),       // FIELD_INTEGER
		sizeof(int *),     // FIELD_FUNCTION
		sizeof(int),       // FIELD_BOOLEAN
		sizeof(short),     // FIELD_SHORT
		sizeof(char),      // FIELD_CHARACTER
		sizeof(float),     // FIELD_TIME
		sizeof(int),       // FIELD_MODELNAME
		sizeof(int),       // FIELD_SOUNDNAME
	};

	SAVERESTOREDATA *m_pData;
	void BufferRewind(int size);
	unsigned int HashString(const char *pszToken);
};

class CSave: public CSaveRestoreBuffer
{
public:
	CSave(SAVERESTOREDATA *pdata) : CSaveRestoreBuffer(pdata) {};

	void WriteShort(const char *pname, const short *value, int count);
	void WriteInt(const char *pname, const int *value, int count);
	void WriteFloat(const char *pname, const float *value, int count);
	void WriteTime(const char *pname, const float *value, int count);
	void WriteData(const char *pname, int size, const char *pdata);
	void WriteString(const char *pname, const char *pstring);
	void WriteString(const char *pname, const int *stringId, int count);
	void WriteVector(const char *pname, const Vector &value);
	void WriteVector(const char *pname, const float *value, int count);
	void WritePositionVector(const char *pname, const Vector &value);
	void WritePositionVector(const char *pname, const float *value, int count);
	void WriteFunction(const char *pname, void **data, int count);
	int WriteEntVars(const char *pname, entvars_t *pev);
	int WriteFields(const char *pname, void *pBaseData, TYPEDESCRIPTION *pFields, int fieldCount);

private:
	int DataEmpty(const char *pdata, int size);
	void BufferField(const char *pname, int size, const char *pdata);
	void BufferString(char *pdata, int len);
	void BufferData(const char *pdata, int size);
	void BufferHeader(const char *pname, int size);
};

class CRestore: public CSaveRestoreBuffer
{
public:
	CRestore(SAVERESTOREDATA *pdata) : CSaveRestoreBuffer(pdata)
	{
		m_global = FALSE;
		m_precache = TRUE;
	}
	int ReadEntVars(const char *pname, entvars_t *pev);
	int ReadFields(const char *pname, void *pBaseData, TYPEDESCRIPTION *pFields, int fieldCount);
	int ReadField(void *pBaseData, TYPEDESCRIPTION *pFields, int fieldCount, int startField, int size, char *pName, void *pData);
	int ReadInt();
	short ReadShort();
	int ReadNamedInt(const char *pName);
	char *ReadNamedString(const char *pName);

	bool Empty() const { return (!m_pData || ((m_pData->pCurrentData - m_pData->pBaseData) >= m_pData->bufferSize)); }
	void SetGlobalMode(BOOL global) { m_global = global; }
	void PrecacheMode(BOOL mode) { m_precache = mode; }

private:
	char *BufferPointer();
	void BufferReadBytes(char *pOutput, int size);
	void BufferSkipBytes(int bytes);
	int BufferSkipZString();
	int BufferCheckZString(const char *string);
	void BufferReadHeader(HEADER *pheader);

private:
	BOOL m_global;
	BOOL m_precache;
};

class CGlobalState
{
public:
	CGlobalState();

	void Reset();
	void ClearStates();
	void EntityAdd(string_t globalname, string_t mapName, GLOBALESTATE state);
	void EntitySetState(string_t globalname, GLOBALESTATE state);
	void EntityUpdate(string_t globalname, string_t mapname);
	const globalentity_t *EntityFromTable(string_t globalname);
	GLOBALESTATE EntityGetState(string_t globalname);

	BOOL EntityInTable(string_t globalname) { return Find(globalname) ? TRUE : FALSE; }
	int Save(CSave &save);
	int Restore(CRestore &restore);
	void DumpGlobals();

	static TYPEDESCRIPTION m_SaveData[];
	static TYPEDESCRIPTION m_GlobalEntitySaveData[];

private:
	globalentity_t *Find(string_t globalname);

	globalentity_t *m_pList;
	int m_listCount;
};

void SaveGlobalState(SAVERESTOREDATA *pSaveData);
void RestoreGlobalState(SAVERESTOREDATA *pSaveData);
void ResetGlobalState();

extern CGlobalState gGlobalState;
