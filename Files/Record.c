#include "Shuddup.h"
#include "RDriver.h"
#include "RDriverInt.h"
#include "AIFF.h"
#include "sound.h"
#include "soundinput.h"
#include "Undo.h"
#include "FixMath.h"
#include "Math.h"
#include "PPPlug.h"
#include "navigation.h"

void 	SaveMODAPPL( short APPLType, short fRefNum);
//pascal Boolean MyFilterPro();
void 	TurnRadio( short	item, DialogPtr	dlog, Boolean alors);
void 	ConvertInstrumentIn( register	Byte	*tempPtr,	register long sSize);
//Handle 	ConvertWAV(FSSpec*, long*, long*, short*, short*);
void 	TurnRadio( short	item, DialogPtr	dlog, Boolean alors);
void 	ConvertInstrumentIn16( register	short	*tempPtr,	register long sSize);
void 	ExportFile( OSType theType, FSSpec *newFile);
OSErr	ActiveSoundInput( Boolean RecordingMode, Handle *RecordedSound, Str255 name);
OSErr	OpenInstrumentsList( FSSpec *file);
void	ConvertInstrumentMode( sData	*curData, short menuItem);
void ConvertInstrument( register	Byte	*tempPtr,	register long sSize);

extern	Boolean			PatchSave;
extern	Boolean			PianoRecording, PianoRecordingShift, OscilloMicrophone, SpectrumMicrophone;
extern	Cursor			PlayCrsr;

void ActiveControl( short id, DialogPtr aDia)
{
	short	itemType;
	Rect	itemRect;
	Handle	itemHandle;

	GetDialogItem (aDia, id, &itemType, &itemHandle, &itemRect);
	HiliteControl( (ControlHandle) itemHandle, 0);
}

void InactiveControl( short id, DialogPtr aDia)
{
	short	itemType;
	Rect	itemRect;
	Handle	itemHandle;

	GetDialogItem (aDia, id, &itemType, &itemHandle, &itemRect);
	HiliteControl( (ControlHandle) itemHandle, 255);
}

void UpdateSoundQualityExportSndWindow( DialogPtr theDia)
{
GrafPtr	savePort;

	GetPort( &savePort);
	SetPortDialogPort( theDia);
	
	BeginUpdate( GetDialogWindow( theDia));
	
		DrawDialog( theDia);
		oldFrameButton( theDia);
		
	EndUpdate( GetDialogWindow( theDia));
	SetPort( savePort);
}

void GetSampInstPPIN( short *samp, short *ins);

OSErr NSoundQualityExportSnd( short ins, short samp, OSType *fileType, Str255 sName)
{
	DialogPtr		aDialog;
	short			itemHit, itemType, i;
	char			cstr[ 200];
	Str255			aStr, theStr;
	Rect			tempRect, itemRect;
	Point			myPt;
	Handle			itemHandle;
	long			mresult, temp, xL, x;
	Ptr				bufferSin, bufferSout;
	sData			*curData;
	short			sampNo, instNo;
	GrafPtr			thePort;
	
	GetPort( &thePort);
	
	GetSampInstPPIN( &sampNo, &instNo);
	if( samp < 0)
	{
		if( instNo <= 0) { Erreur( 86, 86);	return -1;}
	}
	else
	{
		if( sampNo <= 0) { Erreur( 86, 86);	return -1;}
	}
	
	SetCursor( GetQDGlobalsArrow( &qdarrow));
	
	aDialog = GetNewDialog( 161, 0L, (WindowPtr) -1L);
	SetPortDialogPort( aDialog);
	
	ChangeDialogFont( aDialog);
	
	AutoPosition( aDialog);
	ShowWindow( GetDialogWindow( aDialog));
	
	if( samp < 0)
	{
		strcpy( cstr, curMusic->fid[ ins].name);
		SetDText( aDialog, 4, "\pInstrument");
	}
	else
	{
		curData = curMusic->sample[ curMusic->fid[ ins].firstSample + samp];
		strcpy( cstr, curData->name);
		SetDText( aDialog, 4, "\pSample");
	}
	for( i = 0; i < strlen( cstr); i++) if( cstr[ i] == ':') cstr[ i] = '-';
	
	MyC2PStr( cstr);
	SetDText( aDialog, 9, (unsigned char*) cstr);
	SelIText( aDialog, 9, 0, 2000);
	if( samp >= 0)
	{
		NumToString( curData->size/1024L, aStr);
	}
	else
	{
		long tO = 0;
		
		for( i = 0; i < curMusic->fid[ ins].numSamples ; i++)
		{
			tO += curMusic->sample[ curMusic->fid[ ins].firstSample + i]->size;
		}
		NumToString( tO/1024L, aStr);
	}
	pStrcat( aStr, "\p Kb");
	SetDText( aDialog, 11, aStr);
	
	if( samp < 0)	*fileType = thePrefs.SoundTypeIns;
	else *fileType = thePrefs.SoundTypeSamp;
//	OSType2Str( *fileType, aStr);

	if( PPINGetPlugName( *fileType, aStr) == noErr) SetDText( aDialog, 6, aStr);
	else
	{
		if( PPINGetPlugByID( fileType, 0, samp) == noErr)
		{
			PPINGetPlugName( *fileType, aStr);
			SetDText( aDialog, 6, aStr);
		}
		else SetDText( aDialog, 6, "\pNone");
	}
	
	REGODIA:
	
	do
	{
		//ModalDialog( MyDlgFilterDesc, &itemHit);
		MyModalDialog( aDialog, &itemHit);
		
		switch( itemHit)
		{
			case 7:
				{
				OSType	tempOS;
				
				GetDialogItem( aDialog, 7, &itemType, &itemHandle, &itemRect);
				tempOS = PressPPINMenu( &itemRect, *fileType, samp, aStr);
				if( tempOS != '????')
				{
					*fileType = tempOS;
				//	OSType2Str( *fileType, aStr);
					SetDText( aDialog, 6, aStr);
				}
				}
			break;
		}
		
	}while( itemHit != 1 && itemHit != 2);
	
	if( itemHit == 1)
	{
		GetDText( aDialog, 9, sName);
		if( sName[ 0] == 0)
		{
			SelIText( aDialog, 9, 0, 2000);
			Erreur( 65, 65);
			goto REGODIA;
		}
		
		if( thePrefs.addExtension)
		{
			Str255	cStr;
			
			for( i = sName[ 0]; i > 0 ; i--)
			{
				if( sName[ i] == '.')
				{
					sName[ 0] = i-1;
					i = 0;
				}
			}
			OSType2Str( *fileType, cStr);
				
			pStrcat( sName, "\p.");
			pStrcat( sName, cStr);
		}
		
		if( PPINAvailablePlug( *fileType, 0L) != noErr)
		{
			Erreur( 86, 86);
			goto REGODIA;
		}
		
		/*sndHandle = MyNewHandle( curData->size);
		if( sndHandle == 0L)
		{
			Erreur( 63, -3);
			goto ENDDialog;
		}
		HLock( sndHandle);
		
		if( curData->amp == 8)
		{
			for( x = 0; x < curData->size; x++)
			{
				*(*sndHandle + x) =  *(curData->data + x) - 0x80;
			}
		}
		else
		{
			for( temp = 0; temp < curData->size/2; temp++)
			{
				*(((short*)*sndHandle) + temp) = *(((short*) curData->data) + temp);
			}
		}
		
		if( curData->amp == 16) ConvertInstrumentIn16( (short*) *sndHandle, (long) curData->size);
		
		HUnlock( sndHandle);*/
	}

	ENDDialog:
	
	DisposDialog( aDialog);
	
	UpdateALLWindow();
	
	if( samp < 0) thePrefs.SoundTypeIns = *fileType;
	else thePrefs.SoundTypeSamp = *fileType;
	
	SetPort( thePort);
	
	if( itemHit == 1) return noErr;
	else return -1;
}

void Convert8to16( Ptr srcPtr, Ptr destPtr, long size)
{
long 	i;

	for( i = size -1; i >= 0; i --)
	{
		destPtr[ i*2] = srcPtr[ i];
		destPtr[ i*2 + 1] = 0;
	}
}

void Convert16to8( Ptr srcPtr, Ptr destPtr, long size)
{
long 	i;

size /= 2;

	for( i = 0; i < size; i ++)
	{
		destPtr[ i] = srcPtr[i*2];
	}
}

