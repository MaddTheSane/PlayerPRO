#include "MAD.h"
#include "RDriver.h"
#include "RDriverInt.h"
#include "Folders.h"
#include "ptmid.h"
#include "QuickTimeMusic.h"

Handle NSndToHandle( Handle sound, long *loopStart, long *loopEnd, short *sampleSize, unsigned long *sampleRate, long *baseFreq, MADDriverSettings *init);
void ConvertInstrument( register	Byte	*tempPtr,	register long sSize);
void NInitSmallPiano( Rect mainRect, Rect *listRect);
void DrawSmallPianoKey( short i, short color, Rect aRect);
void NDoPlayInstru(short	Note, short Instru, short effect, short arg, short vol);
void ConvertInstrumentIn( register	Byte	*tempPtr,	register long sSize);

/**** Resource Format QK25 ****/

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

void OctavesMIDIName(short	id, Str255	String)
{
	short			NNames[ 12] =	{'C ','C#','D ','D#','E ','F ','F#','G ','G#','A ','A#','B '};
									/*	{'Do','Do#','R�','R�#','Mi','Fa','Fa#','Sol','Sol#','La','La#','Si'};	*/
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

short OpenResFileQK( long dirID, short VRefNum)
{
	CInfoPBRec		info;
	Str255			tempStr;
	long			dirIDCopy;
	short			i, vRefNum;
	OSErr			iErr;
	
	short			ret;

	ret = OpenResFile( "\pQuickTime� Musical Instruments");
	
	if( ret != -1) return ret;
	
	/************/
	
	ret = -1;

	info.hFileInfo.ioNamePtr = tempStr;
	info.hFileInfo.ioVRefNum = VRefNum;
	
	for (i = 1; true; i ++)
	{
		info.hFileInfo.ioDirID = dirID;
		info.hFileInfo.ioFDirIndex = i;
		
		if (PBGetCatInfo(&info, false) != noErr) break;
		
		if( info.hFileInfo.ioFlFndrInfo.fdType == 'INIT' && info.hFileInfo.ioFlFndrInfo.fdCreator == 'dvb ')
		{	
			HGetVol( 0L, &vRefNum, &dirIDCopy);
			
			iErr = HSetVol( 0L, info.hFileInfo.ioVRefNum, dirID);
			
			ret = OpenResFile( info.hFileInfo.ioNamePtr);
			
			iErr = HSetVol( 0L, vRefNum, dirIDCopy);
			if( iErr != noErr) DebugStr("\pHSetVol error...");
		}
	}
	
	return ret;
}

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
		HSetVol( 0L, foundVRefNum, foundDirID);	
		
		iFileRef = OpenResFileQK( foundDirID, foundVRefNum);
		if( iFileRef != -1)
		{
			UseResFile( iFileRef);
			
			ENCORE:
			
			while( (hRsrc = GetResource( 'ssai', GMInstruID)) == 0L) GMInstruID++;
			if( SizeResource( hRsrc) < 600) goto ENCORE;
			
			if( hRsrc != 0L)
			{
				QuictimeRsrc25	*rsrc, *maxrsrc;
				short			sampleIDMap[ 500];
				
				for( i = 0; i < 500; i++) sampleIDMap[ i] = -1;
				
				DetachResource( hRsrc);
				HLock( hRsrc);
				if( hRsrc == 0L) DebugStr("\pMIDI Ins Err");
				
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
						Ptr					ptr2delete = 0L;
						
						sdesc = (InstSampleDescRec*) rsrc->data;
						
						// SampleData exists already ??
						
						if( sampleIDMap[ sdesc->sampleDataID] != -1)
						{
							for( ii = sdesc->pitchLow - 12; ii <= sdesc->pitchHigh - 12; ii++)
							{
								if( ii < NUMBER_NOTES && ii > 0) inst->what[ ii] = sampleIDMap[ sdesc->sampleDataID];
							}
							
							goto NEXTSAMP;
						}
						else sampleIDMap[ sdesc->sampleDataID] = inst->numSamples;
						
						for( ii = sdesc->pitchLow - 12; ii <= sdesc->pitchHigh - 12; ii++)
						{
							if( ii < NUMBER_NOTES && ii > 0) inst->what[ ii] = inst->numSamples;
						}
						
						if( sdesc->offset != 0) MyDebugStr("\pOffset!");
						
						// Go to DATA
						
						if( sdesc->dataFormat == 'raw ' || sdesc->dataFormat == 'twos')
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
										if( rsrc2->id  == sdesc->sampleDataID)
										{
											Founded = true;
										}
									}
								}
							}
							
							if( Founded)
							{
								ptr2delete = 0L;
								rsrc2 = rsrc3;			// go to  
								inOutBytes = (sdesc->sampleSize * sdesc->numSamples) / 8L;
							}
							else
							{
								inOutBytes = (sdesc->sampleSize * sdesc->numSamples) / 8L;
								
								LoadQK25Sample( sdesc->sampleDataID, GMInstruID, &ptr2delete, inOutBytes);
								
								if( !ptr2delete) inOutBytes = 0L;
								else Founded = true;
							}
						}
						else inOutBytes = 0;
						
						// ************************
						// * Add it to instrument *
						// ************************
						
						{
							curData = MADCreateSample();
							
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
									if( !ptr2delete) BlockMoveData( rsrc2->data, curData->data, inOutBytes);
									else
									{
										BlockMoveData( ptr2delete, curData->data, inOutBytes);
										DisposePtr( ptr2delete);
										ptr2delete = 0L;
									}
								}
								
								if( sdesc->dataFormat == 'raw ')
								{
									switch( sdesc->sampleSize)
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
								curData->loopBeg 	= sdesc->loopStart;
								curData->loopSize 	= sdesc->loopEnd - sdesc->loopStart;
								curData->vol 		= 64;
								curData->amp	 	= sdesc->sampleSize;
								
								if( sdesc->numChannels == 2)
								{
									curData->stereo = true;
								}
								if( sdesc->numChannels > 2) MyDebugStr("\pMore than 2 channels");
								
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
						// ************************
					}
					else Debugger();
					
					NEXTSAMP:
					
					rsrc = (QuictimeRsrc25*) (((Ptr) rsrcCopy) + rsrcCopy->size);
				}
				
				HUnlock( hRsrc);
				DisposHandle( hRsrc);
			}
			CloseResFile( iFileRef);
		}
	//	else Erreur( 72, iFileRef);
	}
	else Erreur( 72, 0);
	
	SetCursor( &qd.arrow);
}

