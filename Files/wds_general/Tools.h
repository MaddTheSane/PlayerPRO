//
//  Tools.h
//  PPMacho
//
//  Created by C.W. Betts on 2/14/14.
//
//

#ifndef PPMacho_Tools_h
#define PPMacho_Tools_h

#include "wds_general.h"

namespace PlayerPRO {
	class Tools : public wds_general {
	public:
		Tools();
		~Tools();
		void DoHelp(short **items, short *lsize);
		void DoNull();
		void DrawTimeBar();
		void UpdateToolsWindow(DialogPtr GetSelection);
		void CreateDialog();
		void CloseToolsWindow();
		void DoRecord();
		void DoRecule();
		void DoPause();
		void DoStop();
		void DoSearchUp();
		void DoSearchDown();
		void SetCurrentMOD(ConstStr255Param theMODName);
		void DoRemember();
		void DoPlay(void);
		void ScanTime();
		void DoChangeLoop();
		void DoItemPressTools(short whichItem, DialogPtr whichDialog);

	private:
		short	oldPartition, oldPartition2, oldPL;
		long	maxTime, oldTime;
		bool	canAcceptDrag;
		short	RememberPat, RememberReader, RememberPL;
		short	LoopCntlState, PreviousLoop;
		
		ControlHandle	playCntl, progCntl, stopCntl, RecordCntl, BackCntl, ForCntl, JumpNextCntl, JumpBeforeCntl, LoopCntl;
		PixPatHandle	workPixPat;
		
		ControlActionUPP ForeUPP, BackUPP;
		DragTrackingHandlerUPP MyTrackingToolsUPP;
		DragReceiveHandlerUPP MyReceiveToolsUPP;
		bool PianoRecording;
	};
}

#endif
