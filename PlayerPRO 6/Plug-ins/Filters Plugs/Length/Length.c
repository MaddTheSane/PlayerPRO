/*	Length	*/
/*	v 1.0			*/
/*	1999 by ANR 	*/

#include <PlayerPROCore/PlayerPROCore.h>

static void ControlSwitch(short item, DialogPtr dlog, short Switch)
{
	Handle		itemHandle;
	short		itemType;
	Rect		itemRect;

	GetDialogItem (dlog, item, &itemType, &itemHandle, &itemRect);
	HiliteControl((ControlHandle) itemHandle, Switch);
}

static void AutoPosition(DialogPtr aDia)
{
	Point		Position, mouse;
	Rect		ViewRect, caRect;
	short		XSize, YSize;
	GDHandle	aH;
	BitMap		screenBits;
	
	GetMouse(&mouse);
	LocalToGlobal(&mouse);
	
	GetPortBounds(GetDialogPort(aDia), &caRect);
	
	XSize = (caRect.right - caRect.left);
	YSize = (caRect.bottom - caRect.top);
	
	GetQDGlobalsScreenBits(&screenBits);
	
	SetRect(&ViewRect, screenBits.bounds.left + 8, screenBits.bounds.top + 43,
						screenBits.bounds.right - 8, screenBits.bounds.bottom - 8);
	
	aH = GetDeviceList();
	do
	{
		aH = GetNextDevice(aH);
		if (aH != NULL)
		{
			if (PtInRect(mouse, &(*(*aH)->gdPMap)->bounds))
			{
				Rect	ar = (*(*aH)->gdPMap)->bounds;
			
				SetRect(&ViewRect, ar.left + 8, ar.top + 43,
									ar.right - 8, ar.bottom - 8);
			}
		}
	}
	while(aH != NULL);
	
	Position.h = mouse.h - XSize/2;
	if (Position.h + XSize >= ViewRect.right) Position.h = ViewRect.right - XSize;
	else if (Position.h <= ViewRect.left) Position.h = ViewRect.left;

	Position.v = mouse.v - YSize/2;
	if (Position.v + YSize >= ViewRect.bottom) Position.v = ViewRect.bottom - YSize;
	else if (Position.v <= ViewRect.top) Position.v = ViewRect.top;

	SetDialogDefaultItem(aDia, 1 );
	SetDialogCancelItem(aDia, 2 );

	MoveWindow(GetDialogWindow(aDia), Position.h, Position.v, false);

	ShowWindow(GetDialogWindow(aDia));
}

static void GetDText (DialogPtr dlog, short item, StringPtr str)
{
	Handle	itemHandle;
	short	itemType;
	Rect	itemRect;

	GetDialogItem (dlog, item, &itemType, &itemHandle, &itemRect);
	GetDialogItemText (itemHandle, str);
}

static void SetDText (DialogPtr dlog, short item, Str255 str)
{
	ControlHandle	control;

	GetDialogItemAsControl(dlog, item, &control );
	SetControlData(control, 0, kControlStaticTextTextTag, str[0], (Ptr)(str+1) );
	DrawOneControl(control);
}

