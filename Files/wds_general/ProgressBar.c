#include "Shuddup.h"
#include "RDriver.h"
#include "RDriverInt.h"

static	long		WorkStatus, WorkTotal;
		DialogPtr	theProgressDia;
static	long		FirstCall, MoyCall;
static	long		oldTime, oldPos;
static	short		currentPat;
static	unsigned long	totalSize, finalSize;
extern	KeyMap		km;
extern	RGBColor	theColor;

static	Str255		EstimatedTime, ElapsedTime, KWritten, Throughput;

void NTStr(short	NoDigi, short		val, Ptr	theText);
Boolean	IsPressed(unsigned short );
Boolean	AbortInQueue();
void UpdateWorkingWindow();
void StopAIFFExporting();
OSErr ConvertMovieToMPEG4(FSSpec *inputFile, FSSpec *outputFile);
void FlushPlugin(void);
void GetDigitalSelection(short *XStart, short *YStart, short *XEnd, short *YEnd, short*);
void MADCheckSpeedPattern(MADMusic *MDriver, MADDriverRec *intDriver);
Boolean SoundQualityExport(	Boolean OnlyCurrent,
								short *ChannelNo,
								OSType *CompressionType,
								Fixed *FrequenceSpeed,
								short *amplitude,
								short *PatternID,
								MADDriverSettings 		*driver,
								Boolean	MPG4);
						
#define 	MINIMUS	4

static	const RGBColor	kBlack		= {0x0000, 0x0000, 0x0000};
static	const RGBColor	kWhite		= {0xFFFF, 0xFFFF, 0xFFFF};
static	const RGBColor	kDarkGrey	= {0x4000, 0x4000, 0x4000};
static	const RGBColor	kSteelBlue	= {0xCCCC, 0xCCCC, 0xFFFF};
static	ControlHandle	progCntl;

void SetFrameColor(void)
{
	RGBForeColor(&kBlack);
	RGBBackColor(&kWhite);
}

void SetDoneColor(void)
{
	RGBForeColor(&kDarkGrey);
	RGBBackColor(&kWhite);
}

void SetToDoColor(void)
{
	RGBForeColor(&kSteelBlue);
	RGBBackColor(&kBlack);
}

void OpenWorkingWindow(short NumberCalls)
{
	short 	itemType;
	Handle	itemHandle;
	Rect	itemRect;
	GrafPtr	SavePort;

	WorkStatus = 0;
	WorkTotal = NumberCalls;
	
//	if (WorkTotal < MINIMUS) return;
	
	
	GetPort(&SavePort);
	
	theProgressDia = GetNewDialog(156, NULL, (WindowPtr) -1L);
	SetPortDialogPort(theProgressDia);
	ShowWindow(GetDialogWindow(theProgressDia));
	SelectWindow2(GetDialogWindow(theProgressDia));
	
	DisableMenuItem(ExportMenu, 1);
	DisableMenuItem(ExportMenu, 2);
	
	FirstCall = TickCount();
	
	if (AppearanceManager)
	{
		GetDialogItem(theProgressDia, 1, &itemType, &itemHandle, &itemRect);
		progCntl = NewControl(	GetDialogWindow(theProgressDia),
								&itemRect,
								"\p.",
								true,
								0,		//icl8 id
								0,
								100,
								80,
								0);
	}

	
	UpdateWorkingWindow();
	
	SetPort(SavePort);
}

void UpdateWorkingWindow()
{
	GrafPtr		SavePort;
	short 		itemType;
	Handle		itemHandle;
	Rect		itemRect;
	long		temp;
	
	if (theProgressDia == NULL) return;
	
	GetPort(&SavePort);
	SetPortDialogPort(theProgressDia);
	
	BeginUpdate(GetDialogWindow(theProgressDia));
	
	DrawDialog(theProgressDia);
	
	if (AppearanceManager)
	{
		
	}
	else
	{
		GetDialogItem(theProgressDia, 1, &itemType, &itemHandle, &itemRect);
		
		itemRect.left -=1;				itemRect.top -=1;
		itemRect.right +=1;				itemRect.bottom +=1;
		SetFrameColor();
		FrameRect(&itemRect);
		
		GetDialogItem(theProgressDia, 1, &itemType, &itemHandle, &itemRect);
		SetToDoColor();
		PaintRect(&itemRect);	
		
		temp = itemRect.right - itemRect.left;
		temp = (temp * WorkStatus) / WorkTotal;
		
		itemRect.right = itemRect.left + temp;
		
		SetDoneColor();
		PaintRect(&itemRect);
		
		ForeColor(blackColor);
		RGBBackColor(&theColor);
	}
	
	GetDialogItem (theProgressDia, 7, &itemType, &itemHandle, &itemRect);
	TETextBox(EstimatedTime + 1, EstimatedTime[ 0], &itemRect, teJustRight);
	
	GetDialogItem (theProgressDia, 5, &itemType, &itemHandle, &itemRect);
	TETextBox(ElapsedTime + 1, ElapsedTime[ 0], &itemRect, teJustRight);
	
	GetDialogItem (theProgressDia, 9, &itemType, &itemHandle, &itemRect);
	TETextBox(Throughput + 1, Throughput[ 0], &itemRect, teJustRight);
	
	GetDialogItem (theProgressDia, 3, &itemType, &itemHandle, &itemRect);
	TETextBox(KWritten + 1, KWritten[ 0], &itemRect, teJustRight);
	
	EndUpdate(GetDialogWindow(theProgressDia));
	
	SetPort(SavePort);
}

