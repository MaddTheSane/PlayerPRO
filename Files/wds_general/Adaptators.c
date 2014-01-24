#include "Shuddup.h"
#include "MAD.h"
#include "RDriver.h"
#include <math.h>
#include "PrivateList.h"
#include "Undo.h"

	/******** HELP MODULE ********/
	enum
	{
		HLoad	= 25,
		HSave	= 17,
		HReset	= 22,
		HPan	= 26,
		HPitch	= 12,
		HSpeed	= 14,
		HVol	= 10,
		HTVol	= 16,
		HSVol	= 8,
		HPref	= 23
	};
#define	AHELPSIZE 10

	static	short					AHelp[ AHELPSIZE] = { HLoad, HSave, HReset, HPan, HPitch, HSpeed, HVol, HTVol, HSVol, HPref};

	void DoHelpAdap(short **items, short *lsize)
	{
		*lsize = AHELPSIZE;
		*items = AHelp;
	}
	
	/*****************************/
	
	extern	long					TickInterne;
	extern	Boolean					DragManagerUse;
	extern	RGBColor				theColor;
	extern	WindowPtr				oldWindow;
	extern	DialogPtr				EditorDlog;
	
			DialogPtr				AdapDlog;
	
	static	Rect					viewRect;
	static	Rect					VolumeRect, LeftRightRect;
	static	ControlHandle			MainFx[ 10];
	static	ControlHandle			MainFXOnOff;
	static	ControlHandle			FineBut, ScrollAdap, SaveBut, FlipBut, LoadBut, ResetPitchBut, ResetPanBut, ResetSpeedBut, volCntl[MAXTRACK], pannCntl[MAXTRACK];
	static	ControlHandle			OnOff[MAXTRACK], Mono[MAXTRACK], hardvolCntl, softvolCntl, speedCntl, pitchCntl, panCntl;
	static	ControlHandle			VSTFx[MAXTRACK][4];
	static	ControlHandle			VSTOnOff[MAXTRACK];
	static	ControlHandle			VSTDest[MAXTRACK];
	static	Rect					pulseRect[MAXTRACK];
	static	Boolean					canAcceptDrag;
	static	short 					gThumbPrev, gCurrentTrack;
	static	long					oldNullTicks, oldNullTicks2;
	static	short 					oldTube[MAXTRACK];
	static	PicHandle				pulsePic;
	static	short					oldSpeed = 0, oldTempo = 0;
	
#define DISVOL		20
#define SPACE		94
#define PICSIZE		59
#define MINPULSE	23
#define XWINDOWSIZE	375
#define TUNINGTYPE	'tuni'
	DragTrackingHandlerUPP	MyTrackingAdapUPP;
	DragReceiveHandlerUPP	MyReceiveAdapUPP;


//void MyTETextBox2(Str255 aStr, Rect *Zone);
void CheckVSTEditor(VSTEffect *ce);
pascal OSErr MyTrackingAdap(short message, WindowPtr theWindow, void *handlerRefCon, DragReference theDrag);
pascal OSErr MyReceiveAdap(WindowPtr theWindow, void* handlerRefCon, DragReference theDrag);
void UPDATE_TrackActive(void);
void DisposeVSTEffect(VSTEffect	*myEffect);
void HandleVSTChoice(short item, VSTEffect** vst, short channelID);
void CreateControlAdap(void);
void FillVSTEffects(void);

void DrawValueIndicator(double val, Boolean percent, Str255 text)
{
	RgnHandle	saveClip = NewRgn();
	Rect		caRect;
	
	GetClip(saveClip);
	
	GetPortBounds(GetDialogPort(AdapDlog), &caRect);
	
	ClipRect(&caRect);
	
	if (val == -1)
	{
	//	TextFont(0);	TextSize(0);
		SetDText(AdapDlog, 40, "\p");
	//	TextFont(kFontIDGeneva);	TextSize(9);
	}
	else
	{
		Str255	str;
		
		if (text) 
			pStrcpy(str, text);
		else {
			long intval = val;
			
			if ((double) intval == val) {
				sprintf((Ptr) str, "%.0f", val);
				MyC2PStr((Ptr) str);
			} else {
				sprintf((Ptr)str, "%.2f", val);
				MyC2PStr((Ptr)str);
			}
		}
		
		if (percent) pStrcat(str, "\p %");
		
	//	TextFont(0);	TextSize(0);
		SetDText(AdapDlog, 40, str);
	//	TextFont(kFontIDGeneva);	TextSize(9);
	}
	
	SetClip(saveClip);
	DisposeRgn(saveClip);
}

void FineSpeedPitchSettings()
{
	short					itemHit;
	Str255					theString;
	GrafPtr					savedPort;
	DialogPtr				aDia;
	long					previousPitch = MADDriver->FreqExt, previousSpeed = MADDriver->VExt, temp;
	double					val;

	GetPort(&savedPort);

	aDia = GetNewDialog(182, NULL, (WindowPtr) -1L);
	SetPortDialogPort(aDia);
	
	AutoPosition(aDia);
	
	SetCursor(GetQDGlobalsArrow(&qdarrow));
	
	val = 100.*(8000./ (MADDriver->FreqExt));
	
	sprintf((Ptr) theString, "%.1f", val);
	MyC2PStr((Ptr) theString);
	
	SetDText(aDia, 5, theString);
	
	val = 100. * (MADDriver->VExt / 8000.);
	
	sprintf((Ptr) theString, "%.1f", val);
	MyC2PStr((Ptr) theString);
	
	SetDText(aDia, 6, theString);
	
	SelectDialogItemText(aDia, 5, 0, 32767);
	
	BACK:
	
	do
	{
		//	ModalDialog(MyDlgFilterDesc, &itemHit);
		
		MyModalDialog(aDia, &itemHit);
		
		if (itemHit == -5 || itemHit == 0)
		{
			GetDText(aDia, 5, theString);
			MyP2CStr(theString);
			val = atof((Ptr) theString);
			temp = (8000. * 100.) / val;
			if (temp >= 160 && temp <= 16000)
			{
				SetControlVisibility(pitchCntl, true, false);
				
				if (temp != MADDriver->FreqExt) SetControlValue(pitchCntl, 16000- temp);
				MADDriver->FreqExt = temp;
				
				SetControlVisibility(pitchCntl, false, false);
			}
			
			GetDText(aDia, 6, theString);
			MyP2CStr(theString);
			val = atof((Ptr) theString);
			temp = (val * 8000.) / 100.;
			if (temp >= 160 && temp <= 16000)
			{
				SetControlVisibility(speedCntl, true, false);
				
				if (temp != MADDriver->VExt) SetControlValue(speedCntl, temp);
				MADDriver->VExt = temp;
				
				SetControlVisibility(speedCntl, false, false);
			}
		}
		
	}while (itemHit != 1 && itemHit != 2);
	
	UpdateALLWindow();
	
	if (itemHit == 1)
	{
		GetDText(aDia, 5, theString);
		MyP2CStr(theString);
		val = atof((Ptr) theString);
		temp = (8000. * 100.) / val;
		if (temp >= 160 && temp <= 16000) MADDriver->FreqExt = temp;
		else
		{
			SelectDialogItemText(aDia, 5, 0, 32767);
			SysBeep(1);
			goto BACK;
		}
		
		GetDText(aDia, 6, theString);
		MyP2CStr(theString);
		val = atof((Ptr) theString);
		temp = (val * 8000.) / 100.;
		if (temp >= 160 && temp <= 16000) MADDriver->VExt = temp;
		else
		{
			SelectDialogItemText(aDia, 6, 0, 32767);
			SysBeep(1);
			goto BACK;
		}
		
		curMusic->hasChanged = true;
	}
	
	if (itemHit == 2) {	// CANCEL
		MADDriver->FreqExt = previousPitch;
		MADDriver->VExt = previousSpeed;
	}
	
	if (MADDriver->FreqExt != 8000)
		HiliteControl(ResetPitchBut, 0);
	else
		HiliteControl(ResetPitchBut, 255);
	
	if (MADDriver->VExt != 8000)
		HiliteControl(ResetSpeedBut, 0);
	else
		HiliteControl(ResetSpeedBut, 255);
	
	
	DisposeDialog(aDia);
	SetPort(savedPort);
}

void DoNullAdap(void)
{
	GrafPtr		SavePort;
	short		i;
	Rect		itemRect;
	Point		pt;
	Boolean		found;
	RgnHandle	saveClip;
	Str255		str2, str1;
	RgnHandle	visibleRegion;
	Str255		TextVal;

	if (!AdapDlog) return;

	GetPort(&SavePort);
	
	SetPortDialogPort(AdapDlog);
	
	if (TickCount() <= oldNullTicks) return;
	oldNullTicks = TickCount();
	
	if (MADGetHardwareVolume() != thePrefs.volumeLevel)
	{
		thePrefs.volumeLevel = MADGetHardwareVolume();
		SetControlVisibility(hardvolCntl, true, false);
		SetControlValue(hardvolCntl, thePrefs.volumeLevel);
		SetControlVisibility(hardvolCntl, false, false);
	}
	
	if (oldTempo != MADDriver->finespeed || oldSpeed != MADDriver->speed)
	{
		oldTempo = MADDriver->finespeed;
		oldSpeed = MADDriver->speed;
		
		NTStr(3, MADDriver->finespeed, (Ptr) str2);
		MyC2PStr((Ptr) str2);
		
		pStrcat(str2, "\p / ");
		
		NTStr(3, MADDriver->speed, (Ptr) str1);
		MyC2PStr((Ptr) str1);
		
		pStrcat(str2, str1);
		
	//	TextFace(bold);
		
		SetDText(AdapDlog, 9, str2);
		
	//	TextFace(0);
	}
	
	// Pulse Rects
	
	saveClip = NewRgn();
 	GetClip(saveClip);
 	ClipRect(&viewRect);
	
	for (i = 0; i < curMusic->header->numChn; i++)
	{
		if (MADDriver->Tube[i] >= -MINPULSE*2-1)
		{
			itemRect = pulseRect[i];
			
			InsetRect(&itemRect, 2, 2);
			
			if (MADDriver->Tube[ i] == oldTube[ i])
			{
				if (MADDriver->chan[ i].maxPtr > MADDriver->chan[ i].curPtr && MADDriver->chan[ i].curPtr != NULL && MADDriver->Tube[i] <= 0)
			 	{
			 		
			 	}
				else
				{
					short val = itemRect.left + MINPULSE + MADDriver->Tube[i]/2;
					MADDriver->Tube[i]--;
				
					oldTube[ i] = MADDriver->Tube[i];
					
				 	if (oldTube[i] >= -MINPULSE*2-1)
					{
						MoveTo(val, itemRect.top);
						LineTo(val, itemRect.bottom);
					}
				}
			}
			else
			{
				SwitchColor(i);
				
				if (MADDriver->Tube[i] <= 0) MADDriver->Tube[i] = 1;
				
				itemRect.right = itemRect.left + MINPULSE + MADDriver->Tube[i]/2;
				PaintRect(&itemRect);
				ForeColor(blackColor);
				
				if (MADDriver->Tube[ i] != 64)
				{
					itemRect.left = itemRect.right;
					itemRect.right = pulseRect[i].right-2;
					PaintRect(&itemRect);
				}
				
				oldTube[ i] = MADDriver->Tube[i];
				
				MoveTo(pulseRect[ i].left + 3, pulseRect[i].bottom -3);
				NTStr(3, MADDriver->chan[ i].ins+1, (Ptr) str2); MyC2PStr((Ptr) str2);
				TextFace(bold);
				DrawString(str2);
				TextFace(0);
			}
		}
	}
	SetClip(saveClip);
	DisposeRgn(saveClip);
		
	if (Button()) return;
	
	if (TickCount() <= oldNullTicks2) return;
	oldNullTicks2 = TickCount() + 10;
	
//	if (oldWindow == AdapDlog)
	{
		double val;
		
		GetMouse(&pt);
		
		visibleRegion = NewRgn();
		
		GetPortVisibleRegion(GetDialogPort(AdapDlog), visibleRegion);
		
		TextVal[ 0] = 0;
		
		if (PtInRgn(pt, visibleRegion))
		{
			Rect	contrlRect;
			Boolean	percent = false;
			short	x;
			
			found = false;
			
			for (i = 0; i < gCurrentTrack; i++)
			{
				
				GetControlBounds(volCntl[ i], &contrlRect);
				
				if (PtInRect(pt, &contrlRect)) { found = true;	val = (100*curMusic->header->chanVol[ i])/64; percent = true;}
				
				GetControlBounds(pannCntl[ i], &contrlRect);
				
				if (PtInRect(pt, &contrlRect)) { found = true;	val = 100. * ((curMusic->header->chanPan[ i] - 32)/32.); percent = true;}
				
				for (x = 0; x < 4; x++)
				{
					GetControlBounds(VSTFx[ i][ x], &contrlRect);
					
					if (PtInRect(pt, &contrlRect))
					{
						if (curMusic->header->chanBus[ i].copyId == i)
						{
							found = true;
							if (MADDriver->chanVST[ i][ x]) pStrcpy(TextVal, MADDriver->chanVST[ i][ x]->name);
							else pStrcpy(TextVal, "\pNo Effect");
						}
					}
				}
				
				if (found) break;
			}
			
			for (x = 0; x < 10; x++)
			{
				GetControlBounds(MainFx[ x], &contrlRect);
			
				if (PtInRect(pt, &contrlRect)) { found = true;	 if (MADDriver->masterVST[ x]) pStrcpy(TextVal, MADDriver->masterVST[ x]->name); else pStrcpy(TextVal, "\pNo Effect");}
			}
			GetControlBounds(hardvolCntl, &contrlRect);
			
			if (PtInRect(pt, &contrlRect)) { found = true;	val = (100 * thePrefs.volumeLevel)/256L; percent = true;}
			
			GetControlBounds(softvolCntl, &contrlRect);
			
			if (PtInRect(pt, &contrlRect)) { found = true;	val = (100 * MADDriver->VolGlobal)/64L; percent = true;}
			
			GetControlBounds(speedCntl, &contrlRect);
			
			if (PtInRect(pt, &contrlRect)) { found = true;	val = 100. * (MADDriver->VExt / 8000.); percent = true;}
			
			GetControlBounds(pitchCntl, &contrlRect);
			
			if (PtInRect(pt, &contrlRect)) { found = true;	 val = 100.*(8000./ (MADDriver->FreqExt)); percent = true;}
			
			GetControlBounds(panCntl, &contrlRect);
			
			if (PtInRect(pt, &contrlRect)) { found = true;	val = 100. *((MADDriver->globPan - 64)/64.); percent = true;}
			
			if (found)
			{
				if (TextVal[ 0]) DrawValueIndicator(val, false, TextVal);
				else DrawValueIndicator(val, percent, NULL);
			}
			else DrawValueIndicator(-1, false, NULL);
		}
		else DrawValueIndicator(-1, false, NULL);
		
		DisposeRgn(visibleRegion);
	}
	
	SetPort(SavePort);
}

