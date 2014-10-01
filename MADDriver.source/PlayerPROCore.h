/*
 *  PlayerPROCore.h
 *  PPMacho
 *
 *  Created by C.W. Betts on 11/22/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef __PLAYERPROCORE_H__
#define __PLAYERPROCORE_H__

#include "PPDefs.h"
#include "MAD.h"
#include "FileUtils.h"
#include "RDriver.h"
#include "MADPlug.h"

#ifdef _MAC_H
//! Project version number for PlayerPROCore.
EXP double PlayerPROCoreVersionNumber;

//! Project version string for PlayerPROCore.
EXP const unsigned char PlayerPROCoreVersionString[];
#endif

#if defined(_MAC_H) && !TARGET_OS_IPHONE
#include "RDriverCarbon.h"
#endif

#endif
