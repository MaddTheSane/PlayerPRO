#include <PlayerPROCore/PlayerPROCore.h>
#include <PlayerPROCore/RDriverInt.h>
#include <Carbon/Carbon.h>
#include "PTMID.H"
#include "dlsmac.h"
#include "dls.h"
#include "dls2.h"
#include <QuickTime/QuickTime.h>

#include "WAV.h"

//NOTE: A lot of code was copied from MIDI-GM.c.  This file needs to be checked
//to see if it still works like it did.

Handle NSndToHandle( Handle sound, long *loopStart, long *loopEnd, short *sampleSize, unsigned long *sampleRate, long *baseFreq, MADDriverSettings *init);
static NoteRequest		myNoteRequest;
static Boolean			QK50;
Boolean					QuicktimeInstruAvailable;
extern		MADMusic			*curMusic;


void ConvertInstrument( register	Byte	*tempPtr,	register long sSize);
void NInitSmallPiano( Rect mainRect, Rect *listRect);
void DrawSmallPianoKey( short i, short color, Rect aRect);
void NDoPlayInstru(short	Note, short Instru, short effect, short arg, short vol);
void ConvertInstrumentIn( register	Byte	*tempPtr,	register long sSize);

static void DeleteDLSFile();
static short GenerateDLSFromBundle();
static void TESTNEWSYSTEM( sData **sample, InstrData *inst, AtomicInstrument ai);
static void Quicktime5( NoteRequest *NoteRequest, sData **sample, InstrData *inst);
static short OpenDataFileQK( long dirID, short VRefNum);


/**** Resource Format QK25 ****/

//FIXME: Is this the proper way to do this?
//Tip: the original code was written thus:
//long GetNELong(BigEndianLong toget)
//{
//#ifdef __BIG_ENDIAN__
//return toget;
//#else
//return EndianU32_BtoL(toget.bigEndianValue);
//#endif
//}
//
//void SetNEOSType(BigEndianOSType *toset, OSType theval)
//{
//#ifdef __BIG_ENDIAN__
//*toset = theval;
//#else
//toset->bigEndianValue = EndianU32_LtoB(theval);
//#endif
//}
//As such, you can substitute in your mind what the function would have
//originally looked like


#ifdef __BIG_ENDIAN__
#define GetNELong(toget) toget
#else
#define	GetNELong(toget) EndianS32_BtoL(toget.bigEndianValue)
#endif

#ifdef __BIG_ENDIAN__
#define SetNELong(toset, theval)	*toset = theval
#else
#define SetNELong(toset, theval)	(toset)->bigEndianValue = EndianS32_LtoB(theval)	
#endif

#ifdef __BIG_ENDIAN__
#define SetNEOSType(toset, theval)	*toset = theval
#else
#define SetNEOSType(toset, theval)	(toset)->bigEndianValue = EndianU32_LtoB(theval)	
#endif

#ifdef __BIG_ENDIAN__
#define GetNEShort
#else
#define GetNEShort(toget) EndianS16_BtoL(toget.bigEndianValue)
#endif

#ifdef __BIG_ENDIAN__
#define GetNEOSType(toget) toget
#else
#define GetNEOSType(toget) EndianU32_BtoL(toget.bigEndianValue)
#endif

#ifdef __BIG_ENDIAN__
#define GetNEUnsignedFixed(toget) toget
#else
#define	GetNEUnsignedFixed(toget) EndianU32_BtoL(toget.bigEndianValue)
#endif

static Boolean TestRunningOnCarbonX(void)
{
    UInt32 response;
    
    return (Gestalt(gestaltSystemVersion, 
                    (SInt32 *) &response) == noErr)
	&& (response >= 0x01000);
}

static void ConvertInstrument16( register	short	*tempPtr,	register long sSize)
{
	register	short			val = 0x8000;
	
	sSize /= 2;
	
	while( sSize > 0)
	{
		sSize--;
		*(tempPtr++) += val;
	}
}

static inline void ConvertInstrumentIn16( register short *tempPtr, register long sSize)
{
	ConvertInstrument16( tempPtr, sSize);
}

static void DebugLong( long type)
{
	Str31	str;
	
	str[ 0] = 4;
	BlockMoveData( &type, &str[ 1], 4);
	
	DebugStr( str);
}

typedef struct
{
	long	size;
	OSType	type;
	long	id;
	long	a[ 2];
	
	Byte	data[];
	
} QuictimeRsrc25;


typedef struct
{
	long	from;
	long	to;
	
	long	type;
	long	resID;
	long	e;
	long	f;
	
} QuictimeSs25;

typedef struct
{
	char			unused[ 0x58];
	Str31			name;
	QuictimeSs25	Ss[];
	
} QuicktimeInst25;

/*************************/


/**** Resource Format ****/
typedef struct
{
	long	from;
	long	to;
	
	long	type;
	long	resID;
	long	e;
	long	f;
	
} QuictimeSs;

typedef struct
{
	char			unused[ 0x62];
	short			no;
	QuictimeSs		Ss[];
	
} QuicktimeInst;

/*************************/

#pragma mark Atom functions prototypes
static OSErr GetAtomData( MyAtom at, void* data, long size);
static long CountAtomById( MyAtom at, long type);
static OSErr FindAtomById( MyAtom at, MyAtom *retat, Boolean LIST, long type, short id);
static OSErr GetAtomDataById( MyAtom at, long type, void *data, long size);

/*************************/

static void OctavesMIDIName(short	id, Str255	String)
{
	short			NNames[ 12] =	{'C ','C#','D ','D#','E ','F ','F#','G ','G#','A ','A#','B '};
									/*	{'Do','Do#','RŽ','RŽ#','Mi','Fa','Fa#','Sol','Sol#','La','La#','Si'};	*/
	Str255		WorkStr;
	
	if( id == 0xFF)
	{
		pStrcpy( String, "\p---");
		return;
	}
	
	NumToString( (id / 12), WorkStr);
  	String[ 1] = NNames[ (id) % 12]>>8;			String[ 2] = NNames[ (id) % 12];
	String[ 0] = 2;
  	pStrcat( String, WorkStr);	
}

static void SetInstruNameM( short	theNo, Str255 theNewName, short MIDIgm, Ptr destName)
{
	short	i;
	Str255	aStr, bStr;

	pStrcpy( aStr, "\p(");
	NumToString( MIDIgm, bStr);
	pStrcat( aStr, bStr);
	pStrcat( aStr, "\p) ");
	pStrcat( aStr, theNewName);
	
	for(i=0; i<32; i++)
	{
		if( i < aStr[ 0]) destName[ i] = aStr[ i+1];
		else destName[ i] = '\0';
	}
}

static void SetSampNameM( Str255 theNewName, Ptr destName)
{
	short	i;

	for(i=0; i<32; i++)
	{
		if( i < theNewName[ 0]) destName[i] = theNewName[i+1];
		else destName[i] = '\0';
	}
}