void SetUpScroll(void)
{
	Rect	caRect;
	
	GetPortBounds(GetDialogPort(AdapDlog), &caRect);

	viewRect.bottom = caRect.bottom - 15;
	
	MyMoveControl(ScrollAdap, XWINDOWSIZE - 15, viewRect.top);
	MySizeControl(ScrollAdap, 16, viewRect.bottom - viewRect.top + 1);
	
	SetControlMinimum(ScrollAdap, 0);
	
	if (MADDriver->DriverSettings.outPutMode == DeluxeStereoOutPut)
	{
		SetControlMaximum(ScrollAdap, gCurrentTrack - (viewRect.bottom - viewRect.top) / DISVOL);
	}
	else SetControlMaximum(ScrollAdap, gCurrentTrack - (viewRect.bottom - viewRect.top) / DISVOL);
	
	if (GetControlMaximum(ScrollAdap) < 0) SetControlMaximum(ScrollAdap, 0);
	
	if (gUseControlSize) SetControlViewSize(ScrollAdap, (viewRect.bottom - viewRect.top) / DISVOL);
}

void DoGrowAdap(DialogPtr theDialog)
{
	long		lSizeVH;
	GrafPtr		SavePort;
	Rect		temp;
	short		tempB, tempA, avant;
	Point		theCell = { 0, 0}, aPt = { 0, 0};
	Rect		caRect;
	BitMap		screenBits;
	

	GetPort(&SavePort);
 	SetPortDialogPort(theDialog);

	temp.top = 210;
	
	temp.bottom = viewRect.top + 3 + gCurrentTrack * DISVOL;
	temp.bottom += 16;
	
	LocalToGlobal(&aPt);
	
	GetQDGlobalsScreenBits(&screenBits);
	
	if (temp.bottom < temp.top) temp.bottom = temp.top;
	else if (temp.bottom > screenBits.bounds.bottom - aPt.v) temp.bottom = screenBits.bounds.bottom - aPt.v -2;
	
	GetPortBounds(GetDialogPort(theDialog), &caRect);
	
	temp.left = caRect.right;
	temp.right = caRect.right;
	
#if MACOS9VERSION
	temp.left++;
	temp.right++;
#endif
	
	lSizeVH = 0;
	if (theEvent.what == mouseDown) lSizeVH = GrowWindow(GetDialogWindow(theDialog), theEvent.where, &temp);
	
	if (lSizeVH != 0)
	{
		tempA = LoWord(lSizeVH);
		tempB = HiWord(lSizeVH);
	}
	else
	{
		GetPortBounds(GetDialogPort(theDialog), &caRect);
		
		tempA = caRect.right;
		tempB = caRect.bottom;
	}
	
	GetPortBounds(GetDialogPort(theDialog), &caRect);
	
	avant = caRect.bottom - 15;
	
	MySizeWindow(theDialog, XWINDOWSIZE, tempB, true);
	
	GetPortBounds(GetDialogPort(theDialog), &caRect);
	
	viewRect.bottom = caRect.bottom - 15;
	
	EraseRect(&caRect);
	InvalWindowRect(GetDialogWindow(theDialog), &caRect);

	SetUpScroll();

	SetPort(SavePort);
}

void FillOnOffControl(short id, short curval)
{
	Rect	contrlRect;
	
	if (curval)
	{
		HiliteControl(volCntl[ id], 0);
		HiliteControl(pannCntl[ id], 0);
		SetControlValue(OnOff[ id], 1);
	}
	else
	{
		HiliteControl(volCntl[ id], 255);
		HiliteControl(pannCntl[ id], 255);
		SetControlValue(OnOff[ id], 0);
	}
	
	GetControlBounds(OnOff[ id], &contrlRect);
	
	MyMoveControl(OnOff[ id], contrlRect.left, VolumeRect.top - (GetControlValue(ScrollAdap) - id)*DISVOL);
	
	SetControlVisibility(OnOff[ id], true, false);
	Draw1Control(OnOff[ id]);
	SetControlVisibility(OnOff[ id], false, false);
}

void FillVSTOnOffControl(short id, short curval)
{
	Rect	contrlRect;
	
	if (curval)
	{
		SetControlValue(VSTOnOff[ id], 1);
	}
	else
	{
		SetControlValue(VSTOnOff[ id], 0);
	}
	
	GetControlBounds(VSTOnOff[ id], &contrlRect);
	
	MyMoveControl(VSTOnOff[ id], contrlRect.left, VolumeRect.top - (GetControlValue(ScrollAdap) - id)*DISVOL);
	
	SetControlVisibility(VSTOnOff[ id], true, false);
	Draw1Control(VSTOnOff[ id]);
	SetControlVisibility(VSTOnOff[ id], false, false);
}

void FillFxControl(short id)
{
	Rect	contrlRect, itemRect;
	short	x;
	Str32	str1, str2;
	
	for (x = 0; x < 4; x++)
	{
		GetControlBounds(VSTFx[ id][ x], &contrlRect);
		
		MyMoveControl(VSTFx[ id][ x], contrlRect.left, VolumeRect.top - (GetControlValue(ScrollAdap) - id)*DISVOL);
		SetControlVisibility(VSTFx[ id][ x], true, false);
		
		GetControlBounds(VSTFx[ id][ x], &contrlRect);
		
		if (curMusic->header->chanBus[ id].copyId == id || curMusic->header->chanBus[ id].Active == false)
		{
			if (curMusic->header->chanBus[ id].Active == false)
			{
				HiliteControl(VSTFx[ id][ x], 255);
			}
			else
			{
				if (MADDriver->chanVST[ id][ x] != NULL) HiliteControl(VSTFx[ id][ x], kControlButtonPart);
				else HiliteControl(VSTFx[ id][ x], 0);
			}
			Draw1Control(VSTFx[ id][ x]);
		}
		else
		{
			if (x == 0)
			{
				MoveTo(contrlRect.left, contrlRect.bottom-3);
				
				pStrcpy(str1, "\pTrack ");
				NumToString(curMusic->header->chanBus[ id].copyId + 1, str2);
				pStrcat(str1, str2);
				
				TextFace(bold);
				SetRect(&itemRect, contrlRect.left + 2, contrlRect.top, contrlRect.left + 20 + 40, contrlRect.bottom-1);
				SwitchColor(curMusic->header->chanBus[ id].copyId);
				TETextBox((Ptr) str1 + 1, str1[ 0], &itemRect, teJustCenter);
				//MyTETextBox2(str1, &itemRect);
				RGBBackColor(&theColor);
				itemRect.left--;
				FrameRectRelief(&itemRect);
				TextFace(0);
			}
		}
		
		
		SetControlVisibility(VSTFx[ id][ x], false, false);
	}
}

void FillFxDestControl(short id)
{
	Rect	contrlRect;
	
	GetControlBounds(VSTDest[ id], &contrlRect);
	
	MyMoveControl(VSTDest[ id], contrlRect.left, VolumeRect.top - (GetControlValue(ScrollAdap) - id)*DISVOL);
	SetControlVisibility(VSTDest[ id], true, false);
	
/*	if (MADDriver->chanVST[ id][ x] != NULL) HiliteControl(VSTDest[ id], kControlButtonPart);
	else HiliteControl(VSTDest[ id], 0);	*/
	
	Draw1Control(VSTDest[ id]);
	SetControlVisibility(VSTDest[ id], false, false);
}


void FillMonoControl(short id)
{
	Rect	contrlRect;
	
/*	if (curval)
	{
		HiliteControl(pannCntl[ id], 255);
		HiliteControl(Mono[ id], inButton);
	}
	else
	{
		if (GetControlValue(OnOff[ id])) HiliteControl(pannCntl[ id], 0);
		HiliteControl(Mono[ id], 0);
	}*/
	
	GetControlBounds(Mono[ id], &contrlRect);
	
	MyMoveControl(Mono[ id], contrlRect.left, VolumeRect.top - (GetControlValue(ScrollAdap) - id)*DISVOL);
	SetControlVisibility(Mono[ id], true, false);
	Draw1Control(Mono[ id]);
	SetControlVisibility(Mono[ id], false, false);
}

void FillBarControl(short id, short curval, short maxval)
{
	Rect		cRect, tRect, contrlRect;
	short		tempH;
	Str255		str;
	RGBColor	color;
	
	if (id == -1) MyDebugStr(__LINE__, __FILE__, "");
	
	SetControlValue(volCntl[ id], (curval * 100) / maxval);
	
	GetControlBounds(volCntl[ id], &contrlRect);
	
	MyMoveControl(volCntl[ id], contrlRect.left, VolumeRect.top - (GetControlValue(ScrollAdap) - id)*DISVOL);
	SetControlVisibility(volCntl[ id], true, false);
	Draw1Control(volCntl[ id]);
	SetControlVisibility(volCntl[ id], false, false);
}

void FillBarStereo(short id, short curval, short maxval)
{
	Rect	contrlRect;

	if (id < 0) MyDebugStr(__LINE__, __FILE__, "");
	
	GetControlBounds(pannCntl[ id], &contrlRect);
	
	MyMoveControl(pannCntl[ id], contrlRect.left, VolumeRect.top - (GetControlValue(ScrollAdap) - id)*DISVOL - 1);
	SetControlValue(pannCntl[ id], (curval * 100) / maxval);
	SetControlVisibility(pannCntl[ id], true, false);
	Draw1Control(pannCntl[ id]);
	SetControlVisibility(pannCntl[ id], false, false);
}

void FillPulseRect(short id)
{
	Str255	str2;
	Rect	itemRect;
	
	if (id < 0) MyDebugStr(__LINE__, __FILE__, "");
	
	pulseRect[ id].top = VolumeRect.top - (GetControlValue(ScrollAdap) - id)*DISVOL;
	pulseRect[ id].bottom = pulseRect[ id].top + 14;
	
	DrawPicture(pulsePic, &pulseRect[ id]);
	
	itemRect = pulseRect[ id];
	
	InsetRect(&itemRect, 2, 2);
	
	SwitchColor(id);
	
	itemRect.right = itemRect.left + MINPULSE + MADDriver->Tube[ id]/2;
	PaintRect(&itemRect);
	ForeColor(blackColor);
	
	if (MADDriver->Tube[ id] != 64)
	{
		itemRect.left = itemRect.right;
		itemRect.right = pulseRect[ id].right-2;
		PaintRect(&itemRect);
	}
	
	oldTube[ id] = MADDriver->Tube[ id];
	
	MoveTo(pulseRect[ id].left + 3, pulseRect[ id].bottom -3);
	NTStr(3, MADDriver->chan[ id].ins+1, (Ptr) str2); MyC2PStr((Ptr) str2);
	TextFace(bold);
	DrawString(str2);
	TextFace(0);
}

