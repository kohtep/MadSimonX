/***
*
*	Copyright (c) 1996-2001, Valve LLC. All rights reserved.
*	
*	This product contains software technology licensed from Id 
*	Software, Inc. ("Id Technology").  Id Technology (c) 1996 Id Software, Inc. 
*	All Rights Reserved.
*
*   Use, distribution, and modification of this source code and/or resulting
*   object code is restricted to non-commercial enhancements to products from
*   Valve LLC.  All other use, distribution, or modification is prohibited
*   without written permission from Valve LLC.
*
****/
#ifndef PLAYER_H
#define PLAYER_H


#include "pm_materials.h"


#define PLAYER_FATAL_FALL_SPEED		1024// approx 60 feet
#define PLAYER_MAX_SAFE_FALL_SPEED	580// approx 20 feet
#define DAMAGE_FOR_FALL_SPEED		(float) 100 / ( PLAYER_FATAL_FALL_SPEED - PLAYER_MAX_SAFE_FALL_SPEED )// damage per unit per second.
#define PLAYER_MIN_BOUNCE_SPEED		200
#define PLAYER_FALL_PUNCH_THRESHHOLD (float)350 // won't punch player's screen/make scrape noise unless player falling at least this fast.

//
// Player PHYSICS FLAGS bits
//
#define		PFLAG_ONLADDER		( 1<<0 )
#define		PFLAG_ONSWING		( 1<<0 )
#define		PFLAG_ONTRAIN		( 1<<1 )
#define		PFLAG_ONBARNACLE	( 1<<2 )
#define		PFLAG_DUCKING		( 1<<3 )		// In the process of ducking, but totally squatted yet
#define		PFLAG_USING			( 1<<4 )		// Using a continuous entity
#define		PFLAG_OBSERVER		( 1<<5 )		// player is locked in stationary cam mode. Spectators can move, observers can't.

//
// generic player
//
//-----------------------------------------------------
//This is Half-Life player entity
//-----------------------------------------------------
#define CSUITPLAYLIST	4		// max of 4 suit sentences queued up at any time

#define SUIT_GROUP			TRUE
#define	SUIT_SENTENCE		FALSE

#define	SUIT_REPEAT_OK		0
#define SUIT_NEXT_IN_30SEC	30
#define SUIT_NEXT_IN_1MIN	60
#define SUIT_NEXT_IN_5MIN	300
#define SUIT_NEXT_IN_10MIN	600
#define SUIT_NEXT_IN_30MIN	1800
#define SUIT_NEXT_IN_1HOUR	3600

#define CSUITNOREPEAT		32

#define	SOUND_FLASHLIGHT_ON		"items/flashlight1.wav"
#define	SOUND_FLASHLIGHT_OFF	"items/flashlight1.wav"

#define TEAM_NAME_LENGTH	16

typedef enum
{
	PLAYER_IDLE,
	PLAYER_WALK,
	PLAYER_JUMP,
	PLAYER_SUPERJUMP,
	PLAYER_DIE,
	PLAYER_ATTACK1,
} PLAYER_ANIM;

#define MAX_ID_RANGE 2048
#define SBAR_STRING_SIZE 128

enum sbar_data
{
	SBAR_ID_TARGETNAME = 1,
	SBAR_ID_TARGETHEALTH,
	SBAR_ID_TARGETARMOR,
	SBAR_END,
};

#define CHAT_INTERVAL 1.0f

class CRealCamera;

struct statistics
{
	int shots_fired;
	int shots_hit;
	int syringes_used;
	int shots_missed;
	float gunaccuracy;
	int timessaved;
	int weaponscollected;
	int damagetaken;
	float distancetravelled_metres;
	float distancetravelled_kilometres;
	int time_seconds;
	int time_minutes;
	int time_hours;
	int monsters_killed_melee;
	int monsters_killed_shooting;
	bool started_from_the_beginning;
	int chosen_difficulty;
	bool doctor_mode;
	int language;
};

struct documents
{
	bool page1[16];
	bool page2[16];
	bool page3[16];
	bool page4[16];
	bool note1[16];
	bool note2[16];
	bool note3[16];
	bool note4[16];
	bool turnsound[48];
	bool title[64];
	bool label[48];
};

