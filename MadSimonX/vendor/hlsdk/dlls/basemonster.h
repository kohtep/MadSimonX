/***
*
*	Copyright (c) 1996-2001, Valve LLC. All rights reserved.
*	
*	This product contains software technology licensed from Id 
*	Software, Inc. ("Id Technology").  Id Technology (c) 1996 Id Software, Inc. 
*	All Rights Reserved.
*
*   This source code contains proprietary and confidential information of
*   Valve LLC and its suppliers.  Access to this code is restricted to
*   persons who have executed a written SDK license with Valve.  Any access,
*   use or distribution of this code by or to any unlicensed person is illegal.
*
****/

#ifndef BASEMONSTER_H
#define BASEMONSTER_H

//
// generic Monster
//
class CBaseMonster : public CBaseToggle
{
	typedef enum
	{
		SCRIPT_PLAYING = 0,		// Playing the sequence
		SCRIPT_WAIT,			// Waiting on everyone in the script to be ready
		SCRIPT_CLEANUP,			// Cancelling the script / cleaning up
		SCRIPT_WALK_TO_MARK,
		SCRIPT_RUN_TO_MARK,
	} SCRIPTSTATE;

	int m_afConditions;
	EHANDLE m_hEnemy;
	EHANDLE m_hTargetEnt;
	EHANDLE m_hOldEnemy[4];
	Vector m_vecOldEnemy[4];
	Vector m_vOriginalOrigin;
	BOOL m_bIsFlickerAppearing;
	float m_flFlickerAppearTime;
	float m_flNextFlickerAppear;
	bool m_bWaitingToDie;
	int m_iLastDamageType;
	bool m_bBurning;
	float m_flFieldOfView;
	float m_flWaitFinished;
	float m_flMoveWaitFinished;
	Activity m_Activity;
	Activity m_IdealActivity;
	int m_LastHitGroup;
	bool m_bUngibbable;
	MONSTERSTATE m_MonsterState;
	MONSTERSTATE m_IdealMonsterState;
	int m_iTaskStatus;
	Schedule_t *m_pSchedule;
	int m_iScheduleIndex;
	WayPoint_t m_Route[8];
	int m_movementGoal;
	int m_iRouteIndex;
	float m_moveWaitTime;
	Vector m_vecMoveGoal;
	Activity m_movementActivity;
	int m_iAudibleList;
	int m_afSoundTypes;
	Vector m_vecLastPosition;
	int m_iHintNode;
	int m_afMemory;
	int m_iMaxHealth;
	Vector m_vecEnemyLKP;
	int m_cAmmoLoaded;
	int m_afCapability;
	float m_flNextAttack;
	int m_bitsDamageType;
	char m_rgbTimeBasedDamage[8];
	int m_lastDamageAmount;
	int m_bloodColor;
	int m_failSchedule;
	float m_flHungryTime;
	float m_flDistTooFar;
	float m_flDistLook;
	int m_iTriggerCondition;
	int m_iszTriggerTarget;
	Vector m_HackedGunPos;
	int m_iUseAlertAnims;
	SCRIPTSTATE m_scriptState;
	CCineMonster *m_pCine;

	virtual Vector *CalcRatio(CBaseEntity *pLocus) {}

	virtual int Save(CSave &save) {}
	virtual int Restore(CRestore &restore) {}

	virtual int Classify() {}

	virtual void TraceAttack(entvars_t *pevAttacker, float flDamage, Vector vecDir, TraceResult *ptr, int bitsDamageType) {}
	virtual int TakeDamage(entvars_t *pevInflictor, entvars_t *pevAttacker, float flDamage, int bitsDamageType) {}
	virtual int TakeHealth(float flHealth, int bitsDamageType) {}
	virtual void Killed(entvars_t *pevAttacker, int iGib) {}
	virtual int BloodColor() {}
	virtual CBaseMonster *MyMonsterPointer() {}
	virtual int IsMoving() {}
	virtual BOOL IsAlive() {}
	virtual BOOL FBecomeProne() {};
	virtual Vector BodyTarget(const Vector &posSrc) {} // position to shoot at

	// NOT DONE VTABLE

	int m_iClass;
	int m_iPlayerReact;
	int m_hRushEntity;
	int m_iRushMovetype;
	float m_flRushDistance;
	float m_flRushNextTime;
};

#endif // BASEMONSTER_H
