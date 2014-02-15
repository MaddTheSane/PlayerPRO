//
//  Samples.h
//  PPMacho
//
//  Created by C.W. Betts on 2/13/14.
//
//

#ifndef PPMacho_Samples_h
#define PPMacho_Samples_h
#include "wds_general.h"

extern "C" void DrawSampleInt(long	sampleSize,
							  long	start,
							  long	tSS,
							  long	tSE,
							  long	high,
							  long	larg,
							  long	trueV,
							  long	trueH,
							  short	channel,
							  sData	*curData);

typedef struct IdleSample {
	short oldPos;
	short oldInstru;
} IdleSample;

namespace PlayerPRO {
	class Samples : public wds_general {
		Samples();
		~Samples();
		void DoHelp(short **items, short *lsize);
		void CreateSampleWindow(short ins, short samp);
		void DrawEnvelope(short ins, DialogPtr theDia);
		void DrawPanningEnvelope(short ins, DialogPtr theDia);
		void DrawSample(short tSS, short tSE, short InstruNo, DialogPtr theDia);
		
		long GetStartSampleValue(long val, short InstruNo);
		void SetSelectionZero(short no);
		void SetSelection(short no, long start, long end);
		void EraseTikTak(short ins);
		void DoGrowSample(DialogPtr theDia);
		
		void UpdateDisplaySize(short InstruNo);
		void UpdateDisplay(short InstruNo);
		void UpdateDisplayQuality(short InstruNo);
		void UpdateDisplayPosition(short InstruNo);
		void UpdateDisplayLoop(short InstruNo);
		
		long FindByteStart(short InstruNo);
		short ByteToPos(long bytePosi, short InstruNo);
		long AmpToByte(short Pos, short InstruNo);
		long PosToByte(short Pos, short InstruNo);
		
		void GetSampleRect(DialogPtr theDia);
		long FindByteEnd(short InstruNo);
		
		void Selection2Loop(short InstruNo);
		void SetSustainPoint(short InstruNo);
		void Loop2Selection(short InstruNo);
		void UpdateDisplaySelec(short InstruNo);
		
		void COPYSampleInt(DialogPtr theDia);
		void PASTESampleInt(DialogPtr theDia);
		
		void AfficheOldPoint(short i);
		
		void DoNull();
		void DrawLoop(short ins);
		void UpdateSampleWindow(DialogPtr GetSelection);
		
		void DrawPencil(DialogPtr theDia, short InstruNo);
		void SetControlH(short theInstru);
		void InternalUpdate(short InstruNo);
		void UpdateSampleWindows();
		
		void NAppelPlug(short InstruNo, short samp, short whichPlug);
		void SampleUpdateNow();
		void NAppelVSTPlug(short InstruNo, short samp, short whichPlug);
		void MoveLoopSelection(short InstruNo);
		void DoItemPressSample(short whichItem, DialogPtr whichDialog);
		void DoKeyPressSample(DialogPtr	theDia, short theChar);
		void ResetSelectionSample(short CurWin);
		pascal void actionProcSample(ControlHandle theControl, short ctlPart);

	private:
		DragTrackingHandlerUPP	MyTrackingHandlerUPP;
		DragReceiveHandlerUPP	MyReceiveDropHandlerUPP;
		
		Rect					SampleRect;
		GWorldPtr				BitMapSample[MAXINSTRU];
		bool					Clignote[MAXINSTRU], TokTak[MAXINSTRU];
		long					ZoomLevel[MAXINSTRU];
		IdleSample				theIdle[MAXTRACK];
		ControlHandle			InfoBut[MAXINSTRU], PencilBut[MAXINSTRU], SelectBut[MAXINSTRU], ZoomBut[MAXINSTRU];
		short					DragInstruSource, SelectMode[MAXINSTRU];
		ControlHandle			xScroll[MAXINSTRU];
		short					curSample[MAXINSTRU];
		Byte					curMode[MAXINSTRU];
		ControlHandle			LoopBut[MAXINSTRU], SustainBut[MAXINSTRU], EnvBut[MAXINSTRU], FixedBut[MAXINSTRU];
		short 					gThumbPrev;
		bool					gNowMovingLoop = false;
		void PrintSample(DialogPtr theDia);

	};
}

#endif