#define COMBINE1(X, Y) X##Y  // helper macro
#define COMBINE(X, Y) COMBINE1(X, Y)
#define PAD_HIDDEN(bytes) private: uint8_t COMBINE(pad_, __LINE__)[bytes]; public:

class CBasePlayer : public CBaseMonster
{
public:
	/* +872 */ EHANDLE m_pTelephone;
	/* +880 */ EHANDLE m_pTelescope;
	/* +888 */ EHANDLE m_pPuzzleBar;
	/* +896 */ EHANDLE m_pBoat;
	/* +904 */ EHANDLE m_pAttachedCamera;
	/* +912 */ EHANDLE m_pWheelchair;

#pragma region Unknown fields
	/* +920 */ bool b1;
	/* +921 */ bool m_bUsedVanNumber;
	/* +922 */ bool b3;
	/* +923 */ bool b4;
	/* +924 */ bool b5;
	/* +925 */ bool b6;
	/* +926 */ bool b7;
	/* +927 */ bool b8;
	/* +928 */ bool b9;
	/* +929 */ bool b10;
	/* +930 */ bool b11;
	/* +931 */ bool b12;
	/* +932 */ bool b13;
	/* +933 */ bool b14;
	/* +934 */ bool b15;
	/* +935 */ bool b16;
	/* +936 */ bool b17;
	/* +937 */ bool b18;
#pragma endregion

	/* +938 */ bool m_bJustJumped;
	/* +940 */ float m_flMaxJustJumpedTime;
	bool m_bIsADonator;
	int m_iAllowedToUseTMP;
	int m_iIsAFaggot;
	bool m_bHoldingBreath;
	float m_flNextBreathHoldTime;
	int m_iCoopStuckPushYaw;
	bool m_bCoopWaitingForCutsceneToEnd;
	float m_flNextCoopWaitingForCutsceneFadeTime;
	float m_flNextDeadNotifyTime;
	float m_flCoopRespawnDelayTime;
	int m_iQuickSlotSlot[4];
	float m_flNextAdLibTime;
	float m_flNextNightVisionTime;
	CBasePlayerWeapon *m_pStoredNightVisionWeapon;
	bool m_bFirstDeploy[33];
	int m_iCurrentWeaponAnimation;
	int m_iCurrentWeaponAnimationDual;

	int __PADDING420;

	int m_iUnlockedClothes;
	int m_iClientUnlockedClothes;
	int m_iFoundHoodies;
	float m_flDodgeRechargeTime;
	bool m_bHasNightVision;
	bool m_bChoking;
	float m_flNextTimeTakenCheck;
	bool m_bLowHealth;
	float m_flNextMorphineDreamTime;
	int m_iOwnedWeaponsList[32];
	float m_flNextPunchTime;
	int m_iPlayerSaveLock;
	bool m_bResetGame;
	bool m_b_Ending_RoofBossKilled;
	bool m_b_Ending_GivenP345;
	bool m_b_Ending_DeliveredPackage;
	float m_flCylinderRotateTime;
	float m_flNextHeartbeatTime;
	char *szIPAddress;

	PAD_HIDDEN(4 + 4 + 4);

	bool m_bCoopGameOver;
	float m_flNextCountDown;
	int m_iGameOverCounter;
	float m_flSequenceOverrideTime;
	int m_iSequenceOverride;
	float m_flGetupTime;
	bool m_bHasGotUp;
	bool m_bCantBeRevived;
	float m_flGaitSequenceOverrideTime;
	int m_iGaitSequenceOverride;
	bool m_bWaitingToFall;
	int m_strHintsShown[32];

	PAD_HIDDEN(4); // 1.6a
	
	float m_flTurnAroundTime;
	float m_flTurnVectorTime;
	Vector m_vTurnVector;
	bool m_bStartedDrowning;
	bool m_bWaterVoiceCancel;
	CBaseAnimating *pDeathScene;
	int m_iDoctorWeapon;
	int m_iDeathBits;
	EHANDLE m_pClientBoat;
	EHANDLE m_pClientAttachedCamera;
	bool m_bInWater;
	int m_iPhoneMessage;
	int m_iFlashlightFlags;
	int m_iClientFlashlightFlags;
	float m_flQuickSlotTimeDecay;
	int m_iDualWeaponLeftAmmo;
	int m_iDualWeaponRightAmmo;
	int m_iClientDualWeaponLeftAmmo;
	int m_iClientDualWeaponRightAmmo;
	int g_iDoorSound;
	int m_iGoodPoints;
	bool m_bBossHealthBar;
	int m_sBossBarEmpty;
	int m_sBossBarFull;
	int m_iBossMaxHealth;
	int m_iBossCurrentHealth;
	int m_sBossName;

