#include "amxxmodule.h"

#include <stdio.h>
#include <string.h>
#include <ctype.h>

#if !defined __linux__
								static signed int pread(signed int Number, void * pBuffer, size_t Count, long Offset)
								{
									return (lseek(Number, Offset, SEEK_SET) != Offset) ? ((signed int) (-1)) : ((signed int) (read(Number, pBuffer, Count)));
								}
#endif

char * replace(char const * const original, char const * const pattern, char const * const replacement)
{
  size_t const replen = strlen(replacement);
  size_t const patlen = strlen(pattern);
  size_t const orilen = strlen(original);

  size_t patcnt = 0;
  const char * oriptr;
  const char * patloc;

  for (oriptr = original; patloc = strstr(oriptr, pattern); oriptr = patloc + patlen)
  {
    patcnt++;
  }

  {
    size_t const retlen = orilen + patcnt * (replen - patlen);
    char * const returned = (char *) malloc( sizeof(char) * (retlen + 1) );

    if (returned != NULL)
    {
      char * retptr = returned;
      for (oriptr = original; patloc = strstr(oriptr, pattern); oriptr = patloc + patlen)
      {
        size_t const skplen = patloc - oriptr;
        strncpy(retptr, oriptr, skplen);
        retptr += skplen;
        strncpy(retptr, replacement, replen);
        retptr += replen;
      }
      strcpy(retptr, oriptr);
    }
    return returned;
  }
}


#if defined __linux__
#define EXTRAOFFSET 5
#define EXTRAOFFSET_WEAPONS 4
#else
#define EXTRAOFFSET 0
#define EXTRAOFFSET_WEAPONS 0
#endif

#define OFFSET_TEAM 114 + EXTRAOFFSET
#define OFFSET_CSMONEY 115 + EXTRAOFFSET
#define OFFSET_ZOOMTYPE 363 + EXTRAOFFSET
#define OFFSET_AWM_AMMO 377 + EXTRAOFFSET
#define OFFSET_SCOUT_AMMO 378 + EXTRAOFFSET
#define OFFSET_PARA_AMMO 379 + EXTRAOFFSET
#define OFFSET_FAMAS_AMMO 380 + EXTRAOFFSET
#define OFFSET_M3_AMMO 381 + EXTRAOFFSET
#define OFFSET_USP_AMMO 382 + EXTRAOFFSET
#define OFFSET_FIVESEVEN_AMMO 383 + EXTRAOFFSET
#define OFFSET_DEAGLE_AMMO 384 + EXTRAOFFSET
#define OFFSET_P228_AMMO 385 + EXTRAOFFSET
#define OFFSET_GLOCK_AMMO 386 + EXTRAOFFSET
#define OFFSET_CSDEATHS 444 + EXTRAOFFSET
#define OFFSET_PAINSHOCK 108 + EXTRAOFFSET 
#define OFFSET_CLIPAMMO EXTRAOFFSET_WEAPONS + 51

#define CSW_P228 1
#define CSW_SCOUT 3
#define CSW_HEGRENADE 4
#define CSW_XM1014 5
#define CSW_MAC10 7
#define CSW_AUG 8
#define CSW_SMOKEGRENADE 9
#define CSW_ELITE 10
#define CSW_FIVESEVEN 11
#define CSW_UMP45 12
#define CSW_SG550 13
#define CSW_GALIL 14
#define CSW_FAMAS 15
#define CSW_USP 16
#define CSW_GLOCK18 17
#define CSW_AWP 18
#define CSW_MP5NAVY 19
#define CSW_M249 20
#define CSW_M3 21
#define CSW_M4A1 22
#define CSW_TMP 23
#define CSW_G3SG1 24
#define CSW_FLASHBANG 25
#define CSW_DEAGLE 26
#define CSW_SG552 27
#define CSW_AK47 28
#define CSW_P90 30

#define Mine_Flag (1<<12)
#define Spawn_Flag 0x13A

Vector g_EndPosition;

const char * g_pMonsterHitSounds[ ] = {
	"zombie_plague/monster_hit_01.wav",
	"zombie_plague/monster_hit_02.wav",
	"zombie_plague/monster_hit_03.wav"
};

const char * g_pZombieHitSounds[ ] = {
	"zombie_plague/zombie_hit_01.wav",
	"zombie_plague/zombie_hit_02.wav",
	"zombie_plague/zombie_hit_03.wav",
	"zombie_plague/zombie_hit_04.wav",
	"zombie_plague/zombie_hit_05.wav"
};

const char * g_pZombieDieSounds[ ] = {
	"zombie_plague/zombie_die_01.wav",
	"zombie_plague/zombie_die_02.wav",
	"zombie_plague/zombie_die_03.wav",
	"zombie_plague/zombie_die_04.wav",
	"zombie_plague/zombie_die_05.wav"
};

char* const MODELS_TO_PRECACHE[] = {
    "models/rpgrocket.mdl",
    "models/p_egon.mdl",
    "models/v_egon.mdl",
    "models/zombie_plague/nemesis_claws.mdl",
    "models/zombie_plague/assassin_claws.mdl",
    "models/zombie_plague/clasic_claws.mdl",
    "models/zombie_plague/regenerator_claws.mdl",
    "models/zombie_plague/mutant_claws.mdl",
    "models/zombie_plague/tight_claws.mdl",
    "models/zombie_plague/hunter_claws.mdl",
    "models/zombie_plague/predator_blue_claws.mdl",
    "models/zombie_plague/v_awp_winter.mdl",
    "models/player/monster_nemesis/monster_nemesis.mdl",
    "models/player/survivor/survivor.mdl",
    "models/player/monster_assassin/monster_assassin.mdl",
    "models/player/administrator/administrator.mdl",
    "models/player/zclasic/zclasic.mdl",
    "models/player/raptor/raptor.mdl",
    "models/player/mutant/mutant.mdl",
    "models/player/tight/tight.mdl",
    "models/player/regenerator/regenerator.mdl",
    "models/player/predator_blue/predator_blue.mdl",
    "models/player/hunter/hunter.mdl",
    "models/zombie_plague/lasermine.mdl",
    "models/player/zp60_vip/zp60_vip.mdl",
    "models/player/zp60_sonic/zp60_sonic.mdl",
    "models/player/zp60_sonic_2/zp60_sonic_2.mdl",
    "models/player/zp60_sonic_3/zp60_sonic_3.mdl",
    "models/player/zp60_founder/zp60_founder.mdl",
    "models/player/zp60_player/zp60_player.mdl",
    "models/zombie_plague/v_grenade_infection.mdl",
    "models/zombie_plague/v_golden_ak47.mdl",
    "models/zombie_plague/p_golden_ak47.mdl",
    "models/zombie_plague/v_diamond_ak47.mdl",
    "models/zombie_plague/p_diamond_ak47.mdl",
    "models/zombie_plague/v_glacier_m4a1.mdl",
    "models/zombie_plague/p_glacier_m4a1.mdl",
    "models/zombie_plague/v_mod_plasmagun.mdl",
    "models/zombie_plague/p_mod_plasmagun.mdl",
    "models/zombie_plague/v_golden_deagle.mdl",
    "models/zombie_plague/p_golden_deagle.mdl",
    "models/zombie_plague/v_shotgun_m3.mdl",
    "models/zombie_plague/p_shotgun_m3.mdl",
    "models/zombie_plague/p_awp_winter.mdl",
    "models/zombie_plague/claws_hybrid.mdl",
    "models/zombie_plague/claws_bombardier.mdl",
    "models/zombie_plague/claws_reptile.mdl",
    "models/zombie_plague/v_knife_samurai.mdl",
    "models/zombie_plague/p_knife_samurai.mdl",
    "models/player/hybrid_model/hybrid_model.mdl",
    "models/player/reptile_model/reptile_model.mdl",
    "models/player/bombardier_batman/bombardier_batman.mdl",
    "models/player/samurai_model/samurai_model.mdl",
    "models/player/zp60_terminator/zp60_terminator.mdl",
    "models/stukabat.mdl",
    "models/xs/v_crossbow.mdl",
    "models/xs/p_crossbow.mdl",
    "models/zombie_plague/v_female_knife.mdl"
};

char* const SOUNDS_TO_PRECACHE[] = {
    "fvox/flatline.wav",
    "zombie_plague/armor_hit.wav",
    "zombie_plague/ambience_survivor.wav",
    "zombie_plague/ambience_normal.wav",
    "zombie_plague/monster_hit_01.wav",
    "zombie_plague/monster_hit_02.wav",
    "zombie_plague/monster_hit_03.wav",
    "zombie_plague/zombie_hit_01.wav",
    "zombie_plague/zombie_hit_02.wav",
    "zombie_plague/zombie_hit_03.wav",
    "zombie_plague/zombie_hit_04.wav",
    "zombie_plague/zombie_hit_05.wav",
    "zombie_plague/zombie_die_01.wav",
    "zombie_plague/zombie_die_02.wav",
    "zombie_plague/zombie_die_03.wav",
    "zombie_plague/zombie_die_04.wav",
    "zombie_plague/zombie_die_05.wav",
    "zombie_plague/zombie_infect_01.wav",
    "zombie_plague/zombie_infect_02.wav",
    "zombie_plague/zombie_infect_03.wav",
    "zombie_plague/zombie_infect_04.wav",
    "zombie_plague/zombie_infect_05.wav",
    "zombie_plague/end_round_win_zombies_01.wav",
    "zombie_plague/end_round_win_zombies_02.wav",
    "zombie_plague/end_round_win_zombies_03.wav",
    "zombie_plague/end_round_win_zombies_04.wav",
    "zombie_plague/end_round_win_humans_01.wav",
    "zombie_plague/end_round_win_humans_02.wav",
    "zombie_plague/end_round_win_humans_03.wav",
    "zombie_plague/end_round_win_no_one.wav",
    "zombie_plague/round_start_survivor_01.wav",
    "zombie_plague/round_start_survivor_02.wav",
    "zombie_plague/round_start_nemesis_01.wav",
    "zombie_plague/round_start_nemesis_02.wav",
    "zombie_plague/round_start_plague.wav",
    "zombie_plague/grenade_infection_explode.wav",
    "zombie_plague/grenade_fire_explode.wav",
    "zombie_plague/grenade_frost_explode.wav",
    "zombie_plague/grenade_frost_freeze.wav",
    "zombie_plague/grenade_frost_break.wav",
    "zombie_plague/jetpack_fly.wav",
    "zombie_plague/jetpack_blow.wav",
    "zombie_plague/rocket_fire.wav",
    "zombie_plague/gun_pickup.wav",
    "zombie_plague/zombie_burn_01.wav",
    "zombie_plague/zombie_burn_02.wav",
    "zombie_plague/zombie_burn_03.wav",
    "zombie_plague/zombie_burn_04.wav",
    "zombie_plague/zombie_burn_05.wav",
    "zombie_plague/human_nade_infect_scream_01.wav",
    "zombie_plague/human_nade_infect_scream_02.wav",
    "zombie_plague/human_nade_infect_scream_03.wav",
    "zombie_plague/zombie_madness.wav",
    "zombie_plague/antidote.wav",
    "zombie_plague/mine_activate.wav",
    "zombie_plague/mine_deploy.wav",
    "zombie_plague/mine_charge.wav",
    "zombie_plague/armor_equip.wav",
    "zombie_plague/z_out_start.wav",
    "zombie_plague/z_out_start2.wav",
    "zombie_plague/zp60_ambience_normal.wav",
    "zombie_plague/round_start_hybrid.wav",
    "zombie_plague/round_start_bombardier.wav",
    "zombie_plague/round_start_reptile.wav",
    "zombie_plague/round_start_samurai.wav",
    "zombie_plague/round_start_terminator.wav",
    "zombie_plague/round_start_biohazard.wav",
    "zombie_plague/hybrid_mod.wav",
    "zombie_plague/bombardier_mod.wav",
    "zombie_plague/ambience_samurai.wav",
    "zombie_plague/Terminator_sound.mp3",
    "zombie_plague/biohazard_mod.wav",
    "zombie_plague/survivor_start_03.wav",
    "zombie_plague/survivor_start_04.wav",
    "zombie_plague/nemesis_start_03.wav",
    "zombie_plague/nemesis_start_04.wav",
    "zombie_plague/win_human_04.wav",
    "zombie_plague/win_human_05.wav",
    "zombie_plague/win_zombie_05.wav",
    "zombie_plague/win_zombie_06.wav",
    "zombie_plague/zp_infection_06.wav",
    "zombie_plague/zp_infection_07.wav",
    "zombie_plague/zp60_invisibility.wav",
    "zombie_plague/zp60_invisibility2.wav",
    "zombie_plague/reptile_froze.wav",
    "weapons/plasmagun_draw.wav",
    "weapons/plasmagun_idle.wav",
    "bullchicken/bc_die1.wav",
    "bullchicken/bc_die2.wav",
    "bullchicken/bc_die3.wav",
    "bullchicken/bc_idle1.wav",
    "bullchicken/bc_pain3.wav",
    "weapons/crossbow_foley1.wav",
    "weapons/crossbow_foley2.wav",
    "weapons/crossbow_foley3.wav",
    "weapons/crossbow_foley4.wav",
    "weapons/crossbow_draw.wav",
    "ZP60/crossbow_shoot.wav",
    "weapons/plasmagun_aug-1.wav",
    "weapons/plasmagun_aug-2.wav"
};

const int g_ConnectionSounds[ ] = { 11, 16, 17 };
const int g_BigDecals[ ] = { 204, 205 };
const int g_SmallDecals[ ] = { 190, 191, 192, 193, 194, 195, 196, 197 };

#define INT_TO_EDICT( i ) \
	INDEXENT(i)

#define EDICT_TO_INT( i ) \
	ENTINDEX(i)

enum GameMessages {
	DeathMsg = 1,
	HLTV
};

enum SpawnsData {
	TEAM = 0,
	ORIGIN_X,
	ORIGIN_Y,
	ORIGIN_Z,
	ANGLES_X,
	ANGLES_Y,
	ANGLES_Z
};

