#pragma once

#include "config.h"
#include "script.h"

enum PlayerState {
	mLOCAL_PLAYER = 0x00000001,
	mNON_COLLIDABLE = 0x00000002,	// For use, for example, after skater is knocked down so you don't
	// knock him down over and over
	mOBSERVER = 0x00000004,
	mPENDING_PLAYER = 0x00000008,	// Player is observing until the game finishes
	mJUMPING_IN = 0x00000010,	// Transitioning from pending to playing
	mFULLY_IN = 0x00000020,	// Player is fully in the game
	mHAS_PLAYER_INFO = 0x00000040,	// Player has fully received initial player info packet(s)
	mKING_OF_THE_HILL = 0x00000080,	// Player is king of the hill
	mRUN_ENDED = 0x00000100,	// Player's run has ended
	mSERVER_PLAYER = 0x00000200,	// The player acting as host
	mMARKED_PLAYER = 0x00000400,	// The player is marked for an operation
	mHAS_RED_FLAG = 0x00000800,	// The player has the red flag
	mHAS_BLUE_FLAG = 0x00001000,	// The player has the blue flag
	mHAS_GREEN_FLAG = 0x00002000,	// The player has the green flag
	mHAS_YELLOW_FLAG = 0x00004000,	// The player has the yellow flag
	mRESTARTING = 0x00008000,	// The player is restarting
	mSURVEYING = 0x00010000,	// The player is surveying the playing field

	mHAS_ANY_FLAG = mHAS_RED_FLAG | mHAS_BLUE_FLAG | mHAS_GREEN_FLAG | mHAS_YELLOW_FLAG,
};

enum {
	vMAX_CONNECTIONS = 20,
	vMAX_PLAYERS = 8,
	vMAX_SERVER_NAME_LEN = 15,
	vMAX_PLAYER_NAME_LEN = 15,
	vMAX_APPEARANCE_DATA_SIZE = 4096,
	vMAX_PASSWORD_LEN = 9,
	vMAX_CHAT_MSG_LEN = 127,
	vMAX_TRICK_OBJECTS_IN_LEVEL = 256,
};

void addObserveCFuncs();