static Ptr ConvertSampleSize(Ptr src, long srcSize, short amp, long dstSize, Boolean stereo)
{
	#define LRVAL	3L

	short			*src16 = (short*)src, *dst16;
	char			*src8 = (char*)src, *dst8;
	Ptr				dst;
	long			realsrcSize, tempL, tempR;
	unsigned long	x, left, right, pos, newSize = dstSize;
	
	dst = NewPtrClear(newSize);
	if (dst == NULL) return NULL;
	
	realsrcSize = srcSize;
	
	srcSize /= 100;
	dstSize /= 100;
	
	dst16 = (short*) dst;
	dst8 = (char*) dst;
		
	switch(amp)
	{
	case 8:
		for (x = 0; x < newSize; x++)
		{
			pos			= (x * srcSize << LRVAL) / dstSize;
			right		= pos & ((1 << LRVAL)-1);
			left		= (1 << LRVAL) - right;
			
			if (stereo)
			{
				pos >>= LRVAL;
				pos /= 2;
				pos *= 2;
				
				if (2 + pos >= realsrcSize)	{}
				else tempL = (left * src8[pos] + right * src8[2 + pos]) >> LRVAL;
			
				dst8[x] = tempL;
				
				x++;
				
				if (3 + pos >= realsrcSize)	{}
				else tempR = (left * src8[1 + pos] + right * src8[3 + pos]) >> LRVAL;
				
				dst8[x] = tempR;
			}
			else
			{
				pos >>= LRVAL;
				
				if (1 + pos >= realsrcSize)	{}
				else tempL = (left * src8[pos] + right * src8[1 + pos]) >> LRVAL;
			
				dst8[x] = tempL;
			}
		}
	break;
	
	case 16:
		for (x = 0; x < newSize/2; x++)
		{
			pos			= (x * srcSize << LRVAL) / dstSize;
			right		= pos & ((1 << LRVAL)-1);
			left		= (1 << LRVAL) - right;
			
			if (stereo)
			{
				pos >>= LRVAL;
				pos /= 2;
				pos *= 2;
				
				if (2 + pos >= realsrcSize/2)	{}
				else tempL = (left * src16[pos] + right * src16[2 + pos]) >> LRVAL;
				
				dst16[x] = tempL;
				
				x++;
				
				if (3 + pos >= realsrcSize/2)	{}
				else tempR = (left * src16[1 + pos] + right * src16[3 + pos]) >> LRVAL;
				
				dst16[x] = tempR;
				
				
			}
			else
			{
				pos >>= LRVAL;
				
				if (1 + pos >= realsrcSize/2)	{}
				else tempL = (left * src16[pos] + right * src16[1 + pos]) >> LRVAL;
				
				dst16[x] = tempL;
			}
		}
	break;
	}
	
	return dst;
}