void UpdateAdapWindow(DialogPtr	GetSelection)
{
	Rect   		itemRect;   			/* Temporary rectangle */
 	GrafPtr		SavePort;
 	short		itemType,i, max;
 	Handle		itemHandle;
 	Str255		str1, str2;
 	RgnHandle	saveClip;
 	Rect		contrlRect, caRect;
	RgnHandle	visibleRegion;
	
	
	GetPort(&SavePort);
 	SetPortDialogPort(GetSelection);
 	
 	BeginUpdate(GetDialogWindow(GetSelection));
 	
	visibleRegion = NewRgn();
	
	GetPortVisibleRegion(GetDialogPort(GetSelection), visibleRegion);
	
	
	SetControlVisibility(hardvolCntl, true, false);
	SetControlVisibility(softvolCntl, true, false);
	SetControlVisibility(speedCntl, true, false);
	SetControlVisibility(pitchCntl, true, false);
	
	UpdateDialog(GetSelection, visibleRegion);
	
	SetControlVisibility(hardvolCntl, false, false);
	SetControlVisibility(softvolCntl, false, false);
	SetControlVisibility(speedCntl, false, false);
	SetControlVisibility(pitchCntl, false, false);
	
	DisposeRgn(visibleRegion);
	
	RGBBackColor(&theColor);
	
	ForeColor(blackColor);
	
	GetPortBounds(GetDialogPort(GetSelection), &caRect);
	
	MoveTo(0, viewRect.top);		LineTo(caRect.right, viewRect.top);
	
	saveClip = NewRgn();
	GetClip(saveClip);
	ClipRect(&viewRect);
	
	max = (caRect.bottom - 15 - VolumeRect.top) / (VolumeRect.bottom - VolumeRect.top);
	max++;
	
	if (GetControlValue(ScrollAdap) + max > gCurrentTrack) max = gCurrentTrack - GetControlValue(ScrollAdap);
	
	for(i = 0; i < gCurrentTrack; i++)
	{
		if (i >= 0 && i < gCurrentTrack)
		{
			FillFxDestControl(i);
			
			FillFxControl(i);
			
			FillMonoControl(i);
			
			FillOnOffControl(i, MADDriver->Active[ i]);
			
			FillVSTOnOffControl(i, curMusic->header->chanBus[ i].Active);
			
			FillBarControl(i, curMusic->header->chanVol[i], MAX_CHANVOL);
			
			// Draw Track ID
			
			pStrcpy(str1, "\p");
			NTStr(3, i+1, (Ptr) str2); MyC2PStr((Ptr) str2);	pStrcat(str1, str2);
			
			GetControlBounds(volCntl[ i], &contrlRect);
			
			itemRect = contrlRect;
			itemRect.top++;
			TextFace(bold);
			SetRect(&itemRect, itemRect.left - 42, itemRect.top, itemRect.left - 19, itemRect.top + 12);
			
			//itemRect.left++;
			SwitchColor(i);
			TETextBox((Ptr) str1 + 1, str1[ 0], &itemRect, teJustLeft);
			//MyTETextBox2(str1, &itemRect);
			RGBBackColor(&theColor);
			itemRect.left--;
			FrameRectRelief(&itemRect);
			TextFace(0);
			
			/////////////////
			
			FillBarStereo(i, curMusic->header->chanPan[ i], MAX_PANNING);
			
			/////////////////
			
			FillPulseRect(i);
		}
	}
	
	SetClip(saveClip);
	DisposeRgn(saveClip);
	
	MoveTo(0, 25);
	LineTo(caRect.right, 25);
	
	DrawGrowIconP(GetSelection);
	
	ForeColor(blackColor);
	
	MoveTo(0, viewRect.bottom);	LineTo(caRect.right, viewRect.bottom);
	
	if (MADDriver->FreqExt != 8000) HiliteControl(ResetPitchBut, 0);
	else HiliteControl(ResetPitchBut, 255);
	
	if (MADDriver->VExt != 8000) HiliteControl(ResetSpeedBut, 0);
	else HiliteControl(ResetSpeedBut, 255);
	
	if (MADDriver->globPan != 64) HiliteControl(ResetPanBut, 0);
	else HiliteControl(ResetPanBut, 255);
	
	for (i = 0; i < 10; i++)
	{
		if (curMusic->header->globalFXActive == true)
		{
			if (MADDriver->masterVST[ i] != NULL) HiliteControl(MainFx[ i], kControlButtonPart);
			else HiliteControl(MainFx[ i], 0);
		}
		else
		{
			HiliteControl(MainFx[ i], 255);
		}
	}
	
	SetControlValue(MainFXOnOff, curMusic->header->globalFXActive);
	
	EndUpdate(GetDialogWindow(GetSelection));
	
	SetPort(SavePort);
}

void ResetAdaptators(void)
{
	Rect		itemRect;
	Handle		itemHandle;
	short		itemType, i, x;
	Rect		caRect;
	
	gCurrentTrack = 4;
	
	if (curMusic)
	{
		if (curMusic->header)
		{
			gCurrentTrack = curMusic->header->numChn;
		}
	}
	
	CreateControlAdap();
	
	GetDialogItem (AdapDlog, 2, &itemType, &itemHandle, &itemRect);
	VolumeRect = itemRect;
	LeftRightRect = itemRect;
	LeftRightRect.left += SPACE;
	LeftRightRect.right += SPACE;

	GetPortBounds(GetDialogPort(AdapDlog), &caRect);

	viewRect.left	= 0;
	viewRect.top	= VolumeRect.top - 4;
	viewRect.right	= XWINDOWSIZE - 16;
	
	for (i = 0; i < MAXTRACK; i++)
	{
		HideControl(pannCntl[ i]);		HideControl(volCntl[ i]);		HideControl(OnOff[ i]);
	}
	
	MySizeWindow(AdapDlog, XWINDOWSIZE, caRect.bottom, true);	//viewRect.right + 19
	
	GetPortBounds(GetDialogPort(AdapDlog), &caRect);
	
	viewRect.bottom	= viewRect.top + 3 + gCurrentTrack * DISVOL;
	SetMaxWindow(caRect.right + 3, viewRect.bottom + 15, AdapDlog);
	
//	GetDialogItem (AdapDlog, 24, &itemType, &itemHandle, &itemRect);	
//	ScrollAdap = (ControlHandle) itemHandle;
		
	theEvent.what = 0;
	DoGrowAdap(AdapDlog);
	
	if (MADDriver->DriverSettings.outPutMode == DeluxeStereoOutPut) HiliteControl(FlipBut, 0);
	else HiliteControl(FlipBut, 255);
	
	SetControlVisibility(pitchCntl, true, false);		SetControlValue(pitchCntl, 16000 - MADDriver->FreqExt);	SetControlVisibility(pitchCntl, false, false);
	SetControlVisibility(speedCntl, true, false);		SetControlValue(speedCntl, MADDriver->VExt);			SetControlVisibility(speedCntl, false, false);
	SetControlVisibility(hardvolCntl, true, false);	SetControlValue(hardvolCntl, thePrefs.volumeLevel);	SetControlVisibility(hardvolCntl, false, false);
	SetControlVisibility(softvolCntl, true, false);	SetControlValue(softvolCntl, MADDriver->VolGlobal);	SetControlVisibility(softvolCntl, false, false);
	SetControlValue(panCntl, MADDriver->globPan);
	
	if (MADDriver->FreqExt != 8000) HiliteControl(ResetPitchBut, 0);
	else HiliteControl(ResetPitchBut, 255);
	
	if (MADDriver->VExt != 8000) HiliteControl(ResetSpeedBut, 0);
	else HiliteControl(ResetSpeedBut, 255);
	
	for (i = 0; i < 10; i++)
	{
		if (curMusic->header->globalFXActive == true)
		{
 			if (MADDriver->masterVST[ i] != NULL)
 			{
 				HiliteControl(MainFx[ i], 0);
 				HiliteControl(MainFx[ i], kControlButtonPart);
 			}
			else
			{
				HiliteControl(MainFx[ i], 0);
				HiliteControl(MainFx[ i], 0);
			}
		}
		else
		{
			HiliteControl(MainFx[ i], 255);
		}
	}
	
	SetControlValue(MainFXOnOff, curMusic->header->globalFXActive);
	
	for (i = 0; i < MAXTRACK; i++)
	{
		for (x = 0; x < 4; x++)
		{
			if (MADDriver->chanVST[ i][ x] != NULL) HiliteControl(VSTFx[ i][ x], kControlButtonPart);
			else HiliteControl(VSTFx[ i][ x], 0);
		}
	}
	
	for (i = 0; i < MAXTRACK; i++)
	{
		if (curMusic->header->chanBus[ i].ByPass) HiliteControl(VSTDest[ i], kControlButtonPart);
		else HiliteControl(VSTDest[ i], 0);
		
		if (curMusic->header->chanBus[ i].Active)
		{
			SetControlValue(VSTOnOff[ i], 1);
		}
		else
		{
			SetControlValue(VSTOnOff[ i], 0);
			for (x = 0; x < 4; x++) HiliteControl(VSTFx[ i][ x], 255);
			HiliteControl(VSTDest[ i], 255);
		}
	}
}

void CreateControlAdap()
{
	short 	i, x;
	short	itemType;
	long	cntlID, cntlID2;
	Rect	itemRect, cRect;
	Handle	itemHandle;

	if (AppearanceManager)
	{
		cntlID = 57;
		cntlID2 = 57;
	}
	else
	{
		cntlID = 16;
		cntlID2 = 16;
	}

	// Purge OLD controls
	
	for (i = 0; i < MAXTRACK; i++)
	{
		if (volCntl[ i]) { DisposeControl(volCntl[ i]);	volCntl[ i] = NULL;}
		if (Mono[ i]) { DisposeControl(Mono[ i]);	Mono[ i] = NULL;}
		if (pannCntl[ i]) { DisposeControl(pannCntl[ i]);	pannCntl[ i] = NULL;}
		
		if (VSTOnOff[ i]) { DisposeControl(VSTOnOff[ i]);	VSTOnOff[ i] = NULL;}
		if (VSTDest[ i]) { DisposeControl(VSTDest[ i]);	VSTDest[ i] = NULL;}
		if (OnOff[ i]) { DisposeControl(OnOff[ i]);	OnOff[ i] = NULL;}
		
		for (x = 0; x < 4; x++)
		{
			if (VSTFx[ i][ x]) { DisposeControl(VSTFx[ i][ x]);	VSTFx[ i][ x] = NULL;}
		}
	}
	
	GetDialogItem(AdapDlog, 2, &itemType, &itemHandle, &itemRect);
	for (i = 0; i < gCurrentTrack; i++)
	{
		//#if MACOS9VERSION
		volCntl[ i] = NewControl(	GetDialogWindow(AdapDlog),
									&itemRect,
									"\pV",
									false,
									0,		//icl8 id
									0,
									100,
									cntlID,		// 16
									i);
		
		cRect = itemRect;
		
		OffsetRect(&cRect, 4 + itemRect.right - itemRect.left, 0);
		cRect.right = cRect.left + 13;
		cRect.bottom = cRect.top + 13;
		Mono[ i] = NewControl(	GetDialogWindow(AdapDlog),
									&cRect,
									"\p",
									false,
									0,
									kControlContentPictRes,
									152,
									kControlBevelButtonNormalBevelProc,
									i);
		
		cRect = itemRect;
		
		OffsetRect(&cRect, 20 + itemRect.right - itemRect.left, 0);
					
		pannCntl[ i] = NewControl(	GetDialogWindow(AdapDlog),
									&cRect,
									"\pP",
									false,
									0,		//icl8 id
									0,
									100,
									cntlID2,		//16
									i);
		
		OffsetRect(&cRect, 3 + itemRect.right - itemRect.left, 0);
		
		cRect.right = cRect.left + 18;
		cRect.bottom = cRect.top + 14;
		
		VSTOnOff[ i]	= 	NewControl(	GetDialogWindow(AdapDlog),
											&cRect,
											"\pX",
											false,
											1,
											0,
											100,
											checkBoxProc,
											i);

		OffsetRect(&cRect, 3 + 15, 0);
		
		for (x = 0; x < 4; x++)
		{
			
			cRect.right = cRect.left + 13;
			cRect.bottom = cRect.top + 13;
		
			VSTFx[ i][ x] = 	NewControl(	GetDialogWindow(AdapDlog),
												&cRect,
												"\p",
												false,
												0,
												kControlContentPictRes,
												159,
												kControlBevelButtonNormalBevelProc,
												i);
												
			OffsetRect(&cRect, 3 + 13, 0);
		}
		
		VSTDest[ i]	= 	NewControl(	GetDialogWindow(AdapDlog),
										&cRect,
										"\p",
										false,
										0,
										kControlContentPictRes,
										161,
										kControlBevelButtonNormalBevelProc,
										i);
		
		OffsetRect(&cRect, 3 + 13, 0);
		
		
		OffsetRect(&cRect, 3, 0);
		pulseRect[ i] = cRect;
		pulseRect[ i].right = pulseRect[ i].left + PICSIZE;
		pulseRect[ i].bottom = pulseRect[ i].top + 14;
		
		itemRect.top 	+= DISVOL;
		itemRect.bottom += DISVOL;
		
		//#endif
	}
	
	GetDialogItem(AdapDlog, 2, &itemType, &itemHandle, &itemRect);
	
	itemRect.left -= 19;
	itemRect.right = itemRect.left + 18;
	itemRect.bottom = itemRect.top + 14;
	
	for (i = 0; i < gCurrentTrack; i++)
	{
		//#if MACOS9VERSION
		OnOff[ i] = NewControl(	GetDialogWindow(AdapDlog),
									&itemRect,
									"\pO",
									false,
									1,		//icl8 id
									0,
									100,
									checkBoxProc,
									i);
		
		itemRect.top 	+= DISVOL;
		itemRect.bottom += DISVOL;
		
		//#endif
	}
}

