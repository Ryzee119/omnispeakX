#ifndef CK_DEF_H
#define CK_DEF_H

#include <stdbool.h>

#include "ck_phys.h"
#include "id_heads.h"

#define CK_MAX_OBJECTS 100

struct CK_object;

typedef struct RF_SpriteDrawEntry RF_SpriteDrawEntry;

typedef enum CK_Difficulty
{
	D_NotPlaying,
	D_Easy,
	D_Normal,
	D_Hard
} CK_Difficulty;

typedef struct CK_GameState
{
	CK_Difficulty difficulty;		// Difficulty level of current game
	int levelState;				// Level State (should probably be enum)
						// Values:
						// 0 - In Level
						// 1 - Keen Died
						// 2 - Level Completed
						// 3 - Rescued Council Member (Keen 4)
						// 4 - About to Record Demo
						// 5 - ???
						// 6 - ???
						// 15 - Destroyed QED (Keen 5)
	int numShots;

	bool jumpCheat;				// Is the jump cheat enabled?
} CK_GameState;

extern CK_GameState ck_gameState;

typedef enum CK_ActionType
{
	AT_UnscaledOnce,			// Unscaled Motion, Thinks once.
	AT_ScaledOnce,				// Scaled Motion, Thinks once.
	AT_Frame,					// No Motion, Thinks each frame (doesn't advance action)
	AT_UnscaledFrame,			// Unscaled Motion, Thinks each frame
	AT_ScaledFrame				// Scaled Motion, Thinks each frame
} CK_ActionType;

typedef struct CK_action
{
	int chunkLeft;
	int chunkRight;
	CK_ActionType type;
	//int unknown1, unknown2;
	int protectAnimation, stickToGround; 	// See KeenWiki: Galaxy Action Parameters (lemm/levelass)
	int timer;
	int velX, velY;
	void (*think)(struct CK_object *obj);
	void (*collide)(struct CK_object *obj, struct CK_object *other);
	void (*draw)(struct CK_object *obj);
	struct CK_action *next;
} CK_action;

typedef struct CK_object
{
	int type;
	bool active;
	bool visible;
	bool clipped;
	int timeUntillThink;
	int posX;
	int posY;
	int xDirection;
	int yDirection;
	int deltaPosX;
	int deltaPosY;
	int velX;
	int velY;
	int actionTimer;
	CK_action *currentAction;
	int gfxChunk;
	int zLayer;

	CK_objPhysData clipRects;

	CK_objPhysData oldRects;

	int nextX;
	int nextY;

	//TileInfo for surrounding tiles.
	int topTI, bottomTI, leftTI, rightTI;


	RF_SpriteDrawEntry *sde;

	int user1;
	int user2;
	int user3;
	int user4;

	struct CK_object *next;
	struct CK_object *prev;
} CK_object;

typedef struct CK_keenState
{
	int jumpTimer;
	int poleGrabTime;
	bool jumpIsPressed;
	bool jumpWasPressed;
	bool pogoIsPressed;
	bool pogoWasPressed;
	bool shootIsPressed;
	bool shootWasPressed;

	CK_object *platform;
} CK_keenState;

extern CK_keenState ck_keenState;
extern IN_ControlFrame ck_inputFrame;
extern int ck_currentMapNumber;
void CK_SpawnKeen(int tileX, int tileY, int direction);

void CK_HandleDemoKeys();
void CK_KeenRidePlatform(CK_object *obj);
void CK_KeenSetupFunctions();

#endif