Boolean DoWorkingWindow(void)
{
	short		itemType, itemHit;
	Handle		itemHandle;
	Rect		itemRect;
	Str255		str, str2;
	long		Ltemp = 0, temp, curTicks = TickCount();
	GrafPtr		SavePort;
	Point		pt;
	DateTimeRec	dtrp;
	
//	if (WorkTotal < MINIMUS) return false;
	
	if (theProgressDia == NULL) return false;
	
	GetPort(&SavePort);
	SetPortDialogPort(theProgressDia);
	
	WorkStatus ++;
	if (WorkStatus > WorkTotal) WorkStatus = WorkTotal;
	
	if (AppearanceManager)
	{
		SetControlValue(progCntl, (100 * WorkStatus) / WorkTotal);
	}
	else
	{
		GetDialogItem(theProgressDia, 1, &itemType, &itemHandle, &itemRect);
		
		temp = itemRect.right - itemRect.left;
		temp = (temp * WorkStatus) / WorkTotal;
		
		if (oldPos != temp)
		{
			oldPos = temp;
		
			itemRect.right = itemRect.left + temp;
		
			SetDoneColor();
			PaintRect(&itemRect);
			ForeColor(blackColor);
			RGBBackColor(&theColor);
		}
	}
	
	if (WorkStatus == 1)
	{
		FirstCall = TickCount();
		goto NOW;
	}
	else if (WorkStatus == WorkTotal)
	{
		curTicks = TickCount();
		
		Ltemp = ((long) (curTicks - FirstCall) * ((long) WorkTotal))/ ((long) WorkStatus);	// Temps total
		Ltemp -= (long) (curTicks - FirstCall);
	
		Ltemp /= 60;
		
		goto NOW;
	}
	else
	{
		curTicks = TickCount();
		
		if (curTicks - FirstCall > 30)
		{
			Ltemp = ((long) (curTicks - FirstCall) * ((long) WorkTotal))/ ((long) WorkStatus);	// Temps total
			Ltemp -= (long) (curTicks - FirstCall);
		
			Ltemp /= 60;
			
			if (oldTime != Ltemp)
			{
				oldTime = Ltemp;
				
				NOW:
				
				// Estimated remaining time
				
				SecondsToDate(Ltemp, &dtrp);
				
				NTStr(2, dtrp.second, (Ptr) str);
				NTStr(2, dtrp.minute, (Ptr) str2);
				
				strcat((Ptr) str2, ":");
				strcat((Ptr) str2, (Ptr) str);
				MyC2PStr((Ptr) str2);
				
				pStrcpy(EstimatedTime, str2); pStrcat(EstimatedTime, "\p ");
				GetDialogItem (theProgressDia, 7, &itemType, &itemHandle, &itemRect);
				TETextBox(EstimatedTime+1, EstimatedTime[ 0], &itemRect, teJustRight);
				
				// Elapsed time
				
				Ltemp = curTicks - FirstCall;
				Ltemp /= 60;
				
				SecondsToDate(Ltemp, &dtrp);
				
				NTStr(2, dtrp.second, (Ptr) str);
				NTStr(2, dtrp.minute, (Ptr) str2);
				
				strcat((Ptr) str2, ":");
				strcat((Ptr) str2, (Ptr) str);
				MyC2PStr((Ptr) str2);
				
				pStrcpy(ElapsedTime, str2);  pStrcat(ElapsedTime, "\p ");
				GetDialogItem (theProgressDia, 5, &itemType, &itemHandle, &itemRect);
				TETextBox(ElapsedTime+1, ElapsedTime[ 0], &itemRect, teJustRight);
				
				// KBytes written
				
				if (totalSize > finalSize) temp = finalSize;
				else temp = totalSize;
				
				NumToString(temp / 1024L, str);
				pStrcat(str, "\p Kb of ");
				
				NumToString(finalSize / 1024L, str2);
				pStrcat(str2, "\p Kb");
				
				pStrcat(str, str2);
				
				pStrcpy(KWritten, str);	pStrcat(KWritten, "\p ");
				GetDialogItem (theProgressDia, 3, &itemType, &itemHandle, &itemRect);
				TETextBox(KWritten+1, KWritten[ 0], &itemRect, teJustRight);
				
				// Throughput
				
				Ltemp = curTicks - FirstCall;
				Ltemp /= 60;
				
				if (Ltemp != 0)
				{
					NumToString((totalSize / 1024L) / Ltemp, str);
					pStrcat(str, "\p Kb / s");
					
					pStrcpy(Throughput, str);	pStrcat(Throughput, "\p ");
					GetDialogItem (theProgressDia, 9, &itemType, &itemHandle, &itemRect);
					TETextBox(Throughput+1, Throughput[ 0], &itemRect, teJustRight);
				}
			}
		}
	}
	
	SetPort(SavePort);
	
	GetKeys(km);
	if (IsPressed(0x37) && IsPressed(0x2F)) return true;
	else return false;
}