void InitAdapWindow(void)
{
	Rect						itemRect, cRect, caRect;
	Handle						itemHandle;
	short						itemType, i, x, cntlID, cntlID2;
	Str255						StrTemp, str2;
	ControlHandle				ctl;
	ControlFontStyleRec			inStyle;
	
	for (i = 0; i < MAXTRACK; i++)
	{
		volCntl[ i] = 0;
		Mono[ i] = 0;
		pannCntl[ i] = 0;
		
		VSTOnOff[ i] = 0;
		VSTDest[ i] = 0;
		OnOff[ i] = 0;
		
		for (x = 0; x < 4; x++)
		{
			VSTFx[ i][ x] = 0;
		}
	}
	
	AdapDlog = GetNewDialog(141, NULL, GetDialogWindow(ToolsDlog));
	
	SetWindEtat(GetDialogWindow(AdapDlog));
	
	
	SetPortDialogPort(AdapDlog);
		
	oldNullTicks = oldNullTicks2 = 0;
	
	if (AppearanceManager)
	{
		cntlID = 57;
		cntlID2 = 57;
	}
	else
	{
		cntlID = 16;
		cntlID2 = 16;
	}
	
	pulsePic = GetPicture(154);
	
	SetRect(&itemRect, 0, 0, 30, 16);
	ScrollAdap = NewControl(		GetDialogWindow(AdapDlog),
									&itemRect,
									"\p.",
									true,
									0,
									0,
									0,
									gScrollBarID,
									xScrollNum);

		GetDialogItem(AdapDlog, 10, &itemType, &itemHandle, &itemRect);
		hardvolCntl = NewControl(GetDialogWindow(AdapDlog), &itemRect, "\p", false, thePrefs.volumeLevel, 0, 430, cntlID, 0);
		
		GetDialogItem(AdapDlog, 8, &itemType, &itemHandle, &itemRect);
		softvolCntl = NewControl(GetDialogWindow(AdapDlog), &itemRect, "\p", false, MADDriver->VolGlobal, 0, 128, cntlID, 0);
		
		GetDialogItem(AdapDlog, 12, &itemType, &itemHandle, &itemRect);
		pitchCntl = NewControl(GetDialogWindow(AdapDlog), &itemRect, "\p", false, 16000 - MADDriver->FreqExt, 0, 16000, cntlID, 0);
		
		GetDialogItem(AdapDlog, 14, &itemType, &itemHandle, &itemRect);
		speedCntl = NewControl(GetDialogWindow(AdapDlog), &itemRect, "\p", false, MADDriver->VExt, 0, 8000 * 2, cntlID, 0);
		
		GetDialogItem(AdapDlog, 43, &itemType, &itemHandle, &itemRect);
		panCntl = NewControl(GetDialogWindow(AdapDlog), &itemRect, "\p", true, MADDriver->globPan, 0, 128, cntlID2, 0);
		
		GetDialogItem(AdapDlog, 32, &itemType, &itemHandle, &cRect);
		cRect.right = cRect.left + 18;
		cRect.bottom = cRect.top + 13;
		MainFXOnOff = NewControl(	GetDialogWindow(AdapDlog), &cRect, "\p", true, 1, 0, 100, checkBoxProc, 0);
		OffsetRect(&cRect, 3 + 18, 1);
		
		for (x = 0; x < 10; x++)
		{
			cRect.right = cRect.left + 13;
			cRect.bottom = cRect.top + 13;
		
			MainFx[ x] = 	NewControl(	GetDialogWindow(AdapDlog),
											&cRect,
											"\p",
											true,
											0,
											kControlContentPictRes,
											160,
											kControlBevelButtonNormalBevelProc,
											0);
												
			OffsetRect(&cRect, 3 + 13, 0);
		}
		
/*	GetDialogItem(AdapDlog , 18, &itemType, &itemHandle, &itemRect);
	MaxBut = NewControl(	AdapDlog,
							&itemRect,
							"\p.",
							true,
							203,		//icl8 id - Flip
							-32768,
							32767,
							512,
							0);*/
	
	GetDialogItem(AdapDlog , 26, &itemType, &itemHandle, &itemRect);
	//itemRect.right = itemRect.left;
	FlipBut = NewControl(	GetDialogWindow(AdapDlog),
							&itemRect,
							"\p",
							true,
							0,
							kControlContentIconSuiteRes,
							172,
							kControlBevelButtonNormalBevelProc,
							0);

	GetDialogItem(AdapDlog , 41, &itemType, &itemHandle, &itemRect);
	//itemRect.right = itemRect.left;
	FineBut = NewControl(	GetDialogWindow(AdapDlog),
							&itemRect,
							"\p",
							true,
							0,
							kControlContentIconSuiteRes,
							212,
							kControlBevelButtonNormalBevelProc,
							0);

	GetDialogItem(AdapDlog , 25, &itemType, &itemHandle, &itemRect);
	//itemRect.right = itemRect.left;
	LoadBut = NewControl(	GetDialogWindow(AdapDlog),
							&itemRect,
							"\p",
							true,
							0,
							kControlContentIconSuiteRes,
							152,
							kControlBevelButtonNormalBevelProc,
							0);

	GetDialogItem(AdapDlog , 17, &itemType, &itemHandle, &itemRect);
	//itemRect.right = itemRect.left;
	SaveBut = NewControl(	GetDialogWindow(AdapDlog),
							&itemRect,
							"\p",
							true,
							0,
							kControlContentIconSuiteRes,
							151,
							kControlBevelButtonNormalBevelProc,
							0);

	GetDialogItem(AdapDlog , 22, &itemType, &itemHandle, &itemRect);
	//itemRect.right = itemRect.left;
	ResetSpeedBut = NewControl(	GetDialogWindow(AdapDlog),
							&itemRect,
							"\p",
							true,
							0,
							kControlContentIconSuiteRes,
							168,
							kControlBevelButtonNormalBevelProc,
							0);

	GetDialogItem(AdapDlog , 23, &itemType, &itemHandle, &itemRect);
	//itemRect.right = itemRect.left;
	ResetPitchBut = NewControl(	GetDialogWindow(AdapDlog),
							&itemRect,
							"\p",
							true,
							0,
							kControlContentIconSuiteRes,
							168,
							kControlBevelButtonNormalBevelProc,
							0);
	
	GetDialogItem(AdapDlog , 44, &itemType, &itemHandle, &itemRect);
	//itemRect.right = itemRect.left;
	ResetPanBut = NewControl(	GetDialogWindow(AdapDlog),
							&itemRect,
							"\p",
							true,
							0,
							kControlContentIconSuiteRes,
							168,
							kControlBevelButtonNormalBevelProc,
							0);
	
	
//	CreateControlAdap();
	
	
	
	TextFont(kFontIDGeneva);	TextSize(9);
	
	ResetAdaptators();
	
/*	if (thePrefs.ADAPremember)
	{
		FSSpec spec;
	
		FindFolder(kOnSystemDisk, kPreferencesFolderType, kCreateFolder, &spec.vRefNum, &spec.parID);
		pStrcpy(spec.name, "\pPlayerPRO Prefs");
		
		LoadAdaptatorsRsrc(&spec);
	}	*/
	
	SetUpScroll();
	
	if (DragManagerUse)
	{
		MyTrackingAdapUPP = NewDragTrackingHandlerUPP(MyTrackingAdap);
		MyReceiveAdapUPP = NewDragReceiveHandlerUPP(MyReceiveAdap);
		
		InstallTrackingHandler((DragTrackingHandlerUPP) MyTrackingAdapUPP, GetDialogWindow(AdapDlog), (void *) NULL);
		InstallReceiveHandler((DragReceiveHandlerUPP) MyReceiveAdapUPP, GetDialogWindow(AdapDlog), (void *) NULL);
	}
}

void CreateAdapWindow(void)
{
	Rect caRect;
	
	ShowWindow(GetDialogWindow(AdapDlog));
	SelectWindow2(GetDialogWindow(AdapDlog));
	
	SetItemMark(ViewsMenu, mAdap, checkMark);
}

void CloseAdapWindow(void)
{
	HideWindow(GetDialogWindow(AdapDlog));
	
	SetItemMark(ViewsMenu, mAdap, noMark);
}

extern EventRecord				theEvent;

pascal void actionProcAdap(ControlHandle theControl, short ctlPart)
{
	long			lRefCon;
	short			maxValue, minValue, curVal, sVal;
	RgnHandle		aRgn;
	Rect			tempRect;

	if (ctlPart <= 0) return;

	lRefCon = GetControlReference(theControl);
	maxValue = GetControlMaximum(theControl);
	minValue = GetControlMinimum(theControl);
	curVal = sVal = GetControlValue(theControl);

	switch(ctlPart)
		{
			case kControlUpButtonPart:
				curVal -= 1;
				if (curVal < minValue) curVal = minValue;
				break;
			
			case kControlDownButtonPart:
				curVal += 1;
				if (curVal > maxValue) curVal = maxValue;
				break;
			
			case kControlPageUpPart:
				curVal -= (viewRect.bottom - viewRect.top) / DISVOL;
				curVal++;
				if (curVal < minValue) curVal = minValue;
				break;
			
			case kControlPageDownPart:
				curVal += (viewRect.bottom - viewRect.top) / DISVOL;
				curVal--;
				if (curVal > maxValue) curVal = maxValue;
				break;
			
			case kControlIndicatorPart:
				sVal = gThumbPrev;
				gThumbPrev = curVal;
				break;
		}
		
		SetControlValue(theControl, curVal);
		
		if (sVal != curVal)
		{
			SetRect(&tempRect, viewRect.left, viewRect.top+1, viewRect.right, viewRect.bottom);
			
			aRgn = NewRgn();
			
			if (MADDriver->DriverSettings.outPutMode == DeluxeStereoOutPut) ScrollRect(&tempRect, 0, (sVal - curVal)*DISVOL, aRgn);
			else ScrollRect(&tempRect, 0, (sVal - curVal)*DISVOL, aRgn);
			EraseRgn(aRgn);
			InvalWindowRgn(GetDialogWindow(AdapDlog), aRgn);
			UpdateAdapWindow(AdapDlog);
			DisposeRgn(aRgn);
		}
}