void ConvertInstrumentMode( sData	*curData, short menuItem)
{
long	i;
Ptr		aNewPtr;

	if( curData->stereo)
	{
		curData->stereo = false;
		
		aNewPtr = MyNewPtr( curData->size/2L);
		
		curData->loopBeg /=2;
		curData->loopSize /=2;
		
		switch( menuItem)
		{
			case 5:	// Delete Left
				if( curData->amp == 8)
				{
					for( i = 0 ; i < curData->size; i+=2)
					{
						aNewPtr[ i / 2] = (long) curData->data[ i];
					}
				}
				else
				{
					short *short16out = (short*) aNewPtr, *short16in = (short*) curData->data;
					
					for( i = 0 ; i < curData->size/2; i+=2)
					{
						short16out[ i / 2] = (long) short16in[ i];
					}
				}
			break;
			
			case 4: // Delete Right
				if( curData->amp == 8)
				{
					for( i = 0 ; i < curData->size; i+=2)
					{
						aNewPtr[ i / 2] = (long) curData->data[ i + 1];
					}
				}
				else
				{
					short *short16out = (short*) aNewPtr, *short16in = (short*) curData->data;
					
					for( i = 0 ; i < curData->size/2; i+=2)
					{
						short16out[ i / 2] = (long) short16in[ i + 1];
					}
				}
			break;
			
			case 6:	// Add
				if( curData->amp == 8)
				{
					long 	templ;
					
					
					for( i = 0 ; i < curData->size; i+=2)
					{
						templ = ((long) curData->data[ i] + (long) curData->data[ i + 1]);
						
						if( templ < -127) templ = -127;
						else if( templ > 127) templ = 127;
						
						aNewPtr[ i / 2] = templ;
					}
				}
				else
				{
					short	*short16out = (short*) aNewPtr, *short16in = (short*) curData->data;
					short	valP = 0x7FFFL, valN = -0x7FFFL;
					long	templ;
					
					for( i = 0 ; i < curData->size/2; i+=2)
					{
						templ = ((long) short16in[ i] + (long) short16in[ i + 1]);
						
						if( templ > valP) short16out[ i / 2] = valP;
						else if( templ < valN) short16out[ i / 2] = valN;
						else short16out[ i / 2] = templ;
					}
				}
			break;
			
			default:
				if( curData->amp == 8)
				{
					for( i = 0 ; i < curData->size; i+=2)
					{
						aNewPtr[ i / 2] = ((long) curData->data[ i] + (long) curData->data[ i + 1]) / 2L;
					}
				}
				else
				{
					short *short16out = (short*) aNewPtr, *short16in = (short*) curData->data;
					
					for( i = 0 ; i < curData->size/2; i+=2)
					{
						short16out[ i / 2] = ((long) short16in[ i] + (long) short16in[ i + 1]) / 2L;
					}
				}
			break;
		}
		
		curData->size /= 2;
		if( curData->data != 0L) MyDisposePtr( &curData->data);
		curData->data = aNewPtr;
	}
	else
	{
		aNewPtr = MyNewPtr( curData->size*2L);
		if( aNewPtr == 0L) MyDebugStr( __LINE__, __FILE__, "Need more memory");
		else
		{
			curData->stereo = true;
			
			curData->loopBeg *=2;
			curData->loopSize *=2;
			
			if( curData->amp == 8)
			{
				for( i = 0 ; i < curData->size; i++)
				{
					if( 1 + i * 2 >= curData->size*2L) MyDebugStr( __LINE__, __FILE__, "");
					
					aNewPtr[ i * 2] = aNewPtr[ 1 + i * 2] = curData->data[ i];
				}
			}
			else
			{
				short *short16out = (short*) aNewPtr, *short16in = (short*) curData->data;
				
				for( i = 0 ; i < curData->size/2; i++)
				{
					short16out[ i * 2] = short16out[ 1 + i * 2] = short16in[ i];
				}
			}
			
			curData->size *= 2;
			if( curData->data != 0L) MyDisposePtr( &curData->data);
			curData->data = aNewPtr;
		}
	}
}

void ConvertInstrumentAmpli( sData	*curData, short newAmpli)
{
long	i;
Ptr		aNewPtr;

	if( curData->amp == 8)
	{
		switch( newAmpli)
		{
		case 16:			
			aNewPtr = MyNewPtr( curData->size*2L);
			if( aNewPtr)
			{
				Convert8to16(	curData->data,
								aNewPtr, 
								curData->size);

				curData->loopBeg *=2;
				curData->loopSize *=2;
				curData->size *= 2;
				
				if( curData->data != 0L) MyDisposePtr( &curData->data);
				curData->data = aNewPtr;
			}
			else Erreur( 63, MemError());
		break;
		
		case 8:
		break;
		
		default:
			MyDebugStr( __LINE__, __FILE__, "Unknow amplitude");
		break;
		}
	}
	else if( curData->amp == 16)
	{
		switch( newAmpli)
		{
		case 8:
			Convert16to8(	curData->data,
							curData->data, 
							curData->size);
			
			curData->loopBeg /=2;
			curData->loopSize /=2;
			curData->size /= 2;
			
			SetPtrSize( curData->data, curData->size);
		break;
		
		case 16:
		break;
		
		default:
			MyDebugStr( __LINE__, __FILE__, "Unknow amplitude");
		break;
		}
	}
	
	curData->amp = newAmpli;
	
//	ResetSelectionSample( whichInstru);
}


void ConvertInstrument16( register	short	*tempPtr,	register long sSize)
{
	register	short			val = 0x8000;

	sSize /= 2;

	while( sSize > 0)
	{
		sSize--;
		*(tempPtr++) += val;
	}
}


void ConvertInstrumentIn16( register	short	*tempPtr,	register long sSize)
{
/*	register	short			val = 0x8000;

	sSize /= 2;

	while( sSize > 0)
	{
		sSize--;
		*tempPtr++ -= val;
	}*/
}

void ConvertInstrumentOut16( register	short	*tempPtr,	register long sSize)
{
/*	register	short			val = 0x8000;

	sSize /= 2;

	while( sSize > 0)
	{
		sSize--;
		*tempPtr++ += val;
	}*/
}

void CopyInstruInFile(Ptr theSource, Ptr theDest, long	theSize)
{
	BlockMoveData( theSource, theDest, theSize);
}

void SetUpPartition( short newVal)
{
short						i, x, z;
long					OffSetToSample,sndSize, fileSize, lByteCnt, numPat;
Ptr						tempPtr;
Boolean					MusiqueOn = false;
Cmd			 			*aCmd, *aCmdSource;
PatData					*theNewPartition;

		curMusic->hasChanged = true;
		if( MADDriver->Reading == true) MusiqueOn = true;
		MADDriver->Reading = false;

		/****** PARTITION ********/
		for(i = 0; i< curMusic->header->numPat; i++)
		{
			theNewPartition = (PatData*) NewPtrClear( sizeof( PatHeader) + newVal * curMusic->partition[ i]->header.size * sizeof( Cmd));
			if( theNewPartition == 0L) MyDebugStr( __LINE__, __FILE__, "Memory Error");
			theNewPartition->header = curMusic->partition[ i]->header;

			for(x = 0; x < curMusic->partition[ i]->header.size; x++)
			{
				for( z = 0; z < curMusic->header->numChn; z++)
				{
					if( z < newVal)
					{
						aCmd =  &(theNewPartition->Cmds[ ( curMusic->partition[ i]->header.size * z) + x]);
						aCmdSource = GetMADCommand( x, z, curMusic->partition[ i]);
					
						*aCmd = *aCmdSource;
					}
				}
				
				for( z = curMusic->header->numChn; z < newVal; z++)
				{
					aCmd =  &(theNewPartition->Cmds[ ( curMusic->partition[ i]->header.size * z) + x]);
					MADKillCmd( aCmd);
				}
			}
			MyDisposePtr( (Ptr*) &curMusic->partition[ i]);
			curMusic->partition[ i] = theNewPartition;
		}

		if( MemError()) MyDebugStr( __LINE__, __FILE__, "Error in SetUp Partition...");

		curMusic->header->numChn = newVal;
		
		if( MusiqueOn) MADDriver->Reading = true;
}

//#include "CTBUtilities.h"

static		Boolean 		Append;
static		short			base;
static		short			Quality;
static		short			Mode;
static		Str255			instruNom;

void GetInstruName( short theNo, Ptr theName)
{
short	i;
	
	theName[ 32] = 0;
	for(i=0; i<32; i++)
	{
		theName[ i] = curMusic->fid[ theNo].name[i];
	}
}

void SetInstruName( short	theNo, Str255 theNewName)
{
short	i;
	
	for(i=0; i<32; i++)
	{
		if( i < theNewName[0]) curMusic->fid[theNo].name[i] = theNewName[i+1];
		else curMusic->fid[theNo].name[i] = '\0';
	}
}

void SetSampName( short	ins, short samp, Str255 theNewName)
{
short	i;
sData	*curData = curMusic->sample[ curMusic->fid[ ins].firstSample + samp];

	if( samp >= curMusic->fid[ ins].numSamples) return;
	
	for(i=0; i<32; i++)
	{
		if( i < theNewName[ 0]) curData->name[i] = theNewName[i+1];
		else curData->name[i] = '\0';
	}
}


pascal Boolean MyFilterPro (DialogPtr theDialog, EventRecord *theEventI, short *itemHit) 
{ 
Handle			theDITL;

Handle	itemHandle;
short		itemType, temp, tFont, tSize;
Rect	itemRect;
Str255	theString;

		if(theEventI->what == 8)
		{
			theDialog = GetDialogFromWindow( FrontWindow());
			
			if( !Append)
			{
				Append = true;
				theDITL = GetResource('DITL', 150);
				if( theDITL == nil) MyDebugStr( __LINE__, __FILE__, " DITL 150 !!!!!!");
				base = CountDITL( theDialog);
				
				AppendDITL( theDialog, theDITL, appendDITLBottom);
				ReleaseResource( theDITL);
				
				pStrcpy( theString, "\pInstrument ");
				pStrcat( theString, instruNom);
				SetDText (theDialog, base + 1, theString);
				SelIText( theDialog, base + 1, 0, 32767);
				GetDText( theDialog, base + 1, (StringPtr) &instruNom);
			}
		}
		else if( theEventI->what == keyDown)
		{
			theDialog = GetDialogFromWindow( FrontWindow());
			GetDText( theDialog, base + 1, (StringPtr) &instruNom);
		}
		
		return( false);
}