OSErr mainLength(			sData					*theData,
							long					SelectionStart,
							long					SelectionEnd,
							PPInfoPlug				*thePPInfoPlug,
							short					StereoMode)				// StereoMode = 0 apply on all channels, = 1 apply on current channel
{
	DialogPtr		myDia;
	short			itemHit, itemType, changeMode;
	Str255			tStr;
	long			temp;
	Handle			itemHandle;
	Rect			itemRect;
	Boolean			ChangeRate;

	myDia = GetNewDialog(128, NULL, (WindowPtr) -1L);
	SetPortDialogPort(myDia);
	AutoPosition(myDia);
	
	/** Default values **/
	NumToString(theData->size, tStr);
	SetDText(myDia, 9, tStr);
	SetDText(myDia, 12, tStr);
	
	temp = theData->size;
	if (theData->stereo) temp /= 2;
	if (theData->amp == 16) temp /= 2;
	NumToString(temp, tStr);
	SetDText(myDia, 4, tStr);
	SetDText(myDia, 7, tStr);
	
	SelectDialogItemText(myDia, 7, 0, 10000);
	/********************/
	
	GetDialogItem(myDia, 14, &itemType, &itemHandle, &itemRect);
	SetControlValue((ControlHandle) itemHandle, true);
	changeMode = 0;
	ChangeRate = false;
	
	ControlSwitch(13, myDia, 255);
	
	ReGo:
	
	do
	{
//		#if defined(powerc) || defined(__powerc)
		ModalDialog(thePPInfoPlug->MyDlgFilterUPP, &itemHit);
//		#else
//		ModalDialog((ModalFilterProcPtr) thePPInfoPlug->MyDlgFilterUPP, &itemHit);
//		#endif
		
		switch(itemHit)
		{
		case -5:
		case 0:
		//case nullEvent:
			GetDText(myDia, 7, tStr);		StringToNum(tStr, &temp);
			
			if (theData->stereo) temp *= 2;
			if (theData->amp == 16) temp *= 2;
			
			NumToString(temp, tStr);
			
			SetDText(myDia, 12, tStr);
		break;
		
		case 14:
		case 15:
		case 16:
				GetDialogItem(myDia, 14, &itemType, &itemHandle, &itemRect);	SetControlValue((ControlHandle) itemHandle, false);
				GetDialogItem(myDia, 15, &itemType, &itemHandle, &itemRect);	SetControlValue((ControlHandle) itemHandle, false);
				GetDialogItem(myDia, 16, &itemType, &itemHandle, &itemRect);	SetControlValue((ControlHandle) itemHandle, false);
		
				GetDialogItem(myDia, itemHit, &itemType, &itemHandle, &itemRect);
				SetControlValue((ControlHandle) itemHandle, true);
				changeMode = itemHit - 14;
				
				if (changeMode == 2) ControlSwitch(13, myDia, 0);
				else ControlSwitch(13, myDia, true);
			break;
			
		case 13:
			ChangeRate = !ChangeRate;
			GetDialogItem(myDia, 13, &itemType, &itemHandle, &itemRect);	SetControlValue((ControlHandle) itemHandle, ChangeRate);
		break;
		}
	}
	while(itemHit != 1 && itemHit != 2);
	
	if (itemHit == 1)
	{
		long	newSize;
		Ptr		newPtr;
		
		GetDText(myDia, 7, tStr);		StringToNum(tStr, &newSize);
		if (newSize <= 0 || newSize > 1024L*1024L*20L)
		{
			SysBeep(1);
			SelectDialogItemText(myDia, 7, 0, 10000);
			goto ReGo;
		}
		
		if (theData->stereo) newSize *= 2;
		if (theData->amp == 16) newSize *= 2;
		
		if (changeMode == 2)
		{
			newPtr = ConvertSampleSize(		theData->data,
																		theData->size,
																		theData->amp,
																		newSize,
																		theData->stereo);
			
			if (ChangeRate) theData->c2spd = (theData->c2spd * (newSize/100)) / (theData->size/100);
			
			
			theData->loopBeg = (theData->loopBeg * (newSize/100)) / (long) (theData->size/100);
			theData->loopSize = (theData->loopSize * (newSize/100)) / (long) (theData->size/100);
			
			
			if (newPtr != NULL)
			{
				DisposePtr(theData->data);
				theData->data		= newPtr;
				theData->size		= newSize;
			}
		}
		else
		{
			Ptr dst = NewPtrClear(newSize);
			if (dst != NULL)
			{
				if (newSize > theData->size)
				{
					if (changeMode == 0) memcpy(dst, theData->data, theData->size);
					else
					{
						memcpy(dst + newSize - theData->size, theData->data,  theData->size);
						theData->loopBeg += newSize - theData->size;
					}
				}
				else
				{
					if (changeMode == 0)  memcpy(dst, theData->data, newSize);
					else
					{
						memcpy(dst, theData->data + theData->size - newSize, newSize);
						theData->loopBeg -= theData->size - newSize;
					}
				}
								
				DisposePtr(theData->data);
				theData->data		= dst;
				theData->size		= newSize;
			}
		}
		
		if (theData->loopBeg < 0) { theData->loopSize += theData->loopBeg;	theData->loopBeg = 0;}
		if (theData->loopBeg > newSize) {theData->loopBeg = 0;	theData->loopSize = 0;}
		if (theData->loopSize < 0) theData->loopSize = 0;
		if (theData->loopBeg + theData->loopSize > newSize) theData->loopSize = newSize - theData->loopBeg;
	}
	
	DisposeDialog(myDia);

	return MADNoErr;
}

// CC42BFBF-419A-4CCB-9DC7-49A0A702FD0B
#define PLUGUUID CFUUIDGetConstantUUIDWithBytes(kCFAllocatorSystemDefault, 0xCC, 0x42, 0xBF, 0xBF, 0x41, 0x9A, 0x4C, 0xCB, 0x9D, 0xC7, 0x49, 0xA0, 0xA7, 0x02, 0xFD, 0x0B)

#define PLUGMAIN mainLength
#define PLUGINFACTORY LengthFactory

#include "CFPlugin-FilterBridge.c"