static short OpenResFileQK( long dirID, short VRefNum)
{
	CInfoPBRec		info;
	Str255			tempStr;
	long			dirIDCopy;
	short			i, vRefNum;
	OSErr			iErr;
	FSSpec			spec;
	short			ret;
	
	pStrcpy( spec.name, "\pQuickTimeª Musical Instruments");
	spec.vRefNum = VRefNum;
	spec.parID = dirID;
	
	ret = FSpOpenResFile( &spec, fsCurPerm);
	if( ret != -1) return ret;
	
	/************/
	
	ret = -1;
	
	info.hFileInfo.ioNamePtr = tempStr;
	info.hFileInfo.ioVRefNum = VRefNum;
	
	for (i = 1; true; i ++)
	{
		info.hFileInfo.ioDirID = dirID;
		info.hFileInfo.ioFDirIndex = i;
		
		if (PBGetCatInfoSync(&info) != noErr) break;
		
		if( info.hFileInfo.ioFlFndrInfo.fdType == 'INIT' && info.hFileInfo.ioFlFndrInfo.fdCreator == 'dvb ')
		{	
			//	HGetVol( NULL, &vRefNum, &dirIDCopy);
			
			//	iErr = HSetVol( NULL, info.hFileInfo.ioVRefNum, dirID);
			
			pStrcpy( spec.name, info.hFileInfo.ioNamePtr);
			spec.vRefNum = info.hFileInfo.ioVRefNum;
			spec.parID = dirID;
			
			ret = FSpOpenResFile( &spec, fsCurPerm);
			
			//	iErr = HSetVol( NULL, vRefNum, dirIDCopy);
			//	if( iErr != noErr) MyDebugStr( __LINE__, __FILE__, "HSetVol error...");
		}
	}
	
	return ret;
}

#if 0
void ComputeQuicktimeSound25( short GMInstruID, sData **sample, InstrData* inst, short ins)
{
	long					lS, lE, inOutBytes;
	short					sS;
	unsigned long			sRate;
	long					bFreq;
	
	short					ii;
	short 					i, x;
	Handle					hRsrc;
	short					foundVRefNum, iFileRef;
	long					foundDirID;
	OSErr					iErr;
//	QuicktimeInst25			*QuickInst;
	Ptr						tPtr;
	Str255					aStr, bStr;
		
	/***************/
	
	for( i = 0; i < inst->numSamples; i++)
	{
		if( sample[ inst->firstSample + i] != NULL)
		{
			if( sample[ inst->firstSample + i]->data != NULL)
			{
				DisposePtr( (Ptr) sample[ inst->firstSample + i]->data);
				sample[ inst->firstSample + i]->data = NULL;
			}
			DisposePtr( (Ptr) sample[ inst->firstSample + i]);
			sample[ inst->firstSample + i] = NULL;
		}
	}
	
	for( i = 0; i < 32; i++) inst->name[ i]	= 0;
	inst->type			= 0;
	inst->numSamples	= 0;
	
	/**/
	
	for( i = 0; i < 96; i++) inst->what[ i]		= 0;
	for( i = 0; i < 12; i++)
	{
		inst->volEnv[ i].pos		= 0;
		inst->volEnv[ i].val		= 0;
	}
	for( i = 0; i < 12; i++)
	{
		inst->pannEnv[ i].pos	= 0;
		inst->pannEnv[ i].val	= 0;
	}
	inst->volSize		= 0;
	inst->pannSize		= 0;
	
	inst->volSus		= 0;
	inst->volBeg		= 0;
	inst->volEnd		= 0;
	
	inst->pannSus		= 0;
	inst->pannBeg		= 0;
	inst->pannEnd		= 0;

	inst->volType		= 0;
	inst->pannType		= 0;
	
	inst->volFade		= 900;
	inst->vibDepth		= 0;
	inst->vibRate		= 0;

	/***************/
	
	iErr = FindFolder( kOnSystemDisk, kExtensionFolderType, kDontCreateFolder, &foundVRefNum, &foundDirID);
	if( iErr == noErr)
	{
		HSetVol( NULL, foundVRefNum, foundDirID);	
		
		iFileRef = OpenResFileQK( foundDirID, foundVRefNum);
		if( iFileRef != -1)
		{
			UseResFile( iFileRef);
			
			ENCORE:
			
			while( (hRsrc = GetResource( 'ssai', GMInstruID)) == NULL) GMInstruID++;
			if( SizeResource( hRsrc) < 600) goto ENCORE;
			
			if( hRsrc != NULL)
			{
				QuictimeRsrc25	*rsrc, *maxrsrc;
				short			sampleIDMap[ 500];
				
				for( i = 0; i < 500; i++) sampleIDMap[ i] = -1;
				
				DetachResource( hRsrc);
				HLock( hRsrc);
				if( hRsrc == NULL) DebugStr("\pMIDI Ins Err");
				
				maxrsrc = (QuictimeRsrc25*) ((*hRsrc) + GetHandleSize( hRsrc));
				
				/// NEW SYSTEM !!!!!
				
				rsrc = (QuictimeRsrc25*) ((*hRsrc) + 0x20);
				
				while( rsrc < maxrsrc && rsrc->type != kaiToneDescType) rsrc = (QuictimeRsrc25*) (((Ptr) rsrc) + rsrc->size);			// Find the name
				
				SetInstruNameM( ins, (unsigned char*) rsrc->data + 36, GMInstruID, inst->name);
				
				while( rsrc < maxrsrc && rsrc->type != kaiKeyRangeInfoType) rsrc = (QuictimeRsrc25*) (((Ptr) rsrc) + rsrc->size);
				
				while( rsrc->type == kaiKeyRangeInfoType)
				{
					QuictimeRsrc25	*rsrcCopy = rsrc;
					
					rsrc = (QuictimeRsrc25*) (((Ptr) rsrc) + 5L*4L);		// go to  kaiSampleDescType
					
					if( rsrc->type == kaiSampleDescType)
					{
						InstSampleDescRec	*sdesc;
						QuictimeRsrc25		*rsrc2;
						sData				*curData;
						Ptr					ptr2delete = NULL;
						
						sdesc = (InstSampleDescRec*) rsrc->data;
						
						// SampleData exists already ??
						
						if( sampleIDMap[ GetNEShort( sdesc->sampleDataID)] != -1)
						{
							for( ii = GetNELong( sdesc->pitchLow) - 12; ii <= GetNELong( sdesc->pitchHigh) - 12; ii++)
							{
								if( ii < NUMBER_NOTES && ii > 0) inst->what[ ii] = sampleIDMap[ GetNEShort(sdesc->sampleDataID)];
							}
							
							goto NEXTSAMP;
						}
						else sampleIDMap[ GetNEShort(sdesc->sampleDataID)] = inst->numSamples;
						
						for( ii = GetNELong(sdesc->pitchLow) - 12; ii <= GetNELong(sdesc->pitchHigh) - 12; ii++)
						{
							if( ii < NUMBER_NOTES && ii > 0) inst->what[ ii] = inst->numSamples;
						}
						
						if( GetNELong(sdesc->offset) != 0) DebugStr("\pOffset!");
						
						// Go to DATA
						
						if( GetNEOSType(sdesc->dataFormat) == 'raw ' || GetNEOSType(sdesc->dataFormat) == 'twos')
						{
							QuictimeRsrc25		*rsrc3;
							
							Boolean	Founded = false;
							
							rsrc2 = (QuictimeRsrc25*) ((*hRsrc) + 0x20);
							while( rsrc2 < maxrsrc && Founded == false)
							{
								rsrc2 = (QuictimeRsrc25*) (((Ptr) rsrc2) + rsrc2->size);
								
								if( rsrc2->type == kaiSampleInfoType)
								{
									rsrc3 = (QuictimeRsrc25*) (((Ptr) rsrc2) + 5L*4L);
									
									while( rsrc3->type != kaiSampleDataType) rsrc3 = (QuictimeRsrc25*) (((Ptr) rsrc3) + rsrc3->size);
									
									if( rsrc3->type == kaiSampleDataType)
									{
										if( rsrc2->id  == GetNEShort(sdesc->sampleDataID))
										{
											Founded = true;
										}
									}
								}
							}
							
							if( Founded)
							{
								ptr2delete = NULL;
								rsrc2 = rsrc3;			// go to  
								inOutBytes = (GetNEShort(sdesc->sampleSize) * GetNELong(sdesc->numSamples)) / 8L;
							}
							else
							{
								inOutBytes = (GetNEShort(sdesc->sampleSize) * GetNELong(sdesc->numSamples)) / 8L;
								
								LoadQK25Sample( GetNEShort(sdesc->sampleDataID), GMInstruID, &ptr2delete, inOutBytes);
								
								if( !ptr2delete) inOutBytes = 0;
								else Founded = true;
							}
						}
						else inOutBytes = 0;
						
						// ************************
						// * Add it to instrument *
						// ************************
						
						{
							curData = MADCreateSample(NULL, ins, 0);
							
							curData->data = NewPtr( inOutBytes);
							
							if( curData->data == NULL)
							{
								Erreur( 63, -2);
								
								curData->size = 0;
							}
							else
							{
								if( inOutBytes)
								{
									if( !ptr2delete) BlockMoveData( rsrc2->data, curData->data, inOutBytes);
									else
									{
										BlockMoveData( ptr2delete, curData->data, inOutBytes);
										DisposePtr( ptr2delete);
										ptr2delete = NULL;
									}
								}
								
								if( GetNEOSType(sdesc->dataFormat) == 'raw ')
								{
									switch( GetNEShort(sdesc->sampleSize))
									{
										case 8:
											ConvertInstrumentIn( (Byte*) curData->data, inOutBytes);
										break;
										
										case 16:
											ConvertInstrument16( (short*) curData->data, inOutBytes);
										break;
									}
								}
								curData->size 		= inOutBytes;
								curData->loopBeg 	= GetNELong(sdesc->loopStart);
								curData->loopSize 	= GetNELong(sdesc->loopEnd) - GetNELong(sdesc->loopStart);
								curData->vol 		= 64;
								curData->amp	 	= GetNEShort(sdesc->sampleSize);
								
								if( GetNEShort(sdesc->numChannels) == 2)
								{
									curData->stereo = true;
								}
								if( GetNEShort(sdesc->numChannels) > 2) DebugStr("\pMore than 2 channels");
								
								// **
								curData->c2spd		= GetNEUnsignedFixed(sdesc->sampleRate) >> 16;
								curData->relNote	= 60 - GetNELong(sdesc->pitchNormal);
								// **
								
								OctavesMIDIName( GetNELong(sdesc->pitchLow) - 12, aStr);
								pStrcat( aStr, "\p -> ");
								OctavesMIDIName( GetNELong(sdesc->pitchHigh) - 12, bStr);
								pStrcat( aStr, bStr);
								pStrcat( aStr, "\p, ");
								NumToString( curData->c2spd, bStr);
								pStrcat( aStr, bStr);
								pStrcat( aStr, "\p Hz");
								
								SetSampNameM( aStr, curData->name);
							}
						}
						// ************************
					}
					else Debugger();
					
					NEXTSAMP:
					
					rsrc = (QuictimeRsrc25*) (((Ptr) rsrcCopy) + rsrcCopy->size);
				}
				
				HUnlock( hRsrc);
				DisposeHandle( hRsrc);
			}
			CloseResFile( iFileRef);
		}
	//	else Erreur( 72, iFileRef);
	}
	else Erreur( 72, 0);
	
//	SetCursor( &qd.arrow);
}
#endif