Handle DoExp1to3( Handle sound, unsigned long numSampleFrames)
{
long	i;
Ptr		inState, outState;
Handle	outBuffer;

	outState = NewPtrClear( 128);
	inState = NewPtrClear( 128);
	if( inState == nil)
	{
		Erreur( 2, MemError());
	}
	
	outBuffer = MyNewHandle( numSampleFrames*6);
	if( outBuffer == nil)
	{
		Erreur( 2, MemError());
	}
	
	HLock( sound);
	HLock( outBuffer);
	
	#if MACOS9VERSION
	
	Exp1to3( *sound, *outBuffer, numSampleFrames, (StateBlockPtr) inState, (StateBlockPtr) outState, 1, 1);
	
	#endif
	
	HUnlock( sound);
	HUnlock( outBuffer);
		
	MyDisposHandle( & sound);
	sound = outBuffer;
	
	MyDisposePtr( & inState);
	MyDisposePtr( & outState);

	return sound;
}

Handle DoExp1to6( Handle sound, unsigned long numSampleFrames)
{
long	i;
Ptr		inState, outState;
Handle	outBuffer;

	outState = NewPtrClear( 128);
	inState = NewPtrClear( 128);
	if( inState == nil)
	{
		Erreur( 2, MemError());
	}
	
	outBuffer = MyNewHandle( numSampleFrames * 6);
	if( outBuffer == nil)
	{
		Erreur( 2, MemError());
	}
	
	HLock( sound);
	HLock( outBuffer);
	
	#if MACOS9VERSION
	
	Exp1to6( *sound, *outBuffer, numSampleFrames, (StateBlockPtr) inState, (StateBlockPtr) outState, 1, 1);
	
	#endif
	
	HUnlock( sound);
	HUnlock( outBuffer);
		
	MyDisposHandle( & sound);
	sound = outBuffer;
	
	MyDisposePtr( & inState);
	MyDisposePtr( & outState);

	return sound;
}

/*Handle NAIFFtoSnd( Handle sound, long *loopStart, long *loopEnd, short	*sampleSize, unsigned long *rate)
{
long				i, SizeH, numSampleFrames, StartId, numChannels;
ContainerChunk		*CC;
ChunkHeader			*CH;
ExtCommonChunk		*CommC;
SoundDataChunk		*SData;
MarkerChunk			*marker;
Marker				*mm;
unsigned long		theFixed;
Boolean				Compression = false;
ID					compType;

	CC = (ContainerChunk*) *sound;

	*loopStart = *loopEnd = 0;
	*sampleSize = 8;
	*rate = rate22khz;
	numChannels = 1;

	if( CC->formType == AIFCID) Compression = true;
	else if( CC->formType == AIFFID) Compression = false;

	for( i = sizeof( ContainerChunk); i < CC->ckSize;)
	{
		CH = (ChunkHeader*) (*sound + i);
		if( CH->ckID == CommonID)
		{
			CommC = (ExtCommonChunk*) CH;			
			numChannels = CommC->numChannels;
			numSampleFrames = CommC->numSampleFrames;
			compType = CommC->compressionType;
			
			{
				struct _extended80		*too;
				
				too = (struct _extended80* ) &CommC->sampleRate;
				
				*rate = (unsigned short) too->man[ 0];
				*rate >>= (15 - (too->exp - 0x3FFF));
				*rate <<= 16L;
			}
			
			*sampleSize = CommC->sampleSize;
			if( *sampleSize == 16) numSampleFrames *= 2;

		}
		else if( CH->ckID == SoundDataID)
		{
			SData = (SoundDataChunk*) CH;
			StartId = i;
		}
		else if( CH->ckID == MarkerID)
		{
			marker = (MarkerChunk*) CH;
			if( marker->numMarkers == 2)
			{
				*loopStart = marker->Markers[ 0].position;
				mm = (Marker*) marker->Markers;
				mm = (Marker*) ((Ptr) mm + 8L);
				mm = (Marker*) ((Ptr) mm + marker->Markers[ 0].markerName[0]);
				*loopEnd = mm->position;
				
				if( *sampleSize == 16)
				{
					*loopStart *= 2;
					*loopEnd *= 2;
				}
			}
		}
		
		i += CH->ckSize;
		i += 8;
	}

	StartId += 16;  		// Size of the SoundDataChunk beginning
	
	SizeH = GetHandleSize( sound);
	if( numChannels == 1)
	{
		BlockMoveData( *sound + StartId, *sound, SizeH - StartId);
	}
	else
	{
		if( *sampleSize == 8)
		{
			for( i = 0; i < SizeH - StartId; i++)
			{
				(*sound)[ i] = (*sound + StartId)[ i * numChannels];
			}
		}
		else
		{
			for( i = 0; i < (SizeH - StartId)/2; i++)
			{
				((short*) *sound)[ i] = ((short*) (*sound + StartId))[ i * numChannels];
			}
		}
	}
	if( Compression)
	{
		switch( compType)
		{
			case MACE3Type:
				numSampleFrames *= 2;
				sound = DoExp1to3( sound, numSampleFrames);
				numSampleFrames *= 3;
			break;
			
			case MACE6Type:
				sound = DoExp1to6( sound, numSampleFrames);
				numSampleFrames *= 6;
			break;
			
			default:
				Erreur( 7, (OSErr) compType);
			break;
		}
	}
	else
	{
		if( *sampleSize == 8)
		{
			for( i = 0; i < numSampleFrames; i++) *((*sound) + i) -= 128;
		}
	}
	
	HUnlock( sound);
	SetHandleSize( sound, numSampleFrames);
	
	CompactMem( maxSize);
	
	return( sound);
}
 */

Handle NSndToHandle( Handle sound, long *loopStart, long *loopEnd, short *sampleSize, unsigned long *sampleRate, long *baseFreq, Boolean *stereo)
{
	Ptr 			soundPtr;
	short 			soundFormat, numChannels;
	short 			numSynths, numCmds, CompressID;
	long 			offset, MusSize;
	SoundHeaderPtr 	header;
	CmpSoundHeader	*CmpHeader;
	ExtSoundHeader	*ExtHeader;
	SndCommand 		cmd;
	OSErr 			result;
	long			i,x, numFrames;
	Boolean			change = false;
	Str255			aStr;
	
	if( !sound) MyDebugStr( __LINE__, __FILE__, "NSndToHandle");
	
	*loopStart = 0;
	*loopEnd = 0;
	*sampleSize = 8;
	*stereo = false;
	
	// make the sound safe to use at interrupt time.
	HLock( sound);
	soundPtr = *sound;
	
	// determine what format sound we have.
	soundFormat = *(short*)soundPtr;
	
	switch(soundFormat)
	{
		case 1:						// format 1 sound.
			// look inside the format 1 resource and deduce offsets.
			numSynths = ((short*)soundPtr)[1];					// get # synths. 
			numCmds = *(short*)(soundPtr+4+numSynths*6);		// get # commands.
		break;
		
		case 2:						// format 2 sound. 
			numSynths = 0;			// format 2 sounds have no synth's. 
			numCmds = ((short*)soundPtr)[2];
		break;
		
		default:					// jack says, what about 12? or 6?
			MyDebugStr( __LINE__, __FILE__, " NSndToHandle... Burkk");
		break;
	} 

	// compute address of sound header.
	offset = 6 + 6*numSynths + 8*numCmds;
	header = (SoundHeaderPtr) (*sound + offset);
	
	switch( header->encode)
	{
		case cmpSH:
			CmpHeader = (CmpSoundHeader*) header;
			CompressID = CmpHeader->compressionID;
			numChannels = CmpHeader->numChannels;			

			*loopStart = CmpHeader->loopStart;
			*loopEnd = CmpHeader->loopEnd;
			*sampleSize = CmpHeader->sampleSize;
			if( numChannels == 2) *stereo = true;
			else *stereo = false;
			
			if( sampleRate != 0L) 	*sampleRate	= CmpHeader->sampleRate;
			if( baseFreq != 0L) 	*baseFreq 	= CmpHeader->baseFrequency;

			MusSize = (*CmpHeader).numFrames;
			if( *stereo)
			{
				MusSize *= 2;
				*loopStart *=2;
				*loopEnd *=2;
			}
			HLock( sound);
			BlockMoveData( (*CmpHeader).sampleArea, *sound, MusSize);
			HUnlock( sound);
					
			switch( CompressID )
			{
				case threeToOne:
					MusSize *= 2;
					sound = DoExp1to3( sound, MusSize);
					MusSize *= 3;
				break;
				
				case sixToOne:
					sound = DoExp1to6( sound, MusSize);
					MusSize *= 6;
				break;
				
				default:
					Erreur( 7, (OSErr) CompressID);
				break;
			}
			
		break;

		case extSH:
			ExtHeader = (ExtSoundHeader*) header;
			
			MusSize = ExtHeader->numFrames;
			numChannels = ExtHeader->numChannels;

			*loopStart = ExtHeader->loopStart;
			*loopEnd = ExtHeader->loopEnd;
			*sampleSize = ExtHeader->sampleSize;
			
			if( sampleRate != 0L) 	*sampleRate	= ExtHeader->sampleRate;
			if( baseFreq != 0L) 	*baseFreq 	= ExtHeader->baseFrequency;
			
			if( numChannels == 2) *stereo = true;
			else *stereo = false;
			
			if( *stereo)
			{
				MusSize *= 2;
				*loopStart *=2;
				*loopEnd *=2;
			}
			
			if( *sampleSize == 16)
			{
				MusSize *= 2;
				*loopStart *= 2;
				*loopEnd *= 2;
			}
			
			HLock( sound);
			if( numChannels == 1) BlockMoveData( ExtHeader->sampleArea, *sound, MusSize);
			else if( numChannels == 2)
			{
				BlockMoveData( ExtHeader->sampleArea, *sound, MusSize);
			}
			else
			{
				if( *sampleSize == 8)
				{
					for( i = 0; i < MusSize; i ++)
					{
						(*sound)[ i] = ExtHeader->sampleArea[ i * numChannels];
					}
				}
				else
				{
					MusSize /= 2;
					for( i = 0; i < MusSize; i ++)
					{
						((short*) (*sound))[ i] = ((short*) ExtHeader->sampleArea)[ i * numChannels];
					}
					MusSize *= 2;
				}
			}
			HUnlock( sound);
		break;
		
		default:
		case stdSH:
			*loopStart = header->loopStart;
			*loopEnd = header->loopEnd;
			
			if( sampleRate != 0L) 	*sampleRate	= header->sampleRate;
			if( baseFreq != 0L) 	*baseFreq 	= header->baseFrequency;
			
			MusSize = header->length;
			BlockMoveData( (*header).sampleArea, *sound, MusSize);
			HUnlock( sound);
		break;
	}
	
	HUnlock( sound);
	SetHandleSize( sound, MusSize);
	
	if( MemError() != noErr) Erreur( 2, MemError());
	
	if( *loopEnd - *loopStart < 4) { *loopEnd = 0;	*loopStart = 0;}
	
	CompactMem( maxSize);
	
	return( sound);
}

