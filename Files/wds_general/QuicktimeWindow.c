#include "Shuddup.h"
#include "MAD.h"
#include "RDriver.h"
#include "FileUtils.h"
#include <stdio.h>
#include <QuickTime/QuickTime.h>
//#include "MP3Player.h"

//quicktimewindow.proto.h

//pascal Boolean SoundConverterFillBufferDataProc(SoundComponentDataPtr *outData, void *inRefCon);
//static pascal void MySoundCallBackFunction(SndChannelPtr theChannel, SndCommand *theCmd);


extern	EventRecord				theEvent;
extern	Cursor					HandCrsr;
extern	MenuHandle				ViewsMenu;
extern	Boolean 				DragManagerUse, newQuicktime;

DialogPtr				QuicktimeDlog;

static	MovieController				gMovieController;
FSSpec						QTFile;
Movie						QTMovie;
static	Rect						QTMovieRect;
static	long						lastSeconds;
static	Str63						durationString;
static	UNFILE						QTresRefNum;
static	Boolean						QTCheckLoop, QTTemporaryFile;
static	Str255						QTFileName;
static	Boolean						canAcceptDrag;
static	ControlHandle				ResetBalanceBut, balanceCntl, volumeCntl, SaveBut, LoadBut, BassCntl, TrebleCntl, LeftCntl, RightCntl, ResetBassBut, ResetTrebleBut;
static	MovieProgressUPP			gProgressUPP;
static	short						gVolume, gBass, gTreble, gBalance;
MediaHandler				gSoundMediaHandler;
static	DataHandler					gDataHandler;
static	Track						gSoundTrack;
static	MediaEQSpectrumBandsRecord	gEQ;
Boolean						gMovieQuicktime, gBufferDone;

#if 0
static	long			   					whichBuffer = kFirstBuffer;
static	SndCommand	 						*pPlayCmd;
static	Ptr 								pDecomBuffer = NULL;
static	CmpSoundHeaderPtr 					pSndHeader = NULL;
static	SoundConverterFillBufferDataUPP 	theFillBufferDataUPP = NewSoundConverterFillBufferDataUPP(SoundConverterFillBufferDataProc);
static	SCFillBufferData 					scFillBufferData = {NULL};
static	UInt32								outputBytes = 0;
static	SndCommand							thePlayCmd0, thePlayCmd1, theCallBackCmd;
static	Ptr									pDecomBuffer0 = NULL, pDecomBuffer1 = NULL;
static	CmpSoundHeader						mySndHeader0, mySndHeader1;
static	SoundConverter						mySoundConverter = NULL;
static	SndChannelPtr 						pSoundChannel = NULL;
static	SndCallBackUPP 						theSoundCallBackUPP = NewSndCallBackUPP(MySoundCallBackFunction);
static	Handle								hSys7SoundData = NULL;
static	long								BufferSwitch = 0;
#endif
DragTrackingHandlerUPP	MyTrackingQTUPP;
DragReceiveHandlerUPP	MyReceiveQTUPP;

#define MINWIDTH	305

pascal OSErr MyTrackingQuicktime(short message, WindowPtr theWindow, void *handlerRefCon, DragReference theDrag);
pascal OSErr MyReceiveQuicktime(WindowPtr theWindow, void* handlerRefCon, DragReference theDrag);
Boolean DragQTFile( RgnHandle myRgn, EventRecord *theEvent);
OSErr ConvertMovieToAIFF(FSSpec *inputFile, FSSpec *outputFile);
void QTDoAction( Boolean play);
Boolean QTTestConversion( FSSpec	*file, OSType fileType);
void CloseQT(void);



#if 0
// * ----------------------------
// MySoundCallBackFunction
//
// used to signal when a buffer is done playing
static pascal void MySoundCallBackFunction(SndChannelPtr theChannel, SndCommand *theCmd)
{
#pragma unused(theChannel)
	
#ifndef TARGET_API_MAC_CARBON
#if !GENERATINGCFM
	long oldA5;
	oldA5 = SetA5(theCmd->param2);
#else
#pragma unused(theCmd)
#endif
#else
#pragma unused(theCmd)
#endif // TARGET_API_MAC_CARBON
	
	gBufferDone = true;
	
#ifndef TARGET_API_MAC_CARBON
#if !GENERATINGCFM
	oldA5 = SetA5(oldA5);
#endif
#endif // TARGET_API_MAC_CARBON
}

// * ----------------------------
// SoundConverterFillBufferDataProc
//
// the callback routine that provides the source data for conversion - it provides data by setting
// outData to a pointer to a properly filled out ExtendedSoundComponentData structure

pascal Boolean SoundConverterFillBufferDataProc(SoundComponentDataPtr *outData, void *inRefCon)
{
	SCFillBufferDataPtr pFillData = (SCFillBufferDataPtr)inRefCon;
	
	OSErr err = noErr;
	
	// if after getting the last chunk of data the total time is over the duration, we're done
	if (pFillData->getMediaAtThisTime >= pFillData->sourceDuration) {
		pFillData->isThereMoreSource = false;
		pFillData->compData.desc.buffer = NULL;
		pFillData->compData.desc.sampleCount = 0;
		pFillData->compData.bufferSize = 0;
	}
	
	if (pFillData->isThereMoreSource) {
		
		long		sourceBytesReturned;
		long		numberOfSamples;
		TimeValue	sourceReturnedTime, durationPerSample;
		
		HUnlock(pFillData->hSource);
		
		err = GetMediaSample(pFillData->sourceMedia,		// specifies the media for this operation
							 pFillData->hSource,			// function returns the sample data into this handle
							 pFillData->maxBufferSize,		// maximum number of bytes of sample data to be returned
							 &sourceBytesReturned,			// the number of bytes of sample data returned
							 pFillData->getMediaAtThisTime,	// starting time of the sample to be retrieved (must be in Media's TimeScale)
							 &sourceReturnedTime,			// indicates the actual time of the returned sample data
							 &durationPerSample,			// duration of each sample in the media
							 NULL,							// sample description corresponding to the returned sample data (NULL to ignore)
							 NULL,							// index value to the sample description that corresponds to the returned sample data (NULL to ignore)
							 0,								// maximum number of samples to be returned (0 to use a value that is appropriate for the media)
							 &numberOfSamples,				// number of samples it actually returned
							 NULL);							// flags that describe the sample (NULL to ignore)
		
		HLock(pFillData->hSource);
		
		
		
		if ((noErr != err) || (sourceBytesReturned == 0)) {
			pFillData->isThereMoreSource = false;
			pFillData->compData.desc.buffer = NULL;
			pFillData->compData.desc.sampleCount = 0;		
			
			if ((err != noErr) && (sourceBytesReturned > 0))
				DebugStr("\pGetMediaSample - Failed in FillBufferDataProc");
		}
		
		pFillData->getMediaAtThisTime = sourceReturnedTime + (durationPerSample * numberOfSamples);
		pFillData->compData.bufferSize = sourceBytesReturned; 
	}
	
	// set outData to a properly filled out ExtendedSoundComponentData struct
	*outData = (SoundComponentDataPtr)&pFillData->compData;
	
	return (pFillData->isThereMoreSource);
}

// * ----------------------------
// MyGetSoundDescriptionExtension
//
// this function will extract the information needed to decompress the sound file, this includes 
// retrieving the sample description, the decompression atom and setting up the sound header
OSErr MyGetSoundDescriptionExtension(Media inMedia, AudioFormatAtomPtr *outAudioAtom, CmpSoundHeaderPtr outSoundHeader)
{
	OSErr err = noErr;
	
	Size size;
	Handle extension;
	
	// Version 1 of this record includes four extra fields to store information about compression ratios. It also defines
	// how other extensions are added to the SoundDescription record.
	// All other additions to the SoundDescription record are made using QT atoms. That means one or more
	// atoms can be appended to the end of the SoundDescription record using the standard [size, type]
	// mechanism used throughout the QuickTime movie resource architecture.
	// http://developer.apple.com/techpubs/quicktime/qtdevdocs/RM/frameset.htm
	SoundDescriptionV1Handle sourceSoundDescription = (SoundDescriptionV1Handle)NewHandle(0);
	
	// get the description of the sample data
	GetMediaSampleDescription(inMedia, 1, (SampleDescriptionHandle)sourceSoundDescription);
	BailErr(GetMoviesError());
	
	extension = NewHandle(0);
	
	// get the "magic" decompression atom
	// This extension to the SoundDescription information stores data specific to a given audio decompressor.
	// Some audio decompression algorithms require a set of out-of-stream values to configure the decompressor
	// which are stored in a siDecompressionParams atom. The contents of the siDecompressionParams atom are dependent
	// on the audio decompressor.
	err = GetSoundDescriptionExtension((SoundDescriptionHandle)sourceSoundDescription, &extension, siDecompressionParams);
	
	if (noErr == err) {
		size = GetHandleSize(extension);
		HLock(extension);
		*outAudioAtom = (AudioFormatAtom*)NewPtr(size);
		err = MemError();
		// copy the atom data to our buffer...
		BlockMoveData(*extension, *outAudioAtom, size);
		HUnlock(extension);
	} else {
		// if it doesn't have an atom, that's ok
		err = noErr;
	}
	
	// set up our sound header
	outSoundHeader->format = (*sourceSoundDescription)->desc.dataFormat;
	outSoundHeader->numChannels = (*sourceSoundDescription)->desc.numChannels;
	outSoundHeader->sampleSize = (*sourceSoundDescription)->desc.sampleSize;
	outSoundHeader->sampleRate = (*sourceSoundDescription)->desc.sampleRate;
	
	DisposeHandle(extension);
	DisposeHandle((Handle)sourceSoundDescription);
	
bail:
	return err;
}
#endif