void ComputeQuicktimeSound( short GMInstruID, sData **sample, InstrData* inst, short ins, MADDriverSettings *init)
{
	long					lS, lE, inOutBytes;
	short					sS;
	unsigned long			sRate;
	long					bFreq;
	
	short					ii;
	short 					i;
	Handle					HSnd, hRsrc;
	short					foundVRefNum, iFileRef;
	long					foundDirID;
	OSErr					iErr;
	QuicktimeInst			*QuickInst;
	Ptr						tPtr;
	Str255					aStr, bStr;
	Boolean					QK25 = false;
	
	/***************/
	iErr = FindFolder( kOnSystemDisk, kExtensionFolderType, kDontCreateFolder, &foundVRefNum, &foundDirID);
	if( iErr == noErr)
	{
		HSetVol( 0L, foundVRefNum, foundDirID);	
		
		iFileRef = OpenResFileQK( foundDirID, foundVRefNum);
		if( iFileRef != -1)
		{
			Handle	rsrc;
			
			UseResFile( iFileRef);
			
			rsrc = Get1Resource( 'vers', 1);
			if( ((Byte*)*rsrc) [ 0] > 0x02) QK25 = true;
			if( ((Byte*)*rsrc) [ 0] >= 0x02 && ((Byte*)*rsrc) [ 1] >= 0x50) QK25 = true;
			ReleaseResource( rsrc);
			
			CloseResFile( iFileRef);
		}
	}
	
	if( QK25) { ComputeQuicktimeSound25( GMInstruID, sample, inst, ins); return;}
	
	
	for( i = 0; i < inst->numSamples; i++)
	{
		if( sample[ ins][ i] != 0L)
		{
			if( sample[ ins][ i]->data != 0L)
			{
				DisposePtr( (Ptr) sample[ ins][ i]->data);
				sample[ ins][ i]->data = 0L;
			}
			DisposePtr( (Ptr) sample[ ins][ i]);
			sample[ ins][ i] = 0L;
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
	
	inst->volFade		= 0;
	inst->vibDepth		= 0;
	inst->vibRate		= 0;

	/***************/
	
	inst->volFade = 1100;
	
	iErr = FindFolder( kOnSystemDisk, kExtensionFolderType, kDontCreateFolder, &foundVRefNum, &foundDirID);
	if( iErr == noErr)
	{
		HSetVol( 0L, foundVRefNum, foundDirID);	
		
		iFileRef = OpenResFileQK( foundDirID, foundVRefNum);
		if( iFileRef != -1)
		{
			UseResFile( iFileRef);
			
			while( (hRsrc = GetResource( 'INST', GMInstruID)) == 0L) GMInstruID++;
			
			if( hRsrc != 0L)
			{
				DetachResource( hRsrc);
				HLock( hRsrc);
				if( hRsrc == 0L) DebugStr("\pMIDI Ins Err");
				QuickInst = (QuicktimeInst*) *hRsrc;
				
				tPtr = (Ptr) (((Ptr) &QuickInst->no) + 4L + QuickInst->no * sizeof( QuictimeSs));
				SetInstruNameM( ins, (unsigned char*) tPtr, GMInstruID, inst->name);
				
				for( i = 0; i < QuickInst->no; i++)
				{
					sData	*curData;
									
					if( QuickInst->Ss[ i].type > 0)
					{
						for( ii = QuickInst->Ss[ i].from - 12; ii <= QuickInst->Ss[ i].to - 12; ii++)
						{
							if( ii < NUMBER_NOTES && ii > 0) inst->what[ ii] = inst->numSamples;
						}
					}
					
					HSnd = Get1Resource( 'snd ', QuickInst->Ss[ i].resID);
					if( HSnd != 0L)
					{
						DetachResource( HSnd);
						
						HSnd =  NSndToHandle( HSnd, &lS, &lE, &sS, &sRate, &bFreq, init);
						
						inOutBytes = GetHandleSize( HSnd);
						if( sS == 8) ConvertInstrumentIn( (Byte*) *HSnd, inOutBytes);
						
						/************************/
						/* Add it to instrument */
						/************************/
						
						sample[ ins][ inst->numSamples] = curData = MADCreateSample();
						
						curData->data = MADPlugNewPtrClear( inOutBytes, init);
						
						if( curData->data == 0L)
						{
						//	Erreur( 63, -2);
							HUnlock( HSnd);
							DisposeHandle( HSnd);
							
							curData->size = 0L;
						}
						else
						{
							BlockMoveData( *HSnd, curData->data, inOutBytes);
							
							HUnlock( HSnd);
							DisposeHandle( HSnd);
							
							curData->size 		= inOutBytes;
							curData->loopBeg 	= lS;
							curData->loopSize 	= lE - lS;
							curData->vol 		= 64;
							curData->amp	 	= sS;
							
							/**/
							curData->c2spd		= sRate >> 16;
							curData->relNote	= 60 - bFreq;		//QuickInst->Ss[ i].resID - (GMInstruID*100);
							/**/
							
							OctavesMIDIName( QuickInst->Ss[ i].from - 12, aStr);
							pStrcat( aStr, "\p -> ");
							OctavesMIDIName( QuickInst->Ss[ i].to - 12, bStr);
							pStrcat( aStr, bStr);
							pStrcat( aStr, "\p, ");
							NumToString( curData->c2spd, bStr);
							pStrcat( aStr, bStr);
							pStrcat( aStr, "\p Hz");
							
							SetSampNameM( aStr, curData->name);
						}
						inst->numSamples++;
						/************************/
					}
				}
				HUnlock( hRsrc);
				DisposeHandle( hRsrc);
			}
			CloseResFile( iFileRef);
		}
	//	else Erreur( 72, iFileRef);
	}
//	else Erreur( 72, iFileRef);
}