static long RecordCounter = 0;

void RecordSample(void)
{
short			itemType,i,fRefNum, ins, samp, temp, sS, qual, lastNo;
long			inOutBytes, iL, OffSetToSample, lS, lE;
Handle			newSound;
Rect			itemRect;
Str255			theString;
OSErr			iErr;
Point			where = { 50, 50};
Boolean			Info;
GrafPtr			myPort;
ModalFilterUPP	MyFilterProDesc;
Ptr				tempPtr;
sData			*curData;

	UpdateALLWindow();

	GetPort( &myPort);
	
	DoPause();
	
	if( !GetIns( &ins, &samp))
	{
		Erreur( 13, ins);
		return;
	}
	
	RESTART:
	
	RecordCounter++;
	NTStr( (short) 2, (short) RecordCounter, (Ptr) &theString);	MyC2PStr( (Ptr) theString);
	pStrcpy( instruNom, "\pRecording ");
	pStrcat( instruNom, theString);
	
	curMusic->hasChanged = true;
	
	Append = false;
	newSound = 0L;
	
	MADPurgeTrack( MADDriver);
	iErr = ActiveSoundInput( true, &newSound, instruNom);
	
	if( SpectrumMicrophone == false && OscilloMicrophone == false) MicroOff();
	
	if( iErr == noErr || iErr == -999)
	{
		unsigned long	rate;
		long			bFreq;
		Boolean			stereo;
		
		SaveUndo( USample, ins, "\pUndo 'Record sample'");
		
		HLock( newSound);
		newSound = NSndToHandle( newSound, &lS, &lE, &sS, &rate, &bFreq, &stereo);
		HLock( newSound);
		
		inOutBytes = GetHandleSize( newSound);
		
		if( sS == 8) ConvertInstrument( (Byte*) *newSound, inOutBytes);
		else ConvertInstrument16( (short*) *newSound, inOutBytes);
		
		if( iErr == -999) samp = -1;	// ADD IT !
		
		if( samp < 0)
		{
			samp = curMusic->fid[ ins].numSamples;
		//	if( samp >= MAXSAMPLE) return;
			
		//	curMusic->fid[ ins].numSamples++;
		//	curMusic->header->numSamples++;
			
			MADCreateSample( curMusic, ins, samp);
		}
		
		SetSampName( ins, samp, instruNom);
		
		tempPtr = MyNewPtr( inOutBytes);
		if( tempPtr == 0L)
		{
			Erreur( 63, -2);
			HUnlock( newSound);
			DisposeHandle( newSound);
		}
		else
		{
			sData	*curData;
			
			BlockMoveData( *newSound, tempPtr, inOutBytes);
			// ICI: COLLER DANS LE FICHIER MOD !!!
			
			curData = curMusic->sample[ curMusic->fid[ ins].firstSample + samp];
			
			if( curData->data != 0L) MyDisposePtr( &curData->data);
			curData->data = tempPtr;
			
			HUnlock( newSound);
			DisposeHandle( newSound);
			
			curData->size	 		= inOutBytes;
			curData->loopBeg 		= lS;
			curData->loopSize	 	= lE - lS;
			curData->vol	 		= 64;
			curData->amp	 		= sS;
			curData->c2spd			= rate>>16L;
			curData->relNote		= 60 - bFreq;
			curData->stereo			= stereo;
		}
	}
	
	if( iErr == userCanceledErr)
	{
	
	}
	
	CreateInstruList();
	DrawInfoInstrument();
	UpdateSampleWindows();
	UpdateInstruMenu();

	NSelectInstruList( ins, samp);
	
	if( iErr == -999)
	{
		samp = -1;
		goto RESTART;
	}
	
	CloseSoundInput();
	
	SetPort( myPort);
}

void CopyResource( OSType type, short ID, short newID)
{
Handle	hRsrc;

	hRsrc = GetResource( type, ID);			DetachResource( hRsrc);
	AddResource( hRsrc, type, newID, "\p");	WriteResource( hRsrc);
	DetachResource( hRsrc);					MyDisposHandle( & hRsrc);
}

void SaveMOD( Boolean	SaveAS, OSType theType)
{
OSErr					iErr;
short					fRefNum,i;
long					inOutCount, sndSize, fileSize, OffSetToSample, numPat;
FInfo					fndrInfo;
Ptr						outFile;
Str255					fileName, bStr, cStr;
FSSpec					spec;

	if( theType != 'MADK' && theType != 'MADC') if( CallPlug( 0)) return;
	
	#ifndef DEMO
	if( SaveAS == true || PatchSave == true)
	{
		pStrcpy( fileName, curMusic->musicFileName);
		
		if( thePrefs.addExtension)
		{
			for( i = fileName[ 0]; i > 0 ; i--)
			{
				if( fileName[ i] == '.')
				{
					fileName[ 0] = i-1;
					i = 0;
				}
			}
			
			OSType2Str( theType, cStr);
			
			pStrcat( fileName, "\p.");
			pStrcat( fileName, cStr);
		}
		
		pStrcpy( bStr, "\pSave as ");
		
		OSType2Str( theType, cStr);
		
		pStrcat( bStr, "\p('");
		pStrcat( bStr, cStr);
		pStrcat( bStr, "\p') :");
		
		if( theType != 'MPG4')
		{
			if( DoCustomSave( bStr, fileName, theType, &spec)) return;
		}
		else
		{
			pStrcpy( spec.name, fileName);
		}
		
		if( theType != 'AIFF' | theType != 'MPG4')
			curMusic->hasChanged = false;
		
		ExportFile( theType, &spec);
		
		if( OpenableFile( theType, &spec))
		{
			if( theType != 'sTAT' && theType != 'STCf')
			{
				AddMODList( true, spec.name, spec.vRefNum, spec.parID);
				
			}
			if( theType == 'MADK') ImportFile( spec.name, spec.vRefNum, spec.parID, 'MADK');
		}
	}
	else
	{
		FSSpec	fss;
	
		fss.vRefNum = curvRefNum;
		fss.parID 	= curparID;
		pStrcpy( fss.name, curMusic->musicFileName);
		
		ExportFile( theType, &fss);
		
		FSpGetFInfo( &fss, &fndrInfo);
		fndrInfo.fdCreator = 'SNPL';	fndrInfo.fdType = theType;
		FSpSetFInfo( &fss, &fndrInfo);
		
		curMusic->hasChanged = false;
	}
	
	PatchSave = false;
	
	#endif
}

OSErr NOpenSampleInt( short ins, short samp, FSSpec sfFile)
{
short				i, iFileRefI;
OSErr				iErr;
FInfo				fndrInfo;

	SetCursor( &watchCrsr);
	
	iErr = FSpGetFInfo( &sfFile, &fndrInfo);
	
	switch( fndrInfo.fdType)
	{
		case 'INso':	// Instrument List
			OpenInstrumentsList( &sfFile);
		break;
		
		default:
			iErr = PPINTestFile( fndrInfo.fdType, &sfFile);
			if( iErr == noErr)
			{
				iErr = PPINImportFile( fndrInfo.fdType, ins, &samp, &sfFile);
			}
			else	// Try to identify file type
			{
				OSType type;
				
				iErr = PPINIdentifyFile( &type, &sfFile);
				if( iErr == noErr)
				{
					iErr = PPINImportFile( type, ins, &samp, &sfFile);
					
					if( iErr == noErr)
					{
						if( FSpSetFInfo( &sfFile, &fndrInfo) == noErr)		// write permission?
						{
							if( InfoL( 60))
							{
								fndrInfo.fdType		= type;
								fndrInfo.fdCreator	= 'SNPL';
								
								FSpSetFInfo( &sfFile, &fndrInfo);
							}
						}
					}
				}
			}
		break;
	}
	SetCursor( GetQDGlobalsArrow( &qdarrow));
	
	return iErr;
}