pascal OSErr QTProgressUPP(Movie theMovie, short message, short whatOperation, Fixed percentDone, long refcon)
{
	UpdateALLWindow();
	RollCursor();
	return noErr;
}

void DrawQTItem( Boolean selected, Rect* bounds, Str255 data, FSSpec	*spec, RgnHandle rgn)
{
	PenState			ps;
	short			iconID;
	Boolean			doNewWay;
	IconTransformType	iconTransform;
	Rect				iconRect, itemRect, srcRect;
	Boolean			itemEnabled = true;
	Handle			gTheSuite;
	OSErr			iErr;
	short			itemType;
	Handle			itemHandle;
	FInfo				fndrInfo;
	IconRef			iconref;
	SInt16				label;
	
	iconRect.left		= bounds->left;			// 21
	iconRect.top		= bounds->top;
	iconRect.bottom		= iconRect.top + 32;
	iconRect.right		= iconRect.left + 32;
	
	iErr = GetIconRefFromFile( spec, &iconref, &label);
	
	if( iErr == noErr)
	{
		if( !selected) PlotIconRef( &iconRect, kAlignNone, kTransformNone, kIconServicesNormalUsageFlag, iconref);
		else PlotIconRef( &iconRect, kAlignNone, kTransformSelected, kIconServicesNormalUsageFlag, iconref);
		
		if( rgn)
		{
			IconRefToRgn( rgn, bounds, 0, kIconServicesNormalUsageFlag, iconref);
		}
		
		ReleaseIconRef( iconref);
	}
	
	GetDialogItem( QuicktimeDlog, 9, &itemType, &itemHandle, &itemRect);
	srcRect = itemRect;
	
	itemRect.bottom = itemRect.top + 13;
	itemRect.left = itemRect.left +  (itemRect.right - itemRect.left)/2 - StringWidth( data)/2;
	itemRect.right = itemRect.left + 2 + StringWidth( data);
	
	if( itemRect.right > srcRect.right) itemRect.right = srcRect.right;
	if( itemRect.left < srcRect.left) itemRect.left = srcRect.left;
	
	TETextBox( data+1, data[ 0], &itemRect, teCenter);
	if( selected)
	{
		InvertRect( &itemRect);
	}
}


void  UpdateQTWindow( DialogPtr GetSelection)
{
	GrafPtr		SavePort;
	short		itemType;
	Rect		caRect, itemRect;
	Handle		itemHandle;
	
	GetPort( &SavePort);
	SetPortDialogPort( QuicktimeDlog);
	
	BeginUpdate( GetDialogWindow( QuicktimeDlog));
	
	DrawDialog( QuicktimeDlog);
	
	UpdateMovie( QTMovie);
	
	MCDraw( gMovieController, GetDialogWindow( GetSelection));
	
	GetDialogItem( QuicktimeDlog , 3, &itemType, &itemHandle, &itemRect);
	
	GetPortBounds( GetDialogPort( QuicktimeDlog), &caRect);
	
	itemRect.top = 0;
	itemRect.left = 0;
	itemRect.right = caRect.right;
	
	/*	ForeColor( whiteColor);
	 PaintRect( &itemRect);*/
	
	GetDialogItem( QuicktimeDlog , 2, &itemType, &itemHandle, &itemRect);
	DrawQTItem( false, &itemRect, QTFileName, &QTFile, NULL);
	
	InsetRect( &QTMovieRect, -1, -1);
	QTMovieRect.bottom += 16;
	FrameRect( &QTMovieRect);
	QTMovieRect.bottom -= 16;
	InsetRect( &QTMovieRect, 1, 1);
	
	EndUpdate( GetDialogWindow( QuicktimeDlog));
	
	SetPort( SavePort);
} 

short GetQuicktimeChannels( void)
{
	//return mySndHeader0.numChannels;
	
	return 2;
}

short GetQuicktimeBits( void)
{
	//return mySndHeader0.sampleSize;
	return 8;
}

Ptr GetQuicktimeSource( void)
{
	//return pDecomBuffer + BufferSwitch*256;
	return NULL;
}

void DoNullQT( void)
{
	GrafPtr				SavePort;
	TimeValue			aTime;
	TimeRecord			currentTime;
	DateTimeRec			dtrp;
	Str255				aStr, bStr;
	ComponentResult		result;
	
	if( QuicktimeDlog == NULL) return;
	
	GetPort( &SavePort);
	SetPortDialogPort( QuicktimeDlog);
	
	////////
#if 0
	{
		UInt32		actualOutputBytes;
		UInt32		outputFrames, outputFlags;
		OSErr		err;
		Boolean		isSoundDone = false;
		
		if (gBufferDone == true)
		{
			if (kFirstBuffer == whichBuffer) {
				pPlayCmd = &thePlayCmd0;
				pDecomBuffer = pDecomBuffer0;
				pSndHeader = &mySndHeader0;
				whichBuffer = kSecondBuffer;
			} else {
				pPlayCmd = &thePlayCmd1;
				pDecomBuffer = pDecomBuffer1;
				pSndHeader = &mySndHeader1;
				whichBuffer = kFirstBuffer;
			}
			
			BufferSwitch = 0;
			
			err = SoundConverterFillBuffer(mySoundConverter,		// a sound converter
										   theFillBufferDataUPP,	// the callback UPP
										   &scFillBufferData,		// refCon passed to FillDataProc
										   pDecomBuffer,			// the decompressed data 'play' buffer
										   outputBytes,				// size of the 'play' buffer
										   &actualOutputBytes,		// number of output bytes
										   &outputFrames,			// number of output frames
										   &outputFlags);			// fillbuffer retured advisor flags
			//if (err) break;
			if((outputFlags & kSoundConverterHasLeftOverData) == false) {
				isSoundDone = true;
			}
			
			pSndHeader->numFrames = outputFrames;
			
			gBufferDone = false;
			if (!isSoundDone) {
				SndDoCommand(pSoundChannel, &theCallBackCmd, true);	// reuse callBackCmd
			}
			
			SndDoCommand(pSoundChannel, pPlayCmd, true);			// play the next buffer
		}
		if( BufferSwitch < 128) BufferSwitch++;
	}
#endif
	////////
	
	MoviesTask( QTMovie, 0);
	
	aTime = GetMovieTime( QTMovie, &currentTime);
	
	if( lastSeconds != aTime / currentTime.scale)
	{
		lastSeconds = aTime / currentTime.scale;
		
		SecondsToDate( aTime / currentTime.scale, &dtrp);
		
		NTStr( 2, dtrp.minute, (Ptr) aStr);		MyC2PStr( (Ptr) aStr);
		NTStr( 2, dtrp.second, (Ptr) bStr);		MyC2PStr( (Ptr) bStr);
		pStrcat( aStr, "\p:");
		pStrcat( aStr, bStr);
		
		pStrcat( aStr, durationString);
		
		SetDText( QuicktimeDlog, 7, aStr);				// Duration
	}
	
	MCIsPlayerEvent( gMovieController, &theEvent);
	SetPort( SavePort);
	
	if( IsMovieDone( QTMovie) && QTCheckLoop)
	{
		QTCheckLoop = false;
		switch( thePrefs.LoopType)
		{
			case 0:
			{
				Fixed playRate = 1;
				MCDoAction( gMovieController, mcActionPrerollAndPlay, (void*) playRate);
				
				GoToBeginningOfMovie( QTMovie);
				StartMovie( QTMovie);
			}
				break;
				
			default:
				QTDoAction( false);
				DoLoadOtherMusic( true);
				
				//CloseQT();
				break;
		}
	}
	
#if 0
	if( gSoundMediaHandler)
	{
		SoundComponentDataPtr sourceData;
		Component	outputComponent;
		ComponentInstance	ci;
		SoundComponentDataPtr	sd;
		
		result = MediaGetSoundOutputComponent( gSoundMediaHandler, &outputComponent);
		if( result) DebugStr("\pError");
		
		result = SoundComponentGetSourceData( gSoundMediaHandler, &sourceData);
		if( result) DebugStr("\pError");
		
		Debugger();
	}
	
	if( gSoundMediaHandler)
	{
		MediaGetSoundBassAndTreble( gSoundMediaHandler, &gBass, &gTreble);
		
		NumToString( gBass, aStr);
		SetDText( QuicktimeDlog, 13, aStr);
	}
#endif
	
	if( gSoundMediaHandler && newQuicktime)
	{
		LevelMeterInfo	info;
		ComponentResult	cResult;
		
		cResult = MediaGetSoundLevelMeterInfo( gSoundMediaHandler, &info);
		
		if( cResult == 0)
		{
			SetControlValue( LeftCntl, info.leftMeter);
			SetControlValue( RightCntl, info.rightMeter);
		}
		else
		{
			SetControlValue( LeftCntl, 0);
			SetControlValue( RightCntl, 0);
		}
	}
	
}