void LoadAdaptatorsInt(FSSpec *file)
{
	OSErr		iErr;
	short		i, fRefNum;
	long		inOutBytes;
	GrafPtr		myPort;
	Rect		caRect;

	SaveUndo(UHeader, 0, "\pUndo 'Mixer Changes'");
	
	iErr = FSpOpenDF(file, fsCurPerm, &fRefNum);
	
	inOutBytes = sizeof(curMusic->header->chanVol);
	iErr = FSRead(fRefNum, &inOutBytes, curMusic->header->chanVol);
	
	inOutBytes = sizeof(curMusic->header->chanPan);
	iErr = FSRead(fRefNum, &inOutBytes, curMusic->header->chanPan);
	
	inOutBytes = sizeof(MADDriver->FreqExt);
	iErr = FSRead(fRefNum, &inOutBytes, &MADDriver->FreqExt);
	if (MADDriver->FreqExt <= 0) 	MADDriver->FreqExt = 1;
	if (MADDriver->FreqExt <= 160)	MADDriver->FreqExt *= 100;
	
	inOutBytes = sizeof(thePrefs.volumeLevel);
	iErr = FSRead(fRefNum, &inOutBytes, &thePrefs.volumeLevel);
	SKVolume(thePrefs.volumeLevel);
	
	inOutBytes = sizeof(MADDriver->VExt);
	iErr = FSRead(fRefNum, &inOutBytes, &MADDriver->VExt);
	if (MADDriver->VExt <= 0) 	MADDriver->VExt = 1;
	if (MADDriver->VExt <= 160)	MADDriver->VExt *= 100;
	
	inOutBytes = sizeof(MADDriver->VolGlobal);
	iErr = FSRead(fRefNum, &inOutBytes, &MADDriver->VolGlobal);
	
	inOutBytes = sizeof(MADDriver->globPan);
	iErr = FSRead(fRefNum, &inOutBytes, &MADDriver->globPan);
	if (iErr) MADDriver->globPan = 64;
	
	inOutBytes = sizeof(curMusic->header->globalEffect);
	iErr = FSRead(fRefNum, &inOutBytes, &curMusic->header->globalEffect);
	
	inOutBytes = sizeof(curMusic->header->globalFXActive);
	iErr = FSRead(fRefNum, &inOutBytes, &curMusic->header->globalFXActive);
	
	inOutBytes = sizeof(curMusic->header->chanEffect);
	iErr = FSRead(fRefNum, &inOutBytes, &curMusic->header->chanEffect);
	
	inOutBytes = sizeof(curMusic->header->chanBus);
	iErr = FSRead(fRefNum, &inOutBytes, &curMusic->header->chanBus);
	
	{
		short 	x, alpha = 0;
		
		for (i = 0; i < 10 ; i++)	// Global Effects
		{
			if (curMusic->header->globalEffect[ i])
			{
				inOutBytes = sizeof(FXSets);
				iErr = FSRead(fRefNum, &inOutBytes, &curMusic->sets[ alpha]);
				alpha++;
			}
		}
		
		for (i = 0; i < curMusic->header->numChn ; i++)	// Channel Effects
		{
			for (x = 0; x < 4; x++)
			{
				if (curMusic->header->chanEffect[ i][ x])
				{
					inOutBytes = sizeof(FXSets);
					iErr = FSRead(fRefNum, &inOutBytes, &curMusic->sets[ alpha]);
					alpha++;
				}
			}
		}
	}
	
	iErr = FSCloseFork(fRefNum);
	
	MADAttachDriverToMusic(MADDriver, curMusic, NULL);
	
	GetPort(&myPort);
	SetPortDialogPort(AdapDlog);
	
	GetPortBounds(GetDialogPort(AdapDlog), &caRect);
	
	InvalWindowRect(GetDialogWindow(AdapDlog), &caRect);
	
	SetControlVisibility(pitchCntl, true, false);		SetControlValue(pitchCntl, 16000 - MADDriver->FreqExt);SetControlVisibility(pitchCntl, false, false);
	SetControlVisibility(speedCntl, true, false);		SetControlValue(speedCntl, MADDriver->VExt);			SetControlVisibility(speedCntl, false, false);
	SetControlVisibility(hardvolCntl, true, false);	SetControlValue(hardvolCntl, thePrefs.volumeLevel);	SetControlVisibility(hardvolCntl, false, false);
	SetControlVisibility(softvolCntl, true, false);	SetControlValue(softvolCntl, MADDriver->VolGlobal);	SetControlVisibility(softvolCntl, false, false);
	SetControlValue(panCntl, MADDriver->globPan);
	
	if (MADDriver->FreqExt != 8000) HiliteControl(ResetPitchBut, 0);
	else HiliteControl(ResetPitchBut, 255);
	
	if (MADDriver->VExt != 8000) HiliteControl(ResetSpeedBut, 0);
	else HiliteControl(ResetSpeedBut, 255);
	
	for (i = 0; i < 10; i++)
	{
		if (curMusic->header->globalFXActive == true)
		{
 			if (MADDriver->masterVST[ i] != NULL) HiliteControl(MainFx[ i], kControlButtonPart);
			else HiliteControl(MainFx[ i], 0);
		}
		else
		{
			HiliteControl(MainFx[ i], 255);
		}
	}
	
	SetControlValue(MainFXOnOff, curMusic->header->globalFXActive);
	
	SetPort(myPort);
}

/*void LoadAdaptatorsNav()
{
	NavReplyRecord			theReply;
	NavDialogOptions		dialogOptions;
	NavTypeListHandle		openList;
	FSSpec					spec;
	OSErr					iErr;
	
	// default behavior for browser and dialog:
	iErr = NavGetDefaultDialogOptions(&dialogOptions);
	
	dialogOptions.dialogOptionFlags	-=	kNavAllowPreviews;
	dialogOptions.dialogOptionFlags	+=	kNavNoTypePopup;
	dialogOptions.dialogOptionFlags	-=	kNavAllowStationery;
	dialogOptions.dialogOptionFlags	+=	kNavDontAutoTranslate;
	dialogOptions.dialogOptionFlags	-=	kNavAllowMultipleFiles;
	
	pStrcpy(dialogOptions.clientName, "\pPlayerPRO");
	
	openList = (NavTypeListHandle) NewHandle(sizeof(NavTypeList) + 1 * sizeof(OSType));
	if (openList ) HLock((Handle)openList);
	
	(*openList)->componentSignature = 'SNPL';
	(*openList)->osTypeCount		= 1;
	(*openList)->osType[ 0] 		= TUNINGTYPE;
	
	iErr = NavGetFile(			NULL,	// use system's default location
								&theReply,
								&dialogOptions,
								MyDlgFilterNavDesc,
								NULL,	// no custom previews
								NULL,
								openList, //,
								(NavCallBackUserData) 2L);
	
	if (openList != NULL)
	{
		HUnlock((Handle)openList);
		DisposeHandle((Handle)openList);
	}
	
	if (theReply.validRecord && iErr == noErr)
	{
		AEDesc 	resultDesc;
		long	index, count;
		
		// we are ready to open the document(s), grab information about each file for opening:
		iErr = AECountItems(&(theReply.selection),&count);
		for (index=1;index<=count;index++)
		{
			AEKeyword keyword;
			
			if ((iErr = AEGetNthDesc(&(theReply.selection),index,typeFSS, keyword,&resultDesc)) == noErr)
			{
				if ((iErr = MyAEGetDescData (&resultDesc, NULL, &spec, sizeof (FSSpec ), NULL )) == noErr)
				
				iErr = AEDisposeDesc(&resultDesc);
			}
		}
		
		iErr = NavDisposeReply(&theReply);	// clean up after ourselves	
	}
	else iErr = -1;
	
	if (!iErr)
	{
		LoadAdaptatorsInt(&spec);
	}
}*/

void LoadAdaptators(void)
{
	FSSpec	spec;
	OSErr	iErr;
	
	iErr = DoStandardOpen(&spec, "\pmixer file", TUNINGTYPE);
	
	if (!iErr) LoadAdaptatorsInt(&spec);
}

/*void LoadAdaptatorsRsrc(FSSpec *file)
{
	OSErr	iErr;
	short	refNum;
	
	refNum = FSpOpenResFile(file, fsCurPerm);
	if (refNum != -1)
	{
		Handle rsrc;
		
		rsrc = Get1Resource('ADAP', 452);
		if (rsrc != NULL)
		{
			Ptr 	tempPtr;
			GrafPtr	myPort;
			
			DetachResource(rsrc);
			HLock(rsrc);
			
			tempPtr = *rsrc;
			
			BlockMoveData(tempPtr, curMusic->header->chanVol, sizeof(curMusic->header->chanVol));
			tempPtr += sizeof(curMusic->header->chanVol);
			
			BlockMoveData(tempPtr, curMusic->header->chanPan, sizeof(curMusic->header->chanPan));
			tempPtr += sizeof(curMusic->header->chanPan);
			
			BlockMoveData(tempPtr, &MADDriver->FreqExt, sizeof(MADDriver->FreqExt));
			tempPtr += sizeof(MADDriver->FreqExt);
			
			BlockMoveData(tempPtr, &thePrefs.volumeLevel, sizeof(thePrefs.volumeLevel));
			tempPtr += sizeof(thePrefs.volumeLevel);
			
			BlockMoveData(tempPtr, &MADDriver->VExt, sizeof(MADDriver->VExt));
			tempPtr += sizeof(MADDriver->VExt);
			
			BlockMoveData(tempPtr, &MADDriver->VolGlobal, sizeof(MADDriver->VolGlobal));
			tempPtr += sizeof(MADDriver->VolGlobal);
			
			HUnlock(rsrc);
			DisposeHandle(rsrc);
			
			GetPort(&myPort);
			SetPortDialogPort(AdapDlog);
			
			InvalWindowRect(GetDialogWindow(&AdapDlog->portRect);
			
			SetPort(myPort);
		}
		
		CloseResFile(refNum);
	}
}

void SaveAdaptatorsRsrc(FSSpec *file)
{
	OSErr	iErr;
	short	refNum;
	FInfo	fndrInfo;
	
	iErr = FSpGetFInfo(file, &fndrInfo);
	if (iErr != noErr)
	{
		fndrInfo.fdCreator = '????';
		fndrInfo.fdType = '????';
	}
	
	FSpCreateResFile(file, fndrInfo.fdCreator, fndrInfo.fdType, smSystemScript);
	
	refNum = FSpOpenResFile(file, fsCurPerm);
	if (refNum != -1)
	{
		Handle	rsrc;
		Ptr		tempPtr;
		
		// Remove previous one
		
		rsrc = Get1Resource('ADAP', 452);
		if (rsrc != NULL)
		{
			RmveResource(rsrc);
			DisposeHandle(rsrc);
		}
		
		rsrc = NewHandleClear(	sizeof(curMusic->header->chanVol) +
							sizeof(curMusic->header->chanPan) +
							sizeof(MADDriver->FreqExt) +
							sizeof(thePrefs.volumeLevel) +
							sizeof(MADDriver->VExt) +
							sizeof(MADDriver->VolGlobal));
		HLock(rsrc);
		tempPtr = *rsrc;
		
		BlockMoveData(curMusic->header->chanVol, tempPtr, sizeof(curMusic->header->chanVol));
		tempPtr += sizeof(curMusic->header->chanVol);
		
		BlockMoveData(curMusic->header->chanPan, tempPtr, sizeof(curMusic->header->chanPan));
		tempPtr += sizeof(curMusic->header->chanPan);
		
		BlockMoveData(&MADDriver->FreqExt, tempPtr, sizeof(MADDriver->FreqExt));
		tempPtr += sizeof(MADDriver->FreqExt);
		
		BlockMoveData(&thePrefs.volumeLevel, tempPtr, sizeof(thePrefs.volumeLevel));
		tempPtr += sizeof(thePrefs.volumeLevel);
		
		BlockMoveData(&MADDriver->VExt, tempPtr, sizeof(MADDriver->VExt));
		tempPtr += sizeof(MADDriver->VExt);
		
		BlockMoveData(&MADDriver->VolGlobal, tempPtr, sizeof(MADDriver->VolGlobal));
		tempPtr += sizeof(MADDriver->VolGlobal);
		
		HUnlock(rsrc);
		
		AddResource(rsrc, 'ADAP', 452, "\pPlayerPRO Mixer Settings");
		WriteResource(rsrc);
		
		DetachResource(rsrc);
		DisposeHandle(rsrc);
		
		CloseResFile(refNum);
	}
}
*/
static short TuningId = 1;

void SaveAdaptorsFile(FSSpec *file)
{
	short			itemType,i;
	ByteCount		inOutBytes;
	OSErr			iErr;
	Ptr				tempPtr;
	short			fRefNum;
	Str255			theStr, str2;

	FSpDelete(file);
	iErr = FSpCreate(file, 'SNPL', TUNINGTYPE, smSystemScript);
	if (iErr == noErr) {
		iErr = FSpOpenDF(file, fsCurPerm, &fRefNum);
		
		inOutBytes = sizeof(curMusic->header->chanVol);
		iErr = FSWriteFork(fRefNum, fsAtMark, 0, inOutBytes, curMusic->header->chanVol, &inOutBytes);
		
		inOutBytes = sizeof(curMusic->header->chanPan);
		iErr = FSWriteFork(fRefNum, fsAtMark, 0, inOutBytes, curMusic->header->chanPan, &inOutBytes);
		
		inOutBytes = sizeof(MADDriver->FreqExt);
		iErr = FSWriteFork(fRefNum, fsAtMark, 0, inOutBytes, &MADDriver->FreqExt, &inOutBytes);

		inOutBytes = sizeof(thePrefs.volumeLevel);
		iErr = FSWriteFork(fRefNum, fsAtMark, 0, inOutBytes, &thePrefs.volumeLevel, &inOutBytes);
		
		inOutBytes = sizeof(MADDriver->VExt);
		iErr = FSWriteFork(fRefNum, fsAtMark, 0, inOutBytes, &MADDriver->VExt, &inOutBytes);
		
		inOutBytes = sizeof(MADDriver->VolGlobal);
		iErr = FSWriteFork(fRefNum, fsAtMark, 0, inOutBytes, &MADDriver->VolGlobal, &inOutBytes);
		
		inOutBytes = sizeof(MADDriver->globPan);
		iErr = FSWriteFork(fRefNum, fsAtMark, 0, inOutBytes, &MADDriver->globPan, &inOutBytes);
		
		inOutBytes = sizeof(curMusic->header->globalEffect);
		iErr = FSWriteFork(fRefNum, fsAtMark, 0, inOutBytes, &curMusic->header->globalEffect, &inOutBytes);
		
		inOutBytes = sizeof(curMusic->header->globalFXActive);
		iErr = FSWriteFork(fRefNum, fsAtMark, 0, inOutBytes, &curMusic->header->globalFXActive, &inOutBytes);
		
		inOutBytes = sizeof(curMusic->header->chanEffect);
		iErr = FSWriteFork(fRefNum, fsAtMark, 0, inOutBytes, &curMusic->header->chanEffect, &inOutBytes);
		
		inOutBytes = sizeof(curMusic->header->chanBus);
		iErr = FSWriteFork(fRefNum, fsAtMark, 0, inOutBytes, &curMusic->header->chanBus, &inOutBytes);
		
		{
			short 	x, alpha = 0;
			
			for (i = 0; i < 10 ; i++)	// Global Effects
			{
				if (curMusic->header->globalEffect[ i])
				{
					inOutBytes = sizeof(FXSets);
					iErr = FSWriteFork(fRefNum, fsAtMark, 0, inOutBytes, &curMusic->sets[alpha], &inOutBytes);
					alpha++;
				}
			}
			
			for (i = 0; i < curMusic->header->numChn ; i++)	// Channel Effects
			{
				for (x = 0; x < 4; x++)
				{
					if (curMusic->header->chanEffect[ i][ x])
					{
						inOutBytes = sizeof(FXSets);
						iErr = FSWriteFork(fRefNum, fsAtMark, 0, inOutBytes, &curMusic->sets[ alpha], &inOutBytes);
						alpha++;
					}
				}
			}
		}
		
		iErr = FSCloseFork(fRefNum);
	}
}