void CloseWorkingWindow(void)
{
	GrafPtr			SavePort;
	short			temp, itemType;
	Handle			itemHandle;
	Rect			itemRect;
	long			tempLong;
	
	GetPort(&SavePort);
	SetPortDialogPort(theProgressDia);
	
	GetDialogItem(theProgressDia, 8, &itemType, &itemHandle, &itemRect);
	
	SetDoneColor();
	PaintRect(&itemRect);
	
	ForeColor(blackColor);
	RGBBackColor(&theColor);
	
	SelectWindow2(NextWindowVisible(GetDialogWindow(theProgressDia)));
	
	DisposeDialog(theProgressDia);
	theProgressDia = NULL;
	
	EnableMenuItem(ExportMenu, 1);
	EnableMenuItem(ExportMenu, 2);

	SetPort(SavePort);
}

static 		Ptr 					outSound, sndPtr, compSound;
static 		long					Packet, oldPart, SndOffset;
static		MADDriverRec			*copyDriver;
static		MADMusic				*copyMusic;
static		OSType					theType;
static		Ptr						bufferSin, bufferSout;
static		Handle					sndHandle;
static		short					fRefNum, RsrcRefNum, curVolRefNumFF;
static		short					MultiRef[ MAXTRACK];
static		short					MultiRsrcRefNum[ MAXTRACK], patternID = 0;
static		Cmd						*tempCmd;
static		FSSpec					theAIFFSpec, DestFile;

static		SoundConverter			sc;
static		SoundComponentData		inputFormat, outputFormat;
static		unsigned long			inputFrames, outputFrames;
static		unsigned long			inputBytes, outputBytes;
static		Ptr						inputPtr, outputPtr;

