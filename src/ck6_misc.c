/*
Omnispeak: A Commander Keen Reimplementation
Copyright (C) 2012 David Gow <david@ingeniumdigital.com>

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#include <stdio.h>
#include "id_ca.h"
#include "id_fs.h"
#include "id_in.h"
#include "id_rf.h"
#include "id_vl.h"
#include "ck_act.h"
#include "ck_def.h"
#include "ck_game.h"
#include "ck_phys.h"
#include "ck_play.h"
#include "ck6_ep.h"

CK_EpisodeDef *ck6_episode;

CK_EpisodeDef ck6v14e_episode = {
	EP_CK6,
	"CK6",
	&CK6_SetupFunctions,
	&CK6_ScanInfoLayer,
	&CK6_DefineConstants,
	&CK6_MapMiscFlagsCheck,
	&CK6_IsPresent,
	/* .lastLevelToMarkAsDone = */ 16,
	// Note these offsets are for version 1.4
	/* .objArrayOffset = */ 0xA995,
	/* .tempObjOffset = */ 0xC761,
	/* .spriteArrayOffset = */ 0xD7EC,
	/* .printXOffset = */ 0xA6C5,
	/* .animTilesOffset = */ 0xDF78,
	/* .animTileSize = */ 10,
	/* .hasCreatureQuestion = */ true,
};

CK_EpisodeDef ck6v15e_episode = {
	EP_CK6,
	"CK6",
	&CK6_SetupFunctions,
	&CK6_ScanInfoLayer,
	&CK6_DefineConstants,
	&CK6_MapMiscFlagsCheck,
	&CK6_IsPresent,
	/* .lastLevelToMarkAsDone = */ 16,
	// Note these offsets are for version 1.5
	/* .objArrayOffset = */ 0x75CE,
	/* .tempObjOffset = */ 0x939E,
	/* .spriteArrayOffset = */ 0xCEA2,
	/* .printXOffset = */ 0xE81A,
	/* .animTilesOffset = */ 0xD62E,
	/* .animTileSize = */ 10,
	/* .hasCreatureQuestion = */ true,
};

void CK6_SetupFunctions()
{
	CK6_Obj1_SetupFunctions();
	CK6_Obj2_SetupFunctions();
	CK6_Obj3_SetupFunctions();
	CK6_Map_SetupFunctions();
}

// Check if all the game files are present.
bool CK6_IsPresent()
{
	// User-provided files
	if (!FS_IsKeenFilePresent("EGAGRAPH.CK6"))
		return false;
	if (!FS_IsKeenFilePresent("GAMEMAPS.CK6"))
		return false;
	if (!FS_IsKeenFilePresent("AUDIO.CK6"))
		return false;

	FS_File egaGraph = FS_OpenKeenFile("EGAGRAPH.CK6");
	size_t egaGraphSize = FS_GetFileSize(egaGraph);
	FS_CloseFile(egaGraph);
	if (egaGraphSize == 464662)
		ck6_episode = &ck6v15e_episode;
	else
		ck6_episode = &ck6v14e_episode;

	// Omnispeak-provided files
	if (!FS_IsOmniFilePresent("EGAHEAD.CK6"))
		return false;
	if (!FS_IsOmniFilePresent("EGADICT.CK6"))
		return false;
	if (!FS_IsOmniFilePresent("GFXINFOE.CK6"))
		return false;
	if (!FS_IsOmniFilePresent("MAPHEAD.CK6"))
		return false;
	// Map header file may include the tile info
	//if (!FS_IsOmniFilePresent("TILEINFO.CK6"))
	//	return false;
	if (!FS_IsOmniFilePresent("AUDIODCT.CK6"))
		return false;
	if (!FS_IsOmniFilePresent("AUDIOHHD.CK6"))
		return false;
	if (!FS_IsOmniFilePresent("AUDINFOE.CK6"))
		return false;

	if (!FS_IsOmniFilePresent("ACTION.CK6"))
		return false;

	// We clearly have all of the required files.
	return true;
}

// ck_inter.c
uint8_t ck6_starWarsPalette[] = {
	0x00, 0x01, 0x18, 0x19, 0x04, 0x1C, 0x06, 0x07,
	0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x00};

