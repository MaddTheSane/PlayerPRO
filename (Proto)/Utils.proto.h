
#pragma once

/* Utils.c */
void NNumToString(short, Str255);
void oldFrameButton(DialogPtr);
void CenterRect(Rect *, Rect *, Rect *);
pascal Boolean mylClickLoop(void);
pascal Boolean MyDlgFilter(DialogPtr, EventRecord *, short *);
void MyUpdateDialog(DialogPtr);
Boolean MyIntModalDialog(DialogPtr, short *, EventRecord *);
Boolean MyModalDialog(DialogPtr, short *);
pascal Boolean MyDlgFilterSF(DialogPtr, EventRecord *, short *, void *);
void ToolBoxInit(void);
void InverseRadio(short, DialogPtr);
void TurnRadio(short, DialogPtr, Boolean);
void ControlSwitch(short, DialogPtr, short);
void MyMoveControl(ControlHandle, short, short);
void MySizeControl(ControlHandle, short, short);
void MySizeWindow(DialogPtr, short, short, Boolean);
Boolean MyTrackControl(ControlHandle, Point, ControlActionUPP);
short NewOffscreenBitMap(BitMap *, Rect *);
void ZapBitMap(BitMap *);
void BitToPic(PicHandle *, BitMap, Rect *);
void SetDText(DialogPtr, short, Str255);
void WriteCText(DialogPtr, short, char *);
void GetDText(DialogPtr, short, StringPtr);
void OSType2Str(OSType, Str255);

static inline void ByteSwapMADSpec(MADSpec *toSwap)
{
	MOT32( &toSwap->MAD);
	MOT16( &toSwap->speed);
	MOT16( &toSwap->tempo);
	MOT32( &toSwap->EPitch);
	MOT32( &toSwap->ESpeed);
}

static inline void ByteSwapPatHeader(PatHeader *toSwap)
{
	MOT32( &toSwap->size);
	MOT32( &toSwap->compMode);
	MOT32( &toSwap->patBytes);
	MOT32( &toSwap->unused2);
}

static inline void ByteSwapInstrData(InstrData *toSwap)
{
	int x;
	MOT16( &toSwap->numSamples);
	MOT16( &toSwap->firstSample);
	MOT16( &toSwap->volFade);
	MOT16( &toSwap->MIDI);
	MOT16( &toSwap->MIDIType);
	
	for (x = 0; x < 12; x++)
	{
		MOT16( &toSwap->volEnv[ x].pos);
		MOT16( &toSwap->volEnv[ x].val);
		
		MOT16( &toSwap->pannEnv[ x].pos);
		MOT16( &toSwap->pannEnv[ x].val);
		
		MOT16( &toSwap->pitchEnv[ x].pos);
		MOT16( &toSwap->pitchEnv[ x].val);
	}
}

static inline void ByteSwapsData(sData *toSwap)
{
	MOT32( &toSwap->size);
	MOT32( &toSwap->loopBeg);
	MOT32( &toSwap->loopSize);
	MOT16( &toSwap->c2spd);
}

void pStrcpy(register unsigned char *, register const unsigned char *);
void pStrcat(register unsigned char *, register unsigned char *);
void ErasePixMap(PixMapHandle);
short NewOffscreenPixMap(PixMapHandle *, Rect *);
Ptr NewADDRPtrI(short);
void ZapPixMap(PixMapHandle *);
void PixToPic(PicHandle *, PixMapHandle, Rect);
short PicToPix(PicHandle, PixMapHandle *);
short IclToPix(Handle, PixMapHandle *);
unsigned short RangedRdm(unsigned short, unsigned short);
void DessineRgn(Rect *, Rect *);
void FrameButton(DialogPtr, short, Boolean);
short SetScroll(ControlHandle, TEHandle);
void FrameRectRelief(Rect *);