Boolean CreateAIFFExporting(Boolean OnlyCurrent, short  fRef, FSSpec *newFile, OSType type, FSSpec *dstFile)
{
	OSErr					iErr;
	long					i;
	CompressionInfo			cp;
	Boolean					MPG4;
	
	pStrcpy(EstimatedTime, "\p00:00 ");
	pStrcpy(ElapsedTime, "\p00:00 ");
	pStrcpy(Throughput, "\p0 Kb / s ");
	pStrcpy(KWritten, "\p0 Kb ");
	
	bufferSin = NULL;
	bufferSout = NULL;
	sndHandle = NULL;
	
	theAIFFSpec = *newFile;
	if (dstFile) DestFile = *dstFile;
	theType = type;
	fRefNum = fRef;
	curVolRefNumFF = newFile->vRefNum;
	
	if (theType == 'MPG4') MPG4 = true;
	else MPG4 = false;
	
//	thePrefs.DirectDriverType = MADDriver->DriverSettings;
	
	if (!SoundQualityExport(OnlyCurrent, &thePrefs.channelNumber, &thePrefs.Compressor, &thePrefs.FrequenceSpeed, &thePrefs.amplitude, &patternID, &thePrefs.DirectDriverType, MPG4))
	{
		FSCloseFork(fRefNum);
		FSpDelete(newFile);
		return false;
	}
	
	if (theType == 'MPG4') thePrefs.Compressor = 'NONE';
	
	thePrefs.DirectDriverType.numChn			= 	MADDriver->DriverSettings.numChn;
	thePrefs.DirectDriverType.outPutBits		= 	thePrefs.amplitude;
	thePrefs.DirectDriverType.outPutRate		=	thePrefs.FrequenceSpeed;
	thePrefs.DirectDriverType.outPutMode		=	DeluxeStereoOutPut;
	//thePrefs.DirectDriverType.antiAliasing	=	false;
	thePrefs.DirectDriverType.repeatMusic		=	false;
	thePrefs.DirectDriverType.sysMemory			=	false;
	thePrefs.DirectDriverType.driverMode		=	NoHardwareDriver;
	
	UpdateALLWindow();
	
	copyMusic = (MADMusic*) NewPtr(sizeof(MADMusic));
	memcpy(copyMusic, curMusic, sizeof(MADMusic));
	
	if (MADCreateDriver(&thePrefs.DirectDriverType, gMADLib, &copyDriver) != noErr) return false;
	
	
	copyDriver->currentlyExporting	= true;
	copyDriver->thisExport			= true;
	MADDriver->currentlyExporting	= true;
	
	FlushPlugin();
	
	copyDriver->DriverSettings		=	thePrefs.DirectDriverType;
	
	copyDriver->BytesToGenerate		= 	0;
	copyDriver->BufCounter			= 	0;
	copyDriver->JumpToNextPattern	= 	true;

	copyDriver->smallcounter		= 128;
	copyDriver->PartitionReader	= 0;
	
	copyDriver->PL = 0;
	copyDriver->PartitionReader = 0;
	copyDriver->Pat = copyMusic->header->oPointers[ copyDriver->PL];
		
	copyDriver->speed = copyMusic->header->speed;
	copyDriver->finespeed = copyMusic->header->tempo;
	
	for (i = 0; i < MAXTRACK; i++) copyDriver->Active[ i] = MADDriver->Active[ i];
	
	MADCleanDriver(copyDriver);
	MADPurgeTrack(copyDriver);
	
	// Copy Adaptors Settings
	
	MADAttachDriverToMusic(copyDriver, copyMusic, NULL);

	copyDriver->VExt		= MADDriver->VExt;
	copyDriver->FreqExt		= MADDriver->FreqExt;
	copyDriver->VolGlobal	= MADDriver->VolGlobal;
	
	for (i = 0; i < EQPACKET*2; i++)
	{
		copyDriver->Filter[ i] = MADDriver->Filter[ i];
	}
	
	copyDriver->Equalizer = MADDriver->Equalizer;
	
	/**********/
	
	// In case of just one pattern to export // // // // // // // // 
	
	MADCheckSpeedPattern(copyMusic, copyDriver);
	
	switch(patternID)
	{
		case -2:
		{
			short	DStartTrack, DStartPos, DEndTrack, DEndPos;
			short	u, v;
			
			GetDigitalSelection(&DStartTrack, &DStartPos, &DEndTrack, &DEndPos, &currentPat);
			
			copyDriver->PL = copyMusic->header->numPointers-1;
			copyDriver->Pat = currentPat;
			
			copyDriver->PartitionReader = DStartPos;
			
			/****** ALLOCATION *********/
		
			copyMusic->partition[ copyMusic->header->numPat] = (PatData*) NewPtrClear(sizeof(PatHeader) + copyMusic->header->numChn * DEndPos * sizeof(Cmd));
			if (MemError()) MyDebugStr(__LINE__, __FILE__, "Error in AddAPattern...");
			
			copyMusic->partition[ copyMusic->header->numPat]->header.size = DEndPos;
			copyMusic->partition[ copyMusic->header->numPat]->header.compMode = 'NONE';
			strcpy((Ptr) copyMusic->partition[ copyMusic->header->numPat]->header.name, "New pattern");
			copyMusic->partition[ copyMusic->header->numPat]->header.patBytes = 0;
			copyMusic->partition[ copyMusic->header->numPat]->header.unused2 = 0;
			
			for (u = 0; u < copyMusic->header->numChn; u++)
			{
				for (v = 0; v < copyMusic->partition[ copyMusic->header->numPat]->header.size; v++)
				{
					Cmd*	cmd1, *cmd2;
					
					cmd1 = GetMADCommand( v,  u, copyMusic->partition[ copyMusic->header->numPat]);
					
					cmd2 = GetMADCommand( v,  u, copyMusic->partition[ currentPat]);
					
					if (u >= DStartTrack && u < DEndTrack) *cmd1 = *cmd2;
					else MADKillCmd(cmd1);
				}
			}
			
			copyDriver->Pat = copyMusic->header->numPat;
			
			copyMusic->header->numPat++;
			
		//	for (i = 0; i < DStartTrack; i++) copyDriver->Active[ i] = false;
		//	for (i = DStartTrack; i < DEndTrack; i++) if (copyDriver->Active[ i] = true;
		//	for (i = DEndTrack; i < MAXTRACK; i++) copyDriver->Active[ i] = false;
		}
		break;
		
		case -1:
		
		break;
		
		default:
			for (i = 0; i < copyMusic->header->numPointers; i++)
			{
				if (copyMusic->header->oPointers[ i] == patternID)
				{
					copyDriver->PL = i;
					copyDriver->Pat = patternID;
					
					MADCheckSpeed(copyMusic, copyDriver);
				}
			}
			
			copyDriver->PL = copyMusic->header->numPointers-1;
			copyDriver->Pat = patternID;
		break;
	}
	
	// // // // // // // // // // // // // // // // // // 
	
	bufferSin = NewPtrClear(128);
	bufferSout = NewPtrClear(128);
		
	Packet = MaxBlock()/ (copyDriver->ASCBUFFERReal *thePrefs.amplitude/8);
	Packet /= (2L*2L + 1L);
	
	if (Packet > 20) Packet = 20;
	if (Packet <= 0) MyDebugStr(__LINE__, __FILE__, "Memory Error, Increase Memory");
	
	
	if (patternID != -1 || patternID == -2) OpenWorkingWindow(1);
	else OpenWorkingWindow(copyMusic->header->numPointers);
	
	sndPtr =  NewPtrClear((copyDriver->ASCBUFFER*2L*thePrefs.amplitude) / 8L);
	if (sndPtr == NULL) MyDebugStr(__LINE__, __FILE__, "Memory Error, Increase Memory");
	
	outSound = NewPtrClear((copyDriver->ASCBUFFERReal*Packet*2L*thePrefs.amplitude) / 8L);
	if (outSound == NULL) MyDebugStr(__LINE__, __FILE__, "Memory Error, Increase Memory");
	
	compSound = NewPtrClear((copyDriver->ASCBUFFERReal*Packet*2L*thePrefs.amplitude) / 8L);
	if (compSound == NULL) MyDebugStr(__LINE__, __FILE__, "Memory Error, Increase Memory");
	
	////////
	
	inputFormat.flags = 0;
	if (thePrefs.DirectDriverType.outPutBits == 8) inputFormat.format = kOffsetBinary;
	else inputFormat.format = k16BitBigEndianFormat;	//k16BitBigEndianFormat;
	inputFormat.numChannels = 2;
	inputFormat.sampleSize = thePrefs.DirectDriverType.outPutBits;
	inputFormat.sampleRate = thePrefs.DirectDriverType.outPutRate;
	inputFormat.sampleCount = 0;
	inputFormat.buffer = nil;
	inputFormat.reserved = 0;
	
	outputFormat.flags = 0;
	outputFormat.format = thePrefs.Compressor;
	outputFormat.numChannels = thePrefs.channelNumber;	
	outputFormat.sampleSize = thePrefs.amplitude;
	outputFormat.sampleRate = thePrefs.DirectDriverType.outPutRate;
	outputFormat.sampleCount = 0;
	outputFormat.buffer = nil;
	outputFormat.reserved = 0;
	
	iErr = SoundConverterOpen(&inputFormat, &outputFormat, &sc);
	if (iErr != noErr)
	{
		MyDebugStr(__LINE__, __FILE__, "Open failed, Compressor NOT available");
		
		MyDisposePtr(& bufferSin);		MyDisposePtr(& bufferSout);		MyDisposePtr(& sndPtr);
		MyDisposePtr(& outSound);		MyDisposePtr(& compSound);
		
		/* Delete values */
		
		MADDisposeDriver(copyDriver);
		DisposePtr((Ptr) copyMusic);
		
		CloseWorkingWindow();
		
		return false;
	}
	
/*	iErr = SoundConverterGetBufferSizes(sc, 1024, &inputFrames, &inputBytes, &outputBytes);
	if (iErr != noErr)
	MyDebugStr(__LINE__, __FILE__, "GetBufferSizes failed");*/
	
	iErr = SoundConverterBeginConversion(sc);
	if (iErr != noErr)
	MyDebugStr(__LINE__, __FILE__, "Begin Conversion failed");
	
	///////////////////////////////////	
	
	if (theType == 'sfil')
	{
		short	headerLen;
		FInfo		fndrInfo;
		
		FSCloseFork(fRefNum);
		
		iErr = FSpGetFInfo(newFile, &fndrInfo);
		if (iErr != noErr)
		{
			fndrInfo.fdCreator = '\?\?\?\?';
			fndrInfo.fdType = '\?\?\?\?';
		}
		
	//	SetResLoad(false);
		
		{
			FSpCreateResFile(newFile, fndrInfo.fdCreator, fndrInfo.fdType, smSystemScript);
			
			RsrcRefNum = FSpOpenResFile(newFile, fsCurPerm);
			if (RsrcRefNum == -1)
			{
				CloseWorkingWindow();
				return false;
			}
			
			sndHandle = NewHandle(20000);
			if (sndHandle == NULL) {MyDebugStr(__LINE__, __FILE__, "Need MEMORY");	CloseWorkingWindow(); return false;}
			
			iErr = SetupSndHeader(		(SndListHandle) sndHandle,
										thePrefs.channelNumber,
										thePrefs.FrequenceSpeed,
										thePrefs.amplitude,
										thePrefs.Compressor,
										60,
										50000,
										&headerLen);
			
			AddResource(sndHandle, 'snd ', 7438, newFile->name);
			WriteResource(sndHandle);
			ReleaseResource(sndHandle);
			
			SetResLoad(false);
			sndHandle = Get1Resource('snd ', 7438);
			SndOffset = headerLen;
			SetResLoad(true);
		}
	}
	else if (theType == 'AIFF' | theType == 'MPG4')
	{
	/*	if (thePrefs.ChannelType == MultiFiles)
		{
			FSClose(fRefNum);
			
			FSpDelete(newFile);
			
			for (i = 0; i < thePrefs.channelNumber; i++)
			{
				FSSpec	newFileMulti;
				Str255	str;
				
				newFileMulti = *newFile;
				
				pStrcat(newFileMulti.name, "\p.");
				NumToString(i+1, str); 
				pStrcat(newFileMulti.name, str);
				
				FSpDelete(&newFileMulti);
				
				iErr = FSpCreate(&newFileMulti, 'TVOD', 'AIFF', smSystemScript);
				iErr = FSpOpenDF(&newFileMulti, fsCurPerm, &MultiRef[ i]);
				
				iErr = SetupAIFFHeader(		MultiRef[ i],
											1,
											thePrefs.FrequenceSpeed,
											thePrefs.amplitude,
											thePrefs.Compressor,
											copyDriver->ASCBUFFERReal*Packet*thePrefs.channelNumber,
											0);
			}
		}
		else*/
		{
		iErr = SetupAIFFHeader(		fRefNum,
									thePrefs.channelNumber,
									thePrefs.FrequenceSpeed,
									thePrefs.amplitude,
									thePrefs.Compressor,
									copyDriver->ASCBUFFERReal*Packet*thePrefs.channelNumber,
									0);
		if (iErr) MyDebugStr(__LINE__, __FILE__, "");
		}
	}
	else MyDebugStr(__LINE__, __FILE__, "");
	
	/***********************************************************/
	
	finalSize			= 	thePrefs.FrequenceSpeed;
	finalSize			>>= 16L;
	finalSize			*= 	thePrefs.channelNumber;
	finalSize			*= 	(thePrefs.amplitude / 8L);
	{
		long 	fullTime, curTime;
		MADGetMusicStatus(copyDriver, &fullTime, &curTime);
		
		finalSize		*= (fullTime+1) / 60;
	}
	if (patternID != -1 || patternID == -2) finalSize = 1;
	
	switch(thePrefs.Compressor)
	{
		case 'MAC3':		finalSize /= 3L;		break;
		case 'MAC6':		finalSize /= 6L;		break;
	}
	
	totalSize 			= 	0;
	iErr 				= 	noErr;
	copyDriver->musicEnd = 	false;
	copyDriver->Reading	=	true;
	oldPart				=	-1;
	
	return true;
}
/*
Ptr DeInterlace(long inOutCount, Ptr outFile)
{
	Ptr	 	decPtr = NewPtr(inOutCount);
	long	x, i, chanLength = inOutCount/thePrefs.channelNumber;
	
	short	*dec16Ptr, *outFile16;
	
	if (thePrefs.amplitude == 8)
	{
		for (i = 0; i < thePrefs.channelNumber; i ++)
		{
			for (x = 0 ; x < chanLength; x ++) *(decPtr + x + chanLength*i) = *(outFile + i + x*thePrefs.channelNumber);
		}
	}
	else
	{
		dec16Ptr = (short*) decPtr;
		outFile16 = (short*) outFile;
		
		chanLength = inOutCount/thePrefs.channelNumber;
		chanLength /= 2;
		
		for (i = 0; i < thePrefs.channelNumber; i ++)
		{
			for (x = 0 ; x < chanLength; x ++) *(dec16Ptr + x + chanLength*i) = *(outFile16 + i + x*thePrefs.channelNumber);
		}
	}
	
	return decPtr;
}*/