	int m_iHealthCap; // Player health's maximum value. 0 - 100, 1 - 80, 2 - 66

	float m_fUseTime;
	float m_flNextDistanceCheckTime;
	Vector m_vPreviousOrigin;
	int m_sDualWieldViewModel;
	int m_sClientDualWieldViewModel;
	int m_bMirroredDualWield;
	EHANDLE m_pCurrentDocument;
	statistics playerstats;

	PAD_HIDDEN(4);

	PAD_HIDDEN(288); // 1.6a (part of playerstats probably)

	documents documentsstruct[16];
	int transferredentity_message[50];
	Vector transferredentity_origin[50];
	int transferredentity_classname[50];
	int transferredentity_clip[50];

	PAD_HIDDEN(24); // 1.6a

	int m_iDualWeaponLeftID;
	int m_iDualWeaponRightID;

	bool xbox_lshoulderdown;
	bool xbox_rshoulderdown;
	bool xbox_ltriggerdown;
	bool xbox_rtriggerdown;
	bool xbox_dpadup;
	bool xbox_dpaddown;
	bool xbox_dpadleft;
	bool xbox_dpadright;
	bool xbox_ducktoggle;
	bool xbox_haspressedduck;

	float m_fControllerForwardSpeed;
	float m_fControllerSideSpeed;
	int m_sClientObjectiveText;
	int m_sObjectiveText;
	float m_flOutOfBreathDisableTime;
	int m_iMobileFlashMode;
	bool m_bThreeSlots;
	int m_iUsingComputerOrPhone;
	bool b_GoingThroughADoor;
	int m_iFlashlight;
	int m_iClientFlashlight;
	int m_ClientDrawBars;
	int m_DrawBars;
	int m_iClientBlurActive;
	int m_iBlurActive;
	bool m_UnreadSMS;
	int m_ClientLadder;
	bool m_OnStairs;
	float m_fFallingTime;
	bool m_bShouldFallFreeze;
	Vector m_LadderAngles;
	bool m_bNightmare;
	float m_fPhoneCallTime;
	bool m_bInPhoneCall;
	bool m_bDodgeDisabled;
	bool m_bScaredBreathing;
	float m_fNextScaredBreathingTime;
	int m_iBlackAndWhite;
	int m_iTapes;
	bool m_bSilentGive;
	bool m_bWeaponWaitingToDeploy;
	int m_iHintAmount;
	float m_fNextHeartBeatTime;
	bool m_bNearStranger;
	int m_iSwitchbladeCuts;
	EHANDLE m_pLastAttackedEnemy;

	int m_iPhoneDisabled;
	int m_iClientPhoneDisabled;
	bool m_bPhoneLow;
	float m_fNextPhoneWarnTime;

	float m_flDeadTime;
	bool m_bIssuedGameOver;
	bool m_bGibbed;
	bool m_bNoDeadFade;
	float m_flDeadAutoReviveTime;
	float m_fVibrateTime;
	bool m_bVibrate;
	char DestinationText[256];
	int m_iViewEntIndex;
	int m_iClientViewEntIndex;
	int m_iKeypadNumber;
	int m_iKeypadType;
	float m_fDodgeTime;
	char m_sQuickSel1[128];
	char m_sQuickSel2[128];
	char m_sQuickSel3[128];
	float m_flNextLowHealthSound;
	int m_iMorphineCycleTime;
	int m_iMorphineRecover;
	bool m_bHasSaved;
	float m_fUnderwaterBreatheTime;
	bool m_bWasDrowning;
	bool m_bSubmerged;
	bool m_bWasInShallowWater;
	float m_flRippleTime;

	bool m_bInfiniteStamina;
	bool m_bInfiniteHealth;
	bool m_bInfiniteAmmo;

