/*
 *  wds_general.cpp
 *  PPMacho
 *
 *  Created by C.W. Betts on 2/13/14.
 *  Copyright 2014 __MyCompanyName__. All rights reserved.
 *
 */

#include "wds_general.h"
extern "C" {
#include "Shuddup.h"
}

PlayerPRO::wds_general::wds_general(SInt16 dialogID, WindowPtr parentWindow)
{
	theDialog = GetNewDialog(dialogID, NULL, parentWindow);
}

PlayerPRO::wds_general::~wds_general()
{
	DisposeDialog(theDialog);
}
static short AHelp[] = {};

void PlayerPRO::wds_general::DoNull()
{
	
}

void PlayerPRO::wds_general::DoHelp(short **items, short *lsize)
{
	*items = AHelp;
	*lsize = 0;
}