void NPASTESample( long Pos, short ins, short samp)
{
short				itemType,fRefNum, sS, qual;
long				inOutBytes, iL, scrapOffset, lCntOrErr, totalSize, lS, lE, i;
Handle				newSound, tempHandle;
Rect				itemRect;
Str255				theString;
OSErr				iErr, anErr;
Ptr					finalPtr;
Boolean				Info;
Movie				aMovie;
ScrapRef			scrap;
ScrapFlavorFlags	flags;

	SetCursor( &watchCrsr);

	aMovie = NewMovieFromScrap( 0);
	
	if( aMovie)
	{
		FSSpec	newFile;
		
		pStrcpy( newFile.name, "\pPasted sound");
		iErr = FindFolder( kOnSystemDisk, kTemporaryFolderType, kCreateFolder, &newFile.vRefNum, &newFile.parID);
		if( iErr) MyDebugStr( __LINE__, __FILE__, "FindFolder");
		
		FSpDelete( &newFile);
		
		iErr = ConvertMovieToFile( aMovie, nil, &newFile, 'WAVE', 'TVOD', 0, nil, 0, nil);
		if( iErr) MyDebugStr( __LINE__, __FILE__, "ConvertMovieToFile");
		
		iErr = NOpenSampleInt( ins, samp, newFile);
		if( iErr) MyDebugStr( __LINE__, __FILE__, "NOpenSampleInt");
		
		FSpDelete( &newFile);
		
		DisposeMovie( aMovie);
	}
	else
	{
		if( samp < 0) samp = curMusic->fid[ ins].numSamples;
		
		#if MACOS9VERSION
		lCntOrErr = GetScrap( 0L, 'MINo', &scrapOffset);
		#else
		lCntOrErr = 0;
		anErr = GetCurrentScrap( &scrap);
		anErr = GetScrapFlavorFlags( scrap, 'MINo', &flags);
		if( anErr == noErr) GetScrapFlavorSize( scrap, 'MINo', &lCntOrErr);
		#endif

		if( lCntOrErr > 0)
		{
			long	inOutCount;
			short	x;
		
			MADKillInstrument( curMusic, ins);
			
			newSound = MyNewHandle( lCntOrErr);
			if( newSound)
			{
				short numSamples;
				
				#if MACOS9VERSION
				GetScrap( newSound, 'MINo', &scrapOffset);
				#else
				HLock( newSound);
				GetScrapFlavorData( scrap, 'MINo', &lCntOrErr, *newSound);
				HUnlock( newSound);
				#endif
				
				HLock( newSound);
				
				i = curMusic->fid[ ins].firstSample;
				BlockMoveData( *newSound, &curMusic->fid[ ins], sizeof( InstrData));
				curMusic->fid[ ins].firstSample = i;
				
				numSamples = curMusic->fid[ ins].numSamples;
				curMusic->fid[ ins].numSamples = 0;
				
				inOutCount = sizeof( InstrData);
				
				for( x = 0; x < numSamples; x++)
				{
					sData	*curData;
					
					curData = MADCreateSample( curMusic, ins, x);
					
					BlockMoveData( (*newSound) + inOutCount, curData, sizeof( sData));
					inOutCount += sizeof( sData);
					
					curData->data = MyNewPtr( curData->size);
					if( curData->data)
					{
						BlockMoveData( (*newSound) + inOutCount, curData->data, curData->size);
						inOutCount += curData->size;
					}
					else Erreur( 63, MemError());
				}
				
				HUnlock( newSound);
				DisposeHandle( newSound);
			}
			else Erreur( 63, MemError());
			
			// ***********INSTRU NAME, 'STR '
			newSound = MyNewHandle( 60);
			if( newSound)
			{
				#if MACOS9VERSION
				lCntOrErr = GetScrap( newSound, 'STR ', &scrapOffset);
				#else
				lCntOrErr = 60;
				HLock( newSound);
				GetScrapFlavorData( scrap, 'STR ', &lCntOrErr, *newSound);
				HUnlock( newSound);
				#endif
				
				if( lCntOrErr > 0)
				{
					HLock( newSound);
					
					SetInstruName( ins, (unsigned char *) *newSound);
					
					HUnlock( newSound);
				}
				DisposeHandle( newSound);
			}
			
			curMusic->hasChanged = true;
		}
		else
		{	
			// ************  SON 'snd '
			
			#if MACOS9VERSION
			lCntOrErr = GetScrap( 0L, 'snd ', &scrapOffset);
			#else
			lCntOrErr = 0;
			anErr = GetCurrentScrap( &scrap);
			anErr = GetScrapFlavorFlags( scrap, 'snd ', &flags);
			if( anErr == noErr) GetScrapFlavorSize( scrap, 'snd ', &lCntOrErr);
			#endif

			if( lCntOrErr > 0)
			{
				sData			*curData;
				unsigned long	rate;
				long			bFreq;
				Boolean			stereo;
				Boolean			newSampleData = false;
				
				newSound = MyNewHandle( lCntOrErr);
				if( newSound)
				{
					/* Check if it is necessary to create the sData struct */
					if( samp >= curMusic->fid[ ins].numSamples)
					{
					//	if( samp >= MAXSAMPLE) return;
						samp = curMusic->fid[ ins].numSamples;
						
						MADCreateSample( curMusic, ins, samp);
						
						newSampleData = true;
					}
					curData = curMusic->sample[ curMusic->fid[ ins].firstSample + samp];
					/********************************************************/
					
					if( curData->size == 0)
					{
						newSampleData = true;
					}
					
					#if MACOS9VERSION
					GetScrap( newSound, 'snd ', &scrapOffset);
					#else
					HLock( newSound);
					GetScrapFlavorData( scrap, 'snd ', &lCntOrErr, *newSound);
					HUnlock( newSound);
					#endif
					
					HLock( newSound);
					NSndToHandle( newSound, &lS, &lE, &sS, &rate, &bFreq, &stereo);
					HLock( newSound);
					
					inOutBytes = GetHandleSize( newSound);
					
					if( sS == 8) ConvertInstrumentIn( (Byte*) *newSound, inOutBytes);
					else ConvertInstrumentIn16( (short*) *newSound, inOutBytes);
					
					if( !newSampleData)		// We NEED to convert the pasted sound in current data
					{
						if( curData->amp != sS)		// Amplitude conversion
						{
							switch( sS)
							{
								case 8:		// Convert in 16 bits
									HUnlock( newSound);
									SetHandleSize( newSound, inOutBytes * 2L);
									HLock( newSound);
									
									if( MemError() == noErr)
									{
										Convert8to16(	*newSound,	*newSound, 	inOutBytes);
										
										sS = 16;
										inOutBytes *= 2L;
										lS *= 2L;
										lE *= 2L;
									}
								break;
								
								case 16:	// Convert in 8 bits
									Convert16to8(	*newSound, *newSound, inOutBytes);
									
									HUnlock( newSound);
									SetHandleSize( newSound, inOutBytes / 2L);
									HLock( newSound);
									
									inOutBytes /= 2L;
									lS /= 2L;
									lE /= 2L;
									sS = 8;
								break;
							}
						}
						
						if( curData->stereo != stereo)		// Stereo conversion
						{
							switch( stereo)
							{
								case true:		// Convert in mono
									if( sS == 8)
									{
										for( i = 0 ; i < inOutBytes; i+=2) (*newSound)[ i / 2] = ((long) (*newSound)[ i] + (long) (*newSound)[ i + 1]) / 2L;
									}
									else
									{
										short *short16out = (short*) *newSound, *short16in = (short*) *newSound;
										
										for( i = 0 ; i < inOutBytes/2; i+=2) (*newSound)[ i / 2] = ((long) (*newSound)[ i] + (long) (*newSound)[ i + 1]) / 2L;
									}
									
									HUnlock( newSound);
									SetHandleSize( newSound, inOutBytes / 2L);
									HLock( newSound);
									
									inOutBytes /= 2L;
									lS /= 2L;
									lE /= 2L;
									stereo = false;
								break;
								
								case false:		// Convert in stereo
									HUnlock( newSound);
									SetHandleSize( newSound, inOutBytes * 2L);
									HLock( newSound);
									
									if( MemError() == noErr)
									{
										if( sS == 8)
										{
											for( i = inOutBytes-1 ; i >= 0; i--)
											{
												if( 1 + i * 2 >= inOutBytes*2L) MyDebugStr( __LINE__, __FILE__, "");
												
												(*newSound)[ i * 2] = (*newSound)[ 1 + i * 2] = (*newSound)[ i];
											}
										}
										else
										{
											short *short16out = (short*) *newSound, *short16in = (short*) *newSound;
											
											for( i = inOutBytes/2 -1 ; i >= 0; i--)
											{
												short16out[ i * 2] = short16out[ 1 + i * 2] = short16in[ i];
											}
										}
										
										inOutBytes *= 2L;
										lS *= 2L;
										lE *= 2L;
										stereo = true;
									}
								break;
							}
						}
					}
					
					curData->amp 		= sS;
					curData->loopBeg 	= lS + Pos;
					curData->loopSize	= lE - lS;
					curData->c2spd		= rate>>16L;
					curData->relNote	= 60 - bFreq;
					curData->stereo		= stereo;
					
					totalSize = curData->size;
					totalSize += inOutBytes;
					
					// ICI: COLLER DANS LE SAMPLE !!!
					
					finalPtr = MyNewPtr( totalSize);
					if( finalPtr)
					{
						BlockMoveData(	curData->data,
									finalPtr,
									Pos);
									
						BlockMoveData(	*newSound,
									finalPtr + Pos,
									inOutBytes);
						
						BlockMoveData(	curData->data + Pos,
									finalPtr + Pos + inOutBytes,
									curData->size - Pos);
						
						if( curData->data != 0L) MyDisposePtr( &curData->data);
						curData->data = finalPtr;
						curData->size = totalSize;
					}
					else
					{
						Erreur( 63, MemError());
						MADKillSample( curMusic, ins, samp);
					}
					
					HUnlock( newSound);
					DisposeHandle( newSound);
				}
				else Erreur( 63, MemError());
				
				curMusic->hasChanged = true;
				
				// ***********INSTRU NAME, 'STR '
				if( Pos == 0L)
				{
					newSound = MyNewHandle( 60);
					if( newSound)
					{
						#if MACOS9VERSION
						lCntOrErr = GetScrap( newSound, 'STR ', &scrapOffset);
						#else
						lCntOrErr = 60;
						HLock( newSound);
						GetScrapFlavorData( scrap, 'STR ', &lCntOrErr, *newSound);
						HUnlock( newSound);
						#endif
						
						if( lCntOrErr > 0)
						{
							HLock( newSound);
							
							SetSampName( ins, samp, (unsigned char *) *newSound);
							
							HUnlock( newSound);
						}
						DisposeHandle( newSound);
					}
				}
				
				SetSelection( ins, Pos, Pos + inOutBytes);
			}
		}
	}
	CreateInstruList();
	DrawInfoInstrument();
	UpdateSampleWindows();
	UpdateInstruMenu();
	
	SetCursor( GetQDGlobalsArrow( &qdarrow));
}

