#include "precache.hpp"

#include "core/global.hpp"

#include "utils/file_utils.hpp"

static void PrecacheModelSafe(const char *s)
{
	if (s && *s && U::File::FileExists(s))
		G::Server.pfnPrecacheModel(const_cast<char *>(s));
}

static void PrecacheSoundSafe(const char *s)
{
	if (s && *s && U::File::FileExists(s))
		G::Server.pfnPrecacheSound(const_cast<char *>(s));
}

static void PrecachePills()
{
	//
	// Allow to spawn 'aom_pills' entity.
	//

	PrecacheModelSafe("models/items/w_pills.mdl");

	PrecacheSoundSafe("cutscene/smallmedkit1.wav");
}
	
static void PrecacheSawrunner()
{
	PrecacheModelSafe("models/sawrunner.mdl");

	PrecacheSoundSafe("sawrunner/chainsaw_attack_miss.wav");
	PrecacheSoundSafe("sawrunner/chainsaw_attack_hit.wav");

	PrecacheSoundSafe("sawrunner/sawrunner_attack1.wav");
	PrecacheSoundSafe("sawrunner/sawrunner_attack2.wav");

	PrecacheSoundSafe("sawrunner/sawrunner_alert10.wav");
	PrecacheSoundSafe("sawrunner/sawrunner_alert20.wav");
	PrecacheSoundSafe("sawrunner/sawrunner_alert30.wav");

	PrecacheSoundSafe("sawrunner/sawrunner_pain1.wav");
	PrecacheSoundSafe("sawrunner/sawrunner_pain2.wav");

	PrecacheSoundSafe("boss/sawer/chainsaw_loop.wav");
}

static void PrecacheRadio()
{
	// TODO: copy v_radio to police folder?

	// Something is wrong with this part, shouldn't that be
	// like 'police/models/weapons/radio/v_radio.mdl'?
	PrecacheModelSafe("models/weapons/radio/v_radio.mdl");

	PrecacheModelSafe("police/models/weapons/radio/v_radio.mdl");
	PrecacheModelSafe("police/models/weapons/radio/p_radio.mdl");
}

static void PrecacheSoda()
{
	//
	// can.mdl and g_bounce3.wav does not exist in 'Cry of Fear', this
	// function may crash the game.
	//

	PrecacheModelSafe("models/can.mdl");

	PrecacheSoundSafe("weapons/g_bounce3.wav");
}

static void PrecacheRecorder()
{
	PrecacheModelSafe("models/tape_recorder.mdl");
	PrecacheModelSafe("sprites/glow01.spr");

	PrecacheSoundSafe("save/save_no.wav");
	PrecacheSoundSafe("save/save_yes.wav");
	PrecacheSoundSafe("save/save_sequence.wav");
}

static void PrecacheCOFTelephone()
{
	PrecacheSoundSafe("phone/telephone_button_press.wav");
	PrecacheSoundSafe("phone/telephone_dial_tone.wav");
	PrecacheSoundSafe("phone/telephone_dialing.wav");
	PrecacheSoundSafe("phone/telephone_receiver_down.wav");
	PrecacheSoundSafe("phone/telephone_receiver_up.wav");
	PrecacheSoundSafe("phone/telephone_wrong_number.wav");
}

static void PrecacheBattery()
{
	PrecacheModelSafe("models/w_battery.mdl");
	PrecacheSoundSafe("items/gunpickup2.wav");
}

static void PrecacheSuicider()
{
	PrecacheModelSafe("models/suicider.mdl");
	PrecacheModelSafe("models/slower_headgibs.mdl");
	PrecacheModelSafe("sprites/head_burst.spr");
	PrecacheModelSafe("sprites/stmbal1.spr");

	PrecacheSoundSafe("slower/hammer_strike1.wav");
	PrecacheSoundSafe("slower/hammer_strike2.wav");
	PrecacheSoundSafe("slower/hammer_strike3.wav");

	PrecacheSoundSafe("slower/hammer_strike_wall1.wav");
	PrecacheSoundSafe("slower/hammer_strike_wall2.wav");
	PrecacheSoundSafe("slower/hammer_strike_wall3.wav");

	PrecacheSoundSafe("slower/hammer_miss1.wav");
	PrecacheSoundSafe("slower/hammer_miss2.wav");

	PrecacheSoundSafe("slower/slower_attack1.wav");
	PrecacheSoundSafe("slower/slower_attack2.wav");

	PrecacheSoundSafe("slower/slower_alert10.wav");
	PrecacheSoundSafe("slower/slower_alert20.wav");
	PrecacheSoundSafe("slower/slower_alert30.wav");

	PrecacheSoundSafe("slower/slower_pain1.wav");
	PrecacheSoundSafe("slower/slower_pain2.wav");

	PrecacheSoundSafe("weapons/glock/glock_fire.wav");
	PrecacheSoundSafe("baby/b_attack1.wav");
	PrecacheSoundSafe("baby/b_attack2.wav");
}

static void PrecacheTMP()
{
	PrecacheModelSafe("models/ammo/ammo_tmp.mdl");
}

static void PrecacheNightvision()
{
	PrecacheModelSafe("models/items/w_nightvision.mdl");
}

static void PrecachePadlock()
{
	PrecacheModelSafe("models/items/padlock.mdl");
	PrecacheSoundSafe("items/padl_turn.wav");
	PrecacheSoundSafe("items/padl_unlock.wav");
}

static void PrecacheG43()
{
	PrecacheModelSafe("models/ammo/ammo_g43.mdl");
	PrecacheSoundSafe("weapons/item_get.wav");
}

static void PrecacheBookSimon()
{
	PrecacheModelSafe("models/booksimon.mdl");
	PrecacheModelSafe("sprites/muzzleflash.spr");
}

void Precache()
{
	PrecachePills();
	PrecacheSawrunner();
	PrecacheRadio();
	PrecacheSoda();
	PrecacheRecorder();
	PrecacheCOFTelephone();
	PrecacheBattery();
	PrecacheSuicider();
	PrecacheTMP();
	PrecacheNightvision();
	PrecachePadlock();
	PrecacheG43();
	PrecacheBookSimon();
}