void QTDoAction( Boolean play)
{
	Fixed playRate;
	
	if( QuicktimeDlog == NULL) return;
	
	if( play)
	{
		playRate = 1;
		MCDoAction( gMovieController, mcActionPrerollAndPlay, (void*) playRate);
	}
	else
	{
		playRate = 0;
		MCDoAction( gMovieController, mcActionPrerollAndPlay, (void*) playRate);
	}
}

void SelectQTFile( FSSpec	*file)
{
	Rect					caRect, itemRect, tempRect, dataBounds, dispBounds;
	Handle					itemHandle;
	short					itemType, itemHit, temp, i;
	Point					cSize;
	FontInfo				ThisFontInfo;
	Str255					str;
	GrafPtr					savePort;
	ComponentInstance		ci;
	OSType					fileType;
	OSErr					iErr;
	Boolean					boolVal, canceled, dataRefWasChanged;
	Track					usedTrack;
	long					outFlags;
	short					resId;
	FInfo					fndrInfo;
	long					fileSize;
	FSSpec					newFile;
	
	
	QTTemporaryFile = false;
	
	GetPort( &savePort);
	SetPortDialogPort( QuicktimeDlog);
	
	if( QTMovie != NULL)
	{
		MyDebugStr(__LINE__, __FILE__, "QTMovie != NULL");
	}
	
	QTFile = *file;
	QTMovie = NULL;
	
	FSpGetFInfo( &QTFile, &fndrInfo);
	
	SetWTitle( GetDialogWindow( QuicktimeDlog), QTFile.name);
	
	pStrcpy( QTFileName, QTFile.name);
	
	fileSize = 0;
	iErr = FSpOpenDF( &QTFile, fsCurPerm, &QTresRefNum);
	if( iErr == noErr)
	{
		iErr = GetEOF( QTresRefNum, &fileSize);
		FSCloseFork( QTresRefNum);
		
		if( fileSize == 0)
		{
			iErr = FSpOpenRF( &QTFile, fsCurPerm, &QTresRefNum);
			if( iErr == noErr)
			{
				iErr = GetEOF( QTresRefNum, &fileSize);
				FSCloseFork( QTresRefNum);
			}
		}
	}
	
RETRY:
	
	iErr = OpenMovieFile( &QTFile, &QTresRefNum, fsCurPerm);
	if( iErr) MyDebugStr(__LINE__, __FILE__, "OpenMovieFile");
	
	resId = 0;
	iErr = NewMovieFromFile( &QTMovie, QTresRefNum, &resId, NULL, 0, &dataRefWasChanged);
	
	CloseMovieFile( QTresRefNum);
	
#if 0
	if( iErr != noErr)
	{
		return;
	}
#endif
	
	if( iErr == -2048 && QTTemporaryFile == false)	// TRY A CONVERSION FILE->MOVIE
	{
		newFile = QTFile;
		pStrcpy( newFile.name, "\pPlayerPRO.temp");
		
		iErr = FindFolder( kOnSystemDisk, kTemporaryFolderType, kCreateFolder, &newFile.vRefNum, &newFile.parID);
		if( iErr == noErr)
		{
			FSpDelete( &newFile);
			
			resId = 0;
			iErr = ConvertFileToMovieFile( &QTFile, &newFile, 'TVOD', smCurrentScript, &resId, createMovieFileDeleteCurFile, 0, 0, 0);
			if( iErr == noErr)
			{
				QTFile = newFile;
				QTTemporaryFile = true;
				
				goto RETRY;
			}
			else return;
		}
		else return;
	}
	else if( iErr != noErr) return;
	
	////////////////////////////////
	
#if 0
	{
		AudioCompressionAtomPtr	theDecompressionAtom;
		SoundComponentData		theInputFormat,
		theOutputFormat;
		
		Media					theSoundMedia = NULL;
		
		Ptr						pSourceBuffer = NULL;
		
		OSErr 					err = noErr;
		
		// get the first sound track
		usedTrack = GetMovieIndTrackType( QTMovie, 1, SoundMediaType, movieTrackMediaType);
		if (NULL == usedTrack ) iErr = invalidTrack;
		
		// get and return the sound track media
		theSoundMedia = GetTrackMedia( usedTrack);
		if (NULL == theSoundMedia) iErr = invalidTrack;
		
		err = MyGetSoundDescriptionExtension(theSoundMedia, (AudioFormatAtomPtr *)&theDecompressionAtom, &mySndHeader0);
		if (noErr == err) {	
			// setup input/output format for sound converter
			theInputFormat.flags = 0;
			theInputFormat.format = mySndHeader0.format;
			theInputFormat.numChannels = mySndHeader0.numChannels;
			theInputFormat.sampleSize = mySndHeader0.sampleSize;
			theInputFormat.sampleRate = mySndHeader0. sampleRate;
			theInputFormat.sampleCount = 0;
			theInputFormat.buffer = NULL;
			theInputFormat.reserved = 0;
			
			theOutputFormat.flags = 0;
			theOutputFormat.format = kSoundNotCompressed;
			theOutputFormat.numChannels = theInputFormat.numChannels;
			theOutputFormat.sampleSize = theInputFormat.sampleSize;
			theOutputFormat.sampleRate = theInputFormat.sampleRate;
			theOutputFormat.sampleCount = 0;
			theOutputFormat.buffer = NULL;
			theOutputFormat.reserved = 0;
			
			err = SoundConverterOpen(&theInputFormat, &theOutputFormat, &mySoundConverter);
			BailErr(err);
			
			// set up the sound converters decompresson 'environment' by passing in the 'magic' decompression atom
			err = SoundConverterSetInfo(mySoundConverter, siDecompressionParams, theDecompressionAtom);
			if (siUnknownInfoType == err) {
				// clear this error, the decompressor didn't
				// need the decompression atom and that's OK
				err = noErr;
			} else BailErr(err);
			
			UInt32	targetBytes = 32768;
			
			// find out how much buffer space to alocate for our output buffers
			// The differences between SoundConverterConvertBuffer begin with how the buffering is done. SoundConverterFillBuffer will do as much or as
			// little work as is required to satisfy a given request. This means that you can pass in buffers of any size you like and expect that
			// the Sound Converter will never overflow the output buffer. SoundConverterFillBufferDataProc function will be called as many times as
			// necessary to fulfill a request. This means that the SoundConverterFillBufferDataProc routine is free to provide data in whatever chunk size
			// it likes. Of course with both sides, the buffer sizes will control how many times you need to request data and there is a certain amount of
			// overhead for each call. You will want to balance this against the performance you require. While a call to SoundConverterGetBufferSizes is
			// not required by the SoundConverterFillBuffer function, it is useful as a guide for non-VBR formats
			do {
				UInt32 inputFrames, inputBytes;
				targetBytes *= 2;
				err = SoundConverterGetBufferSizes(mySoundConverter, targetBytes, &inputFrames, &inputBytes, &outputBytes);
			} while (notEnoughBufferSpace == err  && targetBytes < (MaxBlock() / 4));
			
			// allocate play buffers
			pDecomBuffer0 = NewPtr(outputBytes);
			BailErr(MemError());
			
			pDecomBuffer1 = NewPtr(outputBytes);
			BailErr(MemError());
			
			err = SoundConverterBeginConversion(mySoundConverter);
			BailErr(err);
			
			// setup first header
			mySndHeader0.samplePtr = pDecomBuffer0;
			mySndHeader0.numChannels = theOutputFormat.numChannels;
			mySndHeader0.sampleRate = theOutputFormat.sampleRate;
			mySndHeader0.loopStart = 0;
			mySndHeader0.loopEnd = 0;
			mySndHeader0.encode = cmpSH;					// compressed sound header encode value
			mySndHeader0.baseFrequency = kMiddleC;
			// mySndHeader0.AIFFSampleRate;					// this is not used
			mySndHeader0.markerChunk = NULL;
			mySndHeader0.format = theOutputFormat.format;
			mySndHeader0.futureUse2 = 0;
			mySndHeader0.stateVars = NULL;
			mySndHeader0.leftOverSamples = NULL;
			mySndHeader0.compressionID = fixedCompression;	// compression ID for fixed-sized compression, even uncompressed sounds use fixedCompression
			mySndHeader0.packetSize = 0;					// the Sound Manager will figure this out for us
			mySndHeader0.snthID = 0;
			mySndHeader0.sampleSize = theOutputFormat.sampleSize;
			mySndHeader0.sampleArea[0] = 0;					// no samples here because we use samplePtr to point to our buffer instead
			
			// setup second header, only the buffer ptr is different
			BlockMoveData(&mySndHeader0, &mySndHeader1, sizeof(mySndHeader0));
			mySndHeader1.samplePtr = pDecomBuffer1;
			
			// fill in struct that gets passed to SoundConverterFillBufferDataProc via the refcon
			// this includes the ExtendedSoundComponentData record		
			scFillBufferData.sourceMedia = theSoundMedia;
			scFillBufferData.getMediaAtThisTime = 0;		
			scFillBufferData.sourceDuration = GetMediaDuration(theSoundMedia);
			scFillBufferData.isThereMoreSource = true;
			scFillBufferData.maxBufferSize = kMaxInputBuffer;
			scFillBufferData.hSource = NewHandle((long)scFillBufferData.maxBufferSize);		// allocate source media buffer
			BailErr(MemError());
			
			scFillBufferData.compData.desc = theInputFormat;
			scFillBufferData.compData.desc.buffer = (Byte *)*scFillBufferData.hSource;
			scFillBufferData.compData.desc.flags = kExtendedSoundData;
			scFillBufferData.compData.recordSize = sizeof(ExtendedSoundComponentData);
			scFillBufferData.compData.extendedFlags = kExtendedSoundSampleCountNotValid | kExtendedSoundBufferSizeValid;
			scFillBufferData.compData.bufferSize = 0;
			
			// setup the callback, create the sound channel and play the sound
			// we will continue to convert the sound data into the free (non playing) buffer
			
			err = SndNewChannel(&pSoundChannel, sampledSynth, 0, theSoundCallBackUPP);
			
			if (err == noErr) {			
				
				
				thePlayCmd0.cmd = bufferCmd;
				thePlayCmd0.param1 = 0;						// not used, but clear it out anyway just to be safe
				thePlayCmd0.param2 = (long)&mySndHeader0;
				
				thePlayCmd1.cmd = bufferCmd;
				thePlayCmd1.param1 = 0;						// not used, but clear it out anyway just to be safe
				thePlayCmd1.param2 = (long)&mySndHeader1;
				
				whichBuffer = kFirstBuffer;					// buffer 1 will be free when callback runs
				theCallBackCmd.cmd = callBackCmd;
				theCallBackCmd.param2 = SetCurrentA5();
				
				gBufferDone = true;
				
				//	if (noErr == err) {
				//		while (!isSoundDone && !Button()) {						
				
				//		} // while
			}
		}
	}
#endif
	////////////////////////////////
	
	
	
	GetMovieBox( QTMovie, &dispBounds);
	OffsetRect( &dispBounds, -dispBounds.left, -dispBounds.top);
	
	GetDialogItem( QuicktimeDlog , 3, &itemType, &itemHandle, &QTMovieRect);
	
	GetPortBounds( GetDialogPort( QuicktimeDlog), &caRect);
	
	if( EmptyRect( &dispBounds))
	{
		QTMovieRect.left = 0;
		QTMovieRect.right = MINWIDTH;	//caRect.right;
		QTMovieRect.bottom = QTMovieRect.top;
	}
	else
	{
		QTMovieRect.left = 0;
		QTMovieRect.right = QTMovieRect.left + dispBounds.right;
		QTMovieRect.bottom = QTMovieRect.top + dispBounds.bottom - dispBounds.top;
		
		if( QTMovieRect.right < MINWIDTH)
		{
			QTMovieRect.left = MINWIDTH/2 - dispBounds.right/2;
			
			QTMovieRect.right = QTMovieRect.left + dispBounds.right;
		}
	}
	
	EraseRect( &caRect);
	InvalWindowRect( GetDialogWindow( QuicktimeDlog), &caRect);
	
	if( QTMovieRect.right < MINWIDTH) SizeWindow( GetDialogWindow( QuicktimeDlog), MINWIDTH, QTMovieRect.bottom + 16, true);
	else SizeWindow( GetDialogWindow( QuicktimeDlog), QTMovieRect.right, QTMovieRect.bottom + 16, true);
	
	SetMovieBox( QTMovie, &QTMovieRect);
	
	SetPortDialogPort( QuicktimeDlog);
	SetMovieGWorld( QTMovie, nil, nil);
	
	GoToBeginningOfMovie( QTMovie);
	iErr = PrerollMovie( QTMovie, 0, 0);
	
	// Get informations about the movie
	{
		Track						aTrack;
		Media						aMedia;
		TimeValue					aTime;
		TimeScale					aTimeScale;
		OSType						mediaType, creatorManufacturer;
		Str255						creatorName;
		DateTimeRec					dtrp;
		Str255						aStr, bStr;
		short						refNum;
		SoundDescriptionHandle		descH;
		
		gSoundMediaHandler = NULL;
		gDataHandler = NULL;
		
		for( i = 1; i <= GetMovieTrackCount( QTMovie); i++)
		{
			aTrack = GetMovieIndTrack( QTMovie, i);
			
			aMedia = GetTrackMedia( aTrack);
			
			GetMediaHandlerDescription( aMedia, &mediaType, creatorName, &creatorManufacturer);
			
			if( mediaType == SoundMediaType)	// Bingo !
			{
				gSoundMediaHandler = GetMediaHandler( aMedia);
				gDataHandler = GetMediaDataHandler( aMedia, 1);
				gSoundTrack = aTrack;
				
				gVolume = GetMovieVolume( QTMovie);
				SetControlValue( volumeCntl, gVolume);
				
				MediaGetSoundBalance( gSoundMediaHandler, &gBalance);
				NumToString( gBalance, aStr);	SetDText( QuicktimeDlog, 30, aStr);
				
				// QT 4.0
				
				if( newQuicktime)
				{
					MediaGetSoundBassAndTreble( gSoundMediaHandler, &gBass, &gTreble);
					
					NumToString( gTreble, aStr);	SetDText( QuicktimeDlog, 18, aStr);
					NumToString( gBass, aStr);	SetDText( QuicktimeDlog, 19, aStr);
					
					MediaGetSoundEqualizerBands( gSoundMediaHandler, &gEQ);
					
					MediaSetSoundLevelMeteringEnabled( gSoundMediaHandler, true);
				}
				
				
				///	///	///	///	///	///	///	///	///	///	///	///	///	///	///	///	
#if 0
				fileSize = 0;
				iErr = FSpOpenDF( &QTFile, fsCurPerm, &refNum);
				if( iErr == noErr)
				{
					iErr = GetEOF( refNum, &fileSize);
					FSClose( refNum);
				}
#endif
				fileSize /= 1024L;
				
				if( fileSize > 1000)
				{
					sprintf( (Ptr) aStr, "%.2f", (double) fileSize / (double) 1024.);
					MyC2PStr( (Ptr) aStr);
					pStrcat( aStr, "\p Mb");
				}
				else
				{
					NumToString( fileSize, aStr);
					pStrcat( aStr, "\p Kb");
				}
				SetDText( QuicktimeDlog, 4, aStr);		// File Size
				
				///	///	///	///	///	///	///	///	///	///	///	///	///	///	///	///	
				
				aTime = GetMovieDuration( QTMovie);
				aTimeScale = GetMovieTimeScale( QTMovie);
				//aTime = GetMediaDuration( aMedia);
				//aTimeScale = GetMediaTimeScale( aMedia);
				SecondsToDate( aTime / aTimeScale, &dtrp);
				
				NTStr( 2, dtrp.minute, (Ptr) aStr);		MyC2PStr( (Ptr) aStr);
				NTStr( 2, dtrp.second, (Ptr) bStr);		MyC2PStr( (Ptr) bStr);
				pStrcat( aStr, "\p:");
				pStrcat( aStr, bStr);
				
				pStrcpy( durationString, "\p / ");
				pStrcat( durationString, aStr);
				
				//SetDText( QuicktimeDlog, 7, aStr);				// Duration
				
				///	///	///	///	///	///	///	///	///	///	///	///	///	///	///	///	
				
				descH = (SoundDescriptionHandle) NewHandleClear( 1);
				
				GetMediaSampleDescription( aMedia, 1, (SampleDescriptionHandle) descH);
				
				HLock( (Handle) descH);
				
				OSType2Str( fndrInfo.fdType, aStr);
				pStrcat( aStr, "\p/");
				
				if( (*descH)->numChannels == 1) pStrcat( aStr, "\pmono");
				if( (*descH)->numChannels == 2) pStrcat( aStr, "\pstereo");
				if( (*descH)->numChannels > 2)
				{
					NumToString( (*descH)->numChannels, bStr);
					pStrcat( aStr, bStr);
				}
				pStrcat( aStr, "\p/");
				
				NumToString( (*descH)->sampleSize, bStr);
				pStrcat( aStr, bStr);
				pStrcat( aStr, "\p Bits/");
				
				sprintf( (Ptr) bStr, "%.1f Khz", (double) ((*descH)->sampleRate>>16L) / (double) 1000.);
				
				MyC2PStr( (Ptr) bStr);
				pStrcat( aStr, bStr);
				
				//NumToString( (*descH)->sampleRate>>16L, bStr);
				//pStrcat( aStr, bStr);
				//pStrcat( aStr, "\p Hz");
				
				SetDText( QuicktimeDlog, 8, aStr);		// File Type
				
				
				///	///	///	///	///	///	///	///	///	///	///	///	///	///	///	///
				
				OSType2Str( (*descH)->dataFormat, aStr);
				pStrcat( aStr, "\p/");
				GetCompressionName( (*descH)->dataFormat, bStr);
				pStrcat( aStr, bStr);
				
				SetDText( QuicktimeDlog, 11, aStr);		// Compressor
				
				HUnlock( (Handle) descH);
				DisposeHandle( (Handle) descH);
			}
		}
	}
	
	//GetMediaSampleDescription( theMedia, index, descH);
	
	SetMovieActive( QTMovie, true);
	
	if( gMovieController) DisposeMovieController( gMovieController);
	
	//QTMovieRect.left--;
	//QTMovieRect.right++;
	gMovieController = NewMovieController( QTMovie, &QTMovieRect, mcTopLeftMovie);//  + mcScaleMovieToFit);
	
	
	//QTMovieRect.left++;
	//QTMovieRect.right--;
	
	//ShowMovieInformation( QTMovie, NULL, 0);
	
	boolVal = true;
	MCDoAction( gMovieController, mcActionSetKeysEnabled, &boolVal);
	
	MCEnableEditing( gMovieController, true);
	
	boolVal = true;
	MCDoAction( gMovieController, mcActionSetPlaySelection, &boolVal);
	
	SetPort( savePort);
	
bail:
	
	return;
}