void NCOPYSample( long start, long length, short ins, short samp)
{
short		itemType,i,fRefNum, numPat;
long		inOutBytes, iL, OffSetToSample;
OSErr		iErr;
Boolean		Info;
Handle 		theSound;
short		temp;
sData		*curData;
ScrapRef	scrap;
OSErr		anErr;

	SetCursor( &watchCrsr);
	
	#if MACOS9VERSION
	ZeroScrap();
	#else
	anErr = ClearCurrentScrap();
	anErr = GetCurrentScrap( &scrap);
	#endif
	
	/**************************/
	// Copy an instrument
	/**************************/
	if( samp < 0)
	{
		long 	inOutCount;
		short	x;
	
		// COPY Instru Name
		
		theSound = MyNewHandle( 200);
		if( theSound)
		{
			strcpy( (Ptr) *theSound, curMusic->fid[ ins].name);
			MyC2PStr( (Ptr) *theSound);
		
			HLock( (Handle) theSound );
			
			#if MACOS9VERSION
			PutScrap( (long) 30, 'STR ', (Ptr) *theSound);
			#else
			anErr = PutScrapFlavor( scrap, 'STR ', 0, (long) 30, (Ptr) *theSound);
			#endif
			
			HUnlock( (Handle) theSound );
			
			MyDisposHandle( & theSound);
		}
		
		// COPY 'MINo' data
		
		inOutCount = sizeof( InstrData);
		for( x = 0; x < curMusic->fid[ ins].numSamples; x++)
		{
			sData	*curData = curMusic->sample[ curMusic->fid[ ins].firstSample + x];
			
			inOutCount += sizeof( sData);
			inOutCount += curData->size;
		}
		
		theSound = MyNewHandle( inOutCount);
		if( theSound)
		{
			HLock( theSound);
			
			BlockMoveData( &curMusic->fid[ ins], *theSound, sizeof( InstrData));
			inOutCount = sizeof( InstrData);
			
			for( x = 0; x < curMusic->fid[ ins].numSamples; x++)
			{
				sData	*curData;
				
				curData = curMusic->sample[ curMusic->fid[ ins].firstSample + x];
				
				BlockMoveData( curData, (*theSound) + inOutCount, sizeof( sData));
				inOutCount += sizeof( sData);
				
				BlockMoveData( curData->data, (*theSound) + inOutCount, curData->size);
				inOutCount += curData->size;
			}
			
			#if MACOS9VERSION
			PutScrap( inOutCount, 'MINo', (Ptr) *theSound);
			#else
			anErr = PutScrapFlavor( scrap, 'MINo', 0, inOutCount, (Ptr) *theSound);
			#endif
			HUnlock( theSound);
			MyDisposHandle( &theSound);
		}
		else
		{
			Erreur( 63, MemError());
			#if MACOS9VERSION
			ZeroScrap();
			#else
			anErr = ClearCurrentScrap();
			#endif
		}
	}
	else
	{
		/**************************/
		// Copy a sample
		/**************************/
		
		curData = curMusic->sample[ curMusic->fid[ ins].firstSample + samp];
		
		inOutBytes = curData->size;
		if( inOutBytes <= 2) return;
		
		if( length == -1) length = inOutBytes;
		if( start + length > inOutBytes) length = inOutBytes - start;
		
		// COPY Sample Name
		
		theSound = MyNewHandle( 200);
		if( theSound)
		{
			strcpy( (Ptr) *theSound, curData->name);
			MyC2PStr( (Ptr) *theSound);
		
			HLock( theSound );
			
			#if MACOS9VERSION
			PutScrap( (long) 30, 'STR ', (Ptr) *theSound);
			#else
			anErr = PutScrapFlavor( scrap, 'STR ', 0, 30, (Ptr) *theSound);
			#endif
	
			HUnlock( theSound );
			
			MyDisposHandle( & theSound);
		}
		// COPY 'snd '
		
		theSound = MyNewHandle( length + 5000L);
		if( theSound)
		{
			short	numChan;
			
			if( curData->stereo) numChan = 2;
			else numChan = 1;
			
			SetupSndHeader(	(SndListHandle) theSound,
							numChan,
							((unsigned long) curData->c2spd)<<16L,
							curData->amp,
							'NONE',
							60 - curData->relNote,
							length,
							&temp);
			
			SetHandleSize( theSound, length + temp);
			
			if( length == curData->size)
			{
				AddLoopToSndHandle(	theSound,
									curData->loopBeg,
									curData->loopBeg + curData->loopSize);
			}	
			
			HLock( theSound);
			
			BlockMoveData( curData->data + start, (Ptr) ((*theSound) + temp), length);
			
			if( curData->amp == 8) ConvertInstrument( (Byte*) (*theSound + temp), length);
			else ConvertInstrumentOut16( (short*) (*theSound + temp), length);
			
			#if MACOS9VERSION
			PutScrap( GetHandleSize( (Handle) theSound), 'snd ', (Ptr) *theSound);
			#else
			anErr = PutScrapFlavor( scrap, 'snd ', 0, GetHandleSize( (Handle) theSound), (Ptr) *theSound);
			#endif
			
			HUnlock( (Handle) theSound );
			
			MyDisposHandle( & theSound);
		}
		else
		{
			Erreur( 63, MemError());
			#if MACOS9VERSION
			ZeroScrap();
			#else
			anErr = ClearCurrentScrap();
			#endif
		}
	}
	SetCursor( GetQDGlobalsArrow( &qdarrow));
}

void AddLoopToSndHandle( Handle sound, long Start, long End)
{
	Ptr 			soundPtr;
	short 			soundFormat;
	short 			numSynths, numCmds;
	long 			offset;
	SoundHeaderPtr 	header;
	CmpSoundHeader	*CmpHeader;
	ExtSoundHeader	*ExtHeader;
	OSErr 			result;
	
	if( !sound) return;
	
	/* make the sound safe to use at interrupt time. */
	HLock( sound);
	soundPtr = *sound;
	
	/* determine what format sound we have. */
	soundFormat = *(short*)soundPtr;
	
	switch(soundFormat)
	{
		case 1:						/* format 1 sound. */
			/* look inside the format 1 resource and deduce offsets. */
			numSynths = ((short*)soundPtr)[1];					/* get # synths. */
			numCmds = *(short*)(soundPtr+4+numSynths*6);		/* get # commands. */
		break;
		
		case 2:						/* format 2 sound. */
			numSynths = 0;			/* format 2 sounds have no synth's. */
			numCmds = ((short*)soundPtr)[2];
		break;
		
		default:					/* jack says, what about 12? or 6? */
			MyDebugStr( __LINE__, __FILE__, " NSndToHandle... Burkk");
	} 

	/* compute address of sound header. */
	offset = 6 + 6*numSynths + 8*numCmds;
	header = (SoundHeaderPtr) (*sound + offset);
		
	switch( header->encode)
	{
		case cmpSH:
			CmpHeader = (CmpSoundHeader*) header;
			if( CmpHeader->sampleSize == 16) { Start /= 2; End /= 2; }
			if( CmpHeader->numChannels > 1) { Start /= CmpHeader->numChannels; End /= CmpHeader->numChannels; }
			
			CmpHeader->loopStart	= Start;
			CmpHeader->loopEnd		= End;
		break;

		case extSH:
			ExtHeader = (ExtSoundHeader*) header;
			if( ExtHeader->sampleSize == 16) { Start /= 2; End /= 2; }
			if( ExtHeader->numChannels > 1) { Start /= ExtHeader->numChannels; End /= ExtHeader->numChannels; }
			
			ExtHeader->loopStart	= Start;
			ExtHeader->loopEnd		= End;
		break;
		
		case stdSH:
			header->loopStart	= Start;
			header->loopEnd		= End;
		break;
	}
}