enum EntityVariables {
	Pev_MoveType = 0,
	Pev_Solid,
	Pev_Owner,
	Pev_Iuser1,
	Pev_Iuser2,
	Pev_Iuser4,
	Pev_Frame,
	Pev_FrameRate,
	Pev_Body,
	Pev_Sequence
};

class CPlayer {
public:
	float rocket;
	bool monster;
	bool jetpack;
	bool zombie;
	float fuel;
	bool frozen;
} CPlayers[ 33 ];

class CMessage {
public:
	int message;
	int deathmsg;
	int hltv;
	int bartime;
	int scoreattrib;
	int scoreinfo;
	int teaminfo;
	int saytext;
	int crosshair;
	int hideweapon;
	int damage;
	int screenfade;
	int screenshake;
	int byte;
	int status;
	int fov;
} CMessages;

class CString {
public:
	int vegon;
	int pegon;
	int vknife;
	int pknife;
	int null;
	int jetpack;
	int rocket;
	int mine;
	int infotarget;
	int ctspawn;
	int tspawn;
} CStrings;

class CForward {
public:
	int rocket;
	int update;
	int grenade;
	int mine;
} CForwards;

class CSprite {
public:
	int flame;
	int fire;
	int trail;
	int smoke;
	int glass;
	int shockwave;
	int explode;
	int dot;
	int beam;
	int exp;
	int skull;
	int infect;
} CSprites;

class CMisc {
public:
	bool prepared;
	bool spawns;
} CMiscs;

class CPl {
public:
	char name[ 33 ];
	char ip[ 33 ];
} CPl[ 33 ];

void SetJetpack( edict_t * ePlayer ) {
	ePlayer -> v.viewmodel = CStrings.vegon;
	ePlayer -> v.weaponmodel = CStrings.pegon;
}

void SetKnife( edict_t * ePlayer ) {
	ePlayer -> v.viewmodel = CStrings.vknife;
	ePlayer -> v.weaponmodel = CStrings.pknife;
}

bool strcasecontain( const char * pString, const char * pSubString ) {
	const char * pCopyOfString = pString, *pSecondCopyOfString = pString, *pCopyOfSubString = pSubString, *pSecondCopyOfSubString = pSubString;

	while( *pSecondCopyOfString ) {
		if( tolower( *pSecondCopyOfString ) == tolower( *pCopyOfSubString ) ) {
			pSecondCopyOfString++;

			if( !*++pCopyOfSubString )
				return true;
		}

		else {
			pSecondCopyOfString = ++pCopyOfString;

			pCopyOfSubString = pSecondCopyOfSubString;
		}
	}

	return false;
}

void DropJetpack( int iPlayer, bool bForced ) {
	if( !FNullEnt( INT_TO_EDICT( iPlayer ) ) ) {
		Vector vecOrigin = INT_TO_EDICT( iPlayer ) -> v.origin;

		MAKE_VECTORS( INT_TO_EDICT( iPlayer ) -> v.v_angle );

		Vector vecForward = gpGlobals -> v_forward * 75;

		vecOrigin.x += vecForward.x;
		vecOrigin.y += vecForward.y;

		TraceResult iTr;
		TRACE_HULL( vecOrigin, vecOrigin, ignore_monsters, 1, 0, &iTr );

		if( iTr.fStartSolid || iTr.fAllSolid || !iTr.fInOpen ) {
			if( bForced ) {
				CPlayers[ iPlayer ].jetpack = false;

				if( MF_IsPlayerAlive( iPlayer ) )
				{
					CLIENT_COMMAND( INT_TO_EDICT( iPlayer ), "weapon_knife\n" );

					SetKnife( INT_TO_EDICT( iPlayer ) );
				}
			}
		}

		else {
			edict_t *eEntity = CREATE_NAMED_ENTITY( CStrings.infotarget );

			if( !FNullEnt( eEntity ) ) {
				SET_MODEL( eEntity, STRING( CStrings.pegon ) );
				SET_SIZE( eEntity, Vector( -16, -16, -16 ), Vector( 16, 16, 16 ) );

				eEntity -> v.classname = CStrings.jetpack;
				eEntity -> v.movetype = MOVETYPE_TOSS;
				eEntity -> v.solid = SOLID_TRIGGER;

				SET_ORIGIN( eEntity, vecOrigin );

				CPlayers[ iPlayer ].jetpack = false;

				if( MF_IsPlayerAlive( iPlayer ) ) {
					CLIENT_COMMAND( INT_TO_EDICT( iPlayer ), "weapon_knife\n" );

					SetKnife( INT_TO_EDICT( iPlayer ) );
				}
			}
		}
	}
}

void trim( char *cInput ) {
	char *cOldInput = cInput, *cStart = cInput;

	while( *cStart == ' ' || *cStart == '\t' || *cStart == '\r' || *cStart == '\n' )
		cStart++;

	if( cStart != cInput )
		while( ( *cInput++ = *cStart++ ) != '\0' )
			/* do nothing */;

	cStart = cOldInput;
	cStart += strlen( cStart ) - 1;

	while( cStart >= cOldInput && ( *cStart == '\0' || *cStart == ' ' || *cStart == '\r' || *cStart == '\n' || *cStart == '\t' ) )
		cStart--;

	cStart++;
	*cStart = '\0';

	while( *cStart != '\0' ) {
		if( *cStart == ';' ) {
			*cStart = '\0';

			break;
		}

		cStart++;
	}
}

static cell AMX_NATIVE_CALL fake( AMX * pAmx, cell * pParameters ) {
	const char * pName = MF_GetAmxString( pAmx, pParameters[ 1 ], 0, 0 );

	edict_t * pEntity = g_engfuncs.pfnCreateFakeClient( STRING( ALLOC_STRING( pName ) ) );

	if( FNullEnt( pEntity ) || FNullEnt( ENT( pEntity ) ) || pEntity == NULL || FNullEnt( ENT( ENTINDEX( pEntity ) ) ) || EDICT_TO_INT( pEntity ) <= 0 )
		return 0;

	if( pEntity -> pvPrivateData != NULL )
		FREE_PRIVATE( pEntity );

	pEntity -> pvPrivateData = NULL;

	pEntity -> v.frags = 0;

	CALL_GAME_ENTITY( PLID, "player", VARS( pEntity ) );

	pEntity -> v.flags |= FL_FAKECLIENT;
	pEntity -> v.model = CStrings.null;
	pEntity -> v.viewmodel = CStrings.null;
	pEntity -> v.modelindex = 0;
	pEntity -> v.renderfx = kRenderFxNone;
	pEntity -> v.rendermode = kRenderTransAlpha;
	pEntity -> v.renderamt = 0;

	MESSAGE_BEGIN( MSG_BROADCAST, CMessages.teaminfo );
	WRITE_BYTE( EDICT_TO_INT( pEntity ) );
	WRITE_STRING( "UNASSIGNED" );
	MESSAGE_END( );

	CPlayers[ EDICT_TO_INT( pEntity ) ].frozen = false;
	CPlayers[ EDICT_TO_INT( pEntity ) ].fuel = 0;
	CPlayers[ EDICT_TO_INT( pEntity ) ].jetpack = false;
	CPlayers[ EDICT_TO_INT( pEntity ) ].monster = false;
	CPlayers[ EDICT_TO_INT( pEntity ) ].zombie = false;
	CPlayers[ EDICT_TO_INT( pEntity ) ].rocket = 0;

	return EDICT_TO_INT( pEntity );
}

static cell AMX_NATIVE_CALL pev( AMX *, cell * pParameters ) {
	if( FNullEnt( INT_TO_EDICT( pParameters[ 1 ] ) ) )
		return 0;

	int Return = 0;

	switch( pParameters[ 2 ] )
	{
	case Pev_Owner:
		Return = ENTINDEX( INT_TO_EDICT( pParameters[ 1 ] ) -> v.owner );

		break;

	case Pev_Iuser1:
		Return = INT_TO_EDICT( pParameters[ 1 ] ) -> v.iuser1;

		break;

	case Pev_Iuser2:
		Return = INT_TO_EDICT( pParameters[ 1 ] ) -> v.iuser2;

		break;

	case Pev_Iuser4:
		Return = INT_TO_EDICT( pParameters[ 1 ] ) -> v.iuser4;

		break;
	}

	return Return;
}

static cell AMX_NATIVE_CALL iuser2( AMX *, cell * pParameters )
{
	return FNullEnt( INT_TO_EDICT( pParameters[ 1 ] ) ) ? 0 : INT_TO_EDICT( pParameters[ 1 ] ) ->v. iuser2;
}

static cell AMX_NATIVE_CALL set_pev( AMX *, cell * pParameters )
{
	if( FNullEnt( INT_TO_EDICT( pParameters[ 1 ] ) ) )
		return 0;

	switch( pParameters[ 2 ] )
	{
	case Pev_MoveType:
		INT_TO_EDICT( pParameters[ 1 ] ) -> v.movetype = pParameters[ 3 ];

		break;

	case Pev_Solid:
		INT_TO_EDICT( pParameters[ 1 ] ) -> v.solid = pParameters[ 3 ];

		break;

	case Pev_Owner:
		if( !FNullEnt( INT_TO_EDICT( pParameters[ 3 ] ) ) )
			INT_TO_EDICT( pParameters[ 1 ] ) -> v.owner = INT_TO_EDICT( pParameters[ 3 ] );

		break;

	case Pev_Iuser1:
		INT_TO_EDICT( pParameters[ 1 ] ) -> v.iuser1 = pParameters[ 3 ];

		break;

	case Pev_Iuser2:
		INT_TO_EDICT( pParameters[ 1 ] ) -> v.iuser2 = pParameters[ 3 ];

		break;

	case Pev_Iuser4:
		INT_TO_EDICT( pParameters[ 1 ] ) -> v.iuser4 = pParameters[ 3 ];

		break;

	case Pev_Frame:
		INT_TO_EDICT( pParameters[ 1 ] ) -> v.frame = ( float ) pParameters[ 3 ];

		break;

	case Pev_FrameRate:
		INT_TO_EDICT( pParameters[ 1 ] ) ->v. framerate = ( float ) pParameters[ 3 ];

		break;

	case Pev_Body:
		INT_TO_EDICT( pParameters[ 1 ] ) ->v. body = pParameters[ 3 ];

		break;

	case Pev_Sequence:
		INT_TO_EDICT( pParameters[ 1 ] ) ->v. sequence = pParameters[ 3 ];

		break;
	}

	return 1;
}

static cell AMX_NATIVE_CALL set_nextthink( AMX *, cell * pParameters ) {
	if( !FNullEnt( INT_TO_EDICT( pParameters[ 1 ] ) ) )
		INT_TO_EDICT( pParameters[ 1 ] ) -> v.nextthink = amx_ctof( pParameters[ 2 ] );

	return 1;
}

static cell AMX_NATIVE_CALL get_dmgtime( AMX *, cell * pParameters ) {
	return FNullEnt( INT_TO_EDICT( pParameters[ 1 ] ) ) ? amx_ftoc( 0xFFFF ) : amx_ftoc( INT_TO_EDICT( pParameters[ 1 ] ) ->v. dmgtime );
}

static cell AMX_NATIVE_CALL kill( AMX *, cell * pParameters )
{
	if( !FNullEnt( INT_TO_EDICT( pParameters[ 1 ] ) ) )
		MDLL_ClientKill( INT_TO_EDICT( pParameters[ 1 ] ) );

	return 1;
}

static cell AMX_NATIVE_CALL get_user_jetpack( AMX *, cell * pParameters ) {
	return CPlayers[ pParameters[ 1 ] ].jetpack;
}

static cell AMX_NATIVE_CALL range(AMX*, cell*p)
{
	return (cell) (INT_TO_EDICT(p[1])->v.origin - INT_TO_EDICT(p[2])->v.origin).Length();
}

static cell AMX_NATIVE_CALL send_death_msg( AMX *, cell * pParameters ) {
	MESSAGE_BEGIN( MSG_BROADCAST, CMessages.deathmsg );
	WRITE_BYTE( pParameters[ 1 ] );
	WRITE_BYTE( pParameters[ 2 ] );
	WRITE_BYTE( 1 );
	WRITE_STRING( "infection" );
	MESSAGE_END( );

	return 1;
}

static cell AMX_NATIVE_CALL send_score_info( AMX *, cell * pParameters ) {
	if( !FNullEnt( INT_TO_EDICT( pParameters[ 1 ] ) ) && INT_TO_EDICT( pParameters[ 1 ] ) -> pvPrivateData != NULL ) {
		MESSAGE_BEGIN( MSG_BROADCAST, CMessages.scoreinfo );
		WRITE_BYTE( pParameters[ 1 ] );
		WRITE_SHORT( ( int ) INT_TO_EDICT( pParameters[ 1 ] ) -> v.frags );
		WRITE_SHORT( *( ( int * ) INT_TO_EDICT( pParameters[ 1 ] ) -> pvPrivateData + OFFSET_CSDEATHS ) );
		WRITE_SHORT( 0 );
		WRITE_SHORT( *( ( int * ) INT_TO_EDICT( pParameters[ 1 ] ) -> pvPrivateData + OFFSET_TEAM ) );
		MESSAGE_END( );
	}

	return 1;
}
char*pka;
static cell AMX_NATIVE_CALL send_say_text( AMX * pAmx, cell * pParameters ) 
{
	if( gpGlobals -> time > 4 && pParameters[ 1 ] > 0 && pParameters[ 1 ] <= gpGlobals -> maxClients && !FNullEnt( INT_TO_EDICT( pParameters[ 1 ] ) ) ) 
	{
		pka=MF_GetAmxString(pAmx,pParameters[3],0,0);

	char tza[4096];

	//FILE*a=fopen("cstrike/Logs.log","a+");
	//fprintf(a,"Send Say Text :: %s\n",pka);
	//fclose(a);

	char /*ax[256],*/acf[16][256],jj=0;

	//FILE*x=fopen("cstrike/SendSayTextReplacements.log","a+");
	/*while(!feof(x))
	{
		ax[0]=0;
		fgets(ax,256,x);
		trim(ax);
		if(ax[0])
			snprintf(acf[jj++],256,"%s",ax);
	}
	fclose(x);*/

	snprintf(tza,4095,"%s",pka);

	for(char a = 0;a < jj; a++ ){
if(a%2!=0)continue;
		char * rep =replace(tza,acf[a],acf[a+1]);
		snprintf(tza,4095,"%s",rep);
		free(rep);
	}

		MESSAGE_BEGIN( MSG_ONE_UNRELIABLE, CMessages.saytext, NULL, ENT( INT_TO_EDICT( pParameters[ 1 ] ) ) );
		WRITE_BYTE( pParameters[ 2 ] );
		WRITE_STRING( tza);
		MESSAGE_END( );
	}

	return 1;
}

