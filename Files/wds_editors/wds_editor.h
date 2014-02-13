/*
 *  wds_editor.h
 *  PPMacho
 *
 *  Created by C.W. Betts on 2/13/14.
 *  Copyright 2014 __MyCompanyName__. All rights reserved.
 *
 */

#include <Carbon/Carbon.h>
#include "wds_general.h"

namespace PlayerPRO {
	class wds_editor : public wds_general {
		wds_editor(SInt16 dialogID, WindowPtr parentWindow = (WindowPtr) -1);
		virtual ~wds_editor();
	};
}	