void ComputeQuicktimeSound( short GMInstruID, sData **sample, InstrData* inst, short ins)
{
	short 		samp, i;
	
	for( i = 0; i < inst->numSamples; i++)
	{
		if( sample[ inst->firstSample + i] != NULL)
		{
			if( sample[ inst->firstSample + i]->data != NULL)
			{
				DisposePtr( (Ptr) sample[ inst->firstSample + i]->data);
				sample[ inst->firstSample + i]->data = NULL;
			}
			DisposePtr( (Ptr) sample[ inst->firstSample + i]);
			sample[ inst->firstSample + i] = NULL;
		}
	}
	
	for( i = 0; i < 32; i++) inst->name[ i]	= 0;
	inst->type			= 0;
	inst->numSamples	= 0;
	inst->no			= ins;
	/**/
	
	for( i = 0; i < 96; i++) inst->what[ i]		= 0;
	for( i = 0; i < 12; i++)
	{
		inst->volEnv[ i].pos		= 0;
		inst->volEnv[ i].val		= 0;
	}
	for( i = 0; i < 12; i++)
	{
		inst->pannEnv[ i].pos	= 0;
		inst->pannEnv[ i].val	= 0;
	}
	inst->volSize		= 0;
	inst->pannSize		= 0;
	
	inst->volSus		= 0;
	inst->volBeg		= 0;
	inst->volEnd		= 0;
	
	inst->pannSus		= 0;
	inst->pannBeg		= 0;
	inst->pannEnd		= 0;
	
	inst->volType		= 0;
	inst->pannType		= 0;
	
	inst->volFade		= 900;
	inst->vibDepth		= 0;
	inst->vibRate		= 0;
	
	/*********/
	
	{
		short 						synthCount, foundVRefNum, iFileRef, no, ii, i;
		OSErr 						iErr;
		NoteAllocator 				na;
		OSType						synthType;
		Str31						synthName;
		AtomicInstrument			ai;
		SynthesizerDescription		sd;
		MusicComponent 				mc;
		
		na = OpenDefaultComponent(kNoteAllocatorComponentType,0);
		
		synthCount = NAGetRegisteredMusicDevice( na, 0, nil, nil, nil, nil);
		
		while( synthCount)
		{
			iErr = NAGetRegisteredMusicDevice(na,synthCount, &synthType, synthName, nil, &mc);
			
			if(!synthName[0])
			{
				SynthesizerDescription sd;
				
				MusicGetDescription( mc, &sd);
				BlockMove( sd.name, synthName, sd.name[0]+1);
			}
			
			if( synthType == kSoftSynthComponentSubType) goto PROCESS;
			
			synthCount--;
		}
		
		goto BAIL;
		
	PROCESS:
		
		// fill out a note request, using NAStuffToneDescription
		
		myNoteRequest.info.flags = 0;
		myNoteRequest.info.midiChannelAssignment = 0;
		*(short *)(&myNoteRequest.info.polyphony) = EndianS16_NtoB(2);				// simultaneous tones
		*(Fixed *)(&myNoteRequest.info.typicalPolyphony) = EndianU32_NtoB(0x00010000);
		
		if( GMInstruID < 1) GMInstruID = 1;
		
		iErr = NAStuffToneDescription( na, GMInstruID, &myNoteRequest.tone);
		if (iErr != noErr)
		{
			iErr = NAStuffToneDescription( na, 1, &myNoteRequest.tone);
			if(iErr != noErr) MyDebugStr(__LINE__, __FILE__, "NAStuff ComputeQTSound");
		}
		
		SetNEOSType(&myNoteRequest.tone.synthesizerType, synthType);
		pStrcpy( myNoteRequest.tone.synthesizerName, synthName);
		
		// display the instrument picker dialog box to elicit an instrument from the user
		//iErr = NAPickInstrument( na, NULL, "\pPick an instrument:", &myNoteRequest.tone, kPickSameSynth + kPickUserInsts, 0, 0, 0);
		//if (iErr != noErr) goto BAIL;
		
		iErr = MusicSetPartInstrumentNumber(mc, 1, GetNELong(myNoteRequest.tone.instrumentNumber));
		if( iErr) goto BAIL;
		
		iErr = MusicGetPartAtomicInstrument(mc, 1, &ai, 0);
		if( iErr) goto BAIL;
		
		if( QK50) Quicktime5( &myNoteRequest, sample, inst);
//		else TESTNEWSYSTEM( sample, inst, ai);
		
		CloseComponent( na);
	}
	
BAIL:
	
	;
}