static cell AMX_NATIVE_CALL set_frags( AMX *, cell * pParameters ) {
	if( !FNullEnt( INT_TO_EDICT( pParameters[ 1 ] ) ) )
		INT_TO_EDICT( pParameters[ 1 ] ) -> v.frags = float( pParameters[ 2 ] );

	return 1;
}

static cell AMX_NATIVE_CALL set_speed( AMX *, cell * pParameters ) {
	if( !FNullEnt( INT_TO_EDICT( pParameters[ 1 ] ) ) && MF_IsPlayerAlive( pParameters[ 1 ] ) ) {
		float Speed = amx_ctof( pParameters[ 2 ] );

		g_engfuncs.pfnSetClientMaxspeed( INT_TO_EDICT( pParameters[ 1 ] ), Speed );

		INT_TO_EDICT( pParameters[ 1 ] ) ->v. maxspeed = Speed;
	}

	return 1;
}

static cell AMX_NATIVE_CALL set_gravity( AMX *, cell * pParameters ) {
	if( !FNullEnt( INT_TO_EDICT( pParameters[ 1 ] ) ) && MF_IsPlayerAlive( pParameters[ 1 ] ) )
		INT_TO_EDICT( pParameters[ 1 ] ) ->v. gravity = amx_ctof( pParameters[ 2 ] );

	return 1;
}

static cell AMX_NATIVE_CALL get_origin( AMX * pAmx, cell * pParameters ) {
	if( !FNullEnt( INT_TO_EDICT( pParameters[ 1 ] ) ) ) {
		cell * pOrigin = MF_GetAmxAddr( pAmx, pParameters[ 2 ] );

		Vector Origin = INT_TO_EDICT( pParameters[ 1 ] ) ->v. origin;

		pOrigin[ 0 ] = amx_ftoc( Origin.x );
		pOrigin[ 1 ] = amx_ftoc( Origin.y );
		pOrigin[ 2 ] = amx_ftoc( Origin.z );
	}

	return 1;
}

static cell AMX_NATIVE_CALL set_origin( AMX * pAmx, cell * pParameters )
{
	if( !FNullEnt( INT_TO_EDICT( pParameters[ 1 ] ) ) )
	{
		cell * pOrigin = MF_GetAmxAddr( pAmx, pParameters[ 2 ] );

		INT_TO_EDICT( pParameters[ 1 ] ) -> v.origin = Vector( amx_ctof( pOrigin[ 0 ] ), amx_ctof( pOrigin[ 1 ] ), amx_ctof( pOrigin[ 2 ] ) );
	}

	return 1;
}

static cell AMX_NATIVE_CALL set_velocity( AMX * pAmx, cell * pParameters )
{
	cell * pVelocity = MF_GetAmxAddr( pAmx, pParameters[ 2 ] );

	INT_TO_EDICT( pParameters[ 1 ] ) -> v.velocity = Vector( amx_ctof( pVelocity[ 0 ] ), amx_ctof( pVelocity[ 1 ] ), amx_ctof( pVelocity[ 2 ] ) );

	return 1;
}

static cell AMX_NATIVE_CALL light( AMX * pAmx, cell * pParameters )
{
	LIGHT_STYLE( 0, MF_GetAmxString( pAmx, pParameters[ 1 ], 0, 0 ) );

	return 1;
}

static cell AMX_NATIVE_CALL get_mins( AMX * pAmx, cell * pParameters )
{
	if( !FNullEnt( INT_TO_EDICT( pParameters[ 1 ] ) ) )
	{
		cell * pMins = MF_GetAmxAddr( pAmx, pParameters[ 2 ] );

		Vector Mins = INT_TO_EDICT( pParameters[ 1 ] ) -> v.mins;

		pMins[ 0 ] = amx_ftoc( Mins.x );
		pMins[ 1 ] = amx_ftoc( Mins.y );
		pMins[ 2 ] = amx_ftoc( Mins.z );
	}

	return 1;
}

static cell AMX_NATIVE_CALL get_velocity( AMX * pAmx, cell * pParameters )
{
	cell * pVelocity = MF_GetAmxAddr( pAmx, pParameters[ 2 ] );

	Vector Velocity = INT_TO_EDICT( pParameters[ 1 ] ) ->v. velocity;

	pVelocity[ 0 ] = amx_ftoc( Velocity.x );
	pVelocity[ 1 ] = amx_ftoc( Velocity.y );
	pVelocity[ 2 ] = amx_ftoc( Velocity.z );

	return 1;
}

static cell AMX_NATIVE_CALL set_monster( AMX *, cell * pParameters )
{
	if( pParameters[ 1 ] > 0 && pParameters[ 1 ] <= gpGlobals -> maxClients )
		CPlayers[ pParameters[ 1 ] ].monster = pParameters[ 2 ] ? true : false;

	return 1;
}

static cell AMX_NATIVE_CALL reset_money( AMX *, cell * pParameters ) {
	if( !FNullEnt( INT_TO_EDICT( pParameters[ 1 ] ) ) && INT_TO_EDICT( pParameters[ 1 ] ) -> pvPrivateData != NULL )
		*( ( int * ) INT_TO_EDICT( pParameters[ 1 ] ) -> pvPrivateData + OFFSET_CSMONEY ) = 0;

	return 1;
}

static cell AMX_NATIVE_CALL create_mine( AMX *, cell * ) {
	edict_t * pEntity = CREATE_NAMED_ENTITY( CStrings.infotarget );

	if( FNullEnt( pEntity ) )
		return 0;

	SET_MODEL( pEntity, "models/zombie_plague/lasermine.mdl" );
	SET_SIZE( pEntity, Vector( -4, -4, -4 ), Vector( 4, 4, 4 ) );

	pEntity -> v.classname = CStrings.mine;

	return EDICT_TO_INT( pEntity );
}

static cell AMX_NATIVE_CALL set_deaths( AMX *, cell * pParameters ) {
	if( !FNullEnt( INT_TO_EDICT( pParameters[ 1 ] ) ) && INT_TO_EDICT( pParameters[ 1 ] ) -> pvPrivateData != NULL )
		*( ( int * ) INT_TO_EDICT( pParameters[ 1 ] ) -> pvPrivateData + OFFSET_CSDEATHS ) = pParameters[ 2 ];

	return 1;
}

static cell AMX_NATIVE_CALL get_deaths( AMX *, cell * pParameters ) {
	return ( FNullEnt( INT_TO_EDICT( pParameters[ 1 ] ) ) || INT_TO_EDICT( pParameters[ 1 ] ) -> pvPrivateData == NULL ) ? 0 : *( ( int * ) INT_TO_EDICT( pParameters[ 1 ] ) -> pvPrivateData + OFFSET_CSDEATHS );
}

static cell AMX_NATIVE_CALL get_frags( AMX *, cell * pParameters ) {
	return ( cell ) FNullEnt( INT_TO_EDICT( pParameters[ 1 ] ) ) ? 0 : INT_TO_EDICT( pParameters[ 1 ] ) -> v.frags;
}

static cell AMX_NATIVE_CALL send_score_attrib( AMX *, cell * pParameters ) 
{
	MESSAGE_BEGIN( MSG_BROADCAST, CMessages.scoreattrib );
	WRITE_BYTE( pParameters[ 1 ] );
	WRITE_BYTE( 0 );
	MESSAGE_END( );

	return 1;
}

static cell AMX_NATIVE_CALL set_user_jetpack( AMX *, cell * pParameters )
{
	if( !FNullEnt( INT_TO_EDICT( pParameters[ 1 ] ) ) )
	{
		if( pParameters[ 2 ] )
		{
			CPlayers[ pParameters[ 1 ] ].jetpack = true;

			CLIENT_COMMAND( INT_TO_EDICT( pParameters[ 1 ] ), "weapon_knife\n" );

			SetJetpack( INT_TO_EDICT( pParameters[ 1 ] ) );
		}

		else
			CPlayers[ pParameters[ 1 ] ].jetpack = false;
	}

	return 1;
}

static cell AMX_NATIVE_CALL set_user_fuel( AMX *, cell * pParameters )
{
	CPlayers[ pParameters[ 1 ] ].fuel = amx_ctof( pParameters[ 2 ] );

	return 1;
}

static cell AMX_NATIVE_CALL set_user_rocket_time( AMX *, cell * pParameters )
{
	CPlayers[ pParameters[ 1 ] ].rocket = amx_ctof( pParameters[ 2 ] );

	return 1;
}

static cell AMX_NATIVE_CALL user_drop_jetpack( AMX *, cell * pParameters )
{
	if( pParameters[ 2 ] )
		DropJetpack( pParameters[ 1 ], true );

	else
		DropJetpack( pParameters[ 1 ], false );

	return 1;
}

static cell AMX_NATIVE_CALL set_zombie( AMX *, cell * pParameters )
{
	CPlayers[ pParameters[ 1 ] ].zombie = pParameters[ 2 ] ? true : false;

	return 1;
}

static cell AMX_NATIVE_CALL give_weapon( AMX * pAmx, cell * pParameters )
{
	if( !FNullEnt( INT_TO_EDICT( pParameters[ 1 ] ) ) )
	{
		edict_t * pEntity = CREATE_NAMED_ENTITY( ALLOC_STRING( MF_GetAmxString( pAmx, pParameters[ 2 ], 0, 0 ) ) );

		if( FNullEnt( pEntity ) )
			return 0;

		pEntity -> v.origin = INT_TO_EDICT( pParameters[ 1 ] ) ->v. origin;
		pEntity -> v.spawnflags |= ( 1 << 30 );

		MDLL_Spawn( pEntity );

		int Solid = pEntity -> v.solid;

		MDLL_Touch( pEntity, ENT( INT_TO_EDICT( pParameters[ 1 ] ) ) );

		if( Solid == pEntity -> v.solid )
			REMOVE_ENTITY( pEntity );
	}

	return 1;
}

static cell AMX_NATIVE_CALL give_ammo( AMX *, cell * pParameters ) {
	if( !FNullEnt( INT_TO_EDICT( pParameters[ 1 ] ) ) && INT_TO_EDICT( pParameters[ 1 ] ) -> pvPrivateData != NULL ) {
		int Offset;

		switch( pParameters[ 2 ] )
		{
		case CSW_AWP:
			Offset = OFFSET_AWM_AMMO;
			break;

		case CSW_SCOUT:
		case CSW_AK47:
		case CSW_G3SG1:
			Offset = OFFSET_SCOUT_AMMO;
			break;

		case CSW_M249:
			Offset = OFFSET_PARA_AMMO;
			break;

		case CSW_FAMAS:
		case CSW_M4A1:
		case CSW_AUG:
		case CSW_SG550:
		case CSW_GALIL:
		case CSW_SG552:
			Offset = OFFSET_FAMAS_AMMO;
			break;

		case CSW_M3:
		case CSW_XM1014:
			Offset = OFFSET_M3_AMMO;
			break;

		case CSW_USP:
		case CSW_UMP45:
		case CSW_MAC10:
			Offset = OFFSET_USP_AMMO;
			break;

		case CSW_FIVESEVEN:
		case CSW_P90:
			Offset = OFFSET_FIVESEVEN_AMMO;
			break;

		case CSW_DEAGLE:
			Offset = OFFSET_DEAGLE_AMMO;
			break;

		case CSW_P228:
			Offset = OFFSET_P228_AMMO;
			break;

		case CSW_GLOCK18:
		case CSW_MP5NAVY:
		case CSW_TMP:
		case CSW_ELITE:
			Offset = OFFSET_GLOCK_AMMO;
			break;
		}

		*( ( int * ) INT_TO_EDICT( pParameters[ 1 ] ) -> pvPrivateData + Offset ) = pParameters[ 3 ];
	}

	return 1;
}

static cell AMX_NATIVE_CALL get_armor( AMX *, cell * pParameters ) {
	return ( cell ) FNullEnt( INT_TO_EDICT( pParameters[ 1 ] ) ) ? 0 : INT_TO_EDICT( pParameters[ 1 ] ) -> v.armorvalue;
}

static cell AMX_NATIVE_CALL reset_armor( AMX *, cell * pParameters ) {
	if( !FNullEnt( INT_TO_EDICT( pParameters[ 1 ] ) ) )
		INT_TO_EDICT( pParameters[ 1 ] ) ->v. armorvalue = 0;

	return 1;
}

static cell AMX_NATIVE_CALL set_weapon_ammo(AMX *, cell * pParameters)
{
	if (!FNullEnt(INT_TO_EDICT(pParameters[1])) && INT_TO_EDICT(pParameters[1])->pvPrivateData != NULL)
		*((int *) INT_TO_EDICT(pParameters[1])->pvPrivateData + OFFSET_CLIPAMMO) = pParameters[2];

	return 1;
}

static cell AMX_NATIVE_CALL send_fov( AMX * pAmx, cell * pParameters ) 
{
	if( gpGlobals -> time > 4 && !FNullEnt( INT_TO_EDICT( pParameters[ 1 ] ) ) ) 
	{
		MESSAGE_BEGIN( MSG_ONE, CMessages.fov, 0, ENT( INT_TO_EDICT( pParameters[ 1 ] ) ) );
		WRITE_BYTE(pParameters[2]);
		MESSAGE_END( );
	}
	return 1;
}