void DoAIFFExporting(void)
{
	long	inOutCount, i, full, z, val;
	Ptr		tempPtr = NULL, outFile;
	Boolean	OnContinue;
	OSErr	stopErr = noErr, iErr = noErr;
	
	if (theProgressDia == NULL) return;
	
	inOutCount 	= 0;
	tempPtr 	= outSound;
	
	for (i = 0;  i < Packet; i++)
	{
		OnContinue = DirectSave(sndPtr, &thePrefs.DirectDriverType, copyDriver);
		
		if (thePrefs.amplitude == 8)
		{
			full = (copyDriver->ASCBUFFERReal - copyDriver->BytesToRemoveAtEnd) * 2L;
			
			if (theType == 'sfil')
			{
				memcpy(tempPtr, sndPtr, full);
				tempPtr += full;
			}
			else
			{
				for(z = 0; z< full; z++)
				{
					*tempPtr = *(sndPtr + z) + 0x80;
					tempPtr++;
				}
			}
		}
		else
		{
			full = (copyDriver->ASCBUFFERReal - copyDriver->BytesToRemoveAtEnd) * 2L * 2L;
			memcpy(tempPtr, sndPtr, full);
			tempPtr += full;
		}
		
		val = (((long) (copyDriver->ASCBUFFERReal - copyDriver->BytesToRemoveAtEnd)* (long) 2L* (long) thePrefs.amplitude) / 8L);
		
		if (thePrefs.channelNumber == 1) totalSize += val/2;
		else totalSize += val;
		
		inOutCount += val;
		
		if (OnContinue == false)
		{
			stopErr = 1;
			i = Packet;
			
		}
		
		if (oldPart != copyDriver->PL) 
		{
			if (DoWorkingWindow())
			{
				stopErr = 1;
				i = Packet;
			}
			oldPart = copyDriver->PL;
		}
	}
	
	inputFrames = inOutCount / inputFormat.numChannels;				// Stereo
	inputFrames = inputFrames / (inputFormat.sampleSize / 8);		// sampleSize
	
	iErr = SoundConverterConvertBuffer(sc, outSound, inputFrames, compSound, &outputFrames, &outputBytes);
	if (iErr != noErr)
	MyDebugStr(__LINE__, __FILE__, "Conversion failed");
	
	if (outputBytes > inOutCount) MyDebugStr(__LINE__, __FILE__, "Err in memory size");
	
	inOutCount = outputBytes;
	
	//totalSize += inOutCount;
	
	//if (thePrefs.channelNumber == 1) inOutCount /= 2;
	
	outFile = compSound;
	
	if (theType == 'sfil')
	{
		{
			SetResourceSize(sndHandle, SndOffset + inOutCount);
			
			SetResLoad(false);
			WritePartialResource(sndHandle, SndOffset, outFile, inOutCount);
			SetResLoad(true);
			
			SndOffset += inOutCount;
		}
	}
	else
	{
	/*	if (thePrefs.ChannelType == MultiFiles)
		{
			Ptr 	inter;
			long	ss = inOutCount / thePrefs.channelNumber;
			
			inter = DeInterlace(inOutCount, outFile);
			
			for (i = 0 ; i < thePrefs.channelNumber; i++)
			{
				iErr = FSWrite(MultiRef[ i], &ss, inter + i*ss);
			}
			
			DisposePtr(inter);
		}
		else*/
		{
			iErr = FSWrite(fRefNum, &inOutCount, outFile);
		}
	}
	
	if (iErr != noErr || stopErr != noErr) StopAIFFExporting();
}

