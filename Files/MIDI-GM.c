#include "Shuddup.h"
#include "MAD.h"
#include "RDriver.h"
#include "RDriverInt.h"
#include "Folders.h"
#include "undo.h"
#include "QuickTimeMusic.h"
#include "Movies.h"

#include "dlsmac.h"
#include "dls.h"
#include "dls2.h"

typedef unsigned short	WORD;
typedef unsigned long	DWORD;
typedef long			FOURCC;

typedef struct{
	Str255	name;
	Str32	smpls;
	Str32	dataBits;
	Str32	stereo;
}InfoRec,*InfoPtr,**InfoHnd;

typedef struct _MMCKINFO{
	FOURCC		ckid;
	DWORD		cksize;
	FOURCC		fccType;
	DWORD		dwDataOffset;
	WORD		dwFlags;
}MMCKINFO;

typedef struct waveformat_tag{

	WORD		wFormatTag;
	WORD		nCannels;
	DWORD		nSamplesPerSec;
	DWORD		nAvgBytesPerSec;
	WORD		nBlockAlign;
	WORD		wBitsPerSample;
	
}WAVEFORMAT;


typedef struct _CK{
	FOURCC		ckid;
	DWORD		cksize;
}CK;

typedef struct _MyAtom{
	long		pos;
	long		id;
	long		size;
	short		ref;
}MyAtom;


void ConvertInstrument( register	Byte	*tempPtr,	register long sSize);
void ConvertInstrumentIn( register	Byte	*tempPtr,	register long sSize);
short OpenDataFileQK( long dirID, short VRefNum);

static NoteRequest					myNoteRequest;


		Boolean			QuicktimeInstruAvailable;
static	Boolean			QK50;
static	short			vers1, vers2;
/*___________________ long byte swap for Intell <-> Motorola Conversions*/

unsigned long longswap(unsigned long ul)
{
	return (ul >> 24) | ((ul >> 8) & 0xff00) | ((ul << 8) & 0xff0000) | (ul << 24);
}

/*___________________ word byte swap for Intell <-> Motorola Conversions*/

unsigned short shrtswap(unsigned int us)
{
	return ((us >> 8) | (us << 8)) & 0xffff;
}