void InitQTWindow( void)
{
	QTMovie = NULL;
	gMovieController = NULL;
	QTTemporaryFile = false;
	QTCheckLoop = false;
	gSoundMediaHandler = NULL;
	gDataHandler = NULL;
	gSoundTrack = NULL;
	gMovieQuicktime = false;
	
	
	MyTrackingQTUPP = NULL;
	MyReceiveQTUPP = NULL;
	
	gProgressUPP = NewMovieProgressUPP( QTProgressUPP);
}

Boolean CreateQTWindow( FSSpec	*file)
{
	Rect					itemRect, tempRect, dataBounds;
	Handle					itemHandle;
	short					itemType, itemHit, temp, i;
	Point					cSize;
	FontInfo				ThisFontInfo;
	Str255					String;
	GrafPtr					savePort;
	MovieImportComponent	ci;
	OSErr					iErr;
	Boolean					canceled;
	Track					usedTrack;
	long					outFlags;
	FInfo					fndrInfo;
	MovieProgressUPP		progressUPP;
	GrafPtr					SavePort;
	
	FSpGetFInfo( file, &fndrInfo);
	
	lastSeconds = -1;
	
	if( !QTTestConversion( file, fndrInfo.fdType)) return false;
	
	if( QuicktimeDlog != NULL)
	{
		
		SelectWindow2( NextWindowVisible( GetDialogWindow( QuicktimeDlog)));
		CloseQT();
	}
	
	UpdateALLWindow();
	
	QuicktimeDlog = GetNewDialog( 180, NULL, (WindowPtr) ToolsDlog);
	
	GetPort( &SavePort);
	SetWindEtat( GetDialogWindow(QuicktimeDlog));
	SetPortDialogPort( QuicktimeDlog);
	
	TextFont( kFontIDGeneva);
	TextSize( 9);
	
	GetDialogItem( QuicktimeDlog , 12, &itemType, &itemHandle, &itemRect);
	//itemRect.right = itemRect.left;
	SaveBut = NewControl( 	GetDialogWindow( QuicktimeDlog),
						 &itemRect,
						 "\p",
						 true,
						 0,
						 kControlContentIconSuiteRes,
						 151,
						 kControlBevelButtonNormalBevelProc,
						 0);
	
	GetDialogItem( QuicktimeDlog , 20, &itemType, &itemHandle, &itemRect);
	//itemRect.right = itemRect.left;
	LoadBut = NewControl( 	GetDialogWindow( QuicktimeDlog),
						 &itemRect,
						 "\p",
						 true,
						 0,
						 kControlContentIconSuiteRes,
						 152,
						 kControlBevelButtonNormalBevelProc,
						 0);
	
	GetDialogItem( QuicktimeDlog , 25, &itemType, &itemHandle, &itemRect);
	//itemRect.right = itemRect.left;
	ResetBassBut = NewControl( 	GetDialogWindow( QuicktimeDlog),
							  &itemRect,
							  "\p",
							  true,
							  0,
							  kControlContentIconSuiteRes,
							  168,
							  kControlBevelButtonNormalBevelProc,
							  0);
	
	GetDialogItem( QuicktimeDlog , 24, &itemType, &itemHandle, &itemRect);
	//itemRect.right = itemRect.left;
	ResetTrebleBut = NewControl( 	GetDialogWindow( QuicktimeDlog),
								&itemRect,
								"\p",
								true,
								0,
								kControlContentIconSuiteRes,
								168,
								kControlBevelButtonNormalBevelProc,
								0);
	
	GetDialogItem( QuicktimeDlog , 21, &itemType, &itemHandle, &itemRect);
	//itemRect.right = itemRect.left;
	ResetBalanceBut = NewControl( 	GetDialogWindow( QuicktimeDlog),
								 &itemRect,
								 "\p",
								 true,
								 0,
								 kControlContentIconSuiteRes,
								 168,
								 kControlBevelButtonNormalBevelProc,
								 0);
	
	HiliteControl( ResetBassBut, 255);
	HiliteControl( ResetTrebleBut, 255);
	HiliteControl( ResetBalanceBut, 255);
	
	//controls.h
	GetDialogItem( QuicktimeDlog, 17, &itemType, &itemHandle, &itemRect);
	BassCntl = NewControl( GetDialogWindow( QuicktimeDlog), &itemRect, "\p", true, 0, -256, 256, 80, 0);
	
	GetDialogItem( QuicktimeDlog, 16, &itemType, &itemHandle, &itemRect);
	TrebleCntl = NewControl( GetDialogWindow( QuicktimeDlog), &itemRect, "\p", true, 0, -256, 256, 80, 0);
	
	GetDialogItem( QuicktimeDlog, 22, &itemType, &itemHandle, &itemRect);
	LeftCntl = NewControl( GetDialogWindow( QuicktimeDlog), &itemRect, "\p", true, 0, 0, 255, 80, 0);
	
	GetDialogItem( QuicktimeDlog, 23, &itemType, &itemHandle, &itemRect);
	RightCntl = NewControl( GetDialogWindow( QuicktimeDlog), &itemRect, "\p", true, 0, 0, 255, 80, 0);
	
	GetDialogItem( QuicktimeDlog, 29, &itemType, &itemHandle, &itemRect);
	volumeCntl = NewControl( GetDialogWindow( QuicktimeDlog), &itemRect, "\p", true, 0, 0, 255, 80, 0);
	
	GetDialogItem( QuicktimeDlog, 27, &itemType, &itemHandle, &itemRect);
	balanceCntl = NewControl( GetDialogWindow( QuicktimeDlog), &itemRect, "\p", true, 0, -128, 128, 57, 0);
	
	
	SelectQTFile( file);
	
	if( QTMovie == NULL)	// Quicktime wasn't able to open it....
	{
		CloseQT();
		
		Erreur( 96, 96);
		
		SetPort( SavePort);
		return false;
	}
	SelectWindow2( GetDialogWindow( QuicktimeDlog));
	
	ShowWindow( GetDialogWindow( QuicktimeDlog));
	
	
	if( thePrefs.AutoPlayWhenOpen)
	{
		Fixed	playRate;
		
		playRate = 1;
		MCDoAction( gMovieController, mcActionPrerollAndPlay, (void*) playRate);
		
	}
	
	if( thePrefs.LoopType == 0)	// LOOP
	{
		MCDoAction( gMovieController, mcActionSetLooping, (void*) (Boolean) true);
	}
	else
	{
		MCDoAction( gMovieController, mcActionSetLooping, (void*) (Boolean) false);
	}
	
	QTCheckLoop = true;
	
	if( DragManagerUse)
	{
		MyTrackingQTUPP = NewDragTrackingHandlerUPP( MyTrackingQuicktime);
		MyReceiveQTUPP = NewDragReceiveHandlerUPP( MyReceiveQuicktime);
		
		InstallTrackingHandler( (DragTrackingHandlerUPP) MyTrackingQTUPP, GetDialogWindow( QuicktimeDlog), (void *) NULL);
		InstallReceiveHandler( (DragReceiveHandlerUPP) MyReceiveQTUPP, GetDialogWindow( QuicktimeDlog), (void *) NULL);
	}
	
	SetPort( SavePort);
	return true;
}