uint8_t ck6_terminator_palette1[] = {0, 0x05, 0x05, 0x15, 1, 1, 1, 1, 0x11, 0x11, 0x11, 0x11, 0x13, 0x13, 0x13, 0x13, 0};
uint8_t ck6_terminator_palette2[] = {0, 0x05, 0x05, 0x15, 1, 1, 1, 1, 0x11, 0x11, 0x11, 0x11, 0x13, 0x13, 0x13, 0x05, 0};

// ck_keen.c

soundnames ck6_itemSounds[] = {19, 19, 19, 19, 8, 8, 8, 8, 8, 8, 17, 9};

// ck_play.c
int16_t ck6_levelMusic[] = {5, 3, 1, 8, 8, 8, 7, 2, 7, 1, 3, 2, 1, 4, 4, 6, 2, 0, 0, 0};

void CK6_DefineConstants(void)
{
	// We can't remove these constants for now, as pointers to them are
	// compiled in.
	PIC_MENUCARD = CK_CHUNKNUM(PIC_MENUCARD);
	PIC_NEWGAMECARD = CK_CHUNKNUM(PIC_NEWGAMECARD);
	PIC_LOADCARD = CK_CHUNKNUM(PIC_LOADCARD);
	PIC_SAVECARD = CK_CHUNKNUM(PIC_SAVECARD);
	PIC_CONFIGURECARD = CK_CHUNKNUM(PIC_CONFIGURECARD);
	PIC_SOUNDCARD = CK_CHUNKNUM(PIC_SOUNDCARD);
	PIC_MUSICCARD = CK_CHUNKNUM(PIC_MUSICCARD);
	PIC_KEYBOARDCARD = CK_CHUNKNUM(PIC_KEYBOARDCARD);
	PIC_MOVEMENTCARD = CK_CHUNKNUM(PIC_MOVEMENTCARD);
	PIC_BUTTONSCARD = CK_CHUNKNUM(PIC_BUTTONSCARD);
	PIC_JOYSTICKCARD = CK_CHUNKNUM(PIC_JOYSTICKCARD);
	PIC_OPTIONSCARD = CK_CHUNKNUM(PIC_OPTIONSCARD);
	PIC_PADDLEWAR = CK_CHUNKNUM(PIC_PADDLEWAR);
	PIC_DEBUGCARD = CK_CHUNKNUM(PIC_DEBUGCARD);

	// Terminator credit chunks also need setting up here.
	PIC_CREDIT1 = CK_CHUNKNUM(PIC_CREDIT1);
	PIC_CREDIT2 = CK_CHUNKNUM(PIC_CREDIT2);
	PIC_CREDIT3 = CK_CHUNKNUM(PIC_CREDIT3);
	PIC_CREDIT4 = CK_CHUNKNUM(PIC_CREDIT4);


	SPR_GEM_A1 = CK_CHUNKNUM(SPR_GEM_A1);
	SPR_GEM_B1 = CK_CHUNKNUM(SPR_GEM_B1);
	SPR_GEM_C1 = CK_CHUNKNUM(SPR_GEM_C1);
	SPR_GEM_D1 = CK_CHUNKNUM(SPR_GEM_D1);
	SPR_100_PTS1 = CK_CHUNKNUM(SPR_100_PTS1);
	SPR_200_PTS1 = CK_CHUNKNUM(SPR_200_PTS1);
	SPR_500_PTS1 = CK_CHUNKNUM(SPR_500_PTS1);
	SPR_1000_PTS1 = CK_CHUNKNUM(SPR_1000_PTS1);
	SPR_2000_PTS1 = CK_CHUNKNUM(SPR_2000_PTS1);
	SPR_5000_PTS1 = CK_CHUNKNUM(SPR_5000_PTS1);
	SPR_1UP1 = CK_CHUNKNUM(SPR_1UP1);
	SPR_STUNNER1 = CK_CHUNKNUM(SPR_STUNNER1);

	TEXT_END = CK_CHUNKNUM(TEXT_END);


	// ck_inter.c
	ck_starWarsPalette = ck6_starWarsPalette;
	ck_terminator_palette1 = ck6_terminator_palette1;
	ck_terminator_palette2 = ck6_terminator_palette2;

	// ck_keen.c
	ck_itemSounds = ck6_itemSounds;

	// ck_play.c
	ck_levelMusic = ck6_levelMusic;
}