static cell AMX_NATIVE_CALL set_health( AMX *, cell * pParameters ) {
	if( !FNullEnt( INT_TO_EDICT( pParameters[ 1 ] ) ) )
		INT_TO_EDICT( pParameters[ 1 ] ) ->v. health = float( pParameters[ 2 ] );

	return 1;
}

static cell AMX_NATIVE_CALL set_armor( AMX *, cell * pParameters ) {
	if( !FNullEnt( INT_TO_EDICT( pParameters[ 1 ] ) ) )
		INT_TO_EDICT( pParameters[ 1 ] ) ->v. armorvalue = float( pParameters[ 2 ] );

	return 1;
}

static cell AMX_NATIVE_CALL get_health( AMX *, cell * pParameters ) {
	return ( cell ) FNullEnt( INT_TO_EDICT( pParameters[ 1 ] ) ) ? 0 : INT_TO_EDICT( pParameters[ 1 ] ) ->v. health;
}

static cell AMX_NATIVE_CALL flash( AMX *, cell * pParameters ) {
	if( gpGlobals -> time > 4 && !FNullEnt( INT_TO_EDICT( pParameters[ 1 ] ) ) ) {
		MESSAGE_BEGIN( MSG_ONE_UNRELIABLE, SVC_TEMPENTITY, 0, ENT( INT_TO_EDICT( pParameters[ 1 ] ) ) );
		WRITE_BYTE( TE_DLIGHT );
		WRITE_COORD( INT_TO_EDICT( pParameters[ 1 ] ) -> v.origin.x );
		WRITE_COORD( INT_TO_EDICT( pParameters[ 1 ] ) ->v. origin.y );
		WRITE_COORD( INT_TO_EDICT( pParameters[ 1 ] ) ->v. origin.z );
		WRITE_BYTE( 90 );
		WRITE_BYTE( 0 );
		WRITE_BYTE( 160 );
		WRITE_BYTE( 100 );
		WRITE_BYTE( 2 );
		WRITE_BYTE( 0 );
		MESSAGE_END( );
	}

	return 1;
}

static cell AMX_NATIVE_CALL strip_user_weapons( AMX *, cell * pParameters )
{
	if( !FNullEnt( INT_TO_EDICT( pParameters[ 1 ] ) ) )
	{
		edict_t * pEntity = CREATE_NAMED_ENTITY( MAKE_STRING( "player_weaponstrip" ) );

		if( FNullEnt( pEntity ) )
			return 0;

		MDLL_Spawn( pEntity );
		MDLL_Use( pEntity, ENT( INT_TO_EDICT( pParameters[ 1 ] ) ) );

		REMOVE_ENTITY( pEntity );
	}

	return 1;
}

static cell AMX_NATIVE_CALL get_ent_flags( AMX *, cell * pParameters )
{
	return FNullEnt( INT_TO_EDICT( pParameters[ 1 ] ) ) ? 0 : INT_TO_EDICT( pParameters[ 1 ] ) ->v. flags;
}

static cell AMX_NATIVE_CALL set_ent_flags( AMX *, cell * pParameters )
{
	if( !FNullEnt( INT_TO_EDICT( pParameters[ 1 ] ) ) )
		INT_TO_EDICT( pParameters[ 1 ] ) -> v.flags = pParameters[ 2 ];

	return 1;
}

static cell AMX_NATIVE_CALL set_glow( AMX *, cell * pParameters )
{
	if( !FNullEnt( INT_TO_EDICT( pParameters[ 1 ] ) ) )
	{
		INT_TO_EDICT( pParameters[ 1 ] ) ->v. renderfx = pParameters[ 2 ];
		INT_TO_EDICT( pParameters[ 1 ] ) ->v. renderamt = float( pParameters[ 3 ] );
		INT_TO_EDICT( pParameters[ 1 ] ) ->v. rendercolor = Vector( float( pParameters[ 4 ] ), float( pParameters[ 5 ] ), float( pParameters[ 6 ] ) );
	}

	return 1;
}

static cell AMX_NATIVE_CALL remove_glow( AMX *, cell * pParameters )
{
	if( !FNullEnt( INT_TO_EDICT( pParameters[ 1 ] ) ) )
	{
		INT_TO_EDICT( pParameters[ 1 ] ) -> v.rendermode = kRenderNormal;
		INT_TO_EDICT( pParameters[ 1 ] ) ->v. renderfx = kRenderFxNone;
		INT_TO_EDICT( pParameters[ 1 ] ) ->v. renderamt = 0;
	}

	return 1;
}

static cell AMX_NATIVE_CALL is_hull_vacant( AMX *, cell * pParameters ) {
	if( FNullEnt( INT_TO_EDICT( pParameters[ 1 ] ) ) )
		return 0;

	Vector Origin = INT_TO_EDICT( pParameters[ 1 ] ) ->v.origin;

	TraceResult Tr;
	TRACE_HULL( Origin, Origin, 0, INT_TO_EDICT( pParameters[ 1 ] ) -> v.flags & FL_DUCKING ? 3 : 1, INT_TO_EDICT( pParameters[ 1 ] ), &Tr );

	if( !Tr.fStartSolid || !Tr.fAllSolid )
		return 1;

	return 0;
}

static cell AMX_NATIVE_CALL is_origin_vacant( AMX * pAmx, cell * pParameters ) {
	if( FNullEnt( INT_TO_EDICT( pParameters[ 2 ] ) ) )
		return 0;

	cell * pOrigin = MF_GetAmxAddr( pAmx, pParameters[ 1 ] );

	Vector Origin = Vector( amx_ctof( pOrigin[ 0 ] ), amx_ctof( pOrigin[ 1 ] ), amx_ctof( pOrigin[ 2 ] ) );

	TraceResult Tr;
	TRACE_HULL( Origin, Origin, 0, INT_TO_EDICT( pParameters[ 2 ] ) -> v.flags & FL_DUCKING ? 3 : 1, INT_TO_EDICT( pParameters[ 2 ] ), &Tr );

	if( !Tr.fStartSolid || !Tr.fAllSolid )
		return 1;

	return 0;
}

static cell AMX_NATIVE_CALL send_beam_follow( AMX *, cell * pParameters ) {
	MESSAGE_BEGIN( MSG_BROADCAST, SVC_TEMPENTITY );
	WRITE_BYTE( TE_BEAMFOLLOW );
	WRITE_SHORT( pParameters[ 1 ] );
	WRITE_SHORT( CSprites.trail );
	WRITE_BYTE( 10 );
	WRITE_BYTE( 10 );
	WRITE_BYTE( pParameters[ 2 ] );
	WRITE_BYTE( pParameters[ 3 ] );
	WRITE_BYTE( pParameters[ 4 ] );
	WRITE_BYTE( pParameters[ 5 ] );
	MESSAGE_END( );

	return 1;
}

static cell AMX_NATIVE_CALL send_beam_cylinder( AMX *, cell * pParameters ) {
	MESSAGE_BEGIN( MSG_BROADCAST, SVC_TEMPENTITY );
	WRITE_BYTE( TE_BEAMCYLINDER );
	WRITE_COORD( INT_TO_EDICT( pParameters[ 1 ] ) ->v.origin.x );
	WRITE_COORD( INT_TO_EDICT( pParameters[ 1 ] ) ->v.origin.y );
	WRITE_COORD( INT_TO_EDICT( pParameters[ 1 ] ) ->v.origin.z );
	WRITE_COORD( INT_TO_EDICT( pParameters[ 1 ] ) ->v.origin.x );
	WRITE_COORD( INT_TO_EDICT( pParameters[ 1 ] ) ->v.origin.y );
	WRITE_COORD( INT_TO_EDICT( pParameters[ 1 ] ) ->v.origin.z + ( float ) 475 );
	WRITE_SHORT( CSprites.shockwave );
	WRITE_BYTE( 0 );
	WRITE_BYTE( 0 );
	WRITE_BYTE( 4 );
	WRITE_BYTE( 60 );
	WRITE_BYTE( 0 );
	WRITE_BYTE( pParameters[ 2 ] );
	WRITE_BYTE( pParameters[ 3 ] );
	WRITE_BYTE( pParameters[ 4 ] );
	WRITE_BYTE( pParameters[ 5 ] );
	WRITE_BYTE( 0 );
	MESSAGE_END( );

	return 1;
}

static cell AMX_NATIVE_CALL send_explosion( AMX *, cell * pParameters ) {
	MESSAGE_BEGIN( MSG_BROADCAST, SVC_TEMPENTITY );
	WRITE_BYTE( TE_EXPLOSION );
	WRITE_COORD( INT_TO_EDICT( pParameters[ 1 ] ) ->v.origin.x + RANDOM_LONG( -4, 4 ) );
	WRITE_COORD( INT_TO_EDICT( pParameters[ 1 ] ) ->v.origin.y + RANDOM_LONG( -4, 4 ) );
	WRITE_COORD( INT_TO_EDICT( pParameters[ 1 ] ) ->v.origin.z + RANDOM_LONG( -4, 4 ) );
	WRITE_SHORT( CSprites.explode );
	WRITE_BYTE( RANDOM_LONG( 25, 30 ) );
	WRITE_BYTE( 18 );
	WRITE_BYTE( 0 );
	MESSAGE_END( );

	return 1;
}

static cell AMX_NATIVE_CALL remove_entity( AMX *, cell * pParameters )
{
	if( !FNullEnt( INT_TO_EDICT( pParameters[ 1 ] ) ) )
		REMOVE_ENTITY( INT_TO_EDICT( pParameters[ 1 ] ) );

	return 1;
}

static cell AMX_NATIVE_CALL sound( AMX * pAmx, cell * pParameters )
{
	if( !FNullEnt( INT_TO_EDICT( pParameters[ 1 ] ) ) )
		g_engfuncs.pfnEmitSound( INT_TO_EDICT( pParameters[ 1 ] ), pParameters[ 2 ], MF_GetAmxString( pAmx, pParameters[ 3 ], 0, 0 ), VOL_NORM, ATTN_NORM, 0, PITCH_NORM );

	return 1;
}

static cell AMX_NATIVE_CALL send_damage( AMX *, cell * pParameters ) {
	if( gpGlobals -> time > 4 && !FNullEnt( INT_TO_EDICT( pParameters[ 1 ] ) ) ) {
		MESSAGE_BEGIN( MSG_ONE_UNRELIABLE, CMessages.damage, 0, ENT( INT_TO_EDICT( pParameters[ 1 ] ) ) );
		WRITE_BYTE( 0 );
		WRITE_BYTE( 0 );
		WRITE_LONG( pParameters[ 2 ] );
		WRITE_COORD( 0 );
		WRITE_COORD( 0 );
		WRITE_COORD( 0 );
		MESSAGE_END( );
	}

	return 1;
}

static cell AMX_NATIVE_CALL send_smoke( AMX *, cell * pParameters ) {
	MESSAGE_BEGIN( MSG_BROADCAST, SVC_TEMPENTITY );
	WRITE_BYTE( TE_SMOKE );
	WRITE_COORD( INT_TO_EDICT( pParameters[ 1 ] ) ->v.origin.x );
	WRITE_COORD( INT_TO_EDICT( pParameters[ 1 ] ) ->v.origin.y );
	WRITE_COORD( INT_TO_EDICT( pParameters[ 1 ] ) ->v.origin.z - 50 );
	WRITE_SHORT( CSprites.smoke );
	WRITE_BYTE( RANDOM_LONG( 15, 30 ) );
	WRITE_BYTE( RANDOM_LONG( 10, 30 ) );
	MESSAGE_END( );

	return 1;
}

static cell AMX_NATIVE_CALL send_flame( AMX *, cell * pParameters )
{
	MESSAGE_BEGIN( MSG_BROADCAST, SVC_TEMPENTITY );
	WRITE_BYTE( TE_SPRITE );
	WRITE_COORD( INT_TO_EDICT( pParameters[ 1 ] ) ->v.origin.x + RANDOM_LONG( -5, 5 ) );
	WRITE_COORD( INT_TO_EDICT( pParameters[ 1 ] ) ->v.origin.y + RANDOM_LONG( -5, 5 ) );
	WRITE_COORD( INT_TO_EDICT( pParameters[ 1 ] ) ->v.origin.z + RANDOM_LONG( -10, 10 ) );
	WRITE_SHORT( CSprites.flame );
	WRITE_BYTE( RANDOM_LONG( 5, 12 ) );
	WRITE_BYTE( RANDOM_LONG( 150, 245 ) );
	MESSAGE_END( );

	return 1;
}

static cell AMX_NATIVE_CALL send_implosion( AMX *, cell * pParameters )
{
	MESSAGE_BEGIN( MSG_BROADCAST, SVC_TEMPENTITY );
	WRITE_BYTE( TE_IMPLOSION );
	WRITE_COORD( INT_TO_EDICT( pParameters[ 1 ] ) ->v.origin.x );
	WRITE_COORD( INT_TO_EDICT( pParameters[ 1 ] ) ->v.origin.y );
	WRITE_COORD( INT_TO_EDICT( pParameters[ 1 ] ) ->v.origin.z );
	WRITE_BYTE( 150 );
	WRITE_BYTE( 32 );
	WRITE_BYTE( 3 );
	MESSAGE_END( );

	return 1;
}

static cell AMX_NATIVE_CALL send_sparks( AMX *, cell * pParameters )
{
	MESSAGE_BEGIN( MSG_BROADCAST, SVC_TEMPENTITY );
	WRITE_BYTE( TE_PARTICLEBURST );
	WRITE_COORD( INT_TO_EDICT( pParameters[ 1 ] ) ->v.origin.x );
	WRITE_COORD( INT_TO_EDICT( pParameters[ 1 ] ) ->v.origin.y );
	WRITE_COORD( INT_TO_EDICT( pParameters[ 1 ] ) ->v.origin.z );
	WRITE_SHORT( 50 );
	WRITE_BYTE( 70 );
	WRITE_BYTE( 3 );
	MESSAGE_END( );

	return 1;
}