void SaveAdaptators(void)
{
	short				itemType,i;
	long				inOutBytes;
	Point				where = { -1, -1};
	OSErr				iErr;
	GrafPtr				myPort;
	SFTypeList 			typeList;
	FSSpec				spec;
	Ptr					tempPtr;
	short				fRefNum;
	Str255				theStr, str2;

	NumToString(TuningId, str2);

	pStrcpy(theStr, "\pTuning #");
	pStrcat(theStr, str2);

	if (DoCustomSave("\pSave mixer settings as:", theStr, TUNINGTYPE, &spec)) return;

	TuningId++;

	SaveAdaptorsFile(&spec);
}

pascal void myTrackVolumeAction(ControlHandle theCntl, short ctlPart)
{
	thePrefs.volumeLevel = GetControlValue(theCntl);
	MADSetHardwareVolume(thePrefs.volumeLevel);
	
	DrawValueIndicator((100*thePrefs.volumeLevel) / 256L, true, NULL);
}

void DoItemPressAdap(short whichItem, DialogPtr whichDialog)
{
	short				x, whichFxTrack, bogus,itemType, ctlPart, i, oldH = -10;
	Point				Location, myPt;
	Handle				itemHandle;
	Rect				itemRect;
	Boolean				StereoRect, OnOffButton;
	ControlHandle		theControl;
	RgnHandle			saveClip;
	GrafPtr				savePort;
	ControlActionUPP	MyControlUPP;
	Byte				kindCntl;
	
	GetPort(&savePort);
	SetPortDialogPort(whichDialog);
	
	if (theEvent.what == mouseDown) /* See if a mouse click */
	{
		myPt = theEvent.where;
		GlobalToLocal(&myPt);
		
		theControl = NULL;
		if (TestControl(ScrollAdap, myPt)) theControl = ScrollAdap;
		
		if (theControl == ScrollAdap)
		{
			/*if (ctlPart == kControlIndicatorPart && gUseControlSize == false)
			 {
			 bogus = TrackControl(theControl, myPt, NULL);
			 if (bogus != 0)
			 {
			 EraseRect(&viewRect);
			 InvalWindowRect(GetDialogWindow(whichDialog), &viewRect);
			 }
			 }
			 else if (ctlPart > 0)*/
			{
				MyControlUPP = NewControlActionUPP(actionProcAdap);
				gThumbPrev = GetControlValue(theControl);
				TrackControl(theControl, myPt, MyControlUPP);
				DisposeControlActionUPP(MyControlUPP);
			}
		}
	}
	
	switch(whichItem)
	{
			/*	case 18:
			 if (GetControlHilite(MaxBut) == 0 && MyTrackControl(MaxBut, theEvent.where, NULL))
			 {
			 EraseGrowIcon(whichDialog);
			 
			 if (whichDialog->portRect.right == 300)
			 {
			 MySizeWindow(whichDialog, 200, whichDialog->portRect.bottom, true);
			 SetControlValue(MaxBut, 203);
			 }
			 else
			 {
			 MySizeWindow(whichDialog, 300, whichDialog->portRect.bottom, true);
			 SetControlValue(MaxBut, 204);
			 }
			 
			 EraseGrowIcon(whichDialog);
			 SetMaxWindow(whichDialog->portRect.right + 3, 0, whichDialog);
			 }
			 break;*/
			
		case 14:
			//SaveUndo(UHeader, 0, "\pUndo 'Mixer Changes'");
			
			GetMouse(&Location);
			GetDialogItem (whichDialog, 14, &itemType, &itemHandle, &itemRect);
			
			if(PtInRect(Location, &itemRect))
			{
				SetControlVisibility(pitchCntl, true, false);
				SetControlVisibility(speedCntl, true, false);
				
				while (Button())
				{
					
					
							if (QDIsPortBuffered(GetDialogPort(whichDialog)))
								QDFlushPortBuffer(GetDialogPort(whichDialog), NULL);
					WaitNextEvent(everyEvent, &theEvent, 1, NULL);
					DoGlobalNull();
					SetPortDialogPort(whichDialog);
					GetMouse(&Location);
					
					if (oldH != Location.h)
					{
						oldH = Location.h;
						
						if (Location.h < itemRect.left) 		Location.h = itemRect.left;
						else if (Location.h > itemRect.right)	Location.h = itemRect.right;
						
						MADDriver->VExt = (8000 * 2 * (Location.h - itemRect.left)) / (itemRect.right - itemRect.left);
						if (MADDriver->VExt <= 0) MADDriver->VExt = 1;
						if (MADDriver->VExt <= 160)	MADDriver->VExt = 160;
						
						SetControlValue(speedCntl, MADDriver->VExt);
						
						if (MADDriver->VExt != 8000) HiliteControl(ResetSpeedBut, 0);
						else HiliteControl(ResetSpeedBut, 255);
						
						DrawValueIndicator(100. * (MADDriver->VExt / 8000.), true, NULL);
						
						curMusic->hasChanged = true;
						
						if ((theEvent.modifiers & cmdKey) != 0)
						{
							MADDriver->FreqExt = 16000 - (16000 * (Location.h - itemRect.left)) / (itemRect.right - itemRect.left);
							if (MADDriver->FreqExt <= 0) MADDriver->FreqExt = 1;
							if (16000 - MADDriver->FreqExt <= 160)	MADDriver->FreqExt = 16000 - 160;
							if (MADDriver->FreqExt <= 160)	MADDriver->FreqExt = 160;
							
							SetControlValue(pitchCntl, 16000 - MADDriver->FreqExt);
							
							if (MADDriver->FreqExt != 8000) HiliteControl(ResetPitchBut, 0);
							else HiliteControl(ResetPitchBut, 255);
							
							DrawValueIndicator(100.*(8000./ (MADDriver->FreqExt)), true, NULL);
						}
					}
				}
				
				SetControlVisibility(pitchCntl, false, false);
				SetControlVisibility(speedCntl, false, false);
				
				DrawValueIndicator(-1, false, NULL);
			}
			break;
			
		case 43:
			//SaveUndo(UHeader, 0, "\pUndo 'Mixer Changes'");
			
			GetMouse(&Location);
			GetDialogItem (whichDialog, 43, &itemType, &itemHandle, &itemRect);
			
			if(PtInRect(Location, &itemRect))
			{
				while (Button())
				{
					WaitNextEvent(everyEvent, &theEvent, 1, NULL);
							if (QDIsPortBuffered(GetDialogPort(whichDialog)))
								QDFlushPortBuffer(GetDialogPort(whichDialog), NULL);
    				
					DoGlobalNull();
					SetPortDialogPort(whichDialog);
					GetMouse(&Location);
					
					if (oldH != Location.h)
					{
						oldH = Location.h;
						
						if (Location.h < itemRect.left) 		Location.h = itemRect.left;
						else if (Location.h > itemRect.right)	Location.h = itemRect.right;
						
						MADDriver->globPan = (64 * 2 * (Location.h - itemRect.left)) / (itemRect.right - itemRect.left);
						if (MADDriver->globPan <= 0) MADDriver->globPan = 0;
						
						SetControlValue(panCntl, MADDriver->globPan);
						
						if (MADDriver->globPan != 64) HiliteControl(ResetPanBut, 0);
						else HiliteControl(ResetPanBut, 255);
						
						DrawValueIndicator(100.*((MADDriver->globPan - 64)/64.), true, NULL);
						
						curMusic->hasChanged = true;
					}
				}
				
				DrawValueIndicator(-1, false, NULL);
			}
			break;
			
		case 12:
			//SaveUndo(UHeader, 0, "\pUndo 'Mixer Changes'");
			
			GetMouse(&Location);
			GetDialogItem (whichDialog, 12, &itemType, &itemHandle, &itemRect);
			
			if(PtInRect(Location, &itemRect))
			{
				SetControlVisibility(pitchCntl, true, false);
				SetControlVisibility(speedCntl, true, false);
				
				while (Button())
				{
					WaitNextEvent(everyEvent, &theEvent, 1, NULL);
					
						if (QDIsPortBuffered(GetDialogPort(whichDialog)))
								QDFlushPortBuffer(GetDialogPort(whichDialog), NULL);
    				
					DoGlobalNull();
					SetPortDialogPort(whichDialog);
					
					GetMouse(&Location);
					
					if (oldH != Location.h)
					{
						oldH = Location.h;
						
						if (Location.h < itemRect.left) 		Location.h = itemRect.left;
						else if (Location.h > itemRect.right)	Location.h = itemRect.right;
						
						MADDriver->FreqExt = 16000 - (16000 * (Location.h - itemRect.left)) / (itemRect.right - itemRect.left);
						if (MADDriver->FreqExt <= 0) MADDriver->FreqExt = 1;
						if (16000 - MADDriver->FreqExt <= 160)	MADDriver->FreqExt = 16000 - 160;
						if (MADDriver->FreqExt <= 160)	MADDriver->FreqExt = 160;
						
						SetControlValue(pitchCntl, 16000 - MADDriver->FreqExt);
						
						if (MADDriver->FreqExt != 8000) HiliteControl(ResetPitchBut, 0);
						else HiliteControl(ResetPitchBut, 255);
						
						DrawValueIndicator(100.*(8000./ (MADDriver->FreqExt)), true, NULL);
						
						curMusic->hasChanged = true;
						
						if ((theEvent.modifiers & cmdKey) != 0)
						{
							MADDriver->VExt = (8000 * 2 * (Location.h - itemRect.left)) / (itemRect.right - itemRect.left);
							if (MADDriver->VExt <= 0) MADDriver->VExt = 1;
							if (MADDriver->VExt <= 160)	MADDriver->VExt = 160;
							
							SetControlValue(speedCntl, MADDriver->VExt);
							
							if (MADDriver->VExt != 8000) HiliteControl(ResetSpeedBut, 0);
							else HiliteControl(ResetSpeedBut, 255);
							
							DrawValueIndicator(100. * (MADDriver->VExt / 8000.), true, NULL);
						}
					}
				}
				
				SetControlVisibility(pitchCntl, false, false);
				SetControlVisibility(speedCntl, false, false);
				
				DrawValueIndicator(-1, false, NULL);
			}
			break;
			
		case 26:
			SaveUndo(UHeader, 0, "\pUndo 'Mixer Changes'");
			
			if (GetControlHilite(FlipBut) == 0 && MyTrackControl(FlipBut, theEvent.where, NULL))
			{
				for (i = 1; i < gCurrentTrack; i++)
				{
					if (i % 2 == 0) curMusic->header->chanPan[ i] = curMusic->header->chanPan[ 0];
					else curMusic->header->chanPan[ i] = MAX_PANNING -curMusic->header->chanPan[ 0];
				}
				
				InvalWindowRect(GetDialogWindow(whichDialog), &viewRect);
				
				curMusic->hasChanged = true;
			}
			break;
			
		case 41:
			if (GetControlHilite(FineBut) == 0  && MyTrackControl(FineBut, theEvent.where, NULL))
			{
				FineSpeedPitchSettings();
			}
			break;
			
		case 25:
			if (GetControlHilite(LoadBut) == 0  && MyTrackControl(LoadBut, theEvent.where, NULL))
			{
				LoadAdaptators();
			}
			break;
			
		case 17:
			if (GetControlHilite(SaveBut) == 0  && MyTrackControl(SaveBut, theEvent.where, NULL))
			{
				SaveAdaptators();
			}
			break;
			
		case 44:
			//SaveUndo(UHeader, 0, "\pUndo 'Mixer Changes'");
			
			if (GetControlHilite(ResetPanBut) == 0  && MyTrackControl(ResetPanBut, theEvent.where, NULL))
			{
				HiliteControl(ResetPanBut, 255);
				
				MADDriver->globPan = 64;
				
				SetControlValue(panCntl, MADDriver->globPan);
				
				curMusic->hasChanged = true;
			}
			break;
			
		case 22:
			//SaveUndo(UHeader, 0, "\pUndo 'Mixer Changes'");
			
			if (GetControlHilite(ResetSpeedBut) == 0  && MyTrackControl(ResetSpeedBut, theEvent.where, NULL))
			{
				HiliteControl(ResetSpeedBut, 255);
				
				MADDriver->VExt = 8000;
				
				SetControlVisibility(speedCntl, true, false);
				
				SetControlValue(speedCntl, MADDriver->VExt);
				
				SetControlVisibility(speedCntl, false, false);
				
				curMusic->hasChanged = true;
			}
			break;
			
		case 23:
			//SaveUndo(UHeader, 0, "\pUndo 'Mixer Changes'");
			
			if (GetControlHilite(ResetPitchBut) == 0  && MyTrackControl(ResetPitchBut, theEvent.where, NULL))
			{
				HiliteControl(ResetPitchBut, 255);
				
				MADDriver->FreqExt = 8000;
				
				SetControlVisibility(pitchCntl, true, false);
				
				SetControlValue(pitchCntl, MADDriver->FreqExt);
				
				SetControlVisibility(pitchCntl, false, false);
				
				curMusic->hasChanged = true;
			}
			break;
			
			/*	case 23:
			 if (GetControlHilite(PrefBut)	 == 0 && MyTrackControl(PrefBut, theEvent.where, NULL))
			 {
			 #include "Help.h"
			 
			 ShowPrefs(COMPRESSION);
			 }
			 break;*/
			
		case 10:	// Volume HARDWARE
			//SaveUndo(UHeader, 0, "\pUndo 'Mixer Changes'");
			
			GetMouse(&Location);
			GetDialogItem (whichDialog, 10, &itemType, &itemHandle, &itemRect);
			
			if(PtInRect(Location, &itemRect))
			{
				SetControlVisibility(hardvolCntl, true, false);
				
				while (Button())
				{
					WaitNextEvent(everyEvent, &theEvent, 1, NULL);
					
							if (QDIsPortBuffered(GetDialogPort(whichDialog)))
								QDFlushPortBuffer(GetDialogPort(whichDialog), NULL);
    				
					//	DoGlobalNull();
					SetPortDialogPort(whichDialog);
					GetMouse(&Location);
					
					if (oldH != Location.h)
					{
						oldH = Location.h;
						
						if (Location.h < itemRect.left) 		Location.h = itemRect.left;
						else if (Location.h > itemRect.right)	Location.h = itemRect.right;
						
						thePrefs.volumeLevel = (430* (Location.h - itemRect.left)) / (itemRect.right-itemRect.left);
						
						SKVolume(thePrefs.volumeLevel);
						
						SetControlValue(hardvolCntl, thePrefs.volumeLevel);
						
						DrawValueIndicator((100*thePrefs.volumeLevel) / 256L, true, NULL);
					}
				}
				
				SetControlVisibility(hardvolCntl, false, false);
				
				DrawValueIndicator(-1, false, NULL);
			}
			break;
			
		case 8:	// Volume SOFTWARE
			//SaveUndo(UHeader, 0, "\pUndo 'Mixer Changes'");
			
			GetMouse(&Location);
			GetDialogItem (whichDialog, 8, &itemType, &itemHandle, &itemRect);
			
			if(PtInRect(Location, &itemRect))
			{
				SetControlVisibility(softvolCntl, true, false);
				
				while (Button())
				{
					WaitNextEvent(everyEvent, &theEvent, 1, NULL);
					
							if (QDIsPortBuffered(GetDialogPort(whichDialog)))
								QDFlushPortBuffer(GetDialogPort(whichDialog), NULL);
    				
					DoGlobalNull();
					SetPortDialogPort(whichDialog);
					GetMouse(&Location);
					
					if (oldH != Location.h)
					{
						oldH = Location.h;
						
						if (Location.h < itemRect.left) 		Location.h = itemRect.left;
						else if (Location.h > itemRect.right)	Location.h = itemRect.right;
						
						MADDriver->VolGlobal = (128* (Location.h - itemRect.left)) / (itemRect.right-itemRect.left);
						
						SetControlValue(softvolCntl, MADDriver->VolGlobal);
						
						DrawValueIndicator((100*MADDriver->VolGlobal)/64, true, NULL);
					}
				}
				
				SetControlVisibility(softvolCntl, false, false);
				
				DrawValueIndicator(-1, false, NULL);
				
				curMusic->hasChanged = true;
			}
			break;
			
		case 32:
			myPt = theEvent.where;
			GlobalToLocal(&myPt);
			
			for (i = 0; i < 10; i++)
			{
				Rect contrlRect;
				
				GetControlBounds(MainFx[ i], &contrlRect);
				
				if (PtInRect(myPt, &contrlRect) && curMusic->header->globalFXActive == true)
				{
					long		mresult, temp;
					Point		Zone;
					short		item;
					
					InsertMenu(VSTMenu, hierMenu);
					
					Zone.h = contrlRect.left;
					Zone.v = contrlRect.top;
					
					LocalToGlobal(&Zone);
					
					if (MADDriver->masterVST[ i])
					{
						item = MADDriver->masterVST[ i]->id+3;
						SetItemMark(VSTMenu, item, diamondMark);
					}
					else item = 0;
					
					mresult = PopUpMenuSelect(	VSTMenu,
											  Zone.v,
											  Zone.h,
											  item);
					
					if (item)
					{
						SetItemMark(VSTMenu, item, noMark);
					}
					
					DeleteMenu(GetMenuID(VSTMenu));
					
					if (HiWord(mresult ) != 0 )
					{
						temp = LoWord(mresult );
						
						switch(temp)
						{
							case 1:	// No Effect 
								if (MADDriver->masterVST[ i])
								{
									CheckVSTEditor(MADDriver->masterVST[ i]);
									DisposeVSTEffect(MADDriver->masterVST[ i]);
									MADDriver->masterVST[ i] = NULL;
								}
								break;
								
							default:
								if (MADDriver->masterVST[ i])
								{
									if (temp-3 != MADDriver->masterVST[ i]->id)
									{
										DisposeVSTEffect(MADDriver->masterVST[ i]);
										MADDriver->masterVST[ i] = NULL;
									}
								}
								
								HandleVSTChoice(temp-3, &MADDriver->masterVST[ i], -1);
								break;
						}
						
						if (MADDriver->masterVST[ i] != NULL) HiliteControl(MainFx[ i], kControlButtonPart);
						else HiliteControl(MainFx[ i], 0);
						
						curMusic->hasChanged = true;
					}
				}
			}
			
			if (MyTrackControl(MainFXOnOff, theEvent.where, NULL))
			{
				Rect contrlRect;
				
				GetControlBounds(MainFXOnOff, &contrlRect);
				if (PtInRect(myPt, &contrlRect))
				{
					curMusic->header->globalFXActive = !curMusic->header->globalFXActive;
					
					SetControlValue(MainFXOnOff, curMusic->header->globalFXActive);
					
					ResetAdaptators();
				}
			}
			
			FillVSTEffects();
			break;
			
		case 2:
		case 16:	// Volume Voice
			SaveUndo(UHeader, 0, "\pUndo 'Mixer Changes'");
			
			myPt = theEvent.where;
			GlobalToLocal(&myPt);
			
			theControl = NULL;
			
			for (i = 0; i < gCurrentTrack; i++)
			{
				Rect contrlRect;
				
				GetControlBounds(VSTDest[ i], &contrlRect);
				
				if (PtInRect(myPt, &contrlRect)) {	kindCntl = 'D';	theControl = VSTDest[ i];	}
				
				GetControlBounds(VSTOnOff[ i], &contrlRect);
				
				if (PtInRect(myPt, &contrlRect)) {	kindCntl = 'E';	theControl = VSTOnOff[ i];	}
				
				GetControlBounds(volCntl[ i], &contrlRect);
				
				if (PtInRect(myPt, &contrlRect)) {	kindCntl = 'V';	theControl = volCntl[ i];	}
				
				GetControlBounds(pannCntl[ i], &contrlRect);
				
				if (PtInRect(myPt, &contrlRect)) {	kindCntl = 'P';	theControl = pannCntl[ i];	}
				
				GetControlBounds(OnOff[ i], &contrlRect);
				
				if (PtInRect(myPt, &contrlRect)) {	kindCntl = 'O';	theControl = OnOff[ i];	}
				
				GetControlBounds(Mono[ i], &contrlRect);
				
				if (PtInRect(myPt, &contrlRect)) {	kindCntl = 'M';	theControl = Mono[ i];	}
				
				for (x = 0; x < 4; x++)
				{
					GetControlBounds(VSTFx[ i][ x], &contrlRect);
					
					if (PtInRect(myPt, &contrlRect)) {	kindCntl = 'F';	theControl = VSTFx[ i][ x];	whichFxTrack = x;}
				}
				
				if (theControl) break;
			}
			
			if (theControl)
			{
				Rect contrlRect;
				
				i = GetControlReference(theControl);
				
				saveClip = NewRgn();
				GetClip(saveClip);
				ClipRect(&viewRect);
				
				GetControlBounds(theControl, &contrlRect);
				
				itemRect = contrlRect;
				
				curMusic->hasChanged = true;
				
				switch(kindCntl)
				{
						
					case 'D':
						if (curMusic->header->chanBus[ i].Active == true)
						{
							long		mresult, temp;
							Point		Zone;
							short		item;
							MenuHandle	VSTDestMenu;
							Str255		myStr, myStr2;
							
							VSTDestMenu = GetMenu(181);
							
							for (x = 0; x < curMusic->header->numChn; x++)
							{
								pStrcpy(myStr, "\pUse settings of track ");
								NumToString(x+1, myStr2);
								pStrcat(myStr, myStr2);
								
								AppendMenu(VSTDestMenu, myStr);
							}
							
							DisableMenuItem(VSTDestMenu, i + 4);
							
							for (x = 0; x < curMusic->header->numChn; x++)
							{
								if (curMusic->header->chanBus[ x].copyId != x) DisableMenuItem(VSTDestMenu, x + 4);
							}
							
							InsertMenu(VSTDestMenu, hierMenu);
							
							Zone.h = itemRect.left;
							Zone.v = itemRect.top;
							
							LocalToGlobal(&Zone);
							
							if (curMusic->header->chanBus[ i].copyId != i)
							{
								item = curMusic->header->chanBus[ i].copyId + 4;
							}
							else
							{
								if (curMusic->header->chanBus[ i].ByPass) item = 2;
								else item = 1;
							}
							
							SetItemMark(VSTDestMenu, item, diamondMark);
							
							mresult = PopUpMenuSelect(	VSTDestMenu,
													  Zone.v,
													  Zone.h,
													  item);
							
							if (item)
							{
								SetItemMark(VSTDestMenu, item, noMark);
							}
							
							DeleteMenu(GetMenuID(VSTDestMenu));
							DisposeMenu(VSTDestMenu);
							
							if (HiWord(mresult ) != 0 )
							{
								temp = LoWord(mresult );
								
								switch(temp)
								{
									case 1:
										curMusic->header->chanBus[ i].copyId = i;
										curMusic->header->chanBus[ i].ByPass = false;
										break;
										
									case 2:
										curMusic->header->chanBus[ i].copyId = i;
										curMusic->header->chanBus[ i].ByPass = true;
										break;
										
									default:
										curMusic->header->chanBus[ i].copyId = temp -4;
										curMusic->header->chanBus[ i].ByPass = false;
										
										for (x = 0; x < curMusic->header->numChn; x++)
										{
											if (curMusic->header->chanBus[ x].copyId == i) curMusic->header->chanBus[ x].copyId = x;
										}
										break;
								}
								
								FillFxDestControl(i);
								
								ResetAdaptators();
							}
						}
						break;
						
						
					case 'F':
						if (curMusic->header->chanBus[ i].copyId == i && curMusic->header->chanBus[ i].Active == true)
						{
							long		mresult, temp;
							Point		Zone;
							short		item;
							
							InsertMenu(VSTMenu, hierMenu);
							
							Zone.h = itemRect.left;
							Zone.v = itemRect.top;
							
							LocalToGlobal(&Zone);
							
							if (MADDriver->chanVST[ i][ whichFxTrack])
							{
								item = MADDriver->chanVST[ i][ whichFxTrack]->id+3;
								SetItemMark(VSTMenu, item, diamondMark);
							}
							else item = 0;
							
							mresult = PopUpMenuSelect(	VSTMenu,
													  Zone.v,
													  Zone.h,
													  item);
							
							if (item)
							{
								SetItemMark(VSTMenu, item, noMark);
							}
							
							DeleteMenu(GetMenuID(VSTMenu));
							
							if (HiWord(mresult ) != 0 )
							{
								temp = LoWord(mresult );
								
								switch(temp)
								{
									case 1:	// No Effect 
										if (MADDriver->chanVST[ i][ whichFxTrack])
										{
											CheckVSTEditor(MADDriver->chanVST[ i][ whichFxTrack]);
											DisposeVSTEffect(MADDriver->chanVST[ i][ whichFxTrack]);
											MADDriver->chanVST[ i][ whichFxTrack] = NULL;
										}
										break;
										
									default:
										if (MADDriver->chanVST[ i][ whichFxTrack])
										{
											if (temp-3 != MADDriver->chanVST[ i][ whichFxTrack]->id)
											{
												DisposeVSTEffect(MADDriver->chanVST[ i][ whichFxTrack]);
												MADDriver->chanVST[ i][ whichFxTrack] = NULL;
											}
										}
										
										SetClip(saveClip);
										HandleVSTChoice(temp-3, &MADDriver->chanVST[ i][ whichFxTrack], i);
										ClipRect(&viewRect);
										break;
								}
								
								FillFxControl(i);
							}
						}
						
						FillVSTEffects();
						break;
						
					case 'M':
						if (GetControlHilite(Mono[ i]) == 0 && MyTrackControl(Mono[ i], theEvent.where, NULL))
						{
							if (IsPressed(0x3A))
							{
								short		ww;
								Boolean		newVal;
								
								if (GetControlHilite(Mono[ i]) == false) newVal = true;
								else newVal = false;
								
								for (ww = 0; ww < gCurrentTrack; ww++)
								{
									FillMonoControl(ww);
									
									if (newVal) curMusic->header->chanPan[ ww] = 32;
									FillBarStereo(ww, curMusic->header->chanPan[ ww], MAX_PANNING);
								}
							}
							else
							{
								FillMonoControl(i);
								
								curMusic->header->chanPan[ i] = 32;
								FillBarStereo(i, curMusic->header->chanPan[ i], MAX_PANNING);
							}
						}
						break;
						
					case 'V':
						//	if (GetControlHilite(volCntl[ i]) == 0)
					{
						/*	if (!AppearanceManager)
						 {
						 if (MyTrackControl(volCntl[ i], theEvent.where, NULL))
						 {
						 
						 }
						 }
						 else*/
						{
							while (Button())
							{
								WaitNextEvent(everyEvent, &theEvent, 1, NULL);
								
								if (QDIsPortBuffered(GetDialogPort(whichDialog)))
									QDFlushPortBuffer(GetDialogPort(whichDialog), NULL);
								
								DoGlobalNull();
								SetPortDialogPort(whichDialog);
								GetMouse(&Location);
								
								if (oldH != Location.h)
								{
									oldH = Location.h;
									
									if (Location.h < itemRect.left) 		Location.h = itemRect.left;
									else if (Location.h > itemRect.right)	Location.h = itemRect.right;
									
									if (IsPressed(0x3A))
									{
										short	ww;
										
										for (ww = 0; ww < gCurrentTrack; ww++)
										{
											curMusic->header->chanVol[ ww] = (MAX_CHANVOL * (Location.h - itemRect.left)) / (itemRect.right - itemRect.left);
											FillBarControl(ww, curMusic->header->chanVol[ ww], MAX_CHANVOL);
										}
									}
									else
									{
										curMusic->header->chanVol[ i ] = (MAX_CHANVOL * (Location.h - itemRect.left)) / (itemRect.right - itemRect.left);
										FillBarControl( i, curMusic->header->chanVol[ i], MAX_CHANVOL);
									}
									
									DrawValueIndicator((100*curMusic->header->chanVol[ i])/64L, true, NULL);
								}
							}
							
							DrawValueIndicator(-1, false, NULL);
						}
					}
						break;
						
					case 'P':
						//	if (GetControlHilite(pannCntl[ i]) == 0)
					{
						/*	if (!AppearanceManager) 
						 {
						 if (MyTrackControl(pannCntl[ i], theEvent.where, NULL))
						 {
						 
						 }
						 }*/
						if (GetControlHilite(Mono[ i]) == 0)
						{
							do
							{
								DoGlobalNull();
								SetPortDialogPort(whichDialog);
								GetMouse(&Location);
								
								if (oldH != Location.h)
								{
									oldH = Location.h;
									
									if (Location.h < itemRect.left) 		Location.h = itemRect.left;
									else if (Location.h > itemRect.right)	Location.h = itemRect.right;
									
									MADDriver->chan[ i].pann = curMusic->header->chanPan[ i] = ((Location.h - itemRect.left) * MAX_PANNING) / (itemRect.right - itemRect.left);
									FillBarStereo(i, curMusic->header->chanPan[ i], MAX_PANNING);
									
									if (IsPressed(0x3A))
									{
										short	ww;
										short	OnOff;
										
										if (i % 2 == 0 ) OnOff = 0;
										else OnOff = 1;
										
										for (ww = 0; ww < gCurrentTrack; ww++)
										{
											if (GetControlHilite(Mono[ ww]) == 0)
											{
												if (ww % 2 == OnOff) MADDriver->chan[ ww].pann = curMusic->header->chanPan[ ww] = curMusic->header->chanPan[ i];
												else MADDriver->chan[ ww].pann = curMusic->header->chanPan[ ww] = MAX_PANNING -curMusic->header->chanPan[ i];
												
												FillBarStereo(ww, curMusic->header->chanPan[ ww], MAX_PANNING);
											}
										}
									}
									
									DrawValueIndicator(100.*((curMusic->header->chanPan[ i] - 32) / 32.), true, NULL);
								}
								WaitNextEvent(everyEvent, &theEvent, 1, NULL);
								
								if (QDIsPortBuffered(GetDialogPort(whichDialog)))
									QDFlushPortBuffer(GetDialogPort(whichDialog), NULL);
								
							}while (Button());
							
							DrawValueIndicator(-1, false, NULL);
						}
					}
						break;
						
					case 'E':
						
						SetControlVisibility(VSTOnOff[ i], true, false);
						if (GetControlHilite(VSTOnOff[ i]) == 0 && MyTrackControl(VSTOnOff[ i], theEvent.where, NULL))
						{
							if (IsPressed(0x3A))
							{
								short		ww, noActive;
								Boolean		newVal;
								
								for (ww = 0, noActive = 0; ww < curMusic->header->numChn; ww++)
								{
									if (curMusic->header->chanBus[ ww].Active == true)
									{
										noActive++;
									}
								}
								if (noActive <= 1 && curMusic->header->chanBus[ i].Active == true) newVal = true;
								else newVal = false;
								
								for (ww = 0; ww < gCurrentTrack; ww++)
								{
									curMusic->header->chanBus[ ww].Active = newVal;
								}
								
								if (newVal == false)
								{
									curMusic->header->chanBus[ i].Active = !newVal;
								}
							}
							else
							{
								curMusic->header->chanBus[ i].Active = !curMusic->header->chanBus[ i].Active;
							}
							
							ResetAdaptators();
						}
						break;
						
					case 'O':
						if (GetControlHilite(OnOff[ i]) == 0 && MyTrackControl(OnOff[ i], theEvent.where, NULL))
						{
							if (IsPressed(0x3A))
							{
								short		ww, noActive;
								Boolean		newVal;
								
								for (ww = 0, noActive = 0; ww < curMusic->header->numChn; ww++)
								{
									if (MADDriver->Active[ ww] == true)
									{
										noActive++;
									}
								}
								if (noActive <= 1 && MADDriver->Active[ i] == true) newVal = true;
								else newVal = false;
								
								for (ww = 0; ww < gCurrentTrack; ww++)
								{
									MADDriver->Active[ ww] = newVal;
									
									FillOnOffControl(ww, MADDriver->Active[ ww]);
									FillBarControl( ww, curMusic->header->chanVol[ ww], MAX_CHANVOL);
									FillBarStereo(ww, curMusic->header->chanPan[ ww], MAX_PANNING);
								}
								
								if (newVal == false)
								{
									MADDriver->Active[ i] = !newVal;
									
									FillOnOffControl(i, MADDriver->Active[ i]);
									FillBarControl( i, curMusic->header->chanVol[ i], MAX_CHANVOL);
									FillBarStereo(i, curMusic->header->chanPan[ i], MAX_PANNING);
								}
							}
							else
							{
								MADDriver->Active[ i] = !MADDriver->Active[ i];
								
								FillOnOffControl(i, MADDriver->Active[ i]);
								FillBarControl( i, curMusic->header->chanVol[ i], MAX_CHANVOL);
								FillBarStereo(i, curMusic->header->chanPan[ i], MAX_PANNING);
							}
							
							UPDATE_TrackActive();
						}
						break;
				}
				
				SetClip(saveClip);
				DisposeRgn(saveClip);
			}
			break;
	}
	
	SetPort(savePort);
}