void StopAIFFExporting(void)
{
	OSErr 	iErr = noErr;
	long	inOutCount, i;
	
	
	if (theProgressDia == NULL) return;
	if (curMusic == NULL) return;
	
	//DoWorkingWindow();
	
	iErr = SoundConverterEndConversion(sc, compSound, &outputFrames, &outputBytes);
	if (iErr != noErr)
	MyDebugStr(__LINE__, __FILE__, "End Conversion failed");
	
	if (outputBytes)
	{
		Ptr outFile = compSound;
		
		inOutCount = outputBytes;
			
		if (theType == 'sfil')
		{
			{
				SetResourceSize(sndHandle, SndOffset + inOutCount);
				
				SetResLoad(false);
				WritePartialResource(sndHandle, SndOffset, outFile, inOutCount);
				SetResLoad(true);
				
				SndOffset += inOutCount;
			}
		}
		else
		{
		/*	if (thePrefs.ChannelType == MultiFiles)
			{
				Ptr 	inter;
				long	ss = inOutCount / thePrefs.channelNumber;
				
				inter = DeInterlace(inOutCount, outFile);
				
				for (i = 0 ; i < thePrefs.channelNumber; i++)
				{
					iErr = FSWrite(MultiRef[ i], &ss, inter + i*ss);
				}
				
				DisposePtr(inter);
			}
			else*/
			{
				iErr = FSWrite(fRefNum, &inOutCount, outFile);
			}
		}
	}
	
	
	iErr = SoundConverterClose(sc);
	if (iErr != noErr)
	MyDebugStr(__LINE__, __FILE__, "Close failed");
	
	if (theType == 'sfil')
	{
		Handle 	sndHandle2;
		short	headerLen;
		
		sndHandle2 = NewHandle(20000);
		if (sndHandle2 == NULL) {MyDebugStr(__LINE__, __FILE__, "Need MEMORY");	return;}
		
		iErr = SetupSndHeader(		(SndListHandle) sndHandle2,
									thePrefs.channelNumber,
									thePrefs.FrequenceSpeed,
									thePrefs.amplitude,
									thePrefs.Compressor,
									60,
									totalSize,
									&headerLen);
		
		inOutCount = headerLen;
		
		HLock(sndHandle2);
		SetResLoad(false);
		WritePartialResource(sndHandle, 0, *sndHandle2, inOutCount);
		SetResLoad(true);
		HUnlock(sndHandle2);
		
		ReleaseResource(sndHandle);
		
		CloseResFile(RsrcRefNum);
		
		DisposeHandle(sndHandle2);
	}
	else
	{
		long filePos;
		
	/*	if (thePrefs.ChannelType == MultiFiles)
		{
			for (i = 0; i < thePrefs.channelNumber; i++)
			{
				GetFPos(MultiRef[ i], &filePos);
				SetFPos(MultiRef[ i], fsFromStart, NULL);
				
				SetupAIFFHeader(		MultiRef[ i],
										1, 
										thePrefs.FrequenceSpeed, 
										thePrefs.amplitude, 
										thePrefs.Compressor,
										totalSize,
										0);
				
				SetFPos(MultiRef[ i], fsFromStart, filePos);
				SetEOF(MultiRef[ i], filePos);
				FSClose(MultiRef[ i]);
			}
		}
		else*/
		{
			GetFPos(fRefNum, &filePos);
			SetFPos(fRefNum, fsFromStart, 0);
			
			SetupAIFFHeader(		fRefNum,
									thePrefs.channelNumber, 
									thePrefs.FrequenceSpeed, 
									thePrefs.amplitude, 
									thePrefs.Compressor,
									totalSize,
									0);
			
			SetFPos(fRefNum, fsFromStart, filePos);
			SetEOF(fRefNum, filePos);
			FSCloseFork(fRefNum);
			
			if (theType == 'MPG4')
			{
				FSSpec	newFile;
				
				ConvertMovieToMPEG4(&theAIFFSpec, &DestFile);
			}
		}
	}
	MyDisposePtr(& bufferSin);		MyDisposePtr(& bufferSout);		MyDisposePtr(& sndPtr);
	MyDisposePtr(& outSound);		MyDisposePtr(& compSound);
	
	/***********/
	
	switch(patternID)
	{
		case -2:
		{
			copyMusic->header->numPat--;
			
			MyDisposePtr((Ptr*) &  copyMusic->partition[ copyMusic->header->numPat]);
			copyMusic->partition[ copyMusic->header->numPat] = NULL;
		}
	}
	
	MADDisposeDriver(copyDriver);
	DisposePtr((Ptr) copyMusic);
	
	FlushPlugin();
	
	MADDriver->currentlyExporting	= false;
	
	CloseWorkingWindow();
}