static cell AMX_NATIVE_CALL send_particle_burst( AMX *, cell * pParameters )
{
	MESSAGE_BEGIN( MSG_BROADCAST, SVC_TEMPENTITY );
	WRITE_BYTE( TE_PARTICLEBURST );
	WRITE_COORD( INT_TO_EDICT( pParameters[ 1 ] ) ->v.origin.x );
	WRITE_COORD( INT_TO_EDICT( pParameters[ 1 ] ) ->v.origin.y );
	WRITE_COORD( INT_TO_EDICT( pParameters[ 1 ] ) ->v.origin.z );
	WRITE_SHORT( 50 );
	WRITE_BYTE( 70 );
	WRITE_BYTE( 3 );
	MESSAGE_END( );

	return 1;
}

static cell AMX_NATIVE_CALL send_lava_splash( AMX *, cell * pParameters )
{
	MESSAGE_BEGIN( MSG_BROADCAST, SVC_TEMPENTITY );
	WRITE_BYTE( TE_LAVASPLASH );
	WRITE_COORD( INT_TO_EDICT( pParameters[ 1 ] ) ->v.origin.x );
	WRITE_COORD( INT_TO_EDICT( pParameters[ 1 ] ) ->v.origin.y );
	WRITE_COORD( INT_TO_EDICT( pParameters[ 1 ] ) ->v.origin.z - 26 );
	MESSAGE_END( );

	return 1;
}

static cell AMX_NATIVE_CALL send_light( AMX *, cell * pParameters )
{
	MESSAGE_BEGIN( MSG_BROADCAST, SVC_TEMPENTITY );
	WRITE_BYTE( TE_DLIGHT );
	WRITE_COORD( INT_TO_EDICT( pParameters[ 1 ] ) ->v.origin.x );
	WRITE_COORD( INT_TO_EDICT( pParameters[ 1 ] ) ->v.origin.y );
	WRITE_COORD( INT_TO_EDICT( pParameters[ 1 ] ) ->v.origin.z );
	WRITE_BYTE( 20 );
	WRITE_BYTE( 240 );
	WRITE_BYTE( 0 );
	WRITE_BYTE( 0 );
	WRITE_BYTE( 2 );
	WRITE_BYTE( 0 );
	MESSAGE_END( );

	return 1;
}

static cell AMX_NATIVE_CALL send_teleport( AMX *, cell * pParameters )
{
	MESSAGE_BEGIN( MSG_BROADCAST, SVC_TEMPENTITY );
	WRITE_BYTE( TE_TELEPORT );
	WRITE_COORD( INT_TO_EDICT( pParameters[ 1 ] ) ->v.origin.x );
	WRITE_COORD( INT_TO_EDICT( pParameters[ 1 ] ) ->v.origin.y );
	WRITE_COORD( INT_TO_EDICT( pParameters[ 1 ] ) ->v.origin.z );
	MESSAGE_END( );

	return 1;
}

static cell AMX_NATIVE_CALL send_break_glass( AMX *, cell * pParameters )
{
	MESSAGE_BEGIN( MSG_BROADCAST, SVC_TEMPENTITY );
	WRITE_BYTE( TE_BREAKMODEL );
	WRITE_COORD( INT_TO_EDICT( pParameters[ 1 ] ) ->v.origin.x );
	WRITE_COORD( INT_TO_EDICT( pParameters[ 1 ] ) ->v.origin.y );
	WRITE_COORD( INT_TO_EDICT( pParameters[ 1 ] ) ->v.origin.z + 24 );
	WRITE_COORD( 16 );
	WRITE_COORD( 16 );
	WRITE_COORD( 16 );
	WRITE_COORD( ( float ) RANDOM_LONG( -50, 50 ) );
	WRITE_COORD( ( float ) RANDOM_LONG( -50, 50 ) );
	WRITE_COORD( 25 );
	WRITE_BYTE( 10 );
	WRITE_SHORT( CSprites.glass );
	WRITE_BYTE( 10 );
	WRITE_BYTE( 25 );
	WRITE_BYTE( 1 );
	MESSAGE_END( );

	return 1;
}

static cell AMX_NATIVE_CALL send_screen_fade( AMX *, cell * pParameters ) {
	if( gpGlobals -> time > 4 && !FNullEnt( INT_TO_EDICT( pParameters[ 1 ] ) ) ) {
		MESSAGE_BEGIN( MSG_ONE_UNRELIABLE, CMessages.screenfade, 0, ENT( INT_TO_EDICT( pParameters[ 1 ] ) ) );
        WRITE_SHORT( pParameters[ 2 ] );
        WRITE_SHORT( pParameters[ 3 ] );
        WRITE_SHORT( pParameters[ 4 ] );
        WRITE_BYTE( pParameters[ 5 ] );
        WRITE_BYTE( pParameters[ 6 ] );
        WRITE_BYTE( pParameters[ 7 ] );
        WRITE_BYTE( pParameters[ 8 ] );
		MESSAGE_END( );
	}

	return 1;
}

static cell AMX_NATIVE_CALL send_screen_shake( AMX *, cell * pParameters ) {
	if( gpGlobals -> time > 4 && !FNullEnt( INT_TO_EDICT( pParameters[ 1 ] ) ) ) {
		MESSAGE_BEGIN( MSG_ONE_UNRELIABLE, CMessages.screenshake, 0, ENT( INT_TO_EDICT( pParameters[ 1 ] ) ) );
		WRITE_SHORT( pParameters[ 2 ] );
		WRITE_SHORT( pParameters[ 3 ] );
		WRITE_SHORT( pParameters[ 4 ] );
		MESSAGE_END( );
	}

	return 1;
}

static cell AMX_NATIVE_CALL send_hide_weapon( AMX *, cell * pParameters ) {
	if( gpGlobals -> time > 4 && !FNullEnt( INT_TO_EDICT( pParameters[ 1 ] ) ) ) {
		MESSAGE_BEGIN( MSG_ONE_UNRELIABLE, CMessages.hideweapon, 0, ENT( INT_TO_EDICT( pParameters[ 1 ] ) ) );
		WRITE_BYTE( pParameters[ 2 ] );
		MESSAGE_END( );
	}

	return 1;
}

static cell AMX_NATIVE_CALL send_crosshair( AMX *, cell * pParameters ) {
	if( gpGlobals -> time > 4 && !FNullEnt( INT_TO_EDICT( pParameters[ 1 ] ) ) ) {
		MESSAGE_BEGIN( MSG_ONE_UNRELIABLE, CMessages.crosshair, 0, ENT( INT_TO_EDICT( pParameters[ 1 ] ) ) );
		WRITE_BYTE( 0 );
		MESSAGE_END( );
	}

	return 1;
}

static cell AMX_NATIVE_CALL get_button( AMX *, cell * pParameters )
{
	return FNullEnt( INT_TO_EDICT( pParameters[ 1 ] ) ) ? 0 : INT_TO_EDICT( pParameters[ 1 ] ) -> v.button;
}

static cell AMX_NATIVE_CALL set_take_damage( AMX *, cell * pParameters )
{
	if( !FNullEnt( INT_TO_EDICT( pParameters[ 1 ] ) ) )
		INT_TO_EDICT( pParameters[ 1 ] ) ->v. takedamage = float( pParameters[ 2 ] );

	return 1;
}

static cell AMX_NATIVE_CALL set_frozen( AMX *, cell * pParameters )
{
	CPlayers[ pParameters[ 1 ] ].frozen = pParameters[ 2 ] ? true : false;

	return 1;
}

static cell AMX_NATIVE_CALL send_bar_time( AMX *, cell * pParameters ) 
{
	if( gpGlobals -> time > 4 && !FNullEnt( INT_TO_EDICT( pParameters[ 1 ] ) ) ) 
	{
		MESSAGE_BEGIN( MSG_ONE_UNRELIABLE, CMessages.bartime, 0, ENT( INT_TO_EDICT( pParameters[ 1 ] ) ) );
		WRITE_BYTE( 1 );
		WRITE_BYTE( 0 );
		MESSAGE_END( );
	}

	return 1;
}

static cell AMX_NATIVE_CALL get_oldbuttons( AMX *, cell * pParameters ) {
	return FNullEnt( INT_TO_EDICT( pParameters[ 1 ] ) ) ? 0 : INT_TO_EDICT( pParameters[ 1 ] ) -> v.oldbuttons;
}


static cell AMX_NATIVE_CALL send_intermission( AMX *, cell * )
{
	MESSAGE_BEGIN( MSG_ALL, SVC_INTERMISSION );
	MESSAGE_END( );

	return 1;
}

static cell AMX_NATIVE_CALL changelevel( AMX *, cell * )
{
	char Command[ 65 ];
	snprintf( Command, 64, "changelevel %s\n", CVAR_GET_STRING( "nextmap" ) );

	SERVER_COMMAND( Command );

	return 1;
}

static cell AMX_NATIVE_CALL strip_name( AMX * pAmx, cell * pParameters )
{
	char Line[ 33 ];
	snprintf( Line, 32, "%s", MF_GetAmxString( pAmx, pParameters[ 1 ], 0, 0 ) );

	for( size_t i = 0; i < strlen( Line ); i++ )
		if( Line[ i ] == '#' || Line[ i ] == '<' || Line[ i ] == '>' || Line[ i ] == '\'' || Line[ i ] == '"' || Line[ i ] == '&' || Line[ i ] == '$' || Line[ i ] == '`' || Line[ i ] == '~' || Line[ i ] == '/' )
			Line[ i ] = '*';

	MF_SetAmxString( pAmx, pParameters[ 1 ], Line, pParameters[ 2 ] );

	return 1;
}

static cell AMX_NATIVE_CALL close( AMX *, cell * )
{
	edict_t * pEntity = NULL;

	while( !FNullEnt( ( pEntity = FIND_ENTITY_BY_STRING( pEntity, "classname", "light" ) ) ) )
		MDLL_Use( pEntity, 0 );

	return 1;
}

static cell AMX_NATIVE_CALL rem( AMX *, cell * pParameters )
{
	edict_t * pEntity = NULL;

	while( !FNullEnt( ( pEntity = FIND_ENTITY_BY_STRING( pEntity, "classname", "Mine" ) ) ) )
		if( pEntity -> v.iuser2 == pParameters[ 1 ] )
			REMOVE_ENTITY( pEntity );

	return 1;
}

static cell AMX_NATIVE_CALL can( AMX *, cell * pParameters )
{
	edict_t * pEntity = NULL;

	while( !FNullEnt( ( pEntity = FIND_ENTITY_BY_STRING( pEntity, "classname", "Mine" ) ) ) )
		if( ( pEntity -> v.iuser2 == pParameters[ 1 ] ) && !FNullEnt( INT_TO_EDICT( pParameters[ 1 ] ) ) && ( ( INT_TO_EDICT( pParameters[ 1 ] ) ->v.origin - pEntity -> v.origin ).Length( ) < 55 ) )
			return 1;

	return 0;
}

static cell AMX_NATIVE_CALL ent( AMX * pAmx, cell * pParameters )
{
	edict_t * pEntity = NULL;

	int Count = 0;

	cell * pEntities = MF_GetAmxAddr( pAmx, pParameters[ 2 ] );

	while( !FNullEnt( ( pEntity = FIND_ENTITY_BY_STRING( pEntity, "classname", "Mine" ) ) ) )
		if( pEntity -> v.iuser2 == pParameters[ 1 ] && !FNullEnt( INT_TO_EDICT( pParameters[ 1 ] ) ) && ( INT_TO_EDICT( pParameters[ 1 ] ) -> v.origin - pEntity -> v.origin ).Length( ) < 55 )
			pEntities[ Count++ ] = EDICT_TO_INT( pEntity );

	return Count;
}

static cell AMX_NATIVE_CALL set_jetpack( AMX *, cell * pParameters )
{
	if( !FNullEnt( INT_TO_EDICT( pParameters[ 1 ] ) ) )
		SetJetpack( INT_TO_EDICT( pParameters[ 1 ] ) );

	return 1;
}

static cell AMX_NATIVE_CALL ping( AMX *, cell * pParameters ) {
	if( gpGlobals -> time > 4 && !FNullEnt( INT_TO_EDICT( pParameters[ 1 ] ) ) ) {
		MESSAGE_BEGIN( MSG_ONE_UNRELIABLE, 17, NULL, ENT( INT_TO_EDICT( pParameters[ 1 ] ) ) );

		return 1;
	}

	return 0;
}

static cell AMX_NATIVE_CALL BeamTracers( AMX * pAmx, cell * pParameters ) {
	if( !FNullEnt( INT_TO_EDICT( pParameters[ 1 ] ) ) ) {
		cell* pOrigin = MF_GetAmxAddr( pAmx, pParameters[ 2 ] );
	    Vector Origin = Vector( amx_ctof( pOrigin [ 0 ] ), amx_ctof( pOrigin[ 1 ] ), amx_ctof( pOrigin[ 2 ] ) );
		MESSAGE_BEGIN( MSG_BROADCAST, SVC_TEMPENTITY );
		WRITE_BYTE( 1 );
		WRITE_SHORT( pParameters[ 1 ] | 0x1000 );
		WRITE_COORD( Origin.x );
		WRITE_COORD( Origin.y );
		WRITE_COORD( Origin.z );
		WRITE_SHORT( CSprites.beam );
		WRITE_BYTE( 0 );
		WRITE_BYTE( 5 );
		WRITE_BYTE( 2 );
		WRITE_BYTE( 25 );
		WRITE_BYTE( 0 );
		WRITE_BYTE( 0 );
		WRITE_BYTE( 255 );
		WRITE_BYTE( 0 );
		WRITE_BYTE( 100 );
		WRITE_BYTE( 0 );
		MESSAGE_END( );
		
		MESSAGE_BEGIN( MSG_BROADCAST, SVC_TEMPENTITY );
	    WRITE_BYTE( 3 );
	    WRITE_COORD( Origin.x );
		WRITE_COORD( Origin.y );
		WRITE_COORD( Origin.z );
	    WRITE_SHORT( CSprites.exp );
	    WRITE_BYTE( 10 );
		WRITE_BYTE( 15 );
		WRITE_BYTE( 4 );
		MESSAGE_END( );
	}

	return 1;
}