/*
 * Spawn an enemy projectile
 * Note that the behaviour is slightly different from DOS Keen
 * DOS Keen SpawnEnemyShot returns 0 if shot is spawned, or -1 otherwise
 * omnispeak CK6_SpawnEnemyShot returns pointer if succesful, NULL otherwise
 */

#if 0
CK_object *CK6_SpawnEnemyShot(int posX, int posY, CK_action *action)
{
	CK_object *new_object = CK_GetNewObj(true);

	if (!new_object)
		return NULL;

	new_object->posX = posX;
	new_object->posY = posY;
	new_object->type = CT6_EnemyShot;
	new_object->active = OBJ_EXISTS_ONLY_ONSCREEN;
	CK_SetAction(new_object, action);

	if (CK_NotStuckInWall(new_object))
	{
		return new_object;
	}
	else
	{
		CK_RemoveObj(new_object);
		return NULL;
	}
}
#endif

void CK6_SpawnAxisPlatform(int tileX, int tileY, int direction, bool purple)
{
	CK_object *obj = CK_GetNewObj(false);

	obj->type = 6;
	obj->active = OBJ_ALWAYS_ACTIVE;
	obj->zLayer = 0;
	obj->posX = RF_TileToUnit(tileX);
	obj->posY = RF_TileToUnit(tileY);

	switch (direction)
	{
	case 0:
		obj->xDirection = 0;
		obj->yDirection = -1;
		break;
	case 1:
		obj->xDirection = 1;
		obj->yDirection = 0;
		break;
	case 2:
		obj->xDirection = 0;
		obj->yDirection = 1;
		break;
	case 3:
		obj->xDirection = -1;
		obj->yDirection = 0;
		break;
	}

	if (purple)
	{
		obj->posX += 0x40;
		obj->posY += 0x40;
		CK_SetAction(obj, CK_GetActionByName("CK6_ACT_purpleAxisPlatform"));
	}
	else
	{

		CK_SetAction(obj, CK_GetActionByName("CK_ACT_AxisPlatform"));
	}
	// TODO: These should *not* be done here.
	obj->gfxChunk = obj->currentAction->chunkLeft;
	CA_CacheGrChunk(obj->gfxChunk);
	CK_ResetClipRects(obj);
}

void CK6_SpawnRedStandPlatform(int tileX, int tileY)
{
	CK_object *obj = CK_GetNewObj(false);

	obj->type = 6;
	obj->active = OBJ_ACTIVE;
	obj->zLayer = 0;
	obj->posX = RF_TileToUnit(tileX);
	obj->posY = obj->user1 = RF_TileToUnit(tileY);
	obj->xDirection = 0;
	obj->yDirection = 1;
	obj->clipped = CLIP_not;
	CK_SetAction(obj, CK_GetActionByName("CK6_ACT_redStandPlatform"));
	obj->gfxChunk = obj->currentAction->chunkLeft;
	CA_CacheGrChunk(obj->gfxChunk);
	CK_ResetClipRects(obj);
}

#define MAXLUMPS 0x28
static bool ck6_lumpsNeeded[MAXLUMPS];

typedef enum
{
	Lump_0,
	Lump_Keen,
	Lump_100Pts,
	Lump_200Pts,
	Lump_500Pts,
	Lump_1000Pts,
	Lump_2000Pts,
	Lump_5000Pts,
	Lump_1UP,
	Lump_Gems,
	Lump_Stunner,
	Lump_Mapkeen,
	Lump_12,
	Lump_Bloog,
	Lump_BloogletR,
	Lump_BloogletY,
	Lump_BloogletB,
	Lump_BloogletG,
	Lump_Platform,
	Lump_Gik,
	Lump_Blorb,
	Lump_Bobba,
	Lump_Babobba,
	Lump_Bloogguard,
	Lump_Flect,
	Lump_Bip,
	Lump_PlatBip,
	Lump_Bipship,
	Lump_Nospike,
	Lump_Orbatrix,
	Lump_Ceilick,
	Lump_Fleex,
	Lump_Rope,
	Lump_Sandwich,
	Lump_Turret,
	Lump_Passcard,
	Lump_Molly,
} CK6_LumpType;

static int16_t ck6_itemLumps[] =
	{
		Lump_Gems,
		Lump_Gems,
		Lump_Gems,
		Lump_Gems,
		Lump_100Pts,
		Lump_200Pts,
		Lump_500Pts,
		Lump_1000Pts,
		Lump_2000Pts,
		Lump_5000Pts,
		Lump_1UP,
		Lump_Stunner,
};