void InitQuicktimeInstruments(void)
{
	long					lS, lE, inOutBytes, result;
	short					sS, qQ;
	short 					i;
	short					foundVRefNum, vRefNum, iFileRef;
	long					foundDirID, dirID;
	OSErr					iErr;

	HGetVol( 0L, &vRefNum, &dirID);

	QuicktimeInstruAvailable = false;
	
	iErr = FindFolder( kOnSystemDisk, kExtensionFolderType, kDontCreateFolder, &foundVRefNum, &foundDirID);
	if( iErr == noErr)
	{
		HSetVol( 0L, foundVRefNum, foundDirID);	
		
		iFileRef = OpenDataFileQK( foundDirID, foundVRefNum);
		if( iFileRef != -1)
		{
			QuicktimeInstruAvailable = true;
			
			FSClose( iFileRef);
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
	
	HSetVol( 0L, vRefNum, dirID);
}

void OctavesMIDIName(short	id, Str255	String)
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

void SetInstruNameM( short	theNo, Str255 theNewName, short MIDIgm, Ptr destName)
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
	
	for( i=31; i >= 0; i--)
	{
		if( destName[ i] == 0x20 || destName[ i] == '\0') destName[ i] = '\0';
		else break;
	}
}

void SetSampNameM( Str255 theNewName, Ptr destName)
{
short	i;

	for(i=0; i<32; i++)
	{
		if( i < theNewName[ 0]) destName[i] = theNewName[i+1];
		else destName[i] = '\0';
	}
}

#include "CFBundle.h"

short GenerateDLSFromBundle()
{
#if CALL_NOT_IN_CARBON
return -1;
#else
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

bundleURL = CFURLCreateFromFSRef( 0L, &bundleFSRef);

AudioBundle = CFBundleCreate( 0L, bundleURL);
if( AudioBundle == 0L) Debugger();


// MacOS X 10.2

rsrcURL = 		CFBundleCopyResourceURL( AudioBundle, 
           		CFSTR("gs_instruments"), 			//CoreAudio
              	CFSTR("dls"),	 					//rsrc
               	NULL);
                	
CFURLGetFSRef( rsrcURL, &rsrcRef);
   
iErr = FSOpenFork( &rsrcRef, 0L, 0L, fsRdPerm, &refNum);

if( iErr == noErr) return refNum;

// Look for a resource in the main bundle by name and type.
rsrcURL = 		CFBundleCopyResourceURL( AudioBundle, 
                CFSTR("CoreAudio"), 			//CoreAudio
                CFSTR("rsrc"), 					//rsrc
                NULL);

CFURLGetFSRef( rsrcURL, &rsrcRef);

iErr = FSOpenResourceFile( &rsrcRef, 0L, 0L, fsRdPerm, &refNum);
if( iErr) -1;

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

CloseResFile( refNum);

return ff;
#endif
}

void DeleteDLSFile()
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


short OpenDataFileQK( long dirID, short VRefNum)
{
	CInfoPBRec		info;
	Str255			tempStr;
	long			dirIDCopy;
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
		
		if (PBGetCatInfo(&info, false) != noErr) break;
		
		if( info.hFileInfo.ioFlFndrInfo.fdType == 'INIT' && info.hFileInfo.ioFlFndrInfo.fdCreator == 'dvb ')
		{	
		//	HGetVol( 0L, &vRefNum, &dirIDCopy);
			
		//	iErr = HSetVol( 0L, info.hFileInfo.ioVRefNum, dirID);
			
			pStrcpy( spec.name, info.hFileInfo.ioNamePtr);
			spec.vRefNum = info.hFileInfo.ioVRefNum;
			spec.parID = dirID;
			
			iErr = FSpOpenDF( &spec, fsCurPerm, &iRefNum);
			if( iErr != noErr) iRefNum = -1;
			
		//	iErr = HSetVol( 0L, vRefNum, dirIDCopy);
		//	if( iErr != noErr) MyDebugStr( __LINE__, __FILE__, "HSetVol error...");
		}
	}
	
	return iRefNum;
}

OSErr GetAtomData( MyAtom at, void* data, long size)
{
long 	prePos, fSize, ilistType, nlistType, index, listSize;
CK		sck;
OSErr	iErr;
MyAtom	retat;

iErr = SetFPos( at.ref, fsFromStart, at.pos -4);

fSize = sizeof( sck);
iErr = FSRead( at.ref, &fSize, &sck);
if( iErr) Debugger();
sck.cksize = longswap( sck.cksize);

//if( sck.cksize > size) Debugger();

fSize = size;	//sck.cksize;
iErr = FSRead( at.ref, &fSize, data);
if( iErr) Debugger();

return noErr;
}

void DebugLong( long type)
{
	Str31	str;
	
	str[ 0] = 4;
	BlockMoveData( &type, &str[ 1], 4);
	
	DebugStr( str);
}

OSErr GetAtomString(MyAtom sat4, Ptr text)
{
long	fSize = sat4.size;
OSErr	iErr;

iErr = SetFPos( sat4.ref, fsFromStart, sat4.pos + 4);	// + 4 pourquoi? Paske y a la taille de la string

iErr = FSRead( sat4.ref, &fSize, text);

return iErr;
}