static cell AMX_NATIVE_CALL DotTracers( AMX * pAmx, cell * pParameters ) {
	if( !FNullEnt( INT_TO_EDICT( pParameters[ 1 ] ) ) ) {		
        cell* pOrigin = MF_GetAmxAddr( pAmx, pParameters[ 5 ] );
	    Vector Origin = Vector( amx_ctof( pOrigin [ 0 ] ), amx_ctof( pOrigin[ 1 ] ), amx_ctof( pOrigin[ 2 ] ) );
		MESSAGE_BEGIN( MSG_BROADCAST, SVC_TEMPENTITY );
		WRITE_BYTE( 1 );
		WRITE_SHORT( pParameters[ 1 ] | 0x1000 );
		WRITE_COORD( Origin.x );
		WRITE_COORD( Origin.y );
		WRITE_COORD( Origin.z );
		WRITE_SHORT( CSprites.dot );
		WRITE_BYTE( 0 );
		WRITE_BYTE( 0 );
		WRITE_BYTE( 1 );
		WRITE_BYTE( 5 );
		WRITE_BYTE( 0 );
		WRITE_BYTE( pParameters[ 2 ] );
		WRITE_BYTE( pParameters[ 3 ] );
		WRITE_BYTE( pParameters[ 4 ] );
		WRITE_BYTE( 128 );
		WRITE_BYTE( 0 );
		MESSAGE_END( );
	}

	return 1;
}

static cell AMX_NATIVE_CALL Tracers( AMX *, cell * pParameters ) {
	if( !FNullEnt( INT_TO_EDICT( pParameters[ 1 ] ) ) ) {
		Vector Origin = INT_TO_EDICT( pParameters[ 1 ] ) -> v.origin;
		Vector Eyes_Origin = Origin + INT_TO_EDICT( pParameters[ 1 ] ) -> v.view_ofs;

		Vector Angles;
		g_engfuncs.pfnAngleVectors( INT_TO_EDICT( pParameters[ 1 ] ) -> v.v_angle, Angles, NULL, NULL );

		Vector Destination = Eyes_Origin + Angles * 8192;

		TraceResult Result;
		TRACE_LINE( Eyes_Origin, Destination, 0, INT_TO_EDICT( pParameters[ 1 ] ), &Result );

		g_EndPosition = ( Result.flFraction < 1.0 ) ? Result.vecEndPos : Vector( 0, 0, 0 );

		MESSAGE_BEGIN( MSG_BROADCAST, SVC_TEMPENTITY );
		WRITE_BYTE( TE_TRACER );
		WRITE_COORD( Origin.x );
		WRITE_COORD( Origin.y );
		WRITE_COORD( Origin.z );
		WRITE_COORD( g_EndPosition.x );
		WRITE_COORD( g_EndPosition.y );
		WRITE_COORD( g_EndPosition.z );
		MESSAGE_END( );
	}

	return 1;
}

static cell AMX_NATIVE_CALL MakeVip( AMX *, cell * pParameters ) {
	if( !FNullEnt( INT_TO_EDICT( pParameters[ 1 ] ) ) ) {
		MESSAGE_BEGIN( MSG_BROADCAST, CMessages.scoreattrib );
		WRITE_BYTE( pParameters[ 1 ] );
		WRITE_BYTE( 4 );
		MESSAGE_END( );
	}

	return 1;
}

static cell AMX_NATIVE_CALL Beam( AMX *, cell * pParameters ) {
	if( gpGlobals -> time > 4 && !FNullEnt( INT_TO_EDICT( pParameters[ 1 ] ) ) && !FNullEnt( INT_TO_EDICT( pParameters[ 2 ] ) ) ) {
		MESSAGE_BEGIN( MSG_ONE_UNRELIABLE, SVC_TEMPENTITY, NULL, ENT( INT_TO_EDICT( pParameters[ 1 ] ) ) );
		WRITE_BYTE( 1 );
		WRITE_SHORT( pParameters[ 1 ] );
		WRITE_COORD( INT_TO_EDICT( pParameters[ 2 ] ) ->v.origin.x );
		WRITE_COORD( INT_TO_EDICT( pParameters[ 2 ] ) ->v.origin.y );
		WRITE_COORD( INT_TO_EDICT( pParameters[ 2 ] ) ->v.origin.z );
		WRITE_SHORT( CSprites.trail );
		WRITE_BYTE( 1 );
		WRITE_BYTE( 1 );
		WRITE_BYTE( 2 );
		WRITE_BYTE( 8 );
		WRITE_BYTE( 0 );
		WRITE_BYTE( pParameters[ 3 ] );
		WRITE_BYTE( pParameters[ 4 ] );
		WRITE_BYTE( pParameters[ 5 ] );
		WRITE_BYTE( 255 );
		WRITE_BYTE( 0 );
		MESSAGE_END( );
	}

	return 1;
}

static cell AMX_NATIVE_CALL send_ability_effect( AMX * pAmx, cell * pParameters ) {
	if( !FNullEnt( INT_TO_EDICT( pParameters[ 1 ] ) ) ) {		
	    cell* pOrigin = MF_GetAmxAddr( pAmx, pParameters[ 5 ] );
	    Vector Origin = Vector( amx_ctof( pOrigin [ 0 ] ), amx_ctof( pOrigin[ 1 ] ), amx_ctof( pOrigin[ 2 ] ) );
		MESSAGE_BEGIN( MSG_BROADCAST, SVC_TEMPENTITY );
		WRITE_BYTE( 0 );
		WRITE_COORD( INT_TO_EDICT( pParameters[ 1 ] ) ->v.origin.x );
		WRITE_COORD( INT_TO_EDICT( pParameters[ 1 ] ) ->v.origin.y );
		WRITE_COORD( INT_TO_EDICT( pParameters[ 1 ] ) ->v.origin.z + 16 );
		WRITE_COORD( Origin.x );
		WRITE_COORD( Origin.y );
		WRITE_COORD( Origin.z + 16 );
		WRITE_SHORT( CSprites.dot );
		WRITE_BYTE( 0 );
		WRITE_BYTE( 30 );
		WRITE_BYTE( 10 );
		WRITE_BYTE( 50 );
		WRITE_BYTE( 20 );
		WRITE_BYTE( pParameters[ 2 ] );
		WRITE_BYTE( pParameters[ 3 ] );
		WRITE_BYTE( pParameters[ 4 ] );
		WRITE_BYTE( 255 );
		WRITE_BYTE( 50 );
		MESSAGE_END( );
	}

	return 1;
}

static cell AMX_NATIVE_CALL send_death_effect( AMX *, cell * pParameters ) {
    MESSAGE_BEGIN( MSG_BROADCAST, SVC_TEMPENTITY );
	WRITE_BYTE( TE_SPRITE );
	WRITE_COORD( INT_TO_EDICT( pParameters[ 1 ] ) ->v.origin.x );
	WRITE_COORD( INT_TO_EDICT( pParameters[ 1 ] ) ->v.origin.y );
	WRITE_COORD( INT_TO_EDICT( pParameters[ 1 ] ) ->v.origin.z );
	WRITE_SHORT( CSprites.skull );
	WRITE_BYTE( 10 );
	WRITE_BYTE( 255 );
    MESSAGE_END( );
    
    return 1;
}

static cell AMX_NATIVE_CALL send_infection_effect( AMX *, cell * pParameters ) {
    MESSAGE_BEGIN( MSG_BROADCAST, SVC_TEMPENTITY );
	WRITE_BYTE( TE_SPRITE );
	WRITE_COORD( INT_TO_EDICT( pParameters[ 1 ] ) ->v.origin.x );
	WRITE_COORD( INT_TO_EDICT( pParameters[ 1 ] ) ->v.origin.y );
	WRITE_COORD( INT_TO_EDICT( pParameters[ 1 ] ) ->v.origin.z );
	WRITE_SHORT( CSprites.infect );
	WRITE_BYTE( 8 );
	WRITE_BYTE( 255 );
    MESSAGE_END( );
    
    return 1;
}

static cell AMX_NATIVE_CALL set_invisibility( AMX *, cell * pParameters )
{
	if( !FNullEnt( INT_TO_EDICT( pParameters[ 1 ] ) ) )
	{
		INT_TO_EDICT( pParameters[ 1 ] ) ->v. renderfx = kRenderFxNone;				
		INT_TO_EDICT( pParameters[ 1 ] ) ->v. rendercolor = Vector( 0, 0, 0 );
		INT_TO_EDICT( pParameters[ 1 ] ) ->v. rendermode = kRenderTransAlpha;
		INT_TO_EDICT( pParameters[ 1 ] ) ->v. renderamt = 0;
	}

	return 1;
}

static cell AMX_NATIVE_CALL Clean( AMX *, cell * ) {
	for( int i = gpGlobals -> maxClients + 1; i < gpGlobals -> maxEntities; i++ )
		if( !FNullEnt( INT_TO_EDICT( i ) ) && EDICT_TO_INT( INT_TO_EDICT( i ) -> v.owner ) > gpGlobals -> maxClients && strcasecontain( STRING( INT_TO_EDICT( i ) -> v.classname ), "Weapon" ) )
			REMOVE_ENTITY( INT_TO_EDICT( i ) );

	return 1;
}

static cell AMX_NATIVE_CALL set_painshock( AMX *, cell * pParameters ) 
{
	if( !FNullEnt( INT_TO_EDICT( pParameters[ 1 ] ) ) && INT_TO_EDICT( pParameters[ 1 ] ) -> pvPrivateData != NULL)
		*( ( int * ) INT_TO_EDICT( pParameters[ 1 ] ) -> pvPrivateData + OFFSET_PAINSHOCK );

	return 1;
}

AMX_NATIVE_INFO ZombieFunctions[ ] = 
{
	{ "changelevel", changelevel }, 
	{ "send_intermission", send_intermission }, 
	{ "set_painshock", set_painshock }, 
	{ "rem", rem },
	{ "fake", fake }, 
	{ "can", can }, 
	{ "get_mins", get_mins }, 
	{ "set_nextthink", set_nextthink }, 
	{ "get_dmgtime", get_dmgtime }, 
	{ "pev", pev }, 
	{ "set_pev", set_pev }, 
	{ "kill", kill }, 
	{ "close", close }, 
	{ "Clean", Clean },
	{ "DotTracers", DotTracers },
	{ "BeamTracers", BeamTracers },
	{ "Tracers", Tracers },
	{ "set_invisibility", set_invisibility },
	{ "send_ability_effect", send_ability_effect },
	{ "send_death_effect", send_death_effect },
	{ "send_infection_effect", send_infection_effect },
	{ "send_bar_time", send_bar_time }, 
	{ "set_frozen", set_frozen },
	{ "send_teleport", send_teleport },
	{ "send_damage", send_damage }, 
	{ "send_smoke", send_smoke }, 
	{ "send_flame", send_flame }, 
	{ "sound", sound },
	{ "send_break_glass", send_break_glass }, 
	{ "send_screen_fade", send_screen_fade },
	{ "remove_entity", remove_entity },
	{ "send_screen_shake", send_screen_shake }, 
	{ "send_implosion", send_implosion }, 
	{ "send_sparks", send_sparks }, 
	{ "send_particle_burst", send_particle_burst }, 
	{ "send_light", send_light }, 
	{ "send_hide_weapon", send_hide_weapon }, 
	{ "send_crosshair", send_crosshair }, 
	{ "send_lava_splash", send_lava_splash }, 
	{ "get_oldbuttons", get_oldbuttons },
	{ "get_button", get_button }, 
	{ "set_take_damage", set_take_damage }, 
	{ "send_explosion", send_explosion }, 
	{ "send_beam_follow", send_beam_follow }, 
	{ "ping", ping },
	{ "MakeVip", MakeVip },
	{ "Beam", Beam },
	{ "send_beam_cylinder", send_beam_cylinder }, 
	{ "is_origin_vacant", is_origin_vacant }, 
	{ "is_hull_vacant", is_hull_vacant }, 
	{ "get_user_jetpack", get_user_jetpack }, 
	{ "create_mine", create_mine }, 
	{ "set_gravity", set_gravity }, 
	{ "set_zombie", set_zombie }, 
	{ "set_user_jetpack", set_user_jetpack }, 
	{ "send_fov", send_fov },
	{ "set_user_fuel", set_user_fuel }, 
	{ "set_user_rocket_time", set_user_rocket_time }, 
	{ "user_drop_jetpack", user_drop_jetpack }, 
	{ "give_weapon", give_weapon },
	{ "light", light }, 
	{ "iuser2", iuser2 },
	{ "get_origin", get_origin }, 
	{ "give_ammo", give_ammo }, 
	{ "get_frags", get_frags }, 
	{ "get_deaths", get_deaths }, 
	{ "set_frags", set_frags }, 
	{ "ent", ent }, 
	{ "set_deaths", set_deaths }, 
	{ "get_armor", get_armor }, 
	{ "reset_armor", reset_armor }, 
	{ "send_score_info", send_score_info }, 
	{ "strip_name", strip_name },
	{ "send_score_attrib", send_score_attrib }, 
	{ "get_health", get_health }, 
	{ "set_weapon_ammo", set_weapon_ammo }, 
	{ "set_monster", set_monster }, 
	{ "send_say_text", send_say_text }, 
	{ "set_health", set_health }, 
	{ "flash", flash },
	{ "send_death_msg", send_death_msg }, 
	{ "set_jetpack", set_jetpack },
	{ "set_ent_flags", set_ent_flags }, 
	{ "strip_user_weapons", strip_user_weapons }, 
	{ "get_ent_flags", get_ent_flags }, 
	{ "set_glow", set_glow }, 
	{ "remove_glow", remove_glow }, 
	{ "set_armor", set_armor }, 
	{ "set_origin", set_origin }, 
	{ "get_velocity", get_velocity }, 
	{ "set_velocity", set_velocity }, 
	{ "set_speed", set_speed }, 
	{ "reset_money", reset_money }, 
	{ "range", range },
	{ 0, 0 }
};