static int16_t ck6_lumpStarts[MAXLUMPS] =
	{
		11,
		52,
		150,
		152,
		154,
		156,
		158,
		160,
		162,
		164,
		173,
		184,
		0,
		342,
		351,
		360,
		369,
		378,
		424,
		387,
		399,
		402,
		285,
		254,
		317,
		414,
		423,
		269,
		298,
		329,
		246,
		239,
		183,
		182,
		176,
		435,
		433,
		0,
		0,
		0,
};

static int16_t ck6_lumpEnds[MAXLUMPS] =
	{
		26,
		149,
		151,
		153,
		155,
		157,
		159,
		161,
		163,
		172,
		174,
		238,
		0,
		350,
		359,
		368,
		377,
		386,
		432,
		398,
		401,
		413,
		297,
		268,
		328,
		422,
		423,
		284,
		316,
		341,
		253,
		245,
		183,
		182,
		181,
		435,
		434,
		0,
		0,
		0,
};

// TODO: Cache stuff here instead of spawner handlers
void CK6_ScanInfoLayer()
{

	//TODO: Work out where to store current map number, etc.
	int mapW = CA_MapHeaders[ca_mapOn]->width;
	int mapH = CA_MapHeaders[ca_mapOn]->height;

	for (int y = 0; y < mapH; ++y)
	{
		for (int x = 0; x < mapW; ++x)
		{
			int infoValue = CA_TileAtPos(x, y, 2);
			switch (infoValue)
			{
			case 1:
				CK_SpawnKeen(x, y, 1);
				CK_DemoSignSpawn();
				ca_graphChunkNeeded[175] |= ca_levelbit;
				ck6_lumpsNeeded[Lump_Keen] = true;
				break;
			case 2:
				CK_SpawnKeen(x, y, -1);
				CK_DemoSignSpawn();
				ca_graphChunkNeeded[175] |= ca_levelbit;
				ck6_lumpsNeeded[Lump_Keen] = true;
				break;

			case 3:
				CK_DemoSignSpawn();
				ca_graphChunkNeeded[175] |= ca_levelbit;
				CK_SpawnMapKeen(x, y);
				ck6_lumpsNeeded[Lump_Mapkeen] = true;
				break;

			// Bloogs
			case 6:
				if (ck_gameState.difficulty < D_Hard)
					break;
			case 5:
				if (ck_gameState.difficulty < D_Normal)
					break;
			case 4:
				ck6_lumpsNeeded[Lump_Bloog] = true;
				CK6_SpawnBloog(x, y);
				break;

			case 7:
			case 8:
			case 9:
			case 10:
			case 11:
			case 12:
			case 13:
			case 14:
			{
				int color = (infoValue - 7) % 4;
				ck6_lumpsNeeded[Lump_BloogletR + color] = true;
				CK6_SpawnBlooglet(x, y, infoValue - 7);
				break;
			}

			case 15:
			case 16:
				CK6_SpawnMapCliff(x, y, infoValue - 15);
				break;

			case 24:
				ck6_lumpsNeeded[Lump_Molly] = true;
				CK6_SpawnMolly(x, y);
				break;

				// Fleex
			case 20:
				if (ck_gameState.difficulty < D_Hard)
					break;
			case 19:
				if (ck_gameState.difficulty < D_Normal)
					break;
			case 18:
				ck6_lumpsNeeded[Lump_Fleex] = true;
				CK6_SpawnFleex(x, y);
				break;

			case 25:
				RF_SetScrollBlock(x, y, true);
				break;
			case 26:
				RF_SetScrollBlock(x, y, false);
				break;

				// Platforms
			case 27:
			case 28:
			case 29:
			case 30:
				CK_SpawnAxisPlatform(x, y, infoValue - 27, false);
				ck6_lumpsNeeded[Lump_Platform] = true;
				break;
			case 32:
				CK_SpawnFallPlat(x, y);
				ck6_lumpsNeeded[Lump_Platform] = true;
				break;

			case 33:
				if (ck_gameState.difficulty > D_Easy)
					break;
			case 34:
				if (ck_gameState.difficulty > D_Normal)
					break;
			case 35:
				CK_SpawnStandPlatform(x, y);
				ck6_lumpsNeeded[Lump_Platform] = true;
				break;

			case 36:
			case 37:
			case 38:
			case 39:
				CK_SpawnGoPlat(x, y, infoValue - 36, false);
				ck6_lumpsNeeded[Lump_Platform] = true;
				ck6_lumpsNeeded[Lump_PlatBip] = true;
				break;
			case 40:
				CK_SneakPlatSpawn(x, y);
				ck6_lumpsNeeded[Lump_Platform] = true;
				break;

			// Bobbas
			case 43:
				if (ck_gameState.difficulty < D_Hard)
					break;
			case 42:
				if (ck_gameState.difficulty < D_Normal)
					break;
			case 41:
				ck6_lumpsNeeded[Lump_Bobba] = true;
				CK6_SpawnBobba(x, y);
				break;

			case 44:
			case 45:
				CK6_SpawnSatelliteLoading(x, y, infoValue - 44);
				break;

			// Nospike
			case 49:
				if (ck_gameState.difficulty < D_Hard)
					break;
			case 48:
				if (ck_gameState.difficulty < D_Normal)
					break;
			case 47:
				ck6_lumpsNeeded[Lump_Nospike] = true;
				CK6_SpawnNospike(x, y);
				break;

			// Gik
			case 52:
				if (ck_gameState.difficulty < D_Hard)
					break;
			case 51:
				if (ck_gameState.difficulty < D_Normal)
					break;
			case 50:
				ck6_lumpsNeeded[Lump_Gik] = true;
				CK6_SpawnGik(x, y);
				break;

				// Turrets
			case 53:
			case 54:
			case 55:
			case 56:
				ck6_lumpsNeeded[Lump_Turret] = true;
				CK_TurretSpawn(x, y, infoValue - 53);
				break;

			case 69:
				// Spawn extra stunner if Keen has low ammo
				if (ck_gameState.numShots >= 5)
					break;
				infoValue = 68;
			case 57:
			case 58:
			case 59:
			case 60:
			case 61:
			case 62:
			case 63:
			case 64:
			case 65:
			case 66:
			case 67:
			case 68:
				CK_SpawnItem(x, y, infoValue - 57);
				ck6_lumpsNeeded[ck6_itemLumps[infoValue - 57]] = true;
				break;

			// Orbatrices
			case 72:
				if (ck_gameState.difficulty < D_Hard)
					break;
			case 71:
				if (ck_gameState.difficulty < D_Normal)
					break;
			case 70:
				ck6_lumpsNeeded[Lump_Orbatrix] = true;
				CK6_SpawnOrbatrix(x, y);
				break;

			// Bip
			case 75:
				if (ck_gameState.difficulty < D_Hard)
					break;
			case 74:
				if (ck_gameState.difficulty < D_Normal)
					break;
			case 73:
				ck6_lumpsNeeded[Lump_Bip] = true;
				ck6_lumpsNeeded[Lump_PlatBip] = true;
				ck6_lumpsNeeded[Lump_Bipship] = true;
				CK6_SpawnBipship(x, y);
				break;

			// Flects
			case 78:
				if (ck_gameState.difficulty < D_Hard)
					break;
			case 77:
				if (ck_gameState.difficulty < D_Normal)
					break;
			case 76:
				ck6_lumpsNeeded[Lump_Flect] = true;
				CK6_SpawnFlect(x, y);
				break;

			// Blorbs
			case 81:
				if (ck_gameState.difficulty < D_Hard)
					break;
			case 80:
				if (ck_gameState.difficulty < D_Normal)
					break;
			case 79:
				ck6_lumpsNeeded[Lump_Blorb] = true;
				CK6_SpawnBlorb(x, y);
				break;

			// Ceilicks
			case 84:
				if (ck_gameState.difficulty < D_Hard)
					break;
			case 83:
				if (ck_gameState.difficulty < D_Normal)
					break;
			case 82:
				ck6_lumpsNeeded[Lump_Ceilick] = true;
				CK6_SpawnCeilick(x, y);
				break;

			// Bloogguards
			case 87:
				if (ck_gameState.difficulty < D_Hard)
					break;
			case 86:
				if (ck_gameState.difficulty < D_Normal)
					break;
			case 85:
				ck6_lumpsNeeded[Lump_Bloogguard] = true;
				CK6_SpawnBloogguard(x, y);
				break;

				// Grabbiter
			case 88:
				CK6_SpawnGrabbiter(x, y);
				break;

			// Satellite
			case 89:
				CK6_SpawnSatellite(x, y);
				break;

			// Story Items
			case 99:
				ck6_lumpsNeeded[Lump_Rope] = true;
				CK6_SpawnRope(x, y);
				break;
			case 100:
				ck6_lumpsNeeded[Lump_Sandwich] = true;
				CK6_SpawnSandwich(x, y);
				break;
			case 101:
				ck6_lumpsNeeded[Lump_Passcard] = true;
				CK6_SpawnPasscard(x, y);
				break;

			// Babobbas
			case 104:
				if (ck_gameState.difficulty < D_Hard)
					break;
			case 103:
				if (ck_gameState.difficulty < D_Normal)
					break;
			case 102:
				ck6_lumpsNeeded[Lump_Babobba] = true;
				CK6_SpawnBabobba(x, y);
				break;

			case 105:
				CK6_SpawnRocket(x, y, infoValue - 105);
				break;
			}
		}
	}

	for (CK_object *obj = ck_keenObj; obj != NULL; obj = obj->next)
	{
		if (obj->active != OBJ_ALWAYS_ACTIVE)
			obj->active = OBJ_INACTIVE;
	}
	// TODO: Some more stuff (including opening elevator after breaking fuses)

	if (ck_gameState.currentLevel == 0)
	{
		//int keenYTilePos = RF_UnitToTile(ck_keenObj->posY);
	}

	for (int i = 0; i < MAXLUMPS; i++)
		if (ck6_lumpsNeeded[i])
			for (int j = ck6_lumpStarts[i]; j <= ck6_lumpEnds[i]; j++)
				CA_CacheGrChunk(j);
}