void NSaveSampleInt( short whichIns, short whichSamp, OSType SoundType, FSSpec *sFile)
{
	FSpDelete( sFile);
	
	PPINExportFile( SoundType, whichIns, whichSamp, sFile);
}

void SaveSample(void)
{
short					itemType,i;
long					inOutBytes, iL;
Handle					itemHandle, newSound;
Rect					itemRect;
Str255					theString, mainStr, bStr, cStr;
Point					where ={ -1, -1};
FSSpec					spec;
OSErr					iErr;
GrafPtr					myPort;
short					temp,fRefNum;

OSType					SoundFormat;
short					ins, samp;

	GetPort( &myPort);
	
	if( GetIns( &ins, &samp))
	{
		if( samp >= 0) pStrcpy( theString, "\pSave this sample as:");
		else pStrcpy( theString, "\pSave this instrument as:");
		
		if( NSoundQualityExportSnd( ins, samp, &SoundFormat, mainStr) == noErr)
		{
			if( DoCustomSave( theString, (unsigned char*) mainStr, SoundFormat, &spec)) return;
			
			iErr = HSetVol( 0L, spec.vRefNum, spec.parID);
			
			NSaveSampleInt( ins, samp, SoundFormat, &spec);
		}
	}
}

static	short UMindex = 1;

void CreateNewMOD(void)
{
FSSpec				spec;
Point				where = { -1, -1};
Str255				str, str2;

	NumToString( UMindex, str2);

	pStrcpy( str, "\pUntitled Music #");
	pStrcat( str, str2);
	
	if( DoCustomSave( "\pCreate new MADK File:", str, 'MADK', &spec)) return;
	
	UMindex++;
	
	ImportFile( spec.name, 128, 0, 'Rsrc');
	
	curvRefNum = spec.vRefNum;
	curparID = spec.parID;
	pStrcpy( curMusic->musicFileName, spec.name);
	
	HSetVol( 0L, spec.vRefNum, spec.parID);
	
	PatchSave = false;
	SaveMOD( false, 'MADK');
	
	AddMODList( 	true,
					curMusic->musicFileName,
					curvRefNum,
					curparID);
	
	ImportFile( spec.name, spec.vRefNum, spec.parID, 'MADK');
}

inline short Tdecode16( void *msg_buf)
{
  unsigned char *buf = (unsigned char*) msg_buf;
  
  return ( (short) buf[1] << 8) | ( (short) buf[0]);
}

sData* ComputeRAWSound( Ptr srcSound, long length)
{
	Ptr			destSound;
	sData		*curData;
	long		i, size;
	
	if( thePrefs.RAWEOF) size = length;
	else size = thePrefs.RAWLength;
	
	if( thePrefs.RAWHeader + size > length) size = length - thePrefs.RAWHeader;
	
	if( thePrefs.RAWBits == 16)
	{
		size /= 2;
		size *= 2;
	}
	
	destSound = NewPtr( size);
	
	if( destSound == 0L) return 0L;
	
	switch( thePrefs.RAWBits)
	{
		case 8:
			BlockMoveData( srcSound + thePrefs.RAWHeader, destSound, size);
			
			// Signed
			if( thePrefs.RAWSigned)
			{
				for( i = 0; i < size; i++)
				{
					destSound[ i] = 0x80 - destSound[ i];
				}
			}
		break;
		
		case 16:
		{
			short	*d16Sound = (short*) destSound;
			
			BlockMoveData( srcSound + thePrefs.RAWHeader, destSound, size);
			
			// Coding
			if( thePrefs.RAWLittleEndian)
			{
				for( i = 0; i < size/2; i++)
				{
					d16Sound[ i] = Tdecode16( &d16Sound[ i]);
				}
			}
			
			// Signed
			if( thePrefs.RAWSigned)
			{
				for( i = 0; i < size/2; i++)
				{
					d16Sound[ i] = 0x8000 - d16Sound[ i];
				}
			}
		}
		break;
	}
	
	curData = (sData*) NewPtrClear( sizeof( sData));
	if( curData)
	{
		curData->size		= size;
		curData->loopBeg	= 0L;
		curData->loopSize	= 0L;
		curData->vol		= MAX_VOLUME;
		curData->c2spd		= thePrefs.RAWRate;
		curData->loopType	= eClassicLoop;
		curData->amp		= thePrefs.RAWBits;
		curData->stereo		= thePrefs.RAWStereo;
		curData->relNote	= 0;
		// curData->name
		curData->data		= destSound;
	}
	else
	{
		DisposePtr( destSound);
		return 0L;
	}
	
	return curData;
}