	float m_fClientStamina;
	float m_fStamina;
	float m_fStaminaChargeCyle;
	float m_fStaminaBreatheCyle;
	int inventorySlot[6];
	int inventorySlotModels[6];
	int inventorySlotClassname[6];
	int ClientinventorySlot[6];
	bool m_bUsedFirstSyringe;
	float m_fVomitTime;
	float m_fMorphineTripTime;
	bool m_bIsTripping;
	float m_fTrippingTime;
	bool m_bHasVomited;
	int m_iOldRoomType;

	int random_seed; // See that is shared between client & server for shared weapons code
	int m_iPlayerSound;
	int m_iTargetVolume;
	int m_iWeaponVolume;
	int m_iExtraSoundTypes;
	int m_iWeaponFlash;
	float m_flStopExtraSoundTime;
	float m_flFlashLightTime;
	int m_iFlashBattery;
	int m_afButtonLast;
	int m_afButtonPressed;
	/* +8552 */ int m_afButtonReleased;
	/* +8556 */ edict_s *m_pentSndLast;
	/* +8560 */ float m_flSndRoomtype;
	/* +8564 */ float m_flSndRange;
	/* +8568 */ float m_flFallVelocity;
	/* +8572 */ int m_rgItems[5];
	/* +8592 */ int m_fKnownItem;
	/* +8596 */ int m_fNewAmmo;

	unsigned int m_afPhysicsFlags;	// physics flags - set when 'normal' physics should be revisited or overriden
	float m_fNextSuicideTime; // the time after which the player can next use the suicide command

	/* +8608 */ int m_iGasMaskOn;
	/*       */ int m_iUpdateGasMask;
	/*       */ float m_flGasMaskTime;
	/*       */ float m_flNextBreathTime;
	/*       */ int m_iLastGasMaskSound;
	/*       */ int m_iHeadShieldOn;
	/*       */ int m_iUpdateHeadShield;
	/*       */ float m_flHeadShieldTime;
	/*       */ CBaseEntity *m_pSpecTank;
	/*       */ int m_strCurrentMp3;
	/*       */ int m_iMp3NeedsUpdate;
	/*       */ float m_fMp3FadeTime;
	/*       */ bool m_bMp3ShouldLoop;
	/*       */ float m_fMp3FadeInTime;
	/*       */ int m_strCurrentGoalName;
	/*       */ int m_strCurrentGoalImageName;
	/*       */ int m_strCurrentGoalTitleName;
	/*       */ int m_iGoalNeedsUpdate;
	/*       */ int m_iJumpHeight;
	/*       */ float m_fLightlevel;
	/*       */ int m_iUsingMic;
	/*       */ EHANDLE m_hLastEnemy;
	/*       */ EHANDLE m_hCurrentCamera;
	/* +8708 */ EHANDLE m_pPadlock;
	/* +8716 */ int m_iPadlockNumber[5];

	// these are time-sensitive things that we keep track of
	float	m_flTimeStepSound;	// when the last stepping sound was made
	float	m_flTimeWeaponIdle; // when to play another weapon idle animation.
	float	m_flSwimTime;		// how long player has been underwater
	float	m_flDuckTime;		// how long we've been ducking
	float	m_flWallJumpTime;	// how long until next walljump

	float	m_flSuitUpdate; // when to play next suit update
	int		m_rgSuitPlayList[CSUITPLAYLIST]; // next sentencenum to play for suit update
	int		m_iSuitPlayNext; // next sentence slot for queue storage;
	int		m_rgiSuitNoRepeat[CSUITNOREPEAT]; // suit sentence no repeat list
	float	m_rgflSuitNoRepeatTime[CSUITNOREPEAT]; // how long to wait before allowing repeat

	int		m_lastDamageAmount; // Last damage taken
	float	m_tbdPrev; // Time-based damage timer

	/* +9044 */ float	m_flgeigerRange; // range to nearest radiation source
	/* +9048 */ float	m_flgeigerDelay; // delay per update of range msg to client
	/* +9052 */ int		m_igeigerRangePrev;
	/* +9056 */ int		m_iStepLeft; // alternate left/right foot stepping sound