void CK6_ToggleBigSwitch(CK_object *obj, bool dir)
{

	// Replace switch tiles
	int ty = dir ? obj->clipRects.tileY2 : obj->clipRects.tileY1 - 2;
	int tx = obj->clipRects.tileX1 - 1;

	uint16_t *infoTile = CA_TilePtrAtPos(tx + 1, ty + 1, 2);

	while (!*infoTile)
	{
		tx++;
		infoTile++;
	}

	uint16_t *fgTile = CA_TilePtrAtPos(tx, ty, 1);

	uint16_t tile_array[6];
	for (int y = 0; y < 3; y++)
	{
		for (int x = 0; x < 2; x++)
		{
			tile_array[2 * y + x] = *fgTile + TI_ForeAnimTile(*fgTile);
			fgTile++;
		}

		fgTile += CA_GetMapWidth() - 2;
	}

	RF_ReplaceTiles(tile_array, 1, tx, ty, 2, 3);

	// Apply the switch effect
	infoTile = CA_TilePtrAtPos(tx + 1, ty + 1, 2);
	int destX = *infoTile >> 8;
	int destY = *infoTile & 0xFF;
	SD_PlaySound(CK_SOUNDNUM(SOUND_KEENOUTOFAMMO));

	infoTile = CA_TilePtrAtPos(destX, destY, 2);

	if (*infoTile >= 0x5B && *infoTile < 0x5B + 8)
	{
		// Toggle a goplat arrow
		static uint16_t infoPlaneInverses[8] = {2, 3, 0, 1, 6, 7, 4, 5};
		*infoTile = infoPlaneInverses[(*infoTile - 0x5B)] + 0x5B;
	}
	else
	{
		fgTile = CA_TilePtrAtPos(destX, destY, 1);
		int miscValue = TI_ForeMisc(*fgTile) & 0x7F;

		if (miscValue == MISCFLAG_ACTIVEZAPPER)
		{
			uint16_t start = CA_TileAtPos(0, 0, 1);
			uint16_t mid = CA_TileAtPos(1, 0, 1);
			uint16_t end = CA_TileAtPos(2, 0, 1);

			RF_ReplaceTiles(&start, 1, destX, destY, 1, 1);
			destY++;

			while (TI_ForeMisc(CA_TileAtPos(destX, destY, 1)) == MISCFLAG_DEADLY)
			{
				RF_ReplaceTiles(&mid, 1, destX, destY, 1, 1);
				destY++;
			}

			RF_ReplaceTiles(&end, 1, destX, destY, 1, 1);
		}
		else if (miscValue == MISCFLAG_INACTIVEZAPPER)
		{
			uint16_t start = CA_TileAtPos(3, 0, 1);
			uint16_t mid = CA_TileAtPos(4, 0, 1);
			uint16_t end = CA_TileAtPos(5, 0, 1);

			RF_ReplaceTiles(&start, 1, destX, destY, 1, 1);
			destY++;

			while (TI_ForeMisc(CA_TileAtPos(destX, destY, 1)) != MISCFLAG_INACTIVEZAPPER)
			{
				RF_ReplaceTiles(&mid, 1, destX, destY, 1, 1);
				destY++;
			}

			RF_ReplaceTiles(&end, 1, destX, destY, 1, 1);
		}
		else if (miscValue == MISCFLAG_BRIDGE)
		{
			for (int y = destY; y < destY + 2; ++y)
			{
				for (int x = destX - ((y == destY) ? 0 : 1); x < CA_GetMapWidth(); ++x)
				{
					uint16_t currentTile = CA_TileAtPos(x, y, 1);
					if (!TI_ForeAnimTile(currentTile))
						break;
					uint16_t newTile = currentTile + TI_ForeAnimTile(currentTile);
					RF_ReplaceTiles(&newTile, 1, x, y, 1, 1);
				}
			}
		}
		else
		{
			// Toggle a B block
			*infoTile ^= 0x1F;
		}
	}
}