OSErr RAWImportFile( FSSpec	*file)
{
	DialogPtr		aDialog;
	short			itemHit, itemType, ins, samp;
	char			cstr[ 200];
	Str255			aStr, theStr, bStr;
	Rect			iRect, itemRect;
	Point			myPt;
	Handle			itemHandle;
	long			mresult, temp, xL, x, i;
	Ptr				fileBuf;
	sData			*curData;
	short			sampNo, instNo;
	GrafPtr			thePort;
	MenuHandle		tempMenu;
	short			fRefNum;
	long			curEOF, inOutCount;
	OSErr			iErr;
	Boolean			needUpdateNow;
	Point			pt;
	
	if( !GetIns( &ins, &samp))
	{
		Erreur( 13, ins);
		return -1;
	}
	
	////////////
	
	iErr = FSpOpenDF( file, fsCurPerm, &fRefNum);
	if( iErr) return -1;
	
	iErr = GetEOF( fRefNum, &curEOF);
	
	fileBuf = NewPtr( curEOF*2);
	if( fileBuf == 0L)
	{
		Erreur( 63, iErr);
		return -1;
	}
	DisposePtr( fileBuf);
	fileBuf = NewPtr( curEOF);
	
	inOutCount = curEOF;
	iErr = FSRead(fRefNum, &inOutCount, fileBuf);
	
	GetPort( &thePort);
	
	SetCursor( GetQDGlobalsArrow( &qdarrow));
	
	aDialog = GetNewDialog( 178, 0L, (WindowPtr) -1L);
	SetPortDialogPort( aDialog);
	
	AutoPosition( aDialog);
	ShowWindow( GetDialogWindow( aDialog));
	SetWTitle( GetDialogWindow( aDialog), file->name);
	
	GetDialogItem( aDialog, 25, &itemType, &itemHandle, &iRect);
	
	// Header
	
	NumToString( thePrefs.RAWHeader, aStr);
	SetDText( aDialog, 17, aStr);
	
	thePrefs.RAWLength = curEOF - thePrefs.RAWHeader;
	NumToString( thePrefs.RAWLength, aStr);
	SetDText( aDialog, 20, aStr);

	// Rate
	
	NumToString( thePrefs.RAWRate, aStr);
	SetDText( aDialog, 24, aStr);

	REGODIA:
	
	// Bits
	
	if( thePrefs.RAWBits == 8) { TurnRadio( 8, aDialog, true);	TurnRadio( 9, aDialog, false);}
	else { TurnRadio( 9, aDialog, true);	TurnRadio( 8, aDialog, false);}
	
	// Mode
	
	if( thePrefs.RAWStereo) {TurnRadio( 11, aDialog, true);		TurnRadio( 10, aDialog, false);}
	else {TurnRadio( 10, aDialog, true);	TurnRadio( 11, aDialog, false);}
	
	// Type
	
	if( thePrefs.RAWSigned) {TurnRadio( 12, aDialog, true);		TurnRadio( 13, aDialog, false);}
	else {TurnRadio( 13, aDialog, true);		TurnRadio( 12, aDialog, false);}
	
	// Coding
	
	if( thePrefs.RAWLittleEndian) {TurnRadio( 15, aDialog, true);		TurnRadio( 16, aDialog, false);}
	else {TurnRadio( 16, aDialog, true);		TurnRadio( 15, aDialog, false);}
	
	// Length
	
	if( thePrefs.RAWEOF) {TurnRadio( 26, aDialog, true);		TurnRadio( 27, aDialog, false);}
	else {TurnRadio( 27, aDialog, true);		TurnRadio( 26, aDialog, false);}
	
	needUpdateNow = true;
	
	do
	{
		MyModalDialog( aDialog, &itemHit);
		
		switch( itemHit)
		{
			case 33:
			case -updateEvt:
				needUpdateNow = true;
			
			case -5:
				if( needUpdateNow)
				{
					GWorldPtr	theGWorld = 0L;
					GDHandle	oldGDeviceH;
					CGrafPtr	oldPort;
					
					SetPortDialogPort( aDialog);
					
					needUpdateNow = false;
					
					iErr = noErr;
					
					GetDText( aDialog, 17, (StringPtr) &theStr);
					StringToNum( theStr, &mresult);
					if( mresult >= 0 && mresult < curEOF) thePrefs.RAWHeader = mresult;
					else iErr = -1;
					
					GetDText( aDialog, 20, (StringPtr) &theStr);
					StringToNum( theStr, &mresult);
					if( mresult >= 0 && mresult < curEOF - thePrefs.RAWHeader) thePrefs.RAWLength = mresult;
					else thePrefs.RAWLength = curEOF - thePrefs.RAWHeader;
					
					GetDText( aDialog, 24, theStr);
					StringToNum( theStr, &x);
					thePrefs.RAWRate =  x;
					
					if( iErr == noErr)
					{
						ControlSwitch( 1, aDialog, 0);
						ControlSwitch( 33, aDialog, 0);
						
						curData = ComputeRAWSound( fileBuf, curEOF);
						if( curData)
						{
							GetGWorld( &oldPort, &oldGDeviceH);
							theGWorld = 0L;
							NewGWorld( &theGWorld, 8, &iRect, nil, nil, (GWorldFlags) 0);
							LockPixels( GetPortPixMap( theGWorld));
							
							InsetRect( &iRect, -1, -1);
							FrameRect( &iRect);
							InsetRect( &iRect, 1, 1);
							
							SetGWorld( theGWorld, 0L);
							
							PaintRect( &iRect);
							
							ForeColor( whiteColor);
							
							if( curData->stereo)
							{
								ForeColor( blueColor);
								DrawSampleInt(	curData->size,
											0,
											0,
											iRect.right - iRect.left,
											iRect.bottom - iRect.top,
											iRect.right - iRect.left,
											iRect.top,
											iRect.left,
											1,
											curData);
							}
							
							ForeColor( redColor);
							DrawSampleInt(	curData->size,
											0,
											0,
											iRect.right - iRect.left,
											iRect.bottom - iRect.top,
											iRect.right - iRect.left,
											iRect.top,
											iRect.left,
											0,
											curData);
											
							ForeColor( whiteColor);
							
							SetGWorld( oldPort, oldGDeviceH);
				 	
							CopyBits(		(BitMap*) *GetPortPixMap( theGWorld),
				 							(BitMap*) *GetPortPixMap(GetDialogPort( aDialog)),
				 							&iRect,
				 							&iRect,
				 							srcCopy,
				 							0L);
				 							
				 			UnlockPixels( GetPortPixMap( theGWorld));
							DisposeGWorld( theGWorld);
						}
						
						if( itemHit == 33)
						{
							Boolean		continueLoop;
							KeyMap		km;
							
							GetDText( aDialog, 24, theStr);
							StringToNum( theStr, &x);
							curData->c2spd = thePrefs.RAWRate =  x;
							
							MADPlaySoundData( MADDriver, curData->data, curData->size, 0, 48 + curData->relNote, curData->amp, 0, 0, curData->c2spd << 16, curData->stereo);
							if( iErr == noErr)
							{
								SetCursor( &PlayCrsr);
								continueLoop = true;
								while( continueLoop)
								{
									GetKeys( km);
									
									if( MADDriver->chan[ 0].samplePtr == 0L) continueLoop = false;
									if( MADIsPressed( (unsigned char*) km, 0x37) && MADIsPressed( (unsigned char*) km, 0x2F)) continueLoop = false;
									if( Button()) continueLoop = false;
									DoGlobalNull();
								}
								
								SetCursor( GetQDGlobalsArrow( &qdarrow));
								
								if( MADDriver->chan[ 0].samplePtr != 0L)
								{
									MADDriver->chan[ 0].curPtr 		= MADDriver->chan[ 0].maxPtr;
									MADDriver->chan[ 0].samplePtr	= 0L;
									MADDriver->chan[ 0].lAC			= 0;
									MADDriver->chan[ 0].loopBeg 	= 0;
									MADDriver->chan[ 0].loopSize 	= 0;
								}
							}
						}
						
						DisposePtr( curData->data);
						DisposePtr( (Ptr) curData);
					}
					else
					{
						ControlSwitch( 1, aDialog, 255);
						ControlSwitch( 33, aDialog, 255);
						
						PaintRect( &iRect);
						
						ForeColor( redColor);
						
						pStrcpy( aStr, "\pWrong Settings - Out of range");
						itemType = StringWidth( aStr);
						
						MoveTo( iRect.left + (iRect.right - iRect.left)/2 - itemType/2,
								iRect.top + (iRect.bottom - iRect.top)/2);
						DrawString( aStr);
						ForeColor( blackColor);
					}
					
					if( thePrefs.RAWEOF) NumToString( curEOF, theStr);
					else NumToString( thePrefs.RAWLength, theStr);
					pStrcat( theStr, "\p Bytes of ");
					NumToString( curEOF, bStr);
					pStrcat( theStr, bStr);
					pStrcat( theStr, "\p Bytes");
					
					SetDText( aDialog, 31, theStr);
				}
				
				if( iErr == noErr)
				{
					GetMouse( &pt);
					
					if( PtInRect( pt, &iRect))
					{
						long	size;
						
						if( thePrefs.RAWEOF) size = curEOF;
						else size = thePrefs.RAWLength;
						
						if( thePrefs.RAWHeader + size > curEOF) size = curEOF - thePrefs.RAWHeader;
						
						i = (long) thePrefs.RAWHeader + ((long) size * (long) (pt.h - iRect.left)) / (long) (iRect.right - iRect.left);
						
						pStrcpy( theStr, "\pPos: ");
						
						NumToString( i, bStr);
						pStrcat( theStr, bStr);
						
						SetDText( aDialog, 32, theStr);
					}
					else SetDText( aDialog, 32, "\p");
				}
				else
				{
					SetDText( aDialog, 32, "\p");
					SetDText( aDialog, 31, "\p");
				}
			break;
			
			case 20:	goto REGODIA;	break;
			case 17:	goto REGODIA;	break;
			
			case 8:		thePrefs.RAWBits = 8;	goto REGODIA;	break;
			case 9:		thePrefs.RAWBits = 16;	goto REGODIA;	break;
			
			case 10:	thePrefs.RAWStereo = false;	goto REGODIA;	break;
			case 11:	thePrefs.RAWStereo = true;	goto REGODIA;	break;
			
			case 12:	thePrefs.RAWSigned = true;	goto REGODIA;	break;
			case 13:	thePrefs.RAWSigned = false;	goto REGODIA;	break;
			
			case 15:	thePrefs.RAWLittleEndian = true;	goto REGODIA;	break;
			case 16:	thePrefs.RAWLittleEndian = false;	goto REGODIA;	break;
			
			case 26:	thePrefs.RAWEOF = true;		goto REGODIA;	break;
			case 27:	thePrefs.RAWEOF = false;	goto REGODIA;	break;
			
			case 29:
				tempMenu = GetMenu( 163);
				
				InsertMenu( tempMenu, hierMenu );
				GetDialogItem( aDialog, itemHit, &itemType, &itemHandle, &itemRect);
				
				myPt.v = itemRect.top;	myPt.h = itemRect.left;
				LocalToGlobal( &myPt);
				
				GetDText( aDialog, 24, theStr);
				for( i = 1; i <= CountMenuItems( tempMenu); i++)
				{
					GetItem( tempMenu, i, aStr);
					aStr[ 0] = theStr[ 0];
					if( EqualString( theStr, aStr, false, false)) break;
				}
				
				if( i <= CountMenuItems( tempMenu)) SetItemMark( tempMenu, i, 0xa5);
				else i = 1;
				
				mresult = PopUpMenuSelect(	tempMenu,
											myPt.v,
											myPt.h,
											i);
				
				if( i <= CountMenuItems( tempMenu)) SetItemMark( tempMenu, i, 0);
				
				if ( HiWord(mresult ) != 0 )
				{
					long	tempL;
					
					GetItem( tempMenu, LoWord( mresult), theStr);
					for( i = 1; i <= theStr[ 0]; i++)
					{
						if( theStr[ i] == ' ') break;
					}
					theStr[ 0] = i-1;
					
					StringToNum( theStr, &tempL);
					thePrefs.RAWRate =  tempL;
					
					SetDText( aDialog, 24, theStr);
				}
				DeleteMenu( GetMenuID( tempMenu));
				DisposeMenu( tempMenu);
				
				goto REGODIA;
			break;
		}
		
	}while( itemHit != 1 && itemHit != 2);
	
	if( itemHit == 1)
	{
		curData = ComputeRAWSound( fileBuf, curEOF);
		
		if( samp < 0)
		{
			samp = curMusic->fid[ ins].numSamples;
			curMusic->fid[ ins].numSamples++;
		}
		else
		{
			if( curMusic->sample[ ins * MAXSAMPLE + samp])
			{
				DisposePtr( curMusic->sample[ ins * MAXSAMPLE + samp]->data);
				DisposePtr( (Ptr) curMusic->sample[ ins * MAXSAMPLE + samp]);
			}
		}
		
		curMusic->sample[ ins * MAXSAMPLE + samp] = curData;
		
		SetSampName( ins, samp, file->name);
		
		CreateInstruList();
		DrawInfoInstrument();
		UpdateSampleWindows();
		UpdateInstruMenu();
		
		NSelectInstruList( ins, samp);
	}
	
	ENDDialog:
	
	DisposDialog( aDialog);
	
	UpdateALLWindow();
	
	SetPort( thePort);
	
	iErr = FSClose( fRefNum);
	
	if( itemHit == 1) return noErr;
	else return -1;
}