void InitQuicktimeInstruments(void)
{
	long					lS, lE, inOutBytes, result;
	//	short					sS, qQ;
	//	short 					i;
	short					foundVRefNum, vRefNum;
	UNFILE					iFileRef;
	long					foundDirID, dirID;
	OSErr					iErr;
	
	HGetVol( NULL, &vRefNum, &dirID);
	
	QuicktimeInstruAvailable = false;
	
	iErr = FindFolder( kOnSystemDisk, kExtensionFolderType, kDontCreateFolder, &foundVRefNum, &foundDirID);
	if( iErr == noErr)
	{
		HSetVol( NULL, foundVRefNum, foundDirID);	
		
		iFileRef = OpenDataFileQK( foundDirID, foundVRefNum);
		if( iFileRef != -1)
		{
			QuicktimeInstruAvailable = true;
			
			iClose( iFileRef);
		}
	}
	
	QuicktimeInstruAvailable = true;
	
	QK50 = false;
	
	iErr = Gestalt( gestaltQuickTimeVersion, &result);
	if( iErr == noErr)
	{
		result >>= 16;
		if( result >= 0x0500)
		{
			QK50 = true;
		}
	}
	
	{
		NoteAllocator 				na;
		
		na = OpenDefaultComponent(kNoteAllocatorComponentType,0);
		
		iErr = NAStuffToneDescription( na, 1, &myNoteRequest.tone);
		if (iErr != noErr) MyDebugStr(__LINE__, __FILE__, "NAStuff");
		
		CloseComponent( na);
	}
	
	HSetVol( NULL, vRefNum, dirID);
}