// Story item dialogues

#define SOUND_STORYITEM 0x2D
void CK6_ShowGetSandwich()
{
	SD_WaitSoundDone();
	SD_PlaySound(CK_SOUNDNUM(SOUND_STORYITEM));
	CA_UpLevel();
	CA_CacheGrChunk(CK_CHUNKNUM(PIC_KEENTALK1));

	US_CenterWindow(26, 8);
	VHB_DrawBitmap(US_GetWindowX() + US_GetWindowW() - 0x30, US_GetWindowY(), CK_CHUNKNUM(PIC_KEENTALK1));
	US_SetWindowW(US_GetWindowW() - 0x30);
	US_SetPrintY(US_GetPrintY() + 12);
	US_CPrint(CK_STRING(ck6_str_getSandwich));
	VL_Present();

	VL_DelayTics(30); // VW_WaitVBL(30);
	IN_ClearKeysDown();
	IN_WaitButton();
	CA_DownLevel();
	ck_gameState.ep.ck6.sandwich = true;
}

void CK6_ShowGetRope()
{
	SD_WaitSoundDone();
	SD_PlaySound(CK_SOUNDNUM(SOUND_STORYITEM));
	CA_UpLevel();
	CA_CacheGrChunk(CK_CHUNKNUM(PIC_KEENTALK1));

	US_CenterWindow(26, 8);
	VHB_DrawBitmap(US_GetWindowX() + US_GetWindowW() - 0x30, US_GetWindowY(), CK_CHUNKNUM(PIC_KEENTALK1));
	US_SetWindowW(US_GetWindowW() - 0x30);
	US_SetPrintY(US_GetPrintY() + 12);
	US_CPrint(CK_STRING(ck6_str_getRope));
	VL_Present();

	VL_DelayTics(30); // VW_WaitVBL(30);
	IN_ClearKeysDown();
	IN_WaitButton();
	CA_DownLevel();
	ck_gameState.ep.ck6.rope = true;
}

void CK6_ShowGetPasscard()
{
	SD_WaitSoundDone();
	SD_PlaySound(CK_SOUNDNUM(SOUND_STORYITEM));
	CA_UpLevel();
	CA_CacheGrChunk(CK_CHUNKNUM(PIC_KEENTALK1));

	US_CenterWindow(26, 8);
	VHB_DrawBitmap(US_GetWindowX() + US_GetWindowW() - 0x30, US_GetWindowY(), CK_CHUNKNUM(PIC_KEENTALK1));
	US_SetWindowW(US_GetWindowW() - 0x30);
	US_SetPrintY(US_GetPrintY() + 4);
	US_CPrint(CK_STRING(ck6_str_getPasscard));
	VL_Present();

	VL_DelayTics(30); // VW_WaitVBL(30);
	IN_ClearKeysDown();
	IN_WaitButton();
	CA_DownLevel();
	ck_gameState.ep.ck6.passcard = true;
}