Boolean QTTestConversion( FSSpec	*file, OSType fileType)
{
	OSErr	iErr;
	short	QTresRefNum;
	
	iErr = OpenMovieFile( file, &QTresRefNum, fsCurPerm);
	if( iErr) return false;
	
	iErr = CloseMovieFile( QTresRefNum);
	
	return true;
}

Boolean QTTypeConversion( OSType fileType)
{
	switch( fileType)
	{
		case kQTFileTypeAIFF:
		case kQTFileTypeAIFC:
		case kQTFileTypeDVC:
		case kQTFileTypeMIDI:
		case kQTFileTypeMovie:
		case kQTFileTypeWave:
		case kQTFileTypeMuLaw:
		case kQTFileTypeAVI:
		case kQTFileTypeSoundDesignerII:
		case kQTFileTypeSystemSevenSound:
		case 'MPEG':
		case 'Mp3 ':
		case 'MPG3':
		case 'mp3 ':
		case 'MP3 ':
		case 'mpg4':
		case 'PLAY':
			//case '????':
			return true;
			break;
	}
	
	return false;
}

void DoItemPressQT( short whichItem, DialogPtr whichDialog)
{
	short				temp,itemType, newPL, newPat, newPartitionReader;
	Point				myPt;
	Handle				itemHandle;
	Rect				itemRect;
	long				oldH, tempLong, max, val;
	GrafPtr				savePort;
	Boolean				ReadingCopy;
	ControlActionUPP	MyControlUPP;
	FSSpec				newFile;
	Str255				str;
	
	GetPort( &savePort);
	SetPortDialogPort( whichDialog);
	
	if( !MCIsPlayerEvent( gMovieController, &theEvent))
	{
		switch( whichItem)
		{
			case 20:
				if( GetControlHilite( LoadBut) == 0 && MyTrackControl( LoadBut, theEvent.where, NULL))
				{
					HandleFileChoice( 2);
				}
				break;
				
			case 25:
				if( GetControlHilite( ResetBassBut)  == 0 && MyTrackControl( ResetBassBut, theEvent.where, NULL))
				{
					HiliteControl( ResetBassBut, 255);
					
					gBass = 0;
					
					if( newQuicktime) MediaSetSoundBassAndTreble( gSoundMediaHandler, gBass, gTreble);
					NumToString( gBass, str);	SetDText( QuicktimeDlog, 19, str);
					
					SetControlValue( BassCntl, gBass);
				}
				break;
				
			case 24:
				if( GetControlHilite( ResetTrebleBut) == 0 && MyTrackControl( ResetTrebleBut, theEvent.where, NULL))
				{
					HiliteControl( ResetTrebleBut, 255);
					
					gTreble = 0;
					
					if( newQuicktime) MediaSetSoundBassAndTreble( gSoundMediaHandler, gBass, gTreble);
					NumToString( gTreble, str);	SetDText( QuicktimeDlog, 18, str);
					
					SetControlValue( TrebleCntl, gTreble);
				}
				break;
				
			case 21:
				if( GetControlHilite( ResetBalanceBut) == 0 && MyTrackControl( ResetBalanceBut, theEvent.where, NULL))
				{
					HiliteControl( ResetBalanceBut, 255);
					
					gBalance = 0;
					
					MediaSetSoundBalance( gSoundMediaHandler, gBalance);
					NumToString( gTreble, str);	SetDText( QuicktimeDlog, 30, str);
					
					SetControlValue( balanceCntl, gBalance);
				}
				break;
				
			case 27:
				GetMouse( &myPt);
				GetDialogItem (whichDialog, whichItem, &itemType, &itemHandle, &itemRect);
				
				if(PtInRect( myPt, &itemRect))
				{
					while( Button())
					{
						WaitNextEvent( everyEvent, &theEvent, 1, NULL);
						DoGlobalNull();
						SetPortDialogPort( whichDialog);
						GetMouse( &myPt);
						
						if( oldH != myPt.h)
						{
							oldH = myPt.h;
							
							if( myPt.h < itemRect.left) 		myPt.h = itemRect.left;
							else if ( myPt.h > itemRect.right)	myPt.h = itemRect.right;
							
							gBalance = (255* (myPt.h - itemRect.left)) / ( itemRect.right-itemRect.left);
							gBalance -= 127;
							
							SetControlValue( balanceCntl, gBalance);
							
							MediaSetSoundBalance( gSoundMediaHandler, gBalance);
							
							NumToString( gBalance, str);	SetDText( QuicktimeDlog, 30, str);
							
							if( gBalance != 0) HiliteControl( ResetBalanceBut, 0);
							else HiliteControl( ResetBalanceBut, 255);
						}
					}
				}
				break;
				
			case 17:
				GetMouse( &myPt);
				GetDialogItem (whichDialog, whichItem, &itemType, &itemHandle, &itemRect);
				
				if(PtInRect( myPt, &itemRect))
				{
					while( Button())
					{
						WaitNextEvent( everyEvent, &theEvent, 1, NULL);
						DoGlobalNull();
						SetPortDialogPort( whichDialog);
						GetMouse( &myPt);
						
						if( oldH != myPt.h)
						{
							oldH = myPt.h;
							
							if( myPt.h < itemRect.left) 		myPt.h = itemRect.left;
							else if ( myPt.h > itemRect.right)	myPt.h = itemRect.right;
							
							gBass = (512* (myPt.h - itemRect.left)) / ( itemRect.right-itemRect.left);
							gBass -= 256;
							
							SetControlValue( BassCntl, gBass);
							
							if( newQuicktime) MediaSetSoundBassAndTreble( gSoundMediaHandler, gBass, gTreble);
							
							NumToString( gBass, str);	SetDText( QuicktimeDlog, 19, str);
							
							if( gBass != 0) HiliteControl( ResetBassBut, 0);
							else HiliteControl( ResetBassBut, 255);
						}
					}
				}
				break;
				
			case 16:
				GetMouse( &myPt);
				GetDialogItem (whichDialog, whichItem, &itemType, &itemHandle, &itemRect);
				
				if(PtInRect( myPt, &itemRect))
				{
					while( Button())
					{
						WaitNextEvent( everyEvent, &theEvent, 1, NULL);
						DoGlobalNull();
						SetPortDialogPort( whichDialog);
						GetMouse( &myPt);
						
						if( oldH != myPt.h)
						{
							oldH = myPt.h;
							
							if( myPt.h < itemRect.left) 		myPt.h = itemRect.left;
							else if ( myPt.h > itemRect.right)	myPt.h = itemRect.right;
							
							gTreble = (512* (myPt.h - itemRect.left)) / ( itemRect.right-itemRect.left);
							gTreble -= 256;
							
							SetControlValue( TrebleCntl, gTreble);
							
							if( newQuicktime) MediaSetSoundBassAndTreble( gSoundMediaHandler, gBass, gTreble);
							
							NumToString( gTreble, str);	SetDText( QuicktimeDlog, 18, str);
							
							if( gTreble != 0) HiliteControl( ResetTrebleBut, 0);
							else HiliteControl( ResetTrebleBut, 255);
						}
					}
				}
				break;
				
			case 29:
				GetMouse( &myPt);
				GetDialogItem (whichDialog, whichItem, &itemType, &itemHandle, &itemRect);
				
				if(PtInRect( myPt, &itemRect))
				{
					while( Button())
					{
						WaitNextEvent( everyEvent, &theEvent, 1, NULL);
						DoGlobalNull();
						SetPortDialogPort( whichDialog);
						GetMouse( &myPt);
						
						if( oldH != myPt.h)
						{
							oldH = myPt.h;
							
							if( myPt.h < itemRect.left) 		myPt.h = itemRect.left;
							else if ( myPt.h > itemRect.right)	myPt.h = itemRect.right;
							
							gVolume = (255* (myPt.h - itemRect.left)) / ( itemRect.right-itemRect.left);
							
							SetControlValue( volumeCntl, gVolume);
							
							SetMovieVolume( QTMovie, gVolume);
							
							//	NumToString( gTreble, str);	SetDText( QuicktimeDlog, 18, str);
							
							/*	if( gTreble != 0) HiliteControl( ResetTrebleBut, 0);
							 else HiliteControl( ResetTrebleBut, 255);	*/
						}
					}
				}
				break;
				
			case 12:
				if( GetControlHilite( SaveBut) == 0 && MyTrackControl( SaveBut, theEvent.where, NULL))
				{
					/*	mcActionGetSelectionBegin
					 mcActionGetSelectionDuration*/
					
					newFile = QTFile;
					ConvertMovieToAIFF( &QTFile, &newFile);
					
					ForceUpdateALLWindow();
					
					AddMODList( true, newFile.name, newFile.vRefNum, newFile.parID);
				}
				break;
#if 0
			case 2:
				GetDialogItem( QuicktimeDlog , 2, &itemType, &itemHandle, &itemRect);
				DrawQTItem( true, &itemRect, QTFileName, &QTFile, NULL);
				
				if( WaitMouseMoved( theEvent.where))
				{
					RgnHandle	dstRgn;
					Handle		gTheSuite;
					SInt16	label;
					IconRef	iconref;
					OSErr	iErr;
					
					dstRgn = NewRgn();
					
					GetDialogItem( QuicktimeDlog , 2, &itemType, &itemHandle, &itemRect);
					
					iErr = GetIconRefFromFile( &QTFile, &iconref, &label);
					if( iErr == noErr)
					{
						IconRefToRgn( dstRgn, &itemRect, 0, kIconServicesNormalUsageFlag, iconref);
						
						ReleaseIconRef( iconref);
					}
					
					//	srcRgnB = NewRgn();
					//	OpenRgn();
					//	FrameRect( &itemRect);
					//	CloseRgn( srcRgnB);
					
					//	UnionRgn( tempRgn, srcRgnB, tempRgn);
					//	DisposeRgn( srcRgnB);
					
					DragQTFile( dstRgn, &theEvent);
					
					DisposeRgn( dstRgn);
				}
				
				GetDialogItem( QuicktimeDlog , 2, &itemType, &itemHandle, &itemRect);
				DrawQTItem( false, &itemRect, QTFileName, &QTFile, NULL);
				break;
#endif
		}
	}
	else
	{
		gVolume = GetMovieVolume( QTMovie);
		SetControlValue( volumeCntl, gVolume);
	}
	
	SetPort( savePort);
}