static void Quicktime5( NoteRequest *NoteRequest, sData **sample, InstrData *inst)
{
	short 						foundVRefNum, iFileRef, no, ii, i, x;
	OSErr 						iErr;
	NoteAllocator 				na;
	Str255						aStr, bStr;
	sData						*curData;
	long						size, inOutBytes, foundDirID;
	Ptr							data;
	CK							ck;
	long						listType, noIns, fSize, tot;
	MyAtom						at, sat, insAt, insHe, rgnAt, sat3, sat4, InfoAt, InfoData;
	INSTHEADER 					curIns;
	
	if( TestRunningOnCarbonX())
	{
		iFileRef = GenerateDLSFromBundle();
		iErr = noErr;
	}
	else
	{
		iErr = FindFolder( kOnSystemDisk, kExtensionFolderType, kDontCreateFolder, &foundVRefNum, &foundDirID);
		
		if( iErr == noErr)
		{
			HSetVol( NULL, foundVRefNum, foundDirID);	
			iFileRef = OpenDataFileQK( foundDirID, foundVRefNum);
		}
	}
	
	if( iErr == noErr)
	{
		if( iFileRef != -1)
		{
			// RIFF ID
			fSize = sizeof( ck);
			iErr = FSRead( iFileRef, &fSize, &ck);
			ck.cksize = EndianU32_LtoN( ck.cksize);
			
			if( ck.ckid != 'RIFF') Debugger();
			
			fSize = 4;
			iErr = FSRead( iFileRef, &fSize, &listType);
			
			GetFPos( iFileRef, &at.pos);
			at.id = listType;
			at.ref = iFileRef;
			at.size = ck.cksize;
			
			iErr = FindAtomById( at, &sat, true, 'colh', 0);
			if( iErr) DebugLong( iErr);
			
			iErr = GetAtomData( sat, &noIns, sizeof( long));
			if( iErr) DebugLong( iErr);
			noIns = EndianU32_LtoN( noIns);
			
			
			iErr = FindAtomById( at, &sat, true, 'lins', 0);
			if( iErr) DebugLong( iErr);
			
			tot = CountAtomById( sat, 'ins ');
			for( i = 0 ; i < tot; i++)
			{
				iErr = FindAtomById( sat, &insAt, true, 'ins ', i);
				if( iErr) DebugLong( iErr);
				
				iErr = FindAtomById( insAt, &insHe, true, 'insh', 0);
				if( iErr) DebugLong( iErr);
				
				iErr = GetAtomData( insHe, &curIns, sizeof( curIns));
				if( iErr) DebugLong( iErr);
				
				curIns.cRegions = EndianU32_LtoN( curIns.cRegions);
				curIns.Locale.ulBank = EndianU32_LtoN( curIns.Locale.ulBank);
				curIns.Locale.ulInstrument = EndianU32_LtoN( curIns.Locale.ulInstrument);
				
				/*	iErr = FindAtomById( insAt, &InfoAt, true, 'INFO', 0);
				 if( iErr) DebugLong( iErr);
				 
				 iErr = FindAtomById( InfoAt, &InfoData, true, 'INAM', 0);
				 if( iErr) DebugLong( iErr);
				 
				 iErr = GetAtomData( InfoData, insName, sizeof( insName));*/
				//Apple protects many of the following info in a Big-endian wrapper. We need to work around this
				if( GetNELong(NoteRequest->tone.instrumentNumber) >= 16384) // DRUM KIT
				{
					if( BitTst( &curIns.Locale.ulBank, 31-31))
					{
						long	valeurQT, gmID, valeurBank;
						
						gmID = GetNELong(NoteRequest->tone.instrumentNumber);
						gmID = gmID & 0x000000FF;
						
						if( gmID == curIns.Locale.ulInstrument+1)
						{
							break;
						}
					}
				}
				else  if( GetNELong(NoteRequest->tone.instrumentNumber) != 0 && GetNELong(NoteRequest->tone.instrumentNumber) > 128)
				{
					long	valeurQT, gmID, valeurBank;
					
					valeurQT = GetNELong(NoteRequest->tone.instrumentNumber);
					valeurQT = valeurQT >> 7;
					
					gmID = GetNELong(NoteRequest->tone.instrumentNumber);
					gmID = gmID & 0x0000007F;
					
					valeurBank = curIns.Locale.ulBank;
					valeurBank = valeurBank & 0x0FFFFFFF;
					valeurBank = valeurBank >> 8;
					
					if( gmID == curIns.Locale.ulInstrument+1)
					{
						if( valeurQT == valeurBank)
						{
							break;
						}
					}
				}
				else
				{
					if( curIns.Locale.ulInstrument+1 == GetNELong(NoteRequest->tone.instrumentNumber)) break;
				}
			}
			
			if( i < tot)	// did we find it?
			{
				iErr = FindAtomById( sat, &insAt, true, 'ins ', i);
				if( iErr) DebugLong( iErr);
				
				iErr = FindAtomById( insAt, &insHe, true, 'insh', 0);
				if( iErr) DebugLong( iErr);
				
				iErr = GetAtomData( insHe, &curIns, sizeof( curIns));
				if( iErr) DebugLong( iErr);
				
				curIns.cRegions = EndianU32_LtoN( curIns.cRegions);
				curIns.Locale.ulBank = EndianU32_LtoN( curIns.Locale.ulBank);
				curIns.Locale.ulInstrument = EndianU32_LtoN( curIns.Locale.ulInstrument);
				
				/*	iErr = FindAtomById( insAt, &InfoAt, true, 'INFO', 0);
				 if( iErr) DebugLong( iErr);
				 
				 iErr = FindAtomById( InfoAt, &InfoData, true, 'INAM', 0);
				 if( iErr) DebugLong( iErr);
				 
				 iErr = GetAtomData( InfoData, insName, sizeof( insName));
				 if( iErr) DebugLong( iErr);*/
				
				for( x = 0; x < 32 && x < NoteRequest->tone.instrumentName[0]; x++) inst->name[ x]	= NoteRequest->tone.instrumentName[ x+1]; //insName[ x];
				
				iErr = FindAtomById( insAt, &sat, true, 'lrgn', 0);
				if( iErr) DebugLong( iErr);
				
				for( x = 0; x < curIns.cRegions; x++)
				{
					RGNHEADER 	rgnh;
					WSMPL 		wsmp;
					WLOOP		loop;
					WAVELINK	wlnk;
					MyAtom 		wvpl, wave;
					WAVEFORMAT	fmt;
					MyAtom 		dataAt;
					
					iErr = FindAtomById( sat, &rgnAt, true, 'rgn ', x);
					if( iErr) DebugLong( iErr);
					
					// Key range
					
					iErr = GetAtomDataById( rgnAt, 'rgnh', &rgnh, sizeof( rgnh));
					if( iErr) DebugLong( iErr);
					
					rgnh.RangeKey.usLow = EndianU16_LtoN( rgnh.RangeKey.usLow);
					rgnh.RangeKey.usHigh = EndianU16_LtoN( rgnh.RangeKey.usHigh);
					rgnh.RangeVelocity.usLow = EndianU16_LtoN( rgnh.RangeVelocity.usLow);
					rgnh.RangeVelocity.usHigh = EndianU16_LtoN( rgnh.RangeVelocity.usHigh);
					rgnh.fusOptions = EndianU16_LtoN( rgnh.fusOptions);
					rgnh.usKeyGroup = EndianU16_LtoN( rgnh.usKeyGroup);
					
					// Wave sample
					
					iErr = GetAtomDataById( rgnAt, 'wsmp', &wsmp, sizeof( wsmp));
					if( iErr) DebugLong( iErr);
					
					wsmp.cbSize = EndianU32_LtoN( wsmp.cbSize);
					wsmp.usUnityNote = EndianU16_LtoN( wsmp.usUnityNote);
					wsmp.sFineTune = EndianU16_LtoN( wsmp.sFineTune);
					wsmp.lAttenuation = EndianU32_LtoN( wsmp.lAttenuation);
					wsmp.fulOptions = EndianU32_LtoN( wsmp.fulOptions);
					wsmp.cSampleLoops = EndianU32_LtoN( wsmp.cSampleLoops);
					if( wsmp.cSampleLoops > 0)
					{
						long loopSize = sizeof( loop);
						iErr = FSRead( rgnAt.ref, &loopSize, &loop);
						
						loop.cbSize = EndianU32_LtoN( loop.cbSize);
						loop.ulType = EndianU32_LtoN( loop.ulType);
						loop.ulStart = EndianU32_LtoN( loop.ulStart);
						loop.ulLength = EndianU32_LtoN( loop.ulLength);
					}
					else
					{
						loop.cbSize = 0;
						loop.ulType = 0;
						loop.ulStart = 0;
						loop.ulLength = 0;
					}
					
					// Wave Data ID
					
					iErr = GetAtomDataById( rgnAt, 'wlnk', &wlnk, sizeof( wlnk));
					if( iErr) DebugLong( iErr);
					
					wlnk.fusOptions = EndianU16_LtoN( wlnk.fusOptions);
					wlnk.usPhaseGroup = EndianU16_LtoN( wlnk.usPhaseGroup);
					wlnk.ulChannel = EndianU32_LtoN( wlnk.ulChannel);
					wlnk.ulTableIndex = EndianU32_LtoN( wlnk.ulTableIndex);
					
					// Load Wave Data
					
					
					iErr = FindAtomById( at, &wvpl, true, 'wvpl', 0);
					if( iErr) DebugLong( iErr);
					
					iErr = FindAtomById( wvpl, &wave, true, 'wave', wlnk.ulTableIndex);
					if( iErr) DebugLong( iErr);
					
					// FMT
					
					iErr = GetAtomDataById( wave, 'fmt ', &fmt, sizeof( fmt));
					if( iErr) DebugLong( iErr);
					
					fmt.wFormatTag = EndianU16_LtoN( fmt.wFormatTag);
					fmt.nCannels = EndianU16_LtoN( fmt.nCannels);
					fmt.nSamplesPerSec = EndianU32_LtoN( fmt.nSamplesPerSec);
					fmt.nAvgBytesPerSec = EndianU32_LtoN( fmt.nAvgBytesPerSec);
					fmt.nBlockAlign = EndianU16_LtoN( fmt.nBlockAlign);
					fmt.wBitsPerSample = EndianU16_LtoN( fmt.wBitsPerSample);
					
					// Wave Data
					
					iErr = FindAtomById( wave, &dataAt, true, 'data', 0);
					if( iErr) DebugLong( iErr);
					
					data = NewPtr( dataAt.size);
					if( data == NULL) DebugLong( -1);
					iErr = GetAtomDataById( wave, 'data', data, dataAt.size);
					if( iErr) DebugLong( iErr);
					
					
					// ************************
					// * Add it to instrument *
					// ************************
					
					for( ii = rgnh.RangeKey.usLow - 12; ii <= rgnh.RangeKey.usHigh - 12; ii++)
					{
						if( ii < NUMBER_NOTES && ii > 0) inst->what[ ii] = inst->numSamples;
					}
					
					if( curMusic != NULL) curData = MADCreateSample( curMusic, inst->no, inst->numSamples);
					else
					{
						curData = (sData*) NewPtrClear( sizeof( sData));
						if( curData == NULL) MyDebugStr( __LINE__, __FILE__, "");
						sample[ inst->no * MAXSAMPLE + inst->numSamples] = curData;
						
						inst->numSamples++;
					}
					
					curData->size		= 0;
					curData->loopBeg	= 0;
					curData->loopSize	= 0;
					curData->vol		= MAX_VOLUME;
					curData->loopType	= eClassicLoop;
					curData->amp		= 8;
					curData->relNote	= 60 - wsmp.usUnityNote;// + wsmp.sFineTune;	//(60 - ) - 
					
					// curData->name
					
					curData->data = NewPtr( dataAt.size);
					
					if( curData->data == NULL)
					{
						Erreur( 63, -2);
						
						curData->size = 0;
					}
					else
					{
						if( dataAt.size) BlockMoveData( data, curData->data, dataAt.size);
						DisposePtr( data);
						
						switch( fmt.wBitsPerSample)
						{
								//TODO: Handle 24-bit sound?
							case 8:
								ConvertInstrumentIn( (Byte*) curData->data, dataAt.size);
								break;
								
							case 16:
								ConvertInstrumentIn16( (short*) curData->data, dataAt.size);
								break;
						}
						
						curData->size 		= dataAt.size;
						curData->loopBeg 	= loop.ulStart;
						curData->loopSize 	= loop.ulLength;
						curData->vol 		= 64;
						curData->amp	 	= fmt.wBitsPerSample;
						
						if( fmt.nCannels == 2)
						{
							curData->stereo = true;
						}
						if( fmt.nCannels > 2) MyDebugStr( __LINE__, __FILE__, "More than 2 channels");
						
						// **
						curData->c2spd		= fmt.nSamplesPerSec;
						
						if( wsmp.sFineTune != 0)
						{
							curData->c2spd += wsmp.sFineTune * 12.75;		// Si 22050 Hz
						}
						
						// **
						
						OctavesMIDIName( rgnh.RangeKey.usLow - 12, aStr);
						pStrcat( aStr, "\p -> ");
						OctavesMIDIName( rgnh.RangeKey.usHigh - 12, bStr);
						pStrcat( aStr, bStr);
						pStrcat( aStr, "\p, ");
						NumToString( curData->c2spd, bStr);
						pStrcat( aStr, bStr);
						pStrcat( aStr, "\p Hz");
						
						SetSampNameM( aStr, curData->name);
					}
				}
			}
			
			FSCloseFork( iFileRef);
		}
	}
	
BAIL:
	
	if( TestRunningOnCarbonX())
	{
		DeleteDLSFile();
	}
	
	return;
}