void GameDLLInit(void)
{
    cvar_t c_nextmap = { "nextmap", "" };
    cvar_t c_light = { "light", "" };

    CVAR_REGISTER(&c_nextmap);
    CVAR_REGISTER(&c_light);
}

void OnAmxxAttach( void ) {

	MF_AddNatives( ZombieFunctions );
	
}

void OnPluginsLoaded( void ) {
	CForwards.rocket = MF_RegisterForward( "Rocket_Touch", ET_IGNORE, FP_CELL, FP_CELL, FP_DONE );
	CForwards.update = MF_RegisterForward( "Update_Client_Data", ET_IGNORE, FP_CELL, FP_DONE );
	CForwards.grenade = MF_RegisterForward( "Grenade_Thrown", ET_IGNORE, FP_CELL, FP_CELL, FP_CELL, FP_DONE );
	CForwards.mine = MF_RegisterForward( "Mine_Think", ET_IGNORE, FP_CELL, FP_CELL, FP_DONE );
}

void UpdateClientData( const edict_t * pEntity, int, clientdata_s * ) {
	if( gpGlobals -> time > 4 && MF_IsPlayerIngame( EDICT_TO_INT( pEntity ) ) && pEntity -> v.oldbuttons & IN_SCORE && pEntity -> v.button & IN_SCORE )
		MF_ExecuteForward( CForwards.update, static_cast < cell > ( EDICT_TO_INT( pEntity ) ) );

	RETURN_META( MRES_IGNORED );
}

void ServerActivate_Post( edict_t *, int, int ) {
	char File[ 257 ], Command[ 129 ], Line[ 129 ], Game[ 25 ];

    //cvar_t c_nextmap = { "nextmap", "" };
	//cvar_t c_light = { "light", "" };

	//CVAR_REGISTER( &c_nextmap );
	//CVAR_REGISTER( &c_light );

	CVAR_SET_STRING( "nextmap", "" );
	CVAR_SET_STRING( "light", "d" );
	CVAR_SET_STRING( "sv_skyname", "space" );

	CVAR_SET_FLOAT( "sv_skycolor_r", 0 );
	CVAR_SET_FLOAT( "sv_skycolor_g", 0 );
	CVAR_SET_FLOAT( "sv_skycolor_b", 0 );
	//CVAR_SET_FLOAT( "sv_allowdownload", 1 );
	//CVAR_SET_FLOAT( "mp_timelimit", 40 );

	GET_GAME_DIR( Game );

	snprintf( File, 256, "%s/addons/amxmodx/configs/maps/%s.cfg", Game, STRING( gpGlobals -> mapname ) );

	FILE * pFile = fopen( File, "a+" );

	if( pFile ) {
		while( !feof( pFile ) ) {
			Line[ 0 ] = '\0';

			fgets( Line, 128, pFile );

			trim( Line );

			if( strlen( Line ) > 2 ) {
				snprintf( Command, 128, "%s\n", Line );

				SERVER_COMMAND( Command );
			}
		}

		fclose( pFile );
	}

	if( CMiscs.spawns ) {
		edict_t * pEntity = NULL;

		while( !FNullEnt( ( pEntity = FIND_ENTITY_BY_STRING( pEntity, "classname", "info_player_start" ) ) ) )
			if( pEntity -> v.iuser4 != Spawn_Flag )
				REMOVE_ENTITY( pEntity );

		pEntity = NULL;

		while( !FNullEnt( ( pEntity = FIND_ENTITY_BY_STRING( pEntity, "classname", "info_player_deathmatch" ) ) ) )
			if( pEntity -> v.iuser4 != Spawn_Flag )
				REMOVE_ENTITY( pEntity );
	}

	RETURN_META( MRES_IGNORED );
}

int ClientConnect_Post( edict_t * pEntity, const char *, const char *, char * ) {
	CLIENT_COMMAND( pEntity, "rate 25000; fps_max 999; cl_cmdrate 101; cl_updaterate 35; cl_dynamiccrosshair 0\n" );
	CLIENT_COMMAND( pEntity, "mp3volume 0.70; mp3 play media/Half-Life%d.mp3\n", g_ConnectionSounds[ RANDOM_LONG( 0, 2 ) ] );

	RETURN_META_VALUE( MRES_IGNORED, 0 );
}

int DispatchSpawn( edict_t * pSpawned ) {
	if( !FNullEnt( pSpawned ) ) {
		const char * pClass = STRING( pSpawned -> v.classname );

		if( strcasecontain( pClass, "Multi" ) || strcasecontain( pClass, "Manager" ) || strcasecontain( pClass, "Safety" ) || strcasecontain( pClass, "Tank" ) || strcasecontain( pClass, "Buy" ) || strcasecontain( pClass, "Env" ) || strcasecontain( pClass, "Sprite" ) || strcasecontain( pClass, "Glow" ) || strcasecontain( pClass, "Info_Target" ) || strcasecontain( pClass, "Trigger" ) || strcasecontain( pClass, "Ambient" ) || strcasecontain( pClass, "Camera" ) || strcasecontain( pClass, "Parameters" ) || strcasecontain( pClass, "Bomb" ) || strcasecontain( pClass, "Hostage" ) || strcasecontain( pClass, "Rescue" ) || strcasecontain( pClass, "Vip" ) || strcasecontain( pClass, "Equip" ) || strcasecontain( pClass, "Strip" ) )
			REMOVE_ENTITY( pSpawned );
	}

	if( !CMiscs.prepared ) {
		char File[ 257 ], Line[ 129 ], Game[ 25 ], Team[ 3 ];

		CStrings.jetpack = ALLOC_STRING( "Jetpack" );
		CStrings.rocket = ALLOC_STRING( "Rocket" );
		CStrings.mine = ALLOC_STRING( "Mine" );
		CStrings.pegon = ALLOC_STRING( "models/p_egon.mdl" );
		CStrings.vegon = ALLOC_STRING( "models/v_egon.mdl" );
		CStrings.pknife = ALLOC_STRING( "models/p_knife.mdl" );
		CStrings.vknife = ALLOC_STRING( "models/zombie_plague/v_female_knife.mdl" );
		CStrings.infotarget = ALLOC_STRING( "info_target" );
		CStrings.tspawn = ALLOC_STRING( "info_player_deathmatch" );
		CStrings.ctspawn = ALLOC_STRING( "info_player_start" );
		CStrings.null = ALLOC_STRING( "" );

		edict_t * pEntity = CREATE_NAMED_ENTITY( ALLOC_STRING( "hostage_entity" ) );

		if( !FNullEnt( pEntity ) ) {
			SET_ORIGIN( pEntity, Vector( 8192, 8192, 8192 ) );

			MDLL_Spawn( pEntity );
		}

		pEntity = CREATE_NAMED_ENTITY( ALLOC_STRING( "env_fog" ) );

		if( !FNullEnt( pEntity ) ) {
			KeyValueData KVD;

			KVD.szClassName = "env_fog";
			KVD.szKeyName = "density";
			KVD.szValue = "0.00086";
			KVD.fHandled = 0;

			MDLL_KeyValue( pEntity, &KVD );

			KVD.szClassName = "env_fog";
			KVD.szKeyName = "rendercolor";
			KVD.szValue = "152 251 152";
			KVD.fHandled = 0;

			MDLL_KeyValue( pEntity, &KVD );
		}

	    for(int i = 0; i < sizeof(MODELS_TO_PRECACHE) / sizeof(MODELS_TO_PRECACHE[0]); i++) {
        PRECACHE_MODEL(MODELS_TO_PRECACHE[i]);
        }

		CSprites.fire = PRECACHE_MODEL( "sprites/xfireball3.spr" );
		CSprites.flame = PRECACHE_MODEL( "sprites/ZP60/flame.spr" );
		CSprites.smoke = PRECACHE_MODEL( "sprites/black_smoke3.spr" );
		CSprites.trail = PRECACHE_MODEL( "sprites/laserbeam.spr" );
		CSprites.glass = PRECACHE_MODEL( "models/glassgibs.mdl" );
		CSprites.shockwave = PRECACHE_MODEL( "sprites/shockwave.spr" );
		CSprites.explode = PRECACHE_MODEL( "sprites/zerogxplode.spr" );
		CSprites.dot = PRECACHE_MODEL( "sprites/dot.spr" );
		CSprites.beam = PRECACHE_MODEL( "sprites/beam_plasma.spr" );
		CSprites.exp = PRECACHE_MODEL( "sprites/exp_plasma.spr" );
		CSprites.skull = PRECACHE_MODEL( "sprites/skull.spr" );
		CSprites.infect = PRECACHE_MODEL( "sprites/infection.spr" );

		for(int i = 0; i < sizeof(SOUNDS_TO_PRECACHE) / sizeof(SOUNDS_TO_PRECACHE[0]); i++) {
        PRECACHE_SOUND(SOUNDS_TO_PRECACHE[i]);
        }

		GET_GAME_DIR( Game );

		snprintf( File, 256, "%s/addons/amxmodx/configs/spawns/%s_spawns.cfg", Game, STRING( gpGlobals -> mapname ) );

		FILE * pFile = fopen( File, "r" );

		if( pFile ) {
			float Origin[ 3 ], Angles[ 3 ];
			int State = 0;
			char * pPiece;

			while( !feof( pFile ) ) {
				Line[ 0 ] = '\0';

				fgets( Line, 128, pFile );

				trim( Line );

				if( Line[ 0 ] != '/' && strlen( Line ) > 5 ) {
					State = 0;
					pPiece = strtok( Line, " " );

					while( pPiece != NULL ) {
						switch( State ) {
						case TEAM:
							snprintf( Team, 2, "%s", pPiece );

							break;

						case ORIGIN_X:
							Origin[ 0 ] = atof( pPiece );

							break;

						case ORIGIN_Y:
							Origin[ 1 ] = atof( pPiece );

							break;

						case ORIGIN_Z:
							Origin[ 2 ] = atof( pPiece );

							break;

						case ANGLES_X:
							Angles[ 0 ] = atof( pPiece );

							break;

						case ANGLES_Y:
							Angles[ 1 ] = atof( pPiece );

							break;

						case ANGLES_Z:
							Angles[ 2 ] = atof( pPiece );

							break;
						}

						State++;

						pPiece = strtok( NULL, " " );
					}

					edict_t * pEntity = CREATE_NAMED_ENTITY( Team[ 0 ] == 'T' ? CStrings.tspawn : CStrings.ctspawn );

					if( !FNullEnt( pEntity ) ) {
						( pEntity ) ->v.origin = Origin;
						( pEntity ) -> v.angles = Angles;
						( pEntity ) -> v.iuser4 = Spawn_Flag;
					}
				}
			}

			fclose( pFile );

			CMiscs.spawns = true;
		}

		CMiscs.prepared = true;
	}

	RETURN_META_VALUE( MRES_IGNORED, 0 );
}

void DispatchThink( edict_t * pEntity ) {
	if( pEntity -> v.iuser4 == Mine_Flag )
		MF_ExecuteForward( CForwards.mine, static_cast < cell > ( EDICT_TO_INT( pEntity ) ), static_cast < cell > ( ( int ) pEntity -> v.health ) );

	RETURN_META( MRES_IGNORED );
}

void DispatchTouch( edict_t * pTouched, edict_t * pToucher ) {
	const char * pTouchedClass = STRING( pTouched -> v.classname );

	if( !strcmp( pTouchedClass, "Rocket" ) ) {
		MF_ExecuteForward( CForwards.rocket, static_cast < cell > ( EDICT_TO_INT( pTouched -> v.owner ) ), static_cast < cell > ( EDICT_TO_INT( pTouched ) ) );

		if( !strcmp( "func_breakable", STRING( pToucher -> v.classname ) ) )
			MDLL_Use( pToucher, pTouched );

		for( int i = 0; i < 4; i++ ) {
			MESSAGE_BEGIN( MSG_BROADCAST, SVC_TEMPENTITY );
			WRITE_BYTE( TE_EXPLOSION );
			WRITE_COORD( pTouched -> v.origin.x + RANDOM_LONG( -22, 22 ) );
			WRITE_COORD( pTouched -> v.origin.y + RANDOM_LONG( -22, 22 ) );
			WRITE_COORD( pTouched -> v.origin.z + RANDOM_LONG( -22, 22 ) );
			WRITE_SHORT( CSprites.explode );
			WRITE_BYTE( RANDOM_LONG( 15, 25 ) );
			WRITE_BYTE( 15 );
			WRITE_BYTE( 0 );
			MESSAGE_END( );
		}

		for( int i = 0; i < 4; i++ ) {
			MESSAGE_BEGIN( MSG_BROADCAST, SVC_TEMPENTITY );
			WRITE_BYTE( TE_BEAMCYLINDER );
			WRITE_COORD( pTouched -> v.origin.x );
			WRITE_COORD( pTouched -> v.origin.y );
			WRITE_COORD( pTouched -> v.origin.z );
			WRITE_COORD( pTouched -> v.origin.x );
			WRITE_COORD( pTouched -> v.origin.y );
			WRITE_COORD( pTouched -> v.origin.z + ( 450 + ( i * 100 ) ) );
			WRITE_SHORT( CSprites.shockwave );
			WRITE_BYTE( 0 );
			WRITE_BYTE( 0 );
			WRITE_BYTE( 4 );
			WRITE_BYTE( i * 40 );
			WRITE_BYTE( 0 );
			WRITE_BYTE( 121 );
			WRITE_BYTE( 121 );
			WRITE_BYTE( 121 );
			WRITE_BYTE( RANDOM_LONG( 150, 240 ) );
			WRITE_BYTE( 0 );
			MESSAGE_END( );
		}

		REMOVE_ENTITY( pTouched );
	}

	else if( !strcmp( pTouchedClass, "Jetpack" ) ) {
		if( EDICT_TO_INT( pToucher ) > gpGlobals -> maxClients || EDICT_TO_INT( pToucher ) < 1 || !MF_IsPlayerAlive( EDICT_TO_INT( pToucher ) ) || CPlayers[ EDICT_TO_INT( pToucher ) ].jetpack || CPlayers[ EDICT_TO_INT( pToucher ) ].zombie )
			RETURN_META( MRES_SUPERCEDE );

		if( CPlayers[ EDICT_TO_INT( pToucher ) ].fuel < 2 )
			CPlayers[ EDICT_TO_INT( pToucher ) ].fuel = 250;

		CPlayers[ EDICT_TO_INT( pToucher ) ].jetpack = true;

		CLIENT_COMMAND( pToucher, "weapon_knife\n" );

		SetJetpack( pToucher );

		g_engfuncs.pfnEmitSound( pToucher, CHAN_ITEM, "zombie_plague/gun_pickup.wav", VOL_NORM, ATTN_NORM, 0, PITCH_NORM );

		REMOVE_ENTITY( pTouched );
	}

	RETURN_META( MRES_IGNORED );
}

