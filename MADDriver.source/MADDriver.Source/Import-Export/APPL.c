/********************						***********************/
//
//	Player PRO 5.0 - DRIVER SOURCE CODE -
//
//	Library Version 5.0
//
//	To use with MAD Library for Mac: Symantec, CodeWarrior and MPW
//
//	Antoine ROSSET
//	16 Tranchees
//	1206 GENEVA
//	SWITZERLAND
//
//	COPYRIGHT ANTOINE ROSSET 1996, 1997, 1998
//
//	Thank you for your interest in PlayerPRO !
//
//	FAX:				(+41 22) 346 11 97
//	PHONE: 			(+41 79) 203 74 62
//	Internet: 	RossetAntoine@bluewin.ch
//
/********************						***********************/

#include <PlayerPROCore/PlayerPROCore.h>
#include "MOD.h"

static OSErr MADResetInstrument( InstrData		*curIns)
{
	short i;

	for( i = 0; i < 32; i++) curIns->name[ i]	= 0;
	curIns->type		= 0;
	curIns->numSamples	= 0;
	
	///
	
	for( i = 0; i < 96; i++) curIns->what[ i]		= 0;
	for( i = 0; i < 12; i++)
	{
		curIns->volEnv[ i].pos		= 0;
		curIns->volEnv[ i].val		= 0;
	}
	for( i = 0; i < 12; i++)
	{
		curIns->pannEnv[ i].pos	= 0;
		curIns->pannEnv[ i].val	= 0;
	}
	curIns->volSize		= 0;
	curIns->pannSize	= 0;
	
	curIns->volSus		= 0;
	curIns->volBeg		= 0;
	curIns->volEnd		= 0;
	
	curIns->pannSus		= 0;
	curIns->pannBeg		= 0;
	curIns->pannEnd		= 0;

	curIns->volType		= 0;
	curIns->pannType	= 0;
	
	curIns->volFade		= DEFAULT_VOLFADE;
	curIns->vibDepth	= 0;
	curIns->vibRate		= 0;

	return noErr;
}

static void TurnRadio( short	item, DialogPtr	dlog, Boolean alors)
{
Handle		itemHandle;
short		itemType;
Rect		itemRect;

GetDialogItem (dlog, item, &itemType, &itemHandle, &itemRect);

if( alors) SetControlValue( (ControlHandle) itemHandle, 255);
else SetControlValue( (ControlHandle) itemHandle, 0);
}

static void CopyResource( OSType type, short ID, short newID)
{
Handle	hRsrc;

	hRsrc = GetResource( type, ID);			DetachResource( hRsrc);
	AddResource( hRsrc, type, newID, "\p");	WriteResource( hRsrc);
	DetachResource( hRsrc);					DisposeHandle( hRsrc);
}