void CloseQT(void)
{
	OSErr	iErr, err;
	
	if( QuicktimeDlog != NULL)
	{
		//////////////////
#if 0
		UInt32		outputFrames, outputBytes;
		
		err = SoundConverterEndConversion(mySoundConverter, pDecomBuffer, &outputFrames, &outputBytes);
		
		if (noErr == err && outputFrames) {
			pSndHeader->numFrames = outputFrames;
			SndDoCommand(pSoundChannel, pPlayCmd, true);	// play the last buffer.
		}
		
		if (theFillBufferDataUPP)
			DisposeSoundConverterFillBufferDataUPP(theFillBufferDataUPP);
		
		
		
		if (pSoundChannel)
			err = SndDisposeChannel(pSoundChannel, false);		// wait until sounds stops playing before disposing of channel
		
		if (theSoundCallBackUPP)
			DisposeSndCallBackUPP(theSoundCallBackUPP);
		
		if (scFillBufferData.hSource)
			DisposeHandle(scFillBufferData.hSource);
		
		if (mySoundConverter)
			SoundConverterClose(mySoundConverter);
		
		if (pDecomBuffer0)
			DisposePtr(pDecomBuffer0);
		
		if (pDecomBuffer1)
			DisposePtr(pDecomBuffer1);
		
		if (hSys7SoundData)
			DisposeHandle(hSys7SoundData);
		
#endif
		//////////////////
		
		if( gMovieController) DisposeMovieController( gMovieController);
		gMovieController = NULL;
		
		if( QTMovie) DisposeMovie( QTMovie);
		QTMovie = NULL;
		
		if( QTTemporaryFile)
		{
			FSpDelete( &QTFile);
			QTTemporaryFile = false;
		}
		
		if( MyTrackingQTUPP != NULL && MyReceiveQTUPP != NULL)
		{
			RemoveTrackingHandler( MyTrackingQTUPP, GetDialogWindow( QuicktimeDlog));
			RemoveReceiveHandler( MyReceiveQTUPP, GetDialogWindow( QuicktimeDlog));
			
			DisposeDragTrackingHandlerUPP( MyTrackingQTUPP);
			DisposeDragReceiveHandlerUPP( MyReceiveQTUPP);
		}
		MyTrackingQTUPP = NULL;
		MyReceiveQTUPP = NULL;
		
		DisposeDialog( QuicktimeDlog);
		QuicktimeDlog = NULL;		
	}
	QuicktimeDlog = NULL;
	
	//SetItemMark( ViewsMenu, m3D, noMark);
}

