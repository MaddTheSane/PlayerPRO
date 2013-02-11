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

#include <PlayerPROCore/MAD.h>
#include <PlayerPROCore/FileUtils.h>
#include <PlayerPROCore/PPDefs.h>
#include <PlayerPROCore/RDriver.h>
#ifdef ISUSINGPPPLUG
#include <PlayerPROCore/PPPlug.h>
#endif


#if defined(_MAC_H) && !TARGET_OS_IPHONE
#include <PlayerPROCore/RDriverCarbon.h>
#endif

#endif