static OSErr LoadMADH( Ptr MADPtr, MADMusic *MadFile, MADDriverSettings *init)
{
short 					i, maxInstru;
long 					inOutCount, OffSetToSample;
struct PatHeader		tempPatHeader;
MADSpec					*MadHeader;

/**** HEADER ****/
MadFile->header = (MADSpec*) MADPlugNewPtr( sizeof( MADSpec), init);
if( MadFile->header == NULL) return MADNeedMemory;

OffSetToSample = 0;
BlockMoveData( MADPtr, MadFile->header, sizeof( MADSpec));
OffSetToSample += sizeof( MADSpec);

MadHeader = MadFile->header;

if( MadHeader->MAD != 'MADK') return MADFileNotSupportedByThisPlug;

//////////////////

MadFile->fid = ( InstrData*) MADPlugNewPtrClear( sizeof( InstrData) * (long) MAXINSTRU, init);
if( !MadFile->fid) return MADNeedMemory;

MadFile->sample = ( sData**) MADPlugNewPtrClear( sizeof( sData*) * (long) MAXINSTRU * (long) MAXSAMPLE, init);
if( !MadFile->sample) return MADNeedMemory;


/**** PARTITION ****/
for( i = MadHeader->numPat; i < MAXPATTERN; i++) MadFile->partition[ i] = NULL;

for( i = 0; i < MadHeader->numPat; i++)
{
	inOutCount = sizeof( PatHeader);
	BlockMoveData( MADPtr + OffSetToSample, &tempPatHeader, inOutCount);
	
	inOutCount = sizeof( PatHeader) + MadHeader->numChn * tempPatHeader.size * sizeof( Cmd);
	MadFile->partition[ i] = (PatData*) MADPlugNewPtr( inOutCount, init);
	if( MadFile->partition[ i] == NULL) return MADNeedMemory;
	
	BlockMoveData( MADPtr + OffSetToSample, MadFile->partition[ i], inOutCount);
	OffSetToSample += inOutCount;
}

/**** INSTRUMENTS ****/

inOutCount = sizeof( InstrData) * (long) MadFile->header->numInstru;
BlockMoveData( MADPtr + OffSetToSample, MadFile->fid, inOutCount);
OffSetToSample += inOutCount;

for( i = MadFile->header->numInstru-1; i >= 0 ; i--)
{
	InstrData	*curIns = &MadFile->fid[ i];
	
	if( i != curIns->no)
	{
		MadFile->fid[ curIns->no] = *curIns;
		MADResetInstrument( curIns);
	}
}
MadFile->header->numInstru = MAXINSTRU;

for( i = 0; i < MAXINSTRU ; i++)
{
	short x;
	
	for( x = 0; x < MadFile->fid[ i].numSamples ; x++)
	{
		sData	*curData;
	
		// ** Read Sample header **
		
		curData = MadFile->sample[ i*MAXSAMPLE + x] = (sData*) MADPlugNewPtr( sizeof( sData), init);
		if( curData == NULL) return MADNeedMemory;
		
		inOutCount = sizeof( sData);
		
		BlockMoveData( MADPtr + OffSetToSample, curData, inOutCount);
		OffSetToSample += inOutCount;
		
		// ** Read Sample DATA
		
		inOutCount = curData->size;
		
		curData->data = MADPlugNewPtr( inOutCount, init);
		if( curData->data == NULL) return MADNeedMemory;
		
		BlockMoveData( MADPtr + OffSetToSample, curData->data, inOutCount);
		OffSetToSample += inOutCount;
	}
}

for( i = 0; i < MAXINSTRU; i++) MadFile->fid[ i].firstSample = i * MAXSAMPLE;

/*********************/

{
short	alpha, x;

MadFile->sets = (FXSets*) NewPtrClear( MAXTRACK * sizeof(FXSets));

alpha = 0;

for( i = 0; i < 10 ; i++)	// Global Effects
{
	if( MadFile->header->globalEffect[ i])
	{
		inOutCount = sizeof( FXSets);
		BlockMoveData( MADPtr + OffSetToSample, &MadFile->sets[ alpha], inOutCount);
		OffSetToSample += inOutCount;
		alpha++;
	}
}

for( i = 0; i < MadFile->header->numChn ; i++)	// Channel Effects
{
	for( x = 0; x < 4; x++)
	{
		if( MadFile->header->chanEffect[ i][ x])
		{
			inOutCount = sizeof( FXSets);
			BlockMoveData( MADPtr + OffSetToSample, &MadFile->sets[ alpha], inOutCount);
			OffSetToSample += inOutCount;
			alpha++;
		}
	}
}

}

return( noErr);
}

static OSErr TESTMADH( MADSpec* MADPtr)
{
	if( MADPtr->MAD == 'MADK') return noErr;
	else return MADFileNotSupportedByThisPlug;
}

static OSErr INFOMADF( MADSpec* MADPtr, PPInfoRec *info)
{
short	i;

	strcpy( info->internalFileName, MADPtr->name);
	
	strcpy( info->formatDescription, "MAD Resource (APPL)");
	
	info->totalPatterns		= MADPtr->numPat;
	info->partitionLength	= MADPtr->numPointers;
	
	info->totalTracks		= MADPtr->numChn;
	info->signature			= MADPtr->MAD;
	
	info->totalInstruments	= MADPtr->numInstru;
	
	return noErr;
}


static short ChooseCompilation()
{
	short		itemHit, result;
	DialogPtr	aDialog;

	aDialog = GetNewDialog( 173, NULL, (WindowPtr) -1L);
	SetPort( GetDialogPort(aDialog));
	
	SetDialogDefaultItem( aDialog, 1);
	
	ShowWindow( GetDialogWindow(aDialog));	

	TurnRadio( 6, aDialog, true);
	result = 6;
	
	do
	{
		ModalDialog( NULL, &itemHit);
		
		switch( itemHit)
		{
			case 4:
			case 5:
			case 6:
				TurnRadio( result, aDialog, false);
				TurnRadio( itemHit, aDialog, true);
				result = itemHit;
			break;
		}
	
	}while( itemHit != 1 && itemHit != 2);
	
	DisposeDialog( aDialog);
	
	if( itemHit == 2) return -1;
	
	result -= 4;
	return result;
}