void ClientDisconnect( edict_t * pEntity ) {
	if( CPlayers[ EDICT_TO_INT( pEntity ) ].jetpack )
		DropJetpack( EDICT_TO_INT( pEntity ), true );

	CPlayers[ EDICT_TO_INT( pEntity ) ].frozen = false;
	CPlayers[ EDICT_TO_INT( pEntity ) ].fuel = 0;
	CPlayers[ EDICT_TO_INT( pEntity ) ].rocket = 0;
	CPlayers[ EDICT_TO_INT( pEntity ) ].monster = false;
	CPlayers[ EDICT_TO_INT( pEntity ) ].zombie = false;
	

	RETURN_META( MRES_IGNORED );
}

void ServerDeactivate( void ) {
	CMiscs.prepared = false;
	CMiscs.spawns = false;

	for( int i = 1; i <= gpGlobals -> maxClients; i++ ) {
		CPlayers[ i ].frozen = false;
		CPlayers[ i ].fuel = 0;
		CPlayers[ i ].rocket = 0;
		CPlayers[ i ].jetpack = false;
		CPlayers[ i ].monster = false;
		CPlayers[ i ].zombie = false;
	}

	RETURN_META( MRES_IGNORED );
}

void ClientPutInServer( edict_t * pEntity ) 
{
	CPlayers[ EDICT_TO_INT( pEntity ) ].frozen = false;
	CPlayers[ EDICT_TO_INT( pEntity ) ].fuel = 0;
	CPlayers[ EDICT_TO_INT( pEntity ) ].rocket = 0;
	CPlayers[ EDICT_TO_INT( pEntity ) ].jetpack = false;
	CPlayers[ EDICT_TO_INT( pEntity ) ].monster = false;
	CPlayers[ EDICT_TO_INT( pEntity ) ].zombie = false;

	RETURN_META( MRES_IGNORED );
}

void PlayerPreThink( edict_t * pPlayer ) {
	if( CPlayers[ EDICT_TO_INT( pPlayer ) ].frozen )
		pPlayer -> v.velocity = Vector( 0, 0, 0 );


	else if( CPlayers[ EDICT_TO_INT( pPlayer ) ].jetpack ) {
		int Button = pPlayer -> v.button;
		float Time = gpGlobals -> time;

		if( Button & IN_ATTACK2 && CPlayers[ EDICT_TO_INT( pPlayer ) ].rocket < Time ) {
			edict_t * pEntity = CREATE_NAMED_ENTITY( CStrings.infotarget );

			if( !FNullEnt( pEntity ) ) {
				SET_MODEL( pEntity, "models/rpgrocket.mdl" );

				pEntity -> v.classname = CStrings.rocket;
				pEntity -> v.movetype = MOVETYPE_FLY;
				pEntity -> v.solid = SOLID_BBOX;
				pEntity -> v.effects = EF_LIGHT;

				MAKE_VECTORS( pPlayer -> v.v_angle );

				Vector Forward = gpGlobals -> v_forward * 64;
				Vector Velocity = gpGlobals -> v_forward * 1750;
				Vector Origin = pPlayer -> v.origin;

				Origin.x += Forward.x, Origin.y += Forward.y;

				SET_ORIGIN( pEntity, Origin );

				pEntity -> v.velocity = Velocity;

				Vector Angles;
				VEC_TO_ANGLES( Velocity, Angles );

				pEntity -> v.angles = Angles, pEntity -> v.owner = pPlayer;

				MESSAGE_BEGIN( MSG_BROADCAST, SVC_TEMPENTITY );
				WRITE_BYTE( TE_BEAMFOLLOW );
				WRITE_SHORT( EDICT_TO_INT( pEntity ) );
				WRITE_SHORT( CSprites.trail );
				WRITE_BYTE( 25 );
				WRITE_BYTE( 5 );
				WRITE_BYTE( 191 );
				WRITE_BYTE( 191 );
				WRITE_BYTE( 191 );
				WRITE_BYTE( RANDOM_LONG( 150, 240 ) );
				MESSAGE_END( );

				g_engfuncs.pfnEmitSound( pPlayer, CHAN_WEAPON, "zombie_plague/rocket_fire.wav", VOL_NORM, ATTN_NORM, 0, PITCH_NORM );

				CPlayers[ EDICT_TO_INT( pPlayer ) ].rocket = Time + 15;
			}

			else
				CPlayers[ EDICT_TO_INT( pPlayer ) ].rocket = Time + 1;
		}

		if( Button & IN_DUCK && Button & IN_JUMP && !( pPlayer -> v.flags & FL_ONGROUND ) && CPlayers[ EDICT_TO_INT( pPlayer ) ].fuel > 0 ) {
			Vector Velocity = pPlayer -> v.velocity;
			Vector Angles = pPlayer -> v.angles;

			Angles.z = 0;

			Vector Forward;
			g_engfuncs.pfnAngleVectors( Angles, Forward, 0, 0 );

			Angles = Forward;

			Angles.x *= 300, Angles.y *= 300;

			Velocity.x = Angles.x, Velocity.y = Angles.y;

			if( Velocity.z < 300 )
				Velocity.z += 35;

			pPlayer -> v.velocity = Velocity;

			MESSAGE_BEGIN( MSG_BROADCAST, SVC_TEMPENTITY );
			WRITE_BYTE( TE_SPRITE );
			WRITE_COORD( pPlayer -> v.origin.x );
			WRITE_COORD( pPlayer -> v.origin.y );
			WRITE_COORD( pPlayer -> v.origin.z );
			WRITE_SHORT( CSprites.fire );
			WRITE_BYTE( 8 );
			WRITE_BYTE( 25 );
			MESSAGE_END( );

			if( CPlayers[ EDICT_TO_INT( pPlayer ) ].fuel > 80 )
				g_engfuncs.pfnEmitSound( pPlayer, CHAN_ITEM, "zombie_plague/jetpack_fly.wav", VOL_NORM, ATTN_NORM, 0, PITCH_NORM );

			else
				g_engfuncs.pfnEmitSound( pPlayer, CHAN_ITEM, "zombie_plague/jetpack_blow.wav", VOL_NORM, ATTN_NORM, 0, PITCH_NORM );

			CPlayers[ EDICT_TO_INT( pPlayer ) ].fuel -= 1;
		}

		else if( !( Button & IN_DUCK ) && !( Button & IN_JUMP ) && CPlayers[ EDICT_TO_INT( pPlayer ) ].fuel < 250 )
			CPlayers[ EDICT_TO_INT( pPlayer ) ].fuel += 0.5;
	}

	if( pPlayer -> v.button & IN_USE && pPlayer -> v.velocity.z < 0 )
		pPlayer -> v.velocity.z = -100;

	RETURN_META( MRES_IGNORED );
}

void EmitSound( edict_t * pEntity, int Channel, const char * pSound, float Volume, float Attenuation, int Flags, int Pitch ) {
	if( EDICT_TO_INT( pEntity ) > 0 && EDICT_TO_INT( pEntity ) <= gpGlobals -> maxClients ) {
		if( CPlayers[ EDICT_TO_INT( pEntity ) ].jetpack && pSound[ 0 ] == 'w' && pSound[ 8 ] == 'k' )
			RETURN_META( MRES_SUPERCEDE );

		else if( CPlayers[ EDICT_TO_INT( pEntity ) ].zombie ) {
			if( pSound[ 7 ] == 'b' && pSound[ 8 ] == 'h' && pSound[ 9 ] == 'i' ) {
				if( CPlayers[ EDICT_TO_INT( pEntity ) ].monster )
					g_engfuncs.pfnEmitSound( pEntity, Channel, g_pMonsterHitSounds[ RANDOM_LONG( 0, 2 ) ], Volume, Attenuation, Flags, Pitch );

				else
					g_engfuncs.pfnEmitSound( pEntity, Channel, g_pZombieHitSounds[ RANDOM_LONG( 0, 4 ) ], Volume, Attenuation, Flags, Pitch );

				RETURN_META( MRES_SUPERCEDE );
			}

			else if( pSound[ 7 ] == 'd' && ( ( pSound[ 8 ] == 'i' && pSound[ 9 ] == 'e' ) || ( pSound[ 8 ] == 'e' && pSound[ 9 ] == 'a' ) ) ) {
				g_engfuncs.pfnEmitSound( pEntity, Channel, g_pZombieDieSounds[ RANDOM_LONG( 0, 4 ) ], Volume, Attenuation, Flags, Pitch );

				RETURN_META( MRES_SUPERCEDE );
			}
		}
	}

	RETURN_META( MRES_IGNORED );
}

void MessageBegin_Post( int, int Type, const float *, edict_t * ) {
	if( Type == CMessages.deathmsg && gpGlobals -> time > 6 ) {
		CMessages.message = DeathMsg;

		CMessages.byte = 0;
	}

	else if( Type == CMessages.hltv && gpGlobals -> time > 6 ) {
		CMessages.message = HLTV;

		CMessages.byte = 0;
	}

	RETURN_META( MRES_IGNORED );
}

void MessageEnd_Post( void ) {
	if( CMessages.message )
		CMessages.message = 0;

	RETURN_META( MRES_IGNORED );
}

void WriteByte_Post( int Byte ) {
	if( CMessages.message ) {
		switch( CMessages.message ) {
		case DeathMsg:
			if( ++CMessages.byte == 2 && CPlayers[ Byte ].jetpack )
				DropJetpack( Byte, true );

			break;

		case HLTV:
			switch( ++CMessages.byte ) {
			case 1:
				CMessages.status = Byte;

				break;

			case 2:
				if( !CMessages.status && !Byte ) {
					edict_t * pEntity = NULL;

					while( !FNullEnt( ( pEntity = FIND_ENTITY_BY_STRING( pEntity, "classname", "Rocket" ) ) ) )
						REMOVE_ENTITY( pEntity );

					pEntity = NULL;

					while( !FNullEnt( ( pEntity = FIND_ENTITY_BY_STRING( pEntity, "classname", "Jetpack" ) ) ) )
						REMOVE_ENTITY( pEntity );

					pEntity = NULL;

					while( !FNullEnt( ( pEntity = FIND_ENTITY_BY_STRING( pEntity, "classname", "Mine" ) ) ) )
						REMOVE_ENTITY( pEntity );
				}

				break;
			}

			break;
		}
	}

	RETURN_META( MRES_IGNORED );
}

int RegUserMsg_Post( const char * pName, int ) {
	if( !strcmp( pName, "DeathMsg" ) )
		CMessages.deathmsg = META_RESULT_ORIG_RET( int );

	else if( !strcmp( pName, "HLTV" ) )
		CMessages.hltv = META_RESULT_ORIG_RET( int );

	else if( !strcmp( pName, "ScoreAttrib" ) )
		CMessages.scoreattrib = META_RESULT_ORIG_RET( int );

	else if( !strcmp( pName, "ScoreInfo" ) )
		CMessages.scoreinfo = META_RESULT_ORIG_RET( int );

	else if( !strcmp( pName, "TeamInfo" ) )
		CMessages.teaminfo = META_RESULT_ORIG_RET( int );

	else if( !strcmp( pName, "SayText" ) )
		CMessages.saytext = META_RESULT_ORIG_RET( int );

	else if( !strcmp( pName, "ScreenFade" ) )
		CMessages.screenfade = META_RESULT_ORIG_RET( int );

	else if( !strcmp( pName, "ScreenShake" ) )
		CMessages.screenshake = META_RESULT_ORIG_RET( int );

	else if( !strcmp( pName, "Crosshair" ) )
		CMessages.crosshair = META_RESULT_ORIG_RET( int );

	else if( !strcmp( pName, "HideWeapon" ) )
		CMessages.hideweapon = META_RESULT_ORIG_RET( int );

	else if( !strcmp( pName, "Damage" ) )
		CMessages.damage = META_RESULT_ORIG_RET( int );

	else if( !strcmp( pName, "BarTime" ) )
		CMessages.bartime = META_RESULT_ORIG_RET( int );
	
	else if( !strcmp( pName, "SetFOV" ) )
		CMessages.fov = META_RESULT_ORIG_RET( int );	

	RETURN_META_VALUE( MRES_IGNORED, 0 );
}

void SetModel_Post( edict_t * pEntity, const char * pModel )
{
	if( !strcmp( STRING( pEntity -> v.classname ), "weaponbox" ) )
		pEntity -> v.nextthink = gpGlobals -> time + 0.025;

	else if( pEntity -> v.owner && pModel[ 7 ] == 'w' && pModel[ 8 ] == '_' )
	{
		int Weapon = 0;

		switch( pModel[ 9 ] ) {
		case 'h':
			Weapon = CSW_HEGRENADE;

			break;

		case 'f':
			if( pModel[ 10 ] == 'l' )
				Weapon = CSW_FLASHBANG;

			break;

		case 's':
			if( pModel[ 10 ] == 'm' )
				Weapon = CSW_SMOKEGRENADE;

			break;
		}

		if( Weapon )
		{
			MF_ExecuteForward( CForwards.grenade, static_cast < cell > ( EDICT_TO_INT( pEntity -> v.owner ) ), static_cast < cell > ( EDICT_TO_INT( pEntity ) ), static_cast < cell > ( Weapon ) );
		}
	}

	RETURN_META( MRES_IGNORED );
}

void ClientKill( edict_t * )
{
	RETURN_META( MRES_SUPERCEDE );
}
