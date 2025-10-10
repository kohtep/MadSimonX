#include "precache.hpp"

#include "core/global.hpp"

static void PrecachePills()
{
	//
	// Allow to spawn 'aom_pills' entity.
	//

	G::Server.pfnPrecacheModel("models/items/w_pills.mdl");

	G::Server.pfnPrecacheSound("cutscene/smallmedkit1.wav");
}
	
static void PrecacheSawrunner()
{
	G::Server.pfnPrecacheModel("models/sawrunner.mdl");

	G::Server.pfnPrecacheSound("sawrunner/chainsaw_attack_miss.wav");
	G::Server.pfnPrecacheSound("sawrunner/chainsaw_attack_hit.wav");

	G::Server.pfnPrecacheSound("sawrunner/sawrunner_attack1.wav");
	G::Server.pfnPrecacheSound("sawrunner/sawrunner_attack2.wav");

	G::Server.pfnPrecacheSound("sawrunner/sawrunner_alert10.wav");
	G::Server.pfnPrecacheSound("sawrunner/sawrunner_alert20.wav");
	G::Server.pfnPrecacheSound("sawrunner/sawrunner_alert30.wav");

	G::Server.pfnPrecacheSound("sawrunner/sawrunner_pain1.wav");
	G::Server.pfnPrecacheSound("sawrunner/sawrunner_pain2.wav");

	G::Server.pfnPrecacheSound("boss/sawer/chainsaw_loop.wav");
}

static void PrecacheRadio()
{
	// TODO: copy v_radio to police folder?

	// Something is wrong with this part, shouldn't that be
	// like 'police/models/weapons/radio/v_radio.mdl'?
	G::Server.pfnPrecacheModel("models/weapons/radio/v_radio.mdl");

	G::Server.pfnPrecacheModel("police/models/weapons/radio/v_radio.mdl");
	G::Server.pfnPrecacheModel("police/models/weapons/radio/p_radio.mdl");
}

static void PrecacheSoda()
{
	//
	// can.mdl and g_bounce3.wav does not exist in 'Cry of Fear', this
	// function may crash the game.
	//

	G::Server.pfnPrecacheModel("models/can.mdl");

	G::Server.pfnPrecacheSound("weapons/g_bounce3.wav");
}

static void PrecacheRecorder()
{
	G::Server.pfnPrecacheModel("models/tape_recorder.mdl");
	G::Server.pfnPrecacheModel("sprites/glow01.spr");

	G::Server.pfnPrecacheSound("save/save_no.wav");
	G::Server.pfnPrecacheSound("save/save_yes.wav");
	G::Server.pfnPrecacheSound("save/save_sequence.wav");
}

static void PrecacheCOFTelephone()
{
	G::Server.pfnPrecacheSound("phone/telephone_button_press.wav");
	G::Server.pfnPrecacheSound("phone/telephone_dial_tone.wav");
	G::Server.pfnPrecacheSound("phone/telephone_dialing.wav");
	G::Server.pfnPrecacheSound("phone/telephone_receiver_down.wav");
	G::Server.pfnPrecacheSound("phone/telephone_receiver_up.wav");
	G::Server.pfnPrecacheSound("phone/telephone_wrong_number.wav");
}

static void PrecacheBattery()
{
	G::Server.pfnPrecacheModel("models/w_battery.mdl");
	G::Server.pfnPrecacheSound("items/gunpickup2.wav");
}

static void PrecacheSuicider()
{
	G::Server.pfnPrecacheModel("models/suicider.mdl");
	G::Server.pfnPrecacheModel("models/slower_headgibs.mdl");
	G::Server.pfnPrecacheModel("sprites/head_burst.spr");
	G::Server.pfnPrecacheModel("sprites/stmbal1.spr");

	G::Server.pfnPrecacheSound("slower/hammer_strike1.wav");
	G::Server.pfnPrecacheSound("slower/hammer_strike2.wav");
	G::Server.pfnPrecacheSound("slower/hammer_strike3.wav");

	G::Server.pfnPrecacheSound("slower/hammer_strike_wall1.wav");
	G::Server.pfnPrecacheSound("slower/hammer_strike_wall2.wav");
	G::Server.pfnPrecacheSound("slower/hammer_strike_wall3.wav");

	G::Server.pfnPrecacheSound("slower/hammer_miss1.wav");
	G::Server.pfnPrecacheSound("slower/hammer_miss2.wav");

	G::Server.pfnPrecacheSound("slower/slower_attack1.wav");
	G::Server.pfnPrecacheSound("slower/slower_attack2.wav");

	G::Server.pfnPrecacheSound("slower/slower_alert10.wav");
	G::Server.pfnPrecacheSound("slower/slower_alert20.wav");
	G::Server.pfnPrecacheSound("slower/slower_alert30.wav");

	G::Server.pfnPrecacheSound("slower/slower_pain1.wav");
	G::Server.pfnPrecacheSound("slower/slower_pain2.wav");

	G::Server.pfnPrecacheSound("weapons/glock/glock_fire.wav");
	G::Server.pfnPrecacheSound("baby/b_attack1.wav");
	G::Server.pfnPrecacheSound("baby/b_attack2.wav");
}

static void PrecacheTMP()
{
	G::Server.pfnPrecacheModel("models/ammo/ammo_tmp.mdl");
}

static void PrecacheNightvision()
{
	G::Server.pfnPrecacheModel("models/items/w_nightvision.mdl");
}

static void PrecachePadlock()
{
	G::Server.pfnPrecacheModel("models/items/padlock.mdl");
	G::Server.pfnPrecacheSound("items/padl_turn.wav");
	G::Server.pfnPrecacheSound("items/padl_unlock.wav");
}

static void PrecacheG43()
{
	G::Server.pfnPrecacheModel("models/ammo/ammo_g43.mdl");
	G::Server.pfnPrecacheSound("weapons/item_get.wav");
}

static void PrecacheBookSimon()
{
	G::Server.pfnPrecacheModel("models/booksimon.mdl");
	G::Server.pfnPrecacheModel("sprites/muzzleflash.spr");
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