static OSErr SaveAPPL( short APPLType, short fRefNum, MADMusic *MadFile, MADDriverSettings *init)
{
OSErr					iErr;
short					i, x;
long					fileSize, inOutCount, tt;
Handle					hRsrc;


	// We need to compute number of valid instruments !!! See above....
	
	for( i = 0, x = 0; i < MAXINSTRU; i++)
	{
		MadFile->fid[ i].no = i;
		
		if( MadFile->fid[ i].numSamples > 0 || MadFile->fid[ i].name[ 0] != 0)	// Is there something in this instrument?
		{
			x++;
		}
	}
	MadFile->header->numInstru = x;
	
	/////////////////////////////
	
	fileSize = sizeof( MADSpec);
	fileSize += sizeof( InstrData) * MadFile->header->numInstru;
	for( i = 0; i < MadFile->header->numPat; i++) fileSize += sizeof( PatHeader) + MadFile->header->numChn * MadFile->partition[ i]->header.size * sizeof( Cmd);
	for( i = 0; i < MAXINSTRU ; i++)
	{
		for( x = 0; x < MadFile->fid[ i].numSamples ; x++)
		{
			sData	*curData = MadFile->sample[ i*MAXSAMPLE + x];
			
			fileSize += sizeof( sData);
			fileSize += curData->size;
		}
	}
	
	/*if( init->sysMemory) hRsrc = NewHandleSys( fileSize);
	else */hRsrc =  NewHandle( fileSize);
	if( hRsrc == NULL) return MADNeedMemory;
	
	tt = 0;
	HLock( hRsrc);
	
	/********* Write File *********/
	inOutCount = sizeof( MADSpec);
	BlockMoveData( MadFile->header, (*hRsrc) + tt, inOutCount);
	tt += inOutCount;
	
	for( i = 0; i < MadFile->header->numPat ; i++)
	{
		MadFile->partition[ i]->header.compMode = 'NONE';
		
		inOutCount = sizeof(  PatHeader);
		inOutCount += MadFile->header->numChn * MadFile->partition[ i]->header.size * sizeof( Cmd);
		
		BlockMoveData( MadFile->partition[ i], (*hRsrc) + tt, inOutCount);
		tt += inOutCount;
	}
	
	for( i = 0; i < MAXINSTRU ; i++)
	{
		if( MadFile->fid[ i].numSamples > 0 || MadFile->fid[ i].name[ 0] != 0)	// Is there something in this instrument?
		{	
			MadFile->fid[ i].no = i;
			
			inOutCount = sizeof( InstrData);
			BlockMoveData( &MadFile->fid[ i], (*hRsrc) + tt, inOutCount);
			tt += inOutCount;
		}
	}
	
	for( i = 0; i < MAXINSTRU ; i++)
	{
		for( x = 0; x < MadFile->fid[i].numSamples; x++)
		{
			sData	*curData = MadFile->sample[ i*MAXSAMPLE + x];
			
			inOutCount = sizeof( sData);
			BlockMoveData( curData, (*hRsrc) + tt, inOutCount);
			tt += inOutCount;
			
			inOutCount = curData->size;
			BlockMoveData( curData->data, (*hRsrc) + tt, inOutCount);
			tt += inOutCount;
		}
	}
	
	// EFFECTS *** *** *** *** *** *** *** *** *** *** *** ***
	{
	short alpha = 0;
	
	for( i = 0; i < 10 ; i++)	// Global Effects
	{
		if( MadFile->header->globalEffect[ i])
		{
			inOutCount = sizeof( FXSets);
			BlockMoveData( &MadFile->sets[ alpha], (*hRsrc) + tt, inOutCount);
			tt += inOutCount;
			alpha++;
		}
	}
	
	for( i = 0; i < MadFile->header->numChn ; i++)	// Channel Effects
	{
		for( x = 0; x < 4; x++)
		{
			if( MadFile->header->chanEffect[ i][ x])
			{
				inOutCount = sizeof( FXSets);
				BlockMoveData( &MadFile->sets[ alpha], (*hRsrc) + tt, inOutCount);
				tt += inOutCount;
				alpha++;
			}
		}
	}
	}
	/*******************************/
	
	MadFile->header->numInstru = MAXINSTRU;
	
	HUnlock( hRsrc);
	
	if( fileSize < tt) DebugStr("\pNeed more memory!");
	
	SetHandleSize( hRsrc, tt);
	
	AddResource( hRsrc, 'MADK', 3214, "\p");
	WriteResource( hRsrc);
	DetachResource( hRsrc);
	DisposeHandle( hRsrc);
	
	/**** On copie les ressources nŽcessaires pour l'application **/

	if( APPLType == 0 || APPLType == 2)
	{
		CopyResource( 'CODE', 100, 0);
		CopyResource( 'CODE', 101, 1);
		CopyResource( 'DATA', 100, 0);
	}
	
	CopyResource( 'BGGB', 128, 128);		// In PlayerPRO !!
	CopyResource( 'ALRT', 3215, 3215);
	CopyResource( 'DITL', 3214, 3214);
	CopyResource( 'DLOG', 3214, 3214);
	CopyResource( 'dctb', 3214, 3214);
	CopyResource( 'Prou', 0, 0);
	CopyResource( 'FREF', 228, 128);
	CopyResource( 'FREF', 229, 129);
	CopyResource( 'FREF', 230, 130);
	CopyResource( 'BNDL', 228, 128);
	CopyResource( 'WDEF', 200, 200);
	CopyResource( 'PICT', 140, 140);
	CopyResource( 'MENU', 128, 128);
	CopyResource( 'MENU', 129, 129);
	CopyResource( 'MENU', 131, 131);
	CopyResource( 'MENU', 132, 132);
	CopyResource( 'MENU', 133, 133);
	CopyResource( 'MENU', 165, 165);
	CopyResource( 'MENU', 169, 169);
	CopyResource( 'MENU', 170, 170);
	CopyResource( 'icl8', 228, 128);
	CopyResource( 'ICN#', 228, 128);
	CopyResource( 'ics#', 128, 128);
	
	hRsrc = GetResource( 'SIZE', 99);		DetachResource( hRsrc);
	AddResource( hRsrc, 'SIZE', -1, "\p");
	
	fileSize = 1024L * 1024L;
	*((long*)((*hRsrc)+2)) = (long) fileSize;
	*((long*)((*hRsrc)+6)) = (long) fileSize;
	
	WriteResource( hRsrc);					DetachResource( hRsrc);		DisposeHandle( hRsrc);

	if( APPLType == 1 || APPLType == 2)
	{
		hRsrc = GetResource( 'XXXX', 128);		
		inOutCount = GetResourceSizeOnDisk( hRsrc);
		DetachResource( hRsrc);
		HLock( hRsrc);
		iErr = FSWrite(fRefNum, &inOutCount, *hRsrc);
		HUnlock( hRsrc);
		DisposeHandle( hRsrc);
		
		CopyResource( 'cfrg', 100, 0);
	}
	
	return noErr;
}