pascal OSErr MyTrackingAdap(short message, WindowPtr theWindow, void *handlerRefCon, DragReference theDrag)
{
	short				result, offset, i;
	long				theTime = TickCount();
	unsigned short		count, index;
	unsigned long		flavorFlags, attributes;
	ItemReference		theItem;
	RgnHandle			theRgn;
	Point				theMouse, localMouse, theCell;
	Rect				tempRect;

	if (!mainSystemDrag) return noErr;

	if ((message != kDragTrackingEnterHandler) && (!canAcceptDrag)) return(noErr);

	SetPortWindowPort(theWindow);
	
	GetDragAttributes(theDrag, &attributes);
	
	switch (message) {

		case kDragTrackingEnterHandler:
		{
			FlavorFlags     	theFlags;
			long				textSize;
			HFSFlavor			myFlavor;
			FInfo				fndrInfo;
			
			canAcceptDrag = false;
		
			GetDragItemReferenceNumber(theDrag, 1, &theItem);
    
			result = GetFlavorFlags(theDrag, theItem, flavorTypeHFS, &theFlags);
			if (result == noErr)
			{
				Boolean		targetIsFolder, wasAliased;
				
				GetFlavorDataSize(theDrag, theItem, flavorTypeHFS, &textSize);
				
				GetFlavorData(theDrag, theItem, flavorTypeHFS, &myFlavor, &textSize, 0);
				
				ResolveAliasFile(&myFlavor.fileSpec, true, &targetIsFolder, &wasAliased);
				
				HSetVol(NULL, myFlavor.fileSpec.vRefNum, myFlavor.fileSpec.parID);
				result = FSpGetFInfo(&myFlavor.fileSpec, &fndrInfo);
				if (result != noErr) return false;		// <- Folder
				
				if (fndrInfo.fdType == TUNINGTYPE) canAcceptDrag = true;
			}
		}
		break;

		case kDragTrackingEnterWindow:
			
		break;

		case kDragTrackingInWindow:
			//
			//	We receive InWindow messages as long as the mouse is in one of our windows
			//	during a drag. We draw the window highlighting and blink the insertion caret
			//	when we get these messages.
			//

			GetDragMouse(theDrag, &theMouse, NULL);
			localMouse = theMouse;
			GlobalToLocal(&localMouse);

			//
			//	Show or hide the window highlighting when the mouse enters or leaves the
			//	TextEdit field in our window (we don't want to show the highlighting when
			//	the mouse is over the window title bar or over the scroll bars).
			//
			{
			Rect	caRect;
	
			GetPortBounds(GetDialogPort(AdapDlog), &caRect);
			
			if (PtInRect(localMouse, &caRect))
			{
				RectRgn(theRgn = NewRgn(), &caRect);
				
				ShowDragHilite(theDrag, theRgn, true);
				DisposeRgn(theRgn);
			}
			}
			//
			//	If this application is the sender, do not allow tracking through
			//	the selection in the window that sourced the drag.
			//

			if (attributes & kDragInsideSenderWindow)
			{
			
			}
		break;

		case kDragTrackingLeaveWindow:
			HideDragHilite(theDrag);
		break;

		case kDragTrackingLeaveHandler:
		
		break;
	}
	RGBBackColor(&theColor);

	return(noErr);
}

