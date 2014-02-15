/*
 *  wds_general.h
 *  PPMacho
 *
 *  Created by C.W. Betts on 2/13/14.
 *  Copyright 2014 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef __PLAYERPRO_WDS_GENERAL_H__
#define __PLAYERPRO_WDS_GENERAL_H__

#include <Carbon/Carbon.h>

namespace PlayerPRO {
	class wds_general {
	public:
		wds_general(SInt16 dialogID, WindowPtr parentWindow = (WindowPtr) -1);
		virtual ~wds_general();
		virtual void DoHelp(short **items, short *lsize) = 0;
		virtual void DoNull() = 0;
		
		bool GetCanAcceptDrag();
		void SetCanAcceptDrag(bool val);
		CGrafPtr GetDialogPort();
		WindowRef GetDialogWindow();
		
	protected:
		DialogPtr	theDialog;
		bool		canAcceptDrag;
	};
}

#endif
