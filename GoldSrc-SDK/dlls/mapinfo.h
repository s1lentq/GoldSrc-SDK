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

const float MAX_BOMB_RADIUS = 2048.0f;

class CMapInfo: public CPointEntity
{
public:
	virtual void Spawn() = 0;
	virtual void KeyValue(KeyValueData *pkvd) = 0;
	virtual void UpdateOnRemove() = 0;

public:
	InfoMapBuyParam m_iBuyingStatus;
	float m_flBombRadius;
};