static Boolean HasToClose( FSSpec *spec)
{
	CInfoPBRec	pb;							// 108-byte area
	HFileInfo	*fpb = (HFileInfo *)&pb;	// two pointer types
	DirInfo		*dpb = (DirInfo *)&pb;
	OSErr		rc;
	
	pb.hFileInfo.ioNamePtr 		= spec->name;
	pb.hFileInfo.ioVRefNum 		= spec->vRefNum;
	pb.hFileInfo.ioDirID 		= spec->parID;
	pb.hFileInfo.ioFDirIndex 	= 0;
	
	rc = PBGetCatInfo( &pb, false);
	if( rc != noErr) return false;
	
	if ( fpb->ioFlAttrib & 16)
	{
		return false;
	}
	else
	{
		if( fpb->ioFlAttrib & 4) return false;
		else return true;
	}
	
	return false;
}

/*****************/
/* MAIN FUNCTION */
/*****************/

#ifdef _SRC
OSErr mainAPPL( OSType order, Ptr AlienFileName, MADMusic *MadFile, PPInfoRec *info, MADDriverSettings *init)
#else
EXP OSErr main( OSType order, Ptr AlienFileName, MADMusic *MadFile, PPInfoRec *info, MADDriverSettings *init)
#endif


//OSErr main( OSType order, Ptr AlienFileName, MADMusic *MadFile, PPInfoRec *info, MADDriverSettings *init)
{
	OSErr		myErr;
	UNFILE		iFileRefI;
	short		i;
	Handle		myRes;
	Boolean		hasToClose;
	FSSpec		AlienFileFSSpec;
	
	HGetVol( NULL, &AlienFileFSSpec.vRefNum, &AlienFileFSSpec.parID);
	MYC2PStr( AlienFileName);
	for( i = 0; i <= AlienFileName[ 0]; i++) AlienFileFSSpec.name[ i] = AlienFileName[ i];
	
	if( order != 'EXPL') hasToClose = HasToClose( &AlienFileFSSpec);
	
	myErr = noErr;
	
	switch( order)
	{
		case 'IMPL':
			iFileRefI = FSpOpenResFile( &AlienFileFSSpec, fsRdPerm);
			if( iFileRefI == -1) myErr = MADUnknownErr;
			else
			{
				UseResFile( iFileRefI);
				
				if( Count1Resources( 'MADK') > 0)
				{
					myRes = Get1IndResource( 'MADK', 1);
					DetachResource( myRes);
					HLock( myRes);
					
					myErr = LoadMADH( *myRes, MadFile, init);
					
					HUnlock( myRes);
					DisposeHandle( myRes);
				}
				else myErr = MADUnknowErr;
				
				if( hasToClose) CloseResFile( iFileRefI);
			}
		break;
		
		case 'TEST':
			iFileRefI = FSpOpenResFile( &AlienFileFSSpec, fsRdWrPerm);	
			if( iFileRefI == -1) myErr = MADUnknowErr;
			else
			{
				UseResFile( iFileRefI);
				
				if( Count1Resources( 'MADK') > 0)
				{
					myRes = Get1IndResource( 'MADK', 1);
					DetachResource( myRes);
					
					HLock( myRes);
					
					myErr = TESTMADH( (MADSpec*) *myRes);
					
					HUnlock( myRes);
					DisposeHandle( myRes);
				}
				else myErr = MADUnknowErr;
				
				if( hasToClose) CloseResFile( iFileRefI);
			}
		break;
		
		case 'EXPL':
			{
				short	APPLType;
				short	fRefNum;
				
				APPLType = 1;	//ChooseCompilation();
				if( APPLType == -1) break;
				FSpDelete( &AlienFileFSSpec);
				FSpCreate( &AlienFileFSSpec, 'Prou', 'APPL', 0);
				myErr = FSpOpenDF( &AlienFileFSSpec, fsRdWrPerm, &fRefNum);
				if( myErr == noErr)
				{
					FSpCreateResFile( &AlienFileFSSpec, 'Prou', 'APPL', 0);
					iFileRefI = FSpOpenResFile( &AlienFileFSSpec, fsRdWrPerm);
					if( iFileRefI == -1) myErr = MADUnknowErr;
					else
					{
						UseResFile( iFileRefI);
						
						myErr = SaveAPPL( APPLType, fRefNum, MadFile, init);
						
						CloseResFile( iFileRefI);
					}
					FSCloseFork( fRefNum);
				}
			}
		break;

		case 'INFO':
			iFileRefI = FSpOpenResFile( &AlienFileFSSpec, fsRdWrPerm);
			if( iFileRefI == -1) myErr = MADUnknowErr;
			else
			{
				UseResFile( iFileRefI);
				
				if( Count1Resources( 'MADK') > 0)
				{
					myRes = Get1IndResource( 'MADK', 1);
					info->fileSize = GetResourceSizeOnDisk( myRes);
					
					DetachResource( myRes);
					HLock( myRes);
					
					myErr = INFOMADF( (MADSpec*) *myRes, info);
					
					HUnlock( myRes);
					DisposeHandle( myRes);
				}
				else myErr = MADUnknowErr;
				
				if( hasToClose) CloseResFile( iFileRefI);
			}
		break;
		
		default:
			myErr = MADOrderNotImplemented;
		break;
	}
	
	MYP2CStr( (unsigned char*) AlienFileName);
	
	return myErr;
}