	// TODO: CBTEXTURENAMEMAX
	char m_szTextureName[13]; // current texture name we're standing on
	char m_chTextureType; // current texture type

	int m_idrowndmg; // track drowning damage taken
	int m_idrownrestored; // track drowning damage restored

	int	m_bitsHUDDamage; // Damage bits for the current fame. These get sent to 
						 // the hude via the DAMAGE message

	BOOL m_fInitHUD; // True when deferred HUD restart msg needs to be sent
	BOOL m_fGameHUDInitialized;
	int m_iTrain; // Train control position
	BOOL m_fWeapon; // Set this to FALSE to force a reset of the current weapon HUD info

	EHANDLE m_pTank; // the tank which the player is currently controlling,  NULL if no tank
	float m_fDeadTime; // the time at which the player died  (used in PlayerDeathThink())

	BOOL m_fNoPlayerSound; // a debugging feature. Player makes no sound if this is true. 
	BOOL m_fLongJump; // does this player have the longjump module?

	float m_tSneaking;
	int m_iUpdateTime; // stores the number of frame ticks before sending HUD update messages
	int m_iClientHealth; // the health currently known by the client.  If this changes, send a new
	int m_iClientBattery; // the Battery currently known by the client.  If this changes, send a new
	int m_iHideHUD; // the players hud weapon info is to be hidden
	int m_iClientHideHUD;
	int m_iFOV; // field of view
	/* +9152 */ int m_iClientFOV; // client's known FOV

	CRealCamera *m_pCamera;

	// usable player items
	/* +9180 */ CBasePlayerItem *m_rgpPlayerItems[MAX_ITEM_TYPES];
	/* +9184 */ CBasePlayerItem *m_pActiveItem;
	/* +9188 */ CBasePlayerItem *m_pClientActiveItem;
	/* +9192 */ CBasePlayerItem *m_pLastItem;
	/* +9196 */ CBasePlayerItem *m_pNextItem;
	/* +9200 */ CBasePlayerItem *m_pStoredActionItem;

	int m_rgAmmo[MAX_AMMO_SLOTS];
	int m_rgAmmoLast[MAX_AMMO_SLOTS];

	/* +9460 */ Vector m_vecAutoAim;
	BOOL m_fOnTarget;
	int m_iDeaths;
	float m_iRespawnFrames;	// used in PlayerDeathThink() to make sure players can always respawn

	int m_lastx, m_lasty;  // These are the previous update's crosshair angles, DON"T SAVE/RESTORE

	int m_nCustomSprayFrames;// Custom clan logo frames for this player
	float m_flNextDecalTime;// next time this player can spray a decal

	char m_szTeamName[TEAM_NAME_LENGTH];
	char m_szAnimExtention[32];

	EHANDLE pPlayerToRevive;
	EHANDLE pPlayerReviving;
	CBaseEntity *pCorpseEnt;
	float m_flReviveTime;
	float m_flStartCharge;
	float m_flAmmoStartCharge;
	float m_flPlayAftershock;
	float m_flNextAmmoBurn;

	int m_izSBarState[SBAR_END];

	float m_flNextSBarUpdateTime;
	float m_flStatusBarDisappearDelay;
	char m_SbarString0[SBAR_STRING_SIZE];
	char m_SbarString1[SBAR_STRING_SIZE];

	float m_flNextChatTime;

	int Rain_dripsPerSecond;
	float Rain_windX;
	float Rain_windY;
	float Rain_randX;
	float Rain_randY;
	int Rain_ideal_dripsPerSecond;
	float Rain_ideal_windX;
	float Rain_ideal_windY;
	float Rain_ideal_randX;
	float Rain_ideal_randY;
	float Rain_endFade;
	float Rain_nextFadeUpdate;
	/* +9920 */ int Rain_needsUpdate;

	/* +9924 */ int m_iInitMessagesSent;
	/* +9928 */ int m_iViewValue;
	/* +9932 */ int m_iMoValue;
};

#define AUTOAIM_2DEGREES  0.0348994967025
#define AUTOAIM_5DEGREES  0.08715574274766
#define AUTOAIM_8DEGREES  0.1391731009601
#define AUTOAIM_10DEGREES 0.1736481776669


extern int	gmsgHudText;
extern BOOL gInitHUD;

#endif // PLAYER_H
