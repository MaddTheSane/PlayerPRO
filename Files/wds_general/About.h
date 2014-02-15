//
//  About.h
//  PPMacho
//
//  Created by C.W. Betts on 2/15/14.
//
//

#ifndef PPMacho_About_h
#define PPMacho_About_h

#include "wds_general.h"

namespace PlayerPRO {
	class About : public wds_general {
	public:
		About();
		~About();
		void DoScrollText(DialogPtr aDia);
		void DoAbout();
		void ShowPerformance();
		bool DoHelpWindow();
		void MyAdjustText(ControlHandle vScroll);
		void DoContent(WindowPtr theWindow, EventRecord *theEventI);

	private:
		TEHandle	hTE;
		long		curPosT;
		Rect		scrollRect, textRect;
		Point		zeroPt;
		Boolean		AboutFirstCall;
		short		gThumbPrev;
		GWorldPtr	gGWorldAbout, gMaskWorld, gResultWorld, gFreePict;
		PicHandle	backPict;
	};
}

#endif