long CountAtomById( MyAtom at, long type)
{
long 	prePos, fSize, ilistType, nlistType, index, listSize;
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
	sck.cksize = longswap( sck.cksize);

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

OSErr FindAtomById( MyAtom at, MyAtom *retat, Boolean LIST, long type, short id)
{
long 	prePos, fSize, ilistType, nlistType, index, listSize;
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
	sck.cksize = longswap( sck.cksize);

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

OSErr GetAtomDataById( MyAtom at, long type, void *data, long size)
{
	MyAtom 	tempAt;
	OSErr	iErr;

	iErr = FindAtomById( at, &tempAt, true, type, 0);
	if( iErr) return( iErr);


	iErr = GetAtomData( tempAt, data, size);
	if( iErr) return( iErr);

return noErr;
}

static Boolean TestRunningOnCarbonX(void)
{
    UInt32 response;
    
    return (Gestalt(gestaltSystemVersion, 
                    (SInt32 *) &response) == noErr)
                && (response >= 0x01000);
}

void Quicktime5( NoteRequest *NoteRequest, sData **sample, InstrData *inst)
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
			HSetVol( 0L, foundVRefNum, foundDirID);	
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
			ck.cksize = longswap( ck.cksize);
			
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
			noIns = longswap( noIns);
			
			
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
				
				curIns.cRegions = longswap( curIns.cRegions);
				curIns.Locale.ulBank = longswap( curIns.Locale.ulBank);
				curIns.Locale.ulInstrument = longswap( curIns.Locale.ulInstrument);
				
			/*	iErr = FindAtomById( insAt, &InfoAt, true, 'INFO', 0);
				if( iErr) DebugLong( iErr);
				
				iErr = FindAtomById( InfoAt, &InfoData, true, 'INAM', 0);
				if( iErr) DebugLong( iErr);
				
				iErr = GetAtomData( InfoData, insName, sizeof( insName));*/
				
				if( NoteRequest->tone.instrumentNumber >= 16384) // DRUM KIT
				{
					if( BitTst( &curIns.Locale.ulBank, 31-31))
					{
						long	valeurQT, gmID, valeurBank;
						
						gmID = NoteRequest->tone.instrumentNumber;
						gmID = gmID & 0x000000FF;
						
						if( gmID == curIns.Locale.ulInstrument+1)
						{
							break;
						}
					}
				}
				else  if( NoteRequest->tone.instrumentNumber != 0 && NoteRequest->tone.instrumentNumber > 128)
				{
					long	valeurQT, gmID, valeurBank;
					
					valeurQT = NoteRequest->tone.instrumentNumber;
					valeurQT = valeurQT >> 7;
					
					gmID = NoteRequest->tone.instrumentNumber;
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
					if( curIns.Locale.ulInstrument+1 == NoteRequest->tone.instrumentNumber) break;
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
				
				curIns.cRegions = longswap( curIns.cRegions);
				curIns.Locale.ulBank = longswap( curIns.Locale.ulBank);
				curIns.Locale.ulInstrument = longswap( curIns.Locale.ulInstrument);
				
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
					
					rgnh.RangeKey.usLow = shrtswap( rgnh.RangeKey.usLow);
					rgnh.RangeKey.usHigh = shrtswap( rgnh.RangeKey.usHigh);
					rgnh.RangeVelocity.usLow = shrtswap( rgnh.RangeVelocity.usLow);
					rgnh.RangeVelocity.usHigh = shrtswap( rgnh.RangeVelocity.usHigh);
					rgnh.fusOptions = shrtswap( rgnh.fusOptions);
					rgnh.usKeyGroup = shrtswap( rgnh.usKeyGroup);
					
					// Wave sample
					
					iErr = GetAtomDataById( rgnAt, 'wsmp', &wsmp, sizeof( wsmp));
					if( iErr) DebugLong( iErr);
					
					wsmp.cbSize = longswap( wsmp.cbSize);
					wsmp.usUnityNote = shrtswap( wsmp.usUnityNote);
					wsmp.sFineTune = shrtswap( wsmp.sFineTune);
					wsmp.lAttenuation = longswap( wsmp.lAttenuation);
					wsmp.fulOptions = longswap( wsmp.fulOptions);
					wsmp.cSampleLoops = longswap( wsmp.cSampleLoops);
					if( wsmp.cSampleLoops > 0)
					{
						long loopSize = sizeof( loop);
						iErr = FSRead( rgnAt.ref, &loopSize, &loop);
						
						loop.cbSize = longswap( loop.cbSize);
						loop.ulType = longswap( loop.ulType);
						loop.ulStart = longswap( loop.ulStart);
						loop.ulLength = longswap( loop.ulLength);
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
					
					wlnk.fusOptions = shrtswap( wlnk.fusOptions);
					wlnk.usPhaseGroup = shrtswap( wlnk.usPhaseGroup);
					wlnk.ulChannel = longswap( wlnk.ulChannel);
					wlnk.ulTableIndex = longswap( wlnk.ulTableIndex);
					
					// Load Wave Data
					
					
					iErr = FindAtomById( at, &wvpl, true, 'wvpl', 0);
					if( iErr) DebugLong( iErr);
					
					iErr = FindAtomById( wvpl, &wave, true, 'wave', wlnk.ulTableIndex);
					if( iErr) DebugLong( iErr);
					
					// FMT
					
					iErr = GetAtomDataById( wave, 'fmt ', &fmt, sizeof( fmt));
					if( iErr) DebugLong( iErr);
					
					fmt.wFormatTag = shrtswap( fmt.wFormatTag);
					fmt.nCannels = shrtswap( fmt.nCannels);
					fmt.nSamplesPerSec = longswap( fmt.nSamplesPerSec);
					fmt.nAvgBytesPerSec = longswap( fmt.nAvgBytesPerSec);
					fmt.nBlockAlign = shrtswap( fmt.nBlockAlign);
					fmt.wBitsPerSample = shrtswap( fmt.wBitsPerSample);
					
					// Wave Data
					
					iErr = FindAtomById( wave, &dataAt, true, 'data', 0);
					if( iErr) DebugLong( iErr);
					
					data = NewPtr( dataAt.size);
					if( data == 0L) DebugLong( -1);
					iErr = GetAtomDataById( wave, 'data', data, dataAt.size);
					if( iErr) DebugLong( iErr);
					
					
					// ************************
					// * Add it to instrument *
					// ************************
					
					for( ii = rgnh.RangeKey.usLow - 12; ii <= rgnh.RangeKey.usHigh - 12; ii++)
					{
						if( ii < NUMBER_NOTES && ii > 0) inst->what[ ii] = inst->numSamples;
					}
					
					if( curMusic != 0L) curData = MADCreateSample( curMusic, inst->no, inst->numSamples);
					else
					{
						curData = (sData*) NewPtrClear( sizeof( sData));
						if( curData == 0L) MyDebugStr( __LINE__, __FILE__, "");
						sample[ inst->no * MAXSAMPLE + inst->numSamples] = curData;
						
						inst->numSamples++;
					}
					
					curData->size		= 0L;
					curData->loopBeg	= 0L;
					curData->loopSize	= 0L;
					curData->vol		= MAX_VOLUME;
					curData->loopType	= eClassicLoop;
					curData->amp		= 8;
					curData->relNote	= 60 - wsmp.usUnityNote;// + wsmp.sFineTune;	//(60 - ) - 
					
					// curData->name
						
					curData->data = NewPtr( dataAt.size);
				
					if( curData->data == 0L)
					{
						Erreur( 63, -2);
						
						curData->size = 0L;
					}
					else
					{
						if( dataAt.size) BlockMoveData( data, curData->data, dataAt.size);
						DisposePtr( data);
						
						switch( fmt.wBitsPerSample)
						{
							case 8:
								ConvertInstrumentIn( (Byte*) curData->data, dataAt.size);
							break;
							
							case 16:
								//ConvertInstrumentIn16( (short*) curData->data, dataAt.size);
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
			
			FSClose( iFileRef);
		}
	}
	
	BAIL:
	
	if( TestRunningOnCarbonX())
	{
		DeleteDLSFile();
	}
	
	return;
}

void TESTNEWSYSTEM( sData **sample, InstrData *inst, AtomicInstrument ai)
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
		
		if( sampleIDMap[ sdesc->sampleDataID] != -1)
		{
			for( ii = sdesc->pitchLow - 12; ii <= sdesc->pitchHigh - 12; ii++)
			{
				if( ii < NUMBER_NOTES && ii > 0) inst->what[ ii] = sampleIDMap[ sdesc->sampleDataID];
			}
		}
		else
		{
			sampleIDMap[ sdesc->sampleDataID] = inst->numSamples;
			
			for( ii = sdesc->pitchLow - 12; ii <= sdesc->pitchHigh - 12; ii++)
			{
				if( ii < NUMBER_NOTES && ii > 0) inst->what[ ii] = inst->numSamples;
			}
			
			SetInstruNameM( inst->no, myNoteRequest.tone.instrumentName, myNoteRequest.tone.instrumentNumber, inst->name);
			
			mySampleInfoAtom = QTFindChildByID( ai, kParentAtomIsContainer, kaiSampleInfoType, sdesc->sampleDataID, &no);	//
			if( mySampleInfoAtom != 0)
			{
				no = QTCountChildrenOfType( ai, mySampleInfoAtom, kaiSampleDataType);
				if( no != 1) MyDebugStr(__LINE__, __FILE__, "kaiSampleDataType");
				
				mySampleDataAtom = QTFindChildByIndex( ai, mySampleInfoAtom, kaiSampleDataType, 1, &atomID);
				if( mySampleDataAtom == 0) MyDebugStr(__LINE__, __FILE__, "kaiSampleDataType");
				
				size = 0L;
				iErr = QTGetAtomDataPtr( ai, mySampleDataAtom, &size, &data);
				if( iErr) MyDebugStr(__LINE__, __FILE__, "QTGetAtomDataPtr");
				
				inOutBytes = (sdesc->sampleSize * sdesc->numSamples) / 8L;
				
				// ************************
				// * Add it to instrument *
				// ************************
				
				{
					if( curMusic != 0L) curData = MADCreateSample( curMusic, inst->no, inst->numSamples);
					else
					{
						curData = (sData*) NewPtrClear( sizeof( sData));
						if( curData == 0L) MyDebugStr( __LINE__, __FILE__, "");
						sample[ inst->no * MAXSAMPLE + inst->numSamples] = curData;
						
						inst->numSamples++;
					}
					
					curData->size		= 0L;
					curData->loopBeg	= 0L;
					curData->loopSize	= 0L;
					curData->vol		= MAX_VOLUME;
					curData->c2spd		= NOFINETUNE;
					curData->loopType	= eClassicLoop;
					curData->amp		= 8;
					// curData->name
					
					curData->data = NewPtr( inOutBytes);
					
					if( curData->data == 0L)
					{
						Erreur( 63, -2);
						
						curData->size = 0L;
					}
					else
					{
						if( inOutBytes)
						{
							BlockMoveData( data, curData->data, inOutBytes);
						}
						
						if( sdesc->dataFormat == 'raw ')
						{
							switch( sdesc->sampleSize)
							{
								case 8:
									ConvertInstrumentIn( (Byte*) curData->data, inOutBytes);
								break;
								
								case 16:
									//ConvertInstrumentIn16( (short*) curData->data, inOutBytes);
								break;
							}
						}
						
						curData->size 		= inOutBytes;
						curData->loopBeg 	= sdesc->loopStart;
						curData->loopSize 	= sdesc->loopEnd - sdesc->loopStart;
						curData->vol 		= 64;
						curData->amp	 	= sdesc->sampleSize;
						
						if( sdesc->numChannels == 2)
						{
							curData->stereo = true;
						}
						if( sdesc->numChannels > 2) MyDebugStr( __LINE__, __FILE__, "More than 2 channels");
						
						// **
						curData->c2spd		= sdesc->sampleRate >> 16;
						curData->relNote	= 60 - sdesc->pitchNormal;
						// **
						
						OctavesMIDIName( sdesc->pitchLow - 12, aStr);
						pStrcat( aStr, "\p -> ");
						OctavesMIDIName( sdesc->pitchHigh - 12, bStr);
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

void Quicktime2Converter(void)
{
	short 		ins, samp, i;
	sData 		**sample;
	InstrData 	*inst;
	
	if( !GetIns( &ins, &samp))
	{
		Erreur( 13, ins);
		return;
	}
	
	SaveUndo( USample, ins, "\pUndo 'Create MIDI instrument'");
	
	sample = curMusic->sample;
	inst = &curMusic->fid[ ins];
	
	for( i = 0; i < inst->numSamples; i++)
	{
		if( sample[ inst->firstSample + i] != 0L)
		{
			if( sample[ inst->firstSample + i]->data != 0L)
			{
				DisposePtr( (Ptr) sample[ inst->firstSample + i]->data);
				sample[ inst->firstSample + i]->data = 0L;
			}
			DisposePtr( (Ptr) sample[ inst->firstSample + i]);
			sample[ inst->firstSample + i] = 0L;
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
	
	iErr = NAStuffToneDescription( na, myNoteRequest.tone.instrumentNumber, &myNoteRequest.tone);
	if (iErr != noErr) MyDebugStr(__LINE__, __FILE__, "NAStuff Converter");
	
	myNoteRequest.tone.synthesizerType = synthType;
	pStrcpy( myNoteRequest.tone.synthesizerName, synthName);
	
	// display the instrument picker dialog box to elicit an instrument from the user
	iErr = NAPickInstrument( na, 0L, "\pPick an instrument:", &myNoteRequest.tone, kPickSameSynth + kPickUserInsts, 0, 0, 0);
	//if (iErr != noErr) goto BAIL;
	
	iErr = MusicSetPartInstrumentNumber(mc, 1, myNoteRequest.tone.instrumentNumber);
	if( iErr) goto BAIL;
	
	iErr = MusicGetPartAtomicInstrument(mc, 1, &ai, 0);
	if( iErr) goto BAIL;
	
	if( QK50) Quicktime5( &myNoteRequest, sample, inst);
	else TESTNEWSYSTEM( sample, inst, ai);
	
	CloseComponent( na);
	
	}
	
	BAIL:
	
	#if defined( MAINPLAYERPRO)
	UpdateALLWindow();
	#endif
	
	;
}

// FOR MIDI IMPORT FUNCTION
void ComputeQuicktimeSound( short GMInstruID, sData **sample, InstrData* inst, short ins)
{
	short 		samp, i;
	
	for( i = 0; i < inst->numSamples; i++)
	{
		if( sample[ inst->firstSample + i] != 0L)
		{
			if( sample[ inst->firstSample + i]->data != 0L)
			{
				DisposePtr( (Ptr) sample[ inst->firstSample + i]->data);
				sample[ inst->firstSample + i]->data = 0L;
			}
			DisposePtr( (Ptr) sample[ inst->firstSample + i]);
			sample[ inst->firstSample + i] = 0L;
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
	
	myNoteRequest.tone.synthesizerType = synthType;
	pStrcpy( myNoteRequest.tone.synthesizerName, synthName);
	
	// display the instrument picker dialog box to elicit an instrument from the user
	//iErr = NAPickInstrument( na, 0L, "\pPick an instrument:", &myNoteRequest.tone, kPickSameSynth + kPickUserInsts, 0, 0, 0);
	//if (iErr != noErr) goto BAIL;
	
	iErr = MusicSetPartInstrumentNumber(mc, 1, myNoteRequest.tone.instrumentNumber);
	if( iErr) goto BAIL;
	
	iErr = MusicGetPartAtomicInstrument(mc, 1, &ai, 0);
	if( iErr) goto BAIL;
	
	if( QK50) Quicktime5( &myNoteRequest, sample, inst);
	else TESTNEWSYSTEM( sample, inst, ai);
	
	CloseComponent( na);
	}
	
	BAIL:
	
	;
}