static void TESTNEWSYSTEM( sData **sample, InstrData *inst, AtomicInstrument ai)
{
	short 						no, ii, i;
	OSErr 						iErr;
	OSType						synthType;
	Str31						synthName;
	SynthesizerDescription		sd;
	MusicComponent 				mc;
	
	Str255						aStr, bStr;
	
	sData						*curData;
	long						size, inOutBytes;
	InstSampleDescRec			*sdesc;
	Ptr							data;
	QTAtom						myKeyRangeInfoAtom = 0;
	QTAtom						mySampleInfoAtom = 0;
	QTAtom						mySampleDescAtom = 0;
	QTAtom						mySampleDataAtom = 0;
	QTAtom						myInstInfoAtom = 0;
	QTAtom						myToneDescAtom = 0;
	QTAtom						myInstrumentRefAtom = 0;
	QTAtomID					atomID;
	
	short						x;
	
	short						sampleIDMap[ 500];
	
	for( i = 0; i < 500; i++) sampleIDMap[ i] = -1;
	
	/***************/
	
	for( i = 0; i < QTCountChildrenOfType( ai, kParentAtomIsContainer, kaiKeyRangeInfoType); i++)
	{
		myKeyRangeInfoAtom = QTFindChildByIndex( ai, kParentAtomIsContainer, kaiKeyRangeInfoType, i+1, &atomID);
		
		QTLockContainer( ai);
		
		mySampleDescAtom = QTFindChildByIndex( ai, myKeyRangeInfoAtom, kaiSampleDescType, 1, &atomID);
		
		size = 0;
		iErr = QTGetAtomDataPtr(ai,mySampleDescAtom, &size, (Ptr*)&sdesc);
		if( iErr) goto BAIL;
		
		if( sampleIDMap[ GetNEShort(sdesc->sampleDataID)] != -1)
		{
			for( ii = GetNELong( sdesc->pitchLow) - 12; ii <= GetNELong(sdesc->pitchHigh) - 12; ii++)
			{
				if( ii < NUMBER_NOTES && ii > 0) inst->what[ ii] = sampleIDMap[ GetNEShort(sdesc->sampleDataID)];
			}
		}
		else
		{
			sampleIDMap[ GetNEShort(sdesc->sampleDataID)] = inst->numSamples;
			
			for( ii = GetNELong(sdesc->pitchLow) - 12; ii <= GetNELong(sdesc->pitchHigh) - 12; ii++)
			{
				if( ii < NUMBER_NOTES && ii > 0) inst->what[ ii] = inst->numSamples;
			}
			
			SetInstruNameM( inst->no, myNoteRequest.tone.instrumentName, GetNELong(myNoteRequest.tone.instrumentNumber), inst->name);
			
			mySampleInfoAtom = QTFindChildByID( ai, kParentAtomIsContainer, kaiSampleInfoType, GetNEShort(sdesc->sampleDataID), &no);	//
			if( mySampleInfoAtom != 0)
			{
				no = QTCountChildrenOfType( ai, mySampleInfoAtom, kaiSampleDataType);
				if( no != 1) MyDebugStr(__LINE__, __FILE__, "kaiSampleDataType");
				
				mySampleDataAtom = QTFindChildByIndex( ai, mySampleInfoAtom, kaiSampleDataType, 1, &atomID);
				if( mySampleDataAtom == 0) MyDebugStr(__LINE__, __FILE__, "kaiSampleDataType");
				
				size = 0;
				iErr = QTGetAtomDataPtr( ai, mySampleDataAtom, &size, &data);
				if( iErr) MyDebugStr(__LINE__, __FILE__, "QTGetAtomDataPtr");
				
				inOutBytes = (GetNEShort(sdesc->sampleSize) * GetNELong(sdesc->numSamples)) / 8L;
				
				// ************************
				// * Add it to instrument *
				// ************************
				
				{
					if( curMusic != NULL) curData = MADCreateSample( curMusic, inst->no, inst->numSamples);
					else
					{
						curData = (sData*) NewPtrClear( sizeof( sData));
						if( curData == NULL) MyDebugStr( __LINE__, __FILE__, "");
						sample[ inst->no * MAXSAMPLE + inst->numSamples] = curData;
						
						inst->numSamples++;
					}
					
					curData->size		= 0;
					curData->loopBeg	= 0;
					curData->loopSize	= 0;
					curData->vol		= MAX_VOLUME;
					curData->c2spd		= NOFINETUNE;
					curData->loopType	= eClassicLoop;
					curData->amp		= 8;
					// curData->name
					
					curData->data = NewPtr( inOutBytes);
					
					if( curData->data == NULL)
					{
						Erreur( 63, -2);
						
						curData->size = 0;
					}
					else
					{
						if( inOutBytes)
						{
							BlockMoveData( data, curData->data, inOutBytes);
						}
						
						if( GetNEOSType(sdesc->dataFormat) == 'raw ')
						{
							switch( GetNEShort(sdesc->sampleSize))
							{
								case 8:
									ConvertInstrumentIn( (Byte*) curData->data, inOutBytes);
									break;
									
								case 16:
									ConvertInstrumentIn16( (short*) curData->data, inOutBytes);
									break;
							}
						}
						
						curData->size 		= inOutBytes;
						curData->loopBeg 	= GetNELong(sdesc->loopStart);
						curData->loopSize 	= GetNELong(sdesc->loopEnd) - GetNELong(sdesc->loopStart);
						curData->vol 		= 64;
						curData->amp	 	= GetNEShort(sdesc->sampleSize);
						
						if( GetNEShort(sdesc->numChannels) == 2)
						{
							curData->stereo = true;
						}
						if( GetNEShort(sdesc->numChannels) > 2) MyDebugStr( __LINE__, __FILE__, "More than 2 channels");
						
						// **
						curData->c2spd		= GetNEUnsignedFixed(sdesc->sampleRate) >> 16;
						curData->relNote	= 60 - GetNELong(sdesc->pitchNormal);
						// **
						
						OctavesMIDIName( GetNELong(sdesc->pitchLow) - 12, aStr);
						pStrcat( aStr, "\p -> ");
						OctavesMIDIName( GetNELong(sdesc->pitchHigh) - 12, bStr);
						pStrcat( aStr, bStr);
						pStrcat( aStr, "\p, ");
						NumToString( curData->c2spd, bStr);
						pStrcat( aStr, bStr);
						pStrcat( aStr, "\p Hz");
						
						SetSampNameM( aStr, curData->name);
					}
				}
			}
		}
		QTUnlockContainer( ai);
	}
	
BAIL:
	
#if defined( MAINPLAYERPRO)
	CreateInstruList();
	DrawInfoInstrument();
	UpdateSampleWindows();
	UpdateInstruMenu();
	UpdateALLWindow();
#endif
	
	;
}