pascal OSErr MyReceiveAdap(WindowPtr theWindow, void* handlerRefCon, DragReference theDrag)
{
	FSSpec				pseudoFF;
	HFSFlavor			myFlavor;
	OSErr				result;
	Handle				mySnd;
	Rect				theRect, srcRect;
	unsigned short		items;
	ItemReference		theItem;
	DragAttributes		attributes;
	Ptr					textData;
	StScrpHandle		stylHandle;
	Size				textSize;
	short				offset, selStart, index, mouseDownModifiers, mouseUpModifiers, moveInstru;
	Point				theCell;
	Str255				myStr;
	FInfo				fndrInfo;
	CInfoPBRec			info;
	OSErr				err;

	if (!mainSystemDrag) return dragNotAcceptedErr;
	if (!canAcceptDrag) return dragNotAcceptedErr;

	SetPortWindowPort(theWindow);
	
	GetDragAttributes(theDrag, &attributes);
	GetDragModifiers(theDrag, NULL, &mouseDownModifiers, &mouseUpModifiers);
		
	HideDragHilite(theDrag);

	//
	//	Un fichier en provenance du Finder
	//
	
	CountDragItems(theDrag, &items);

	SetCursor(&watchCrsr);

	for (index = 1; index <= items; index++)
	{
		GetDragItemReferenceNumber(theDrag, index, &theItem);
		
		result = GetFlavorDataSize(theDrag, theItem, flavorTypeHFS, &textSize);
	
		if (result == noErr)
		{
			Boolean		targetIsFolder, wasAliased;
		
			GetFlavorData(theDrag, theItem, flavorTypeHFS, &myFlavor, &textSize, 0);
			
			ResolveAliasFile(&myFlavor.fileSpec, true, &targetIsFolder, &wasAliased);
			
			err = HGetFInfo(myFlavor.fileSpec.vRefNum, myFlavor.fileSpec.parID, myFlavor.fileSpec.name, &fndrInfo);
		
			if (err == noErr)
			{
				LoadAdaptatorsInt(&myFlavor.fileSpec);
			}
			else
			{
				SetCursor(GetQDGlobalsArrow(&qdarrow));
				return(dragNotAcceptedErr);
			} 
		}
	}
	
	SetCursor(GetQDGlobalsArrow(&qdarrow));
	
	return noErr;
}
