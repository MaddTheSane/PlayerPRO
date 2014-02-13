/*
 *  PlayerPROApp.cpp
 *  PPMacho
 *
 *  Created by C.W. Betts on 2/13/14.
 *  Copyright 2014 __MyCompanyName__. All rights reserved.
 *
 */

#include "PlayerPROApp.h"

EventTypeSpec *PlayerPRO::PlayerPROApp::CarbonWinList()
{
	return (EventTypeSpec[]){ {kEventClassWindow, kEventWindowClose },
		{ kEventClassWindow, kEventWindowDrawContent },
		{ kEventClassWindow, kEventWindowBoundsChanged } };
}

std::string PlayerPRO::PlayerPROApp::ENote(int idx)
{
	return "";
}

std::string PlayerPRO::PlayerPROApp::EInstru(int idx)
{
	return "";
}

std::string PlayerPRO::PlayerPROApp::EArgu(int idx)
{
	return "";
}

std::string PlayerPRO::PlayerPROApp::EEffect(int idx)
{
	return "";
}