Boolean IsQTDrag( DragReference theDrag)
{
	short           	items, index;
	FlavorFlags     	theFlags;
	ItemReference   	theItem;
	OSErr           	result;
	long				textSize;
	HFSFlavor			myFlavor;
	FInfo				fndrInfo;
	OSType				theType;
	char				tempC[ 5];
	
	GetDragItemReferenceNumber(theDrag, 1, &theItem);
    
	result = GetFlavorFlags(theDrag, theItem, flavorTypeHFS, &theFlags);
	if (result == noErr)
	{
		Boolean		targetIsFolder, wasAliased;
		
		GetFlavorDataSize( theDrag, theItem, flavorTypeHFS, &textSize);
		
		GetFlavorData( theDrag, theItem, flavorTypeHFS, &myFlavor, &textSize, 0);
		
		ResolveAliasFile( &myFlavor.fileSpec, true, &targetIsFolder, &wasAliased);
		
		HSetVol( NULL, myFlavor.fileSpec.vRefNum, myFlavor.fileSpec.parID);
		result = FSpGetFInfo( &myFlavor.fileSpec, &fndrInfo);
		if( result != noErr) return false;		// <- Il s'agit d'un FOLDER, pas d'un FICHIER !!!
		
		return QTTypeConversion( fndrInfo.fdType);
    }
	
    return( false);
}

OSErr ConvertMovieToAIFF(FSSpec *inputFile, FSSpec *outputFile)
{
	short			fRef;
	OSErr			err;
	Movie			aMovie;
	TimeValue		selectionTime, selectionDuration;
	
#if 0
	err = OpenMovieFile(inputFile, &fRef, fsRdPerm);
	if (err != noErr)
		goto OpenMovieFileFailed;
	
	err = NewMovieFromFile(&theMovie, fRef, nil, nil, 0, nil);
	if (err != noErr) goto NewMovieFromFileFailed;
#endif
	
	//SetMovieProgressProc(theMovie, (MovieProgressUPP) gProgressUPP, 0);
	
	GetMovieSelection( QTMovie, &selectionTime, &selectionDuration);
	
	if( selectionDuration == 0)
	{
		aMovie = QTMovie;
	}
	else aMovie = CopyMovieSelection( QTMovie);
	
	SetMovieProgressProc( aMovie, (MovieProgressUPP) gProgressUPP, 0);
	
	err = ConvertMovieToFile( aMovie, nil, outputFile, 'mpg4', 'TVOD', 0, nil, showUserSettingsDialog, nil);
	
	SetCursor( GetQDGlobalsArrow( &qdarrow));
	
	if( selectionDuration != 0) DisposeMovie( aMovie);
	
	return (err);
}

OSErr ConvertMovieToMPEG4(FSSpec *inputFile, FSSpec *outputFile)
{
	short			fRef, resID;
	OSErr			err;
	Movie			aMovie;
	TimeValue		selectionTime, selectionDuration;
	Movie			theMovie;
	
	
	err = OpenMovieFile(inputFile, &fRef, fsRdPerm);
	if (err != noErr) return err;
	
	err = NewMovieFromFile(&theMovie, fRef, nil, nil, 0, nil);
	if (err != noErr) return err;
	
	SetMovieProgressProc( theMovie, (MovieProgressUPP) gProgressUPP, 0);
	
	err = ConvertMovieToFile( theMovie, nil, outputFile, 'mpg4', 'TVOD', 0, nil, showUserSettingsDialog, nil);
	
	SetCursor( GetQDGlobalsArrow( &qdarrow));
	
	DisposeMovie( theMovie);
	
	CloseMovieFile( fRef);
	
	return (err);
}