static short GenerateDLSFromBundle()
{
	CFBundleRef		AudioBundle;
	CFURLRef		bundleURL;
	FSRef 			bundleFSRef, rsrcRef;
	FSSpec			file, tempDLS;
	CFURLRef        rsrcURL;
	short			refNum;
	OSErr			iErr;
	ResType			theType;
	short			i, ff = -1;
	
	//DoStandardOpen( &file, "\pHello", 'ANYK');
	
	iErr = FindFolder( kOnSystemDisk, kComponentsFolderType, kDontCreateFolder, &file.vRefNum, &file.parID);
	if( iErr == noErr)
	{
		pStrcpy( file.name, "\pCoreAudio.component");
	}
	else return -1;
	
	FSpMakeFSRef( &file, &bundleFSRef);
	
	bundleURL = CFURLCreateFromFSRef( kCFAllocatorDefault, &bundleFSRef);
	
	AudioBundle = CFBundleCreate( kCFAllocatorDefault, bundleURL);
	CFRelease(bundleURL);
	if( AudioBundle == NULL) Debugger();
	
	
	// MacOS X 10.2
	
	rsrcURL = 		CFBundleCopyResourceURL( AudioBundle, 
											CFSTR("gs_instruments"), 			//CoreAudio
											CFSTR("dls"),	 					//rsrc
											NULL);
	
	CFURLGetFSRef( rsrcURL, &rsrcRef);
	
	iErr = FSOpenFork( &rsrcRef, 0, 0, fsRdPerm, &refNum);
	CFRelease(rsrcURL);
	
	if( iErr == noErr) 
	{
		CFRelease(AudioBundle);
		return refNum;
	}
	
	
	// Look for a resource in the main bundle by name and type.
	/*rsrcURL = 		CFBundleCopyResourceURL( AudioBundle, 
											CFSTR("CoreAudio"), 			//CoreAudio
											CFSTR("rsrc"), 					//rsrc
											NULL);
	
	CFURLGetFSRef( rsrcURL, &rsrcRef);
	
	iErr = FSOpenResourceFile( &rsrcRef, 0, 0, fsRdPerm, &refNum);
	CFRelease(rsrcURL);
	if( iErr) return -1;*/
	refNum = CFBundleOpenBundleResourceMap(AudioBundle);
	
	for( i = 0; i < Count1Types(); i++)
	{
		Get1IndType( &theType, i+1);
		
		if( theType == 'dls2')
		{
			Handle	rsrc = Get1IndResource( 'dls2', 1);
			DetachResource( rsrc);
			
			// Write a temp DLS2 File on the hard drive...
			
			iErr = FindFolder( kOnSystemDisk, kTemporaryFolderType, kCreateFolder, &tempDLS.vRefNum, &tempDLS.parID);
			if( iErr == noErr)
			{
				pStrcpy( tempDLS.name, "\pTempDLS2.dls");
				
				FSpDelete( &tempDLS);
				iErr = FSpCreate( &tempDLS, 'INIT', 'dvb ', smSystemScript);
				if( iErr == noErr)
				{
					long	count;
					
					FSpOpenDF( &tempDLS, fsCurPerm, &ff);
					
					HLock( rsrc);
					count = GetHandleSize( rsrc);
					FSWrite( ff, &count, *rsrc);
					HUnlock( rsrc);
					
					SetFPos( ff, fsFromStart, 0);
				}
			}
			
			DisposeHandle( rsrc);
		}
	}
	
	CFBundleCloseBundleResourceMap(AudioBundle, refNum);
	CFRelease(AudioBundle);
	
	return ff;
}

