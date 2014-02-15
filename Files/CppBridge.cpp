//
//  CppBridge.cpp
//  PPMacho
//
//  Created by C.W. Betts on 2/14/14.
//
//

#include "PlayerPROApp.h"

void DoCPPGlobalNull()
{
	TheApp->DoGlobalNull();
}

EventRecord *CurrentEvent()
{
	return &TheApp->theEvent;
}