Boolean DragQTFile( RgnHandle myRgn, EventRecord *theEvent)
{
	short				result, i;
	ItemReference		theItem;
	RgnHandle			IconRgn, dragRegion, tempRgn;
	Point				theLoc;
	DragReference		theDrag;
	AEDesc				dropLocation;
	DragAttributes		attributes;
	short				mouseDownModifiers, mouseUpModifiers, copyText;
	short				temp;
	HFSFlavor			myNewFile;
	FInfo				fndrInfo;
	GWorldPtr			theGWorld = NULL;
	Rect				dragRegionRect;
	
	if( !DragManagerUse) return false;
	
	//******************************************
	//******************************************
	
	CopyRgn( myRgn, dragRegion = NewRgn());
	//SetPt( &theLoc, 0, 0);
	//LocalToGlobal( &theLoc);
	//OffsetRgn( dragRegion, theLoc.h, theLoc.v);
	
	NewDrag( &theDrag);
	
	///////////
	{
		GDHandle			oldGDeviceH;
		OSErr				errCode;
		CGrafPtr			oldPort;
		short				itemType;
		Rect				itemRect;
		Handle				itemHandle;
		Rect				picRect = { 0, 0, 32, 32};
		RgnHandle			rectRgn;
		
		GetGWorld( &oldPort, &oldGDeviceH);
		
		GetDialogItem( QuicktimeDlog, 2, &itemType, &itemHandle, &itemRect);
		
		picRect.top = itemRect.top;
		
		GetDialogItem( QuicktimeDlog, 9, &itemType, &itemHandle, &itemRect);
		
		picRect.left = itemRect.left;
		picRect.right = itemRect.right;
		picRect.bottom = itemRect.bottom;
		
		NewGWorld(	&theGWorld,
				  24,
				  &picRect,
				  NULL,				// CTabHandle
				  nil,
				  (GWorldFlags) 0);
		
		LockPixels( GetPortPixMap( theGWorld));
		SetGWorld( theGWorld, NULL);
		
		TextFont( kFontIDGeneva);
		TextSize( 9);
		
		PaintRect( &picRect);
		
		/*	IconRgn = NewRgn();
		 GetDialogItem( QuicktimeDlog, 2, &itemType, &itemHandle, &itemRect);
		 DrawQTItem( true, &itemRect, QTFileName, &QTFile, IconRgn);*/
		
		GetDialogItem( QuicktimeDlog, 9, &itemType, &itemHandle, &itemRect);
		
		itemRect.bottom = itemRect.top + 13;
		itemRect.left = itemRect.left  +  (itemRect.right - itemRect.left)/2 - StringWidth( QTFileName)/2;
		itemRect.right = itemRect.left + 2 + StringWidth( QTFileName);
		
		if( itemRect.right > picRect.right) itemRect.right = picRect.right;
		if( itemRect.left < picRect.left) itemRect.left = picRect.left;
		
		//TETextBox( QTFileName+1, QTFileName[0], &itemRect, teJustCenter);
		//InvertRect( &itemRect);
		
		SetGWorld( oldPort, oldGDeviceH);
		
		rectRgn = NewRgn();
		OpenRgn();
		FrameRect( &itemRect);
		CloseRgn( rectRgn);
		
		UnionRgn( myRgn, rectRgn, myRgn);
		DisposeRgn( rectRgn);
		
		GetDialogItem( QuicktimeDlog , 2, &itemType, &itemHandle, &itemRect);
		theLoc.h = 0;	//itemRect.left;
		theLoc.v = 0;	//itemRect.top;
		
		LocalToGlobal( &theLoc);
		SetDragImage( theDrag, GetPortPixMap( theGWorld), myRgn, theLoc, kDragDarkTranslucency);
	}
	///////////
	
	
	FSpGetFInfo( &QTFile, &fndrInfo);
	
	myNewFile.fileType			=	fndrInfo.fdType;
	myNewFile.fileCreator		=	fndrInfo.fdCreator;
	myNewFile.fdFlags			=	fndrInfo.fdFlags;
	myNewFile.fileSpec			=	QTFile;
	
	AddDragItemFlavor( theDrag, 1, flavorTypeHFS, &myNewFile, sizeof( myNewFile), 0);
	SetDragItemBounds(theDrag, 1, GetRegionBounds( myRgn, &dragRegionRect));
	
	tempRgn = NewRgn();
	CopyRgn(dragRegion, tempRgn);
	InsetRgn(tempRgn, 1, 1);
	DiffRgn(dragRegion, tempRgn, dragRegion);
	DisposeRgn(tempRgn);
	
	SetCursor( GetQDGlobalsArrow( &qdarrow));
	
	gMovieQuicktime = true;
	
	result = TrackDrag(theDrag, theEvent, myRgn);
	if (result != noErr && result != userCanceledErr) return(true);
	
	gMovieQuicktime = false;
	
	//
	//	Dispose of the drag.
	//
	
	DisposeDrag( theDrag);
	DisposeGWorld( theGWorld);
	DisposeRgn( dragRegion);
	//DisposeRgn( IconRgn);
	
	return(true);
}

pascal OSErr MyTrackingQuicktime(short message, WindowPtr theWindow, void *handlerRefCon, DragReference theDrag)
{	
	short				result, offset, i;
	long				theTime = TickCount();
	unsigned short		count, index;
	unsigned long		flavorFlags, attributes;
	ItemReference		theItem;
	RgnHandle			theRgn;
	Point				theMouse, localMouse, theCell;
	Rect				caRect, tempRect;
	GrafPtr				savePort;
	
	if( !mainSystemDrag) return noErr;
	
	if ((message != kDragTrackingEnterHandler) && (!canAcceptDrag)) return( noErr);
	
	GetPort( &savePort);
	SetPortWindowPort( theWindow);
	
	GetDragAttributes(theDrag, &attributes);
	
	switch (message) {
			
		case kDragTrackingEnterHandler:
			if( attributes & kDragInsideSenderWindow) canAcceptDrag = false;
			else canAcceptDrag = IsQTDrag( theDrag);
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
			GetPortBounds( GetDialogPort( QuicktimeDlog), &caRect);
			
			if( PtInRect( localMouse, &caRect))
			{
				RectRgn(theRgn = NewRgn(), &caRect);
				
				ShowDragHilite(theDrag, theRgn, true);
				DisposeRgn(theRgn);
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
			HideDragHilite( theDrag);
			break;
			
		case kDragTrackingLeaveHandler:
			
			break;
	}
	
	SetPort( savePort);
	
	return(noErr);
}

void COPYQuicktime()
{
	OSErr			anErr;
	Handle			myHandle;
	TimeValue		selectionTime, selectionDuration;
	Str255			str;
	Movie			aMovie;
	FSSpec			newFile;
	
	
	aMovie = MCCopy( gMovieController);
	
	anErr = PutMovieOnScrap( aMovie, 0);
	
	DisposeMovie( aMovie);
	
#if 0
	
	MCDoAction( gMovieController, mcActionGetSelectionBegin, (void*) &start);
	
	MCDoAction( gMovieController, mcActionGetSelectionDuration, (void*) &dur);
	
	GetMovieSelection( QTMovie, &selectionTime, &selectionDuration);
	
	if( selectionDuration == 0)
	{
		aMovie = QTMovie;
	}
	else aMovie = CopyMovieSelection( QTMovie);
	
	myHandle = NewHandle( 10);
	
	anErr = PutMovieIntoTypedHandle( QTMovie, gSoundTrack, 'AIFF', myHandle, selectionTime, selectionDuration, 0, 0);
	
	if( anErr)
	{
		NumToString( anErr, str);
		DebugStr( str);
	}
	else DebugStr("\pWe did it!");
	
	newFile = QTFile;
	
	anErr = ConvertMovieToFile( aMovie, nil, &newFile, 'AIFF', 'TVOD', 0, nil, showUserSettingsDialog, nil);
	
	DisposeMovie( aMovie);
#endif
}

pascal OSErr MyReceiveQuicktime(WindowPtr theWindow, void* handlerRefCon, DragReference theDrag)
{
	HFSFlavor			myFlavor;
	unsigned short		items;
	ItemReference		theItem;
	DragAttributes		attributes;
	Size				textSize;
	short				index, mouseDownModifiers, mouseUpModifiers;
	OSErr				iErr;
	GrafPtr				SavedPort;
	//
	OSType				sign = 'SNPL';
	AppleEvent			aeEvent, reply;
	AEDesc				target, listElem, fileList;
	//
	
	if( !mainSystemDrag) return dragNotAcceptedErr;
	
	GetPort( &SavedPort);
	SetPortWindowPort(theWindow);
	
	GetDragAttributes(theDrag, &attributes);
	GetDragModifiers(theDrag, NULL, &mouseDownModifiers, &mouseUpModifiers);
	
	if( (attributes & kDragInsideSenderWindow)) return dragNotAcceptedErr;
	
	HideDragHilite( theDrag);
	
	//
	//	Un fichier en provenance du Finder
	//
	
	iErr = AECreateDesc(	typeApplSignature,
						(Ptr) &sign,
						sizeof( sign),
						&target);
	
	iErr = AECreateAppleEvent(	kCoreEventClass,
							  kAEOpenDocuments,
							  &target,
							  kAutoGenerateReturnID,
							  kAnyTransactionID,
							  &aeEvent);
	
	iErr = AECreateList( nil,0,false, &fileList);
	
	CountDragItems(theDrag, &items);
	
	SetCursor( &watchCrsr);
	
	for (index = 1; index <= items; index++)
	{
		GetDragItemReferenceNumber(theDrag, index, &theItem);
		
		iErr = GetFlavorDataSize(theDrag, theItem, flavorTypeHFS, &textSize);
		
		if (iErr == noErr)
		{
			Boolean		targetIsFolder, wasAliased;
			
			GetFlavorData(theDrag, theItem, flavorTypeHFS, &myFlavor, &textSize, 0);
			
			ResolveAliasFile( &myFlavor.fileSpec, true, &targetIsFolder, &wasAliased);
			
			AECreateDesc(typeFSS, (Ptr) &myFlavor.fileSpec, sizeof( myFlavor.fileSpec), &listElem);
			
			iErr = AEPutDesc( &fileList, 0, &listElem);
			if( iErr) return iErr;
			
			AEDisposeDesc( &listElem);
			
			iErr = AEPutParamDesc( &aeEvent, keyDirectObject,&fileList);
			if(iErr) return iErr;
		}
	}
	
	iErr = AEDisposeDesc( &fileList);
	
	iErr = AESend(&aeEvent,
				  &reply,
				  kAENoReply,
				  kAENormalPriority,
				  kAEDefaultTimeout,
				  NULL,
				  NULL);
	if( iErr) return iErr;
	
	SetCursor( GetQDGlobalsArrow( &qdarrow));
	
	SetPort( SavedPort);
	
	return noErr;
}