static void DeleteDLSFile()
{
	OSErr	iErr;
	FSSpec	tempDLS;
	
	iErr = FindFolder( kOnSystemDisk, kTemporaryFolderType, kCreateFolder, &tempDLS.vRefNum, &tempDLS.parID);
	if( iErr == noErr)
	{
		pStrcpy( tempDLS.name, "\pTempDLS2.dls");
		
		FSpDelete( &tempDLS);
	}
}

static short OpenDataFileQK( long dirID, short VRefNum)
{
	CInfoPBRec		info;
	Str255			tempStr;
	//	long			dirIDCopy;
	short			i, vRefNum;
	OSErr			iErr;
	FSSpec			spec;
	short			iRefNum = -1;
	
	info.hFileInfo.ioNamePtr = tempStr;
	info.hFileInfo.ioVRefNum = VRefNum;
	
	for (i = 1; true; i ++)
	{
		info.hFileInfo.ioDirID = dirID;
		info.hFileInfo.ioFDirIndex = i;
		
		if (PBGetCatInfoSync(&info) != noErr) break;
		
		if( info.hFileInfo.ioFlFndrInfo.fdType == 'INIT' && info.hFileInfo.ioFlFndrInfo.fdCreator == 'dvb ')
		{	
			//	HGetVol( NULL, &vRefNum, &dirIDCopy);
			
			//	iErr = HSetVol( NULL, info.hFileInfo.ioVRefNum, dirID);
			
			pStrcpy( spec.name, info.hFileInfo.ioNamePtr);
			spec.vRefNum = info.hFileInfo.ioVRefNum;
			spec.parID = dirID;
			
			iErr = FSpOpenDF( &spec, fsCurPerm, &iRefNum);
			if( iErr != noErr) iRefNum = -1;
			
			//	iErr = HSetVol( NULL, vRefNum, dirIDCopy);
			//	if( iErr != noErr) MyDebugStr( __LINE__, __FILE__, "HSetVol error...");
		}
	}
	
	return iRefNum;
}

#pragma mark Atom functions
static OSErr GetAtomData( MyAtom at, void* data, long size)
{
	long 	prePos, fSize, ilistType, nlistType, index, listSize;
	CK		sck;
	OSErr	iErr;
	MyAtom	retat;
	
	iErr = SetFPos( at.ref, fsFromStart, at.pos -4);
	
	fSize = sizeof( sck);
	iErr = FSRead( at.ref, &fSize, &sck);
	if( iErr) Debugger();
	sck.cksize = EndianU32_NtoL( sck.cksize);
	
	//if( sck.cksize > size) Debugger();
	
	fSize = size;	//sck.cksize;
	iErr = FSRead( at.ref, &fSize, data);
	if( iErr) Debugger();
	
	return noErr;
}

static long CountAtomById( MyAtom at, long type)
{
	long 	prePos, fSize, ilistType, /*nlistType,*/ index, listSize;
	CK		sck;
	OSErr	iErr;
	
	listSize = at.size;
	
	listSize -= 4;
	index = 0;
	
	iErr = SetFPos( at.ref, fsFromStart, at.pos);
	
	do
	{
		fSize = sizeof( sck);
		iErr = FSRead( at.ref, &fSize, &sck);
		if( iErr) DebugLong( iErr);
		sck.cksize = EndianU32_NtoL( sck.cksize);
		
		GetFPos( at.ref, &prePos);
		
		listSize -= sizeof( sck);
		
		if( sck.ckid == type) index++;
		
		switch( sck.ckid)
		{
			case 'LIST':
				
				fSize = 4;
				iErr = FSRead( at.ref, &fSize, &ilistType);
				
				if( ilistType == type) index++;
				break;
		}
		
		iErr = SetFPos( at.ref, fsFromStart, prePos + sck.cksize);
		if( iErr) DebugLong( iErr);
		
		listSize -= sck.cksize;
		
		listSize /= 2;
		listSize *= 2;
		
	}while( iErr == noErr && listSize > 0);
	
	if( listSize < 0 ) Debugger();
	
	return index;
}

static OSErr FindAtomById( MyAtom at, MyAtom *retat, Boolean LIST, long type, short id)
{
	long 	prePos, fSize, ilistType, /*nlistType,*/ index, listSize;
	CK		sck;
	OSErr	iErr;
	
	listSize = at.size;
	
	listSize -= 4;
	index = 0;
	
	iErr = SetFPos( at.ref, fsFromStart, at.pos);
	
	do
	{
		fSize = sizeof( sck);
		iErr = FSRead( at.ref, &fSize, &sck);
		if( iErr) DebugLong( iErr);
		sck.cksize = EndianU32_NtoL( sck.cksize);
		
		GetFPos( at.ref, &prePos);
		
		listSize -= sizeof( sck);
		
		if( sck.ckid == type)
		{
			if( index == id)
			{
				// We found it !!!!!
				retat->pos = prePos -4;
				retat->id = sck.ckid;
				retat->size = sck.cksize;
				retat->ref = at.ref;
				
				return noErr;
			}
			index++;
		}
		
		switch( sck.ckid)
		{
			case 'LIST':
				//	case 'INFO':
				fSize = 4;
				iErr = FSRead( at.ref, &fSize, &ilistType);
				
				if( ilistType == type)
				{
					if( index == id)
					{
						GetFPos( at.ref, &prePos);
						
						// We found it !!!!!
						retat->pos = prePos;
						retat->id = ilistType;
						retat->size = sck.cksize;
						retat->ref = at.ref;
						
						return noErr;
					}
					index++;
				}
				break;
		}
		
		iErr = SetFPos( at.ref, fsFromStart, prePos + sck.cksize);
		if( iErr) DebugLong( iErr);
		
		listSize -= sck.cksize;
		
		listSize /= 2;
		listSize *= 2;
		
	}while( iErr == noErr && listSize > 0);
	
	if( listSize < 0 ) Debugger();
	
	return -1;
}

static OSErr GetAtomDataById( MyAtom at, long type, void *data, long size)
{
	MyAtom 	tempAt;
	OSErr	iErr;
	
	iErr = FindAtomById( at, &tempAt, true, type, 0);
	if( iErr) return( iErr);
	
	
	iErr = GetAtomData( tempAt, data, size);
	if( iErr) return( iErr);
	
	return noErr;
}
