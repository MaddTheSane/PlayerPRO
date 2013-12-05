#include "Shuddup.h"
#include "MAD.h"
#include "RDriver.h"
#include "RDriverInt.h"
#include <Carbon/Carbon.h>
#include <QuickTime/QuickTime.h>

extern	DialogPtr		MODListDlog;
extern	KeyMap		km;
extern	Boolean		PatchSave;
extern	WindowPtr		oldWindow;

Boolean OpenFirst2( short pos);
void MODListSelectThisMusic(Str255);
void PathNameFromDirID(long , short , StringPtr );
void OpenWorkingWindow( short NumberCalls);
Boolean DoWorkingWindow();
void OpenMODListSTCf( FSSpec);
void CreateOverShoot( short ACdriver);
void KillOverShoot( short ACdriver);
//Boolean IsCodeOK();
void ReconstructShowInfo();
Boolean PressMenuItem( short menuID, short itemID, DialogPtr dia, long *curVal, short remove, Str255	str);
void ConvertInstrumentIn( register	Byte	*tempPtr,	register long sSize);
void GetDigitalSelection( short *XStart, short *YStart, short *XEnd, short *YEnd, short *currentPat);
void FillVSTEffects( void);
void CheckVSTEditor( VSTEffect *ce);
void OtherIntErreur( short ID, OSErr theErr, Str255 otherstr);
OSErr ConvertMovieToMPEG4(FSSpec *inputFile, FSSpec *outputFile);
Boolean CreateAIFFExporting( Boolean OnlyCurrent, short  fRef, FSSpec *newFile, OSType type, FSSpec *dstFile);

static long				gCompressionID = 0;
extern MenuHandle		thePatternMenu;
extern DialogPtr		EditorDlog;

/*long OpenHeaderMOD( Ptr outPtr, Str255 name, long bytesToRead)
{
OSErr				theErr;
short				srcFile;
long					fileSize;

	theErr = FSpOpenDF( name, 0, &srcFile);
	if( theErr) return -1L;
	
	GetEOF( srcFile, &fileSize);

	theErr = FSRead( srcFile, &bytesToRead, outPtr);
	if( theErr) return -1L;
	
	FSClose( srcFile);
	
	return( fileSize);
}*/

Boolean OpenableFile( OSType theType, FSSpec *file)
{
	FSSpec	saved;
	Boolean	bol = false;
	char		tempC[ 5];
	
	HGetVol( NULL, &saved.vRefNum, &saved.parID);
	HSetVol( NULL, file->vRefNum, file->parID);
	
	switch( theType)
	{
		case 'sTAT':
		case 'STCf':
		case 'MADK':		
			return true;
		break;
		
		case 'APPL':
			OSType2Ptr( theType, tempC);
			if( MADPlugAvailable( gMADLib, tempC))
			{
				OSErr	err;
				
				MyP2CStr( file->name);
				err = PPTestFile( gMADLib, tempC, (Ptr) file->name);
				MyC2PStr( (Ptr) file->name);
				
				if( !err) bol = true;
			}
		break;
		
		default:
			OSType2Ptr( theType, tempC);
			bol = MADPlugAvailable( gMADLib, tempC);
			
			if( bol == false)	// Try a Quicktime Conversion
			{
				bol = QTTypeConversion( theType);
			}
		break;
	}
	
	HSetVol( NULL, saved.vRefNum, saved.parID);
	
	return bol;
}

void PathNameFromDirID(long dirID, short vRefNum, StringPtr fullPathName);

Boolean CheckFileType( FSSpec theSpec, OSType theType)
{
	Ptr				theFile;
	Boolean			Response = false;
	OSErr			err;
	FSSpec			saved;
	char			tempC[ 5];
	
	HGetVol( NULL, &saved.vRefNum, &saved.parID);
	HSetVol( NULL, theSpec.vRefNum, theSpec.parID);
	
	switch( theType)
	{
		case 'MADK':
			MyP2CStr( theSpec.name);
			if( CheckMADFile( (Ptr) theSpec.name) == noErr) Response = true;
			else Response = false;
			MyC2PStr( (Ptr) theSpec.name);
			break;
		
		default:
			MyP2CStr( theSpec.name);
			OSType2Ptr( theType, tempC);
			err = PPTestFile( gMADLib, tempC, (Ptr) theSpec.name);
			MyC2PStr( (Ptr) theSpec.name);
			
			if( err) Response = false;
			else Response = true;
			
			if( Response == false)	// Try to import it with Quicktime
			{
				Response = QTTestConversion( &theSpec, theType);
			}
		break;
	}

	HSetVol( NULL, saved.vRefNum, saved.parID);

	return Response;
}

Ptr ConvertCurrentMusicToPtr(void)
{
	short				i, x;
	long				inOutCount, tt, fileSize;
	Ptr					tempPtr;
	PatData*			PatMAD;

	fileSize = sizeof( MADSpec);
	for( i = 0; i < curMusic->header->numPat; i++) fileSize += sizeof( PatternHeader) + curMusic->header->numChn * curMusic->partition[ i]->header.size * sizeof( Cmd);
	for( i = 0; i < MAXINSTRU ; i++)
	{
		for( x = 0; x < curMusic->fid[i].numSamples; x++)
		{
			fileSize += curMusic->sample[ curMusic->fid[ i].firstSample + x]->size;
		}
	}
	tempPtr = MyNewPtr( fileSize);
	if( tempPtr == NULL) MyDebugStr( __LINE__, __FILE__, "Memory !!!");
	
	tt = 0;
	inOutCount = GetPtrSize( (Ptr) curMusic->header);
	BlockMoveData( curMusic->header, tempPtr, inOutCount);
	tt += inOutCount;
	
	for( i = 0; i < curMusic->header->numPat ; i++)
	{
		if( curMusic->partition[ i]->header.compMode == 'MAD1')
		{
			PatMAD = CompressPartitionMAD1( curMusic, curMusic->partition[ i]);
			inOutCount = PatMAD->header.patBytes + sizeof( PatternHeader);
	
			BlockMoveData( (Ptr) PatMAD, tempPtr + tt, inOutCount);
			tt += inOutCount;
			
			MyDisposePtr( (Ptr*) &PatMAD);
		}
		else
		{
			inOutCount = sizeof( PatternHeader) + curMusic->header->numChn * curMusic->partition[ i]->header.size * sizeof( Cmd);
			BlockMoveData( (Ptr) curMusic->partition[ i], tempPtr + tt, inOutCount);
			tt += inOutCount;
		}
	}

	for( i = 0; i < MAXINSTRU ; i++)
	{
		for( x = 0; x < curMusic->fid[i].numSamples; x++)
		{
			sData	*curData;
		
			curData = curMusic->sample[ curMusic->fid[ i].firstSample + x];
			
			BlockMoveData( curData->data, tempPtr + tt, curData->size);
			tt += curData->size;
		}
	}

	return tempPtr;
}

void InitSoundQualityExport( 	DialogPtr				aDialog,
						short 				*ChannelNo,
						OSType 				*CompressionType,
						Fixed 				*FrequenceSpeed,
						short 				*amplitude,
						short				*PatternID,
						MADDriverSettings 		*drive,
						Boolean				OnlyCurrent)
{
	short	i;
	Str255	str;
	long 	tempLong;
	Str255	str1, str2;
	
	if( *CompressionType == 'MPG4')
	{
		SetDText( aDialog, 34, "\pMPEG 4 - Quicktime");
	}
	else if( *CompressionType != 'NONE')
	{
		CompressionInfo		cp;
		
		GetCompressionName( *CompressionType, str1);
		
		str2[ 0] = 4;
		*((OSType*)(str2+1)) = *CompressionType;
		
		pStrcat( str2, "\p -");
		pStrcat( str2, str1);
		
		SetDText( aDialog, 34, str2);
		
		GetCompressionInfo( -1, *((OSType*)(str2+1)), 2, *amplitude, &cp);
		
		*amplitude = cp.bytesPerSample * 8;
	}
	else SetDText( aDialog, 34, "\pNo compression");
	
	if( EditorDlog) ActiveControl( 39, aDialog);
	else InactiveControl( 39, aDialog);
	
	if( OnlyCurrent)
	{
		*PatternID = -2;
		//drive->MicroDelaySize = 0;
		InactiveControl( 37, aDialog);
		InactiveControl( 38, aDialog);
	}
	
	// Music
	if( *PatternID == -1)
	{
		TurnRadio( 37, aDialog, true);
		TurnRadio( 38, aDialog, false);
		TurnRadio( 39, aDialog, false);
		
		SetDText( aDialog, 6, "\p");
	}
	else if( *PatternID == -2)
	{
		TurnRadio( 37, aDialog, false);
		TurnRadio( 38, aDialog, false);
		TurnRadio( 39, aDialog, true);
		
		SetDText( aDialog, 6, "\p");
	}
	else
	{
		TurnRadio( 38, aDialog, true);
		TurnRadio( 37, aDialog, false);
		TurnRadio( 39, aDialog, false);
		
		pStrcpy( str, "\pID: ");
		
		NumToString( *PatternID, str2);	pStrcat( str, str2);
		
		
		strcpy( (Ptr) str2, curMusic->partition[ *PatternID]->header.name);
		MyC2PStr( (Ptr) str2);
		
		if( str2[ 0] > 0) pStrcat( str, "\p - ");
		
		pStrcat( str, str2);
		
		SetDText( aDialog, 6, str);
	}
	
	// Mode
	
	if( *ChannelNo == 2)
	{
		TurnRadio( 16, aDialog, true);
		TurnRadio( 17, aDialog, false);
	}
	else
	{
		TurnRadio( 16, aDialog, false);
		TurnRadio( 17, aDialog, true);
	}
	
	// Amplitude
	
	for( i = 12; i <= 14; i++) TurnRadio( i, aDialog, false);
	
	switch( *amplitude)
	{
		case 8:			TurnRadio( 12, aDialog, true);		break;
		case 16:		TurnRadio( 13, aDialog, true);		break;
	}
	
	// Frequence
	
	TurnRadio( 9, aDialog, false);		TurnRadio( 10, aDialog, false);
	TurnRadio( 11, aDialog, false);		TurnRadio( 3, aDialog, false);
	
	switch( *FrequenceSpeed)
	{
		case rate48khz:	TurnRadio( 3, aDialog, true);		break;
		case rate44khz:	TurnRadio( 11, aDialog, true);		break;
		case rate22050hz:	TurnRadio( 10, aDialog, true);		break;
		case rate11025hz:	TurnRadio( 9, aDialog, true);		break;
	}
	
	// OutPut Type
	
	if( drive->surround)		TurnRadio( 27, aDialog, true);
	else						TurnRadio( 27, aDialog, false);
	if( drive->MicroDelaySize)	TurnRadio( 19, aDialog, true);
	else						TurnRadio( 19, aDialog, false);
	if( drive->Reverb)			TurnRadio( 29, aDialog, true);
	else						TurnRadio( 29, aDialog, false);
	
	ActiveControl( 19, aDialog);
	ActiveControl( 29, aDialog);
	ActiveControl( 27, aDialog);
	
	NumToString( drive->MicroDelaySize, str);	pStrcat( str, "\p ms");	SetDText( aDialog, 8, str);
	NumToString( drive->ReverbSize, str);		pStrcat( str, "\p ms");	SetDText( aDialog, 28, str);
	NumToString( drive->ReverbStrength, str);	pStrcat( str, "\p %");	SetDText( aDialog, 18, str);
	
#if defined(HAS_LONG_LONG) && defined (HAS_LONG_DOUBLE)
	// OverSampling
	if( drive->oversampling > 1)
	{
		tempLong = (unsigned long) ((unsigned long) *FrequenceSpeed >> 16UL);
		//	tempLong = tempLong >> 16;
		tempLong *= (long) drive->oversampling;
		tempLong /= 1000;
		
		NumToString( tempLong, str1);
		pStrcat( str1, "\p Khz / ");
		
		NumToString( drive->oversampling, str2);
		pStrcat( str1, str2);
		pStrcat( str1, "\p x");
		
		TurnRadio( 23, aDialog, true);
	}
	else
	{
		pStrcpy( str1, "\pOff");
		TurnRadio( 23, aDialog, false);
	}
#else
	drive->oversampling = 1;
	pStrcpy( str1, "\pOff");
	TurnRadio( 23, aDialog, false);
	InactiveControl( 23, aDialog);
#endif
	SetDText( aDialog, 4, str1);
}

static short lastChannelChoice = 2;

Boolean SoundQualityExport( 	Boolean OnlyCurrent,
								short *ChannelNo,
								OSType *CompressionType,
								Fixed *FrequenceSpeed,
								short *amplitude,
								short *PatternID,
								MADDriverSettings 		*driver,
								Boolean	MPG4)
{
	DialogPtr	aDialog;
	short		itemType, itemHit, i, currentPattern;
	Str255		aStr;
	long		mresult;
	Point		myPt;
	Rect		itemRect;
	Handle		itemHandle;
	
	currentPattern = 0;
	
	GetDigitalSelection( NULL,NULL,NULL,NULL,&currentPattern);
	
	aDialog = GetNewDialog( 160, NULL, (WindowPtr) -1L);
	SetPortDialogPort( aDialog);
	
	SetCursor( GetQDGlobalsArrow( &qdarrow));
	
	InactiveControl( 14, aDialog);
	
	//	*ChannelNo = lastChannelChoice;
	*CompressionType = 'NONE';
	*PatternID = -1;
	
	if( MPG4)
	{
		*CompressionType = 'MPG4';
	}
	
	InitSoundQualityExport( aDialog, ChannelNo,  CompressionType,  FrequenceSpeed,  amplitude, PatternID, driver, OnlyCurrent);
	
	do
	{
		//ModalDialog( MyDlgFilterDesc, &itemHit);
		MyModalDialog( aDialog, &itemHit);
		
		switch( itemHit)
		{
			case 37:
				*PatternID = -1;
				InitSoundQualityExport( aDialog, ChannelNo,  CompressionType,  FrequenceSpeed,  amplitude, PatternID, driver, OnlyCurrent);
				break;
				
			case 38:
				*PatternID = currentPattern;
				InitSoundQualityExport( aDialog, ChannelNo,  CompressionType,  FrequenceSpeed,  amplitude, PatternID, driver, OnlyCurrent);
				break;
				
			case 39:
				*PatternID = -2;
				InitSoundQualityExport( aDialog, ChannelNo,  CompressionType,  FrequenceSpeed,  amplitude, PatternID, driver, OnlyCurrent);
				break;
				
			case 15:	// Menu Patterns
			case 6:
				UpdatePatternMenu();
				InsertMenu( thePatternMenu, hierMenu);
				
				SetItemMark( thePatternMenu, currentPattern + 1, 0xa5);
				
				GetDialogItem( aDialog, itemHit, &itemType, &itemHandle, &itemRect);
				myPt.v = itemRect.top;	myPt.h = itemRect.left;
				LocalToGlobal( &myPt);
				
				mresult = PopUpMenuSelect(	thePatternMenu,
										  myPt.v,
										  myPt.h,
										  currentPattern + 1);
				
				SetItemMark( thePatternMenu, currentPattern + 1, 0);
				
				if ( HiWord(mresult ) != 0 )
				{
					currentPattern = (Byte) LoWord( mresult) - 1;
					
					*PatternID = currentPattern;
					
					InitSoundQualityExport( aDialog, ChannelNo,  CompressionType,  FrequenceSpeed,  amplitude, PatternID, driver, OnlyCurrent);
				}
				DeleteMenu( GetMenuID( thePatternMenu));
				break;
				
			case 32:
			case 34:
				if( !MPG4)
				{
					long			mresult, i, curSelec;
					Point			Zone;
					short			temp;
					MenuHandle		tMenu;
					Boolean			returnVal = false;
					
					short			itemType;
					Handle			itemHandle;
					Rect			itemRect;
					
					CompressionInfo	cp;
					
					GetDialogItem( aDialog, itemHit, &itemType, &itemHandle, &itemRect);
					
					tMenu = GetMenu( 177);
					InsertMenu( tMenu, hierMenu);
					
					Zone.h = itemRect.left;	Zone.v = itemRect.top;
					
					LocalToGlobal( &Zone);
					
					for( i = 1 ; i < CountMenuItems( tMenu); i++)
					{
						Str255	str;
						
						GetMenuItemText( tMenu, i+1, str);
						
						GetCompressionName( *((OSType*) (str+1)), aStr);
						
						{
							OSErr					iErr;
							SoundConverter			sc;
							SoundComponentData		inputFormat, outputFormat;
							
							outputFormat.flags = 0;
							outputFormat.format = *((OSType*) (str+1));
							outputFormat.numChannels = 1;	
							outputFormat.sampleSize = 16;
							outputFormat.sampleRate = rate44khz;
							outputFormat.sampleCount = 0;
							outputFormat.buffer = nil;
							outputFormat.reserved = 0;
							
							inputFormat = outputFormat;
							inputFormat.format = 'NONE';
							
							iErr = SoundConverterOpen(&inputFormat, &outputFormat, &sc);
							if( iErr) DisableMenuItem( tMenu, i+1);
							else
							{
								iErr = SoundConverterClose(sc);
								EnableMenuItem( tMenu, i+1);
							}
						}
						
						
						pStrcat( str, "\p -");
						pStrcat( str, aStr);
						
						SetMenuItemText( tMenu, i+1, str);
					}
					
					curSelec = 1;
					
					for( i = 0 ; i < CountMenuItems( tMenu); i++)
					{
						Str255	str;
						long	r;
						
						GetMenuItemText( tMenu, i+1, str);
						
						str[ 0] = 4;
						
						if( *((OSType*) (str+1)) == *CompressionType) curSelec = i+1;
					}
					
					SetItemMark( tMenu, curSelec, 0xa5);
					
					mresult = PopUpMenuSelect(	tMenu,
											  Zone.v,
											  Zone.h,
											  curSelec);
					
					SetItemMark( tMenu, curSelec, 0);
					
					if ( HiWord( mresult ) != 0 )
					{
						long	r;
						
						temp = LoWord( mresult );
						
						if( temp == 1)
						{
							*CompressionType = 'NONE';
						}
						else
						{
							GetMenuItemText( tMenu, temp, aStr);
							aStr[ 0] = 4;
							
							*CompressionType = *((OSType*) (aStr+1));
						}
						
						InitSoundQualityExport( aDialog, ChannelNo,  CompressionType,  FrequenceSpeed,  amplitude, PatternID, driver, OnlyCurrent);
					}
					
					
					
					DeleteMenu( GetMenuID( tMenu));
					DisposeMenu( tMenu);
				}
				break;
				
			case 24:
				PressMenuItem( 165, itemHit, aDialog, &driver->MicroDelaySize, 3, aStr);
				InitSoundQualityExport( aDialog, ChannelNo,  CompressionType,  FrequenceSpeed,  amplitude, PatternID, driver, OnlyCurrent);
				break;
				
#if defined(HAS_LONG_LONG) && defined (HAS_LONG_DOUBLE)
			case 23:
				if( driver->oversampling > 1) driver->oversampling = 1;
				else driver->oversampling = 4;
				
				InitSoundQualityExport( aDialog, ChannelNo,  CompressionType,  FrequenceSpeed,  amplitude, PatternID, driver, OnlyCurrent);
				break;
				
			case 20:
				PressMenuItem( 175, itemHit, aDialog, &driver->oversampling, 2, aStr);
				InitSoundQualityExport( aDialog, ChannelNo,  CompressionType,  FrequenceSpeed,  amplitude, PatternID, driver, OnlyCurrent);
				break;
#endif
				
			case 30:
				PressMenuItem( 170, itemHit, aDialog, &driver->ReverbSize, 3, aStr);
				InitSoundQualityExport( aDialog, ChannelNo,  CompressionType,  FrequenceSpeed,  amplitude, PatternID, driver, OnlyCurrent);
				break;
				
			case 31:
				PressMenuItem( 169, itemHit, aDialog, &driver->ReverbStrength, 2, aStr);
				InitSoundQualityExport( aDialog, ChannelNo,  CompressionType,  FrequenceSpeed,  amplitude, PatternID, driver, OnlyCurrent);
				break;
				
				/*	case 32:
				 driver->TickRemover = !driver->TickRemover;
				 InitSoundQualityExport( aDialog, ChannelNo,  CompressionType,  FrequenceSpeed,  amplitude, PatternID, driver);
				 break;*/
				
			case 27:
				driver->surround = !driver->surround;
				InitSoundQualityExport( aDialog, ChannelNo,  CompressionType,  FrequenceSpeed,  amplitude, PatternID, driver, OnlyCurrent);
				break;
				
			case 29:
				driver->Reverb = !driver->Reverb;
				InitSoundQualityExport( aDialog, ChannelNo,  CompressionType,  FrequenceSpeed,  amplitude, PatternID, driver, OnlyCurrent);
				break;
				
			case 19:
				if( driver->MicroDelaySize) driver->MicroDelaySize = 0;
				else driver->MicroDelaySize = 25;
				
				InitSoundQualityExport( aDialog, ChannelNo,  CompressionType,  FrequenceSpeed,  amplitude, PatternID, driver, OnlyCurrent);
				break;
				
				/*	case 20:
				 driver->Interpolation = !driver->Interpolation;
				 InitSoundQualityExport( aDialog, ChannelNo,  CompressionType,  FrequenceSpeed,  amplitude, PatternID, driver);
				 break;*/
				
			case 12:		// Amplitude
			case 13:
			case 14:
				switch( itemHit)
			{
				case 12:	*amplitude = 8;		break;
				case 13:	*amplitude = 16;		break;
			}
				InitSoundQualityExport( aDialog, ChannelNo,  CompressionType,  FrequenceSpeed,  amplitude, PatternID, driver, OnlyCurrent);
				break;
				
			case 10:	// Frequence
			case 9:
			case 8:
			case 11:
			case 3:
				switch( itemHit)
			{
				case 3:		*FrequenceSpeed = rate48khz;		break;
				case 11:	*FrequenceSpeed = rate44khz;		break;
				case 10:	*FrequenceSpeed = rate22050hz;		break;
				case 9:		*FrequenceSpeed = rate11025hz;		break;
			}
				InitSoundQualityExport( aDialog, ChannelNo,  CompressionType,  FrequenceSpeed,  amplitude, PatternID, driver, OnlyCurrent);
				break;
				
			case 16:	// Stereo
				*ChannelNo = 2;
				InitSoundQualityExport( aDialog, ChannelNo,  CompressionType,  FrequenceSpeed,  amplitude, PatternID, driver, OnlyCurrent);
				break;
				
			case 17:	// Mono
				*ChannelNo = 1;
				InitSoundQualityExport( aDialog, ChannelNo,  CompressionType,  FrequenceSpeed,  amplitude, PatternID, driver, OnlyCurrent);
				break;
				
				/*	case 15:
				 case 16:
				 case 17:
				 case 4:
				 //	case 34:
				 switch( itemHit)
				 {
				 //	case 15:	*ChannelNo = MonoOutPut;			break;
				 //	case 16:	*ChannelNo = StereoOutPut;			break;
				 case 17:	*ChannelNo = DeluxeStereoOutPut;	break;
				 //	case 4:		*ChannelNo = PolyPhonic;			break;
				 //	case 34:	*ChannelNo = MultiFiles;			break;
				 }
				 InitSoundQualityExport( aDialog, ChannelNo,  CompressionType,  FrequenceSpeed,  amplitude, PatternID, driver);
				 break;*/
				
				/*	case 7:
				 case 5:
				 case 6:
				 switch( itemHit)
				 {
				 case 7:	*CompressionType = 'NONE';	break;
				 case 5:	*CompressionType = 'MAC3';	break;
				 case 6:	*CompressionType = 'MAC6';	break;
				 }
				 InitSoundQualityExport( aDialog, ChannelNo,  CompressionType,  FrequenceSpeed,  amplitude, PatternID, driver);
				 break;*/
		}
		
	}while( itemHit != 1 && itemHit != 2);
	
	DisposeDialog( aDialog);
	
	lastChannelChoice = *ChannelNo;
	
	if( itemHit == 1) return true;
	else return false;
}

void ExportFile( OSType theType, FSSpec *newFile)
{
	OSErr						iErr;
	short						alpha, fRefNum, i, x;
	long						inOutCount, tt;
	FInfo						fndrInfo;
	PatData*					PatMAD;
	char						theTypePtr[ 5];
	
	HSetVol( NULL, newFile->vRefNum, newFile->parID);
	/***/
	
	OSType2Ptr( theType, theTypePtr);
	
	if(theType == 'MADK' || theType == 'AIFF' ||theType == 'sfil' || theType == 'MADS')	// theType == 'MADC' || 
	{
		if( theType == 'sfil') FSpDelete( newFile);
		
		iErr = FSpOpenDF( newFile, fsCurPerm, &fRefNum);
		
		if( iErr != noErr)
		{
			iErr = FSpCreate( newFile, 'SNPL', theType, smSystemScript);
			iErr = FSpOpenDF( newFile, fsCurPerm, &fRefNum);
			if( iErr == noErr)
			{
				inOutCount = 1;
				iErr = FSWrite( fRefNum, &inOutCount, theTypePtr);
				SetFPos( fRefNum, fsFromStart, 0);
			}
			
		}
		
		if( iErr)
		{
			Erreur( 75, iErr);
			goto END;
		}
		
		FSpGetFInfo( newFile, &fndrInfo);
		if( theType == 'AIFF') fndrInfo.fdCreator = 'TVOD';
		else if( theType == 'MPG4') fndrInfo.fdCreator = 'TVOD';
		else if( theType == 'MADS') fndrInfo.fdCreator = 'TVOD';
		else if( theType == 'sfil') fndrInfo.fdCreator = 'movr';
		else fndrInfo.fdCreator = 'SNPL';
		
		if( theType == 'MADS') fndrInfo.fdType = 'AIFF';
		else fndrInfo.fdType = theType;
		
		FSpSetFInfo( newFile, &fndrInfo);
	}
	
	/***/
	
	MADCleanCurrentMusic( curMusic, MADDriver);
	
	curMusic->header->generalVol = MADDriver->VolGlobal;
	curMusic->header->ESpeed = MADDriver->VExt;
	curMusic->header->EPitch = MADDriver->FreqExt;
	
	curMusic->header->generalSpeed = MADDriver->VExt/100;
	curMusic->header->generalPitch = MADDriver->FreqExt/100;
	curMusic->header->generalPan = MADDriver->globPan;
	
	FillVSTEffects();
	
	switch( theType)
	{			
		case 'MADS':
			// Export to the MADS format
			
			iErr = SetupAIFFHeader(		fRefNum,
								   2, 
								   rate22khz, 
								   16, 
								   'MADS',
								   200,
								   50);
			
			if( iErr != noErr)
			{
				MyDebugStr( __LINE__, __FILE__, "");
			}
			
		case 'MADK':
			curMusic->header->MAD = 'MADK';
			
			// HEADER
			// We need to compute number of valid instruments !!! See above....
			
			for( i = 0, x = 0; i < MAXINSTRU; i++)
			{
				curMusic->fid[ i].no = i;
				
				if( curMusic->fid[ i].numSamples > 0 || curMusic->fid[ i].name[ 0] != 0)	// Is there something in this instrument?
				{
					x++;
				}
			}
			
			curMusic->header->numInstru = x;
			
			inOutCount = sizeof( MADSpec);
			ByteSwapMADSpec(curMusic->header);
			iErr = FSWrite( fRefNum, &inOutCount, curMusic->header);
			//Just in case...
			ByteSwapMADSpec(curMusic->header);
			if( iErr) Erreur( 75, iErr);
			
			// PATTERNS
			for( i = 0; i < curMusic->header->numPat ; i++)
			{
				if( thePrefs.MADCompression) curMusic->partition[ i]->header.compMode = 'MAD1';
				
				//	curMusic->partition[ i]->header.compMode = 'NONE';
				
				if( curMusic->partition[ i]->header.compMode == 'MAD1')
				{
					PatMAD = CompressPartitionMAD1( curMusic, curMusic->partition[ i]);
					inOutCount = PatMAD->header.patBytes + sizeof( PatternHeader);
					ByteSwapPatHeader(&PatMAD->header);
					iErr = FSWrite( fRefNum, &inOutCount, PatMAD);
					
					MyDisposePtr( (Ptr*) &PatMAD);
				}
				else
				{
					inOutCount = sizeof(  PatHeader);
					inOutCount += curMusic->header->numChn * curMusic->partition[ i]->header.size * sizeof( Cmd);
					ByteSwapPatHeader(&curMusic->partition[ i]->header);
					iErr = FSWrite( fRefNum, &inOutCount, curMusic->partition[ i]);
					//Just in Case...
					ByteSwapPatHeader(&curMusic->partition[ i]->header);

				}
			}
			
			// INSTRUMENTS
			
			for( i = 0; i < MAXINSTRU; i++)
			{
				if( curMusic->fid[ i].numSamples > 0 || curMusic->fid[ i].name[ 0] != 0)	// Is there something in this instrument?
				{
					curMusic->fid[ i].no = i;
					ByteSwapInstrData(&(curMusic->fid[ i]));
					inOutCount = sizeof( InstrData);
					iErr = FSWrite( fRefNum, &inOutCount, &curMusic->fid[ i]);
					//Just in case...
					ByteSwapInstrData(&(curMusic->fid[ i]));

				}
			}
			
			// SAMPLES
			
			for( i = 0; i < MAXINSTRU ; i++)
			{
				for( x = 0; x < curMusic->fid[i].numSamples; x++)
				{
					sData	*curData;
					
					curData = curMusic->sample[ curMusic->fid[i].firstSample + x];
					
					inOutCount = sizeof( sData);
					ByteSwapsData(curData);
					iErr = FSWrite( fRefNum, &inOutCount, curData);
					//Just in case...
					ByteSwapsData(curData);
					inOutCount = curData->size;
					iErr = FSWrite( fRefNum, &inOutCount, curData->data);
				}
			}
			
			// EFFECTS *** *** *** *** *** *** *** *** *** *** *** ***
			
			alpha = 0;
			for( i = 0; i < 10 ; i++)	// Global Effects
			{
				if( curMusic->header->globalEffect[ i])
				{
					inOutCount = sizeof( FXSets);
					iErr = FSWrite( fRefNum, &inOutCount, &curMusic->sets[ alpha]);
					alpha++;
				}
			}
			
			for( i = 0; i < curMusic->header->numChn ; i++)	// Channel Effects
			{
				for( x = 0; x < 4; x++)
				{
					if( curMusic->header->chanEffect[ i][ x])
					{
						inOutCount = sizeof( FXSets);
						iErr = FSWrite( fRefNum, &inOutCount, &curMusic->sets[ alpha]);
						alpha++;
					}
				}
			}
			
			// *** *** *** *** *** *** *** *** *** *** *** *** *** ***
			
			GetFPos( fRefNum, &tt);
			SetEOF( fRefNum, tt);
			
			if( theType == 'MADS')
			{
				SetFPos( fRefNum, fsFromStart, 0);
				
				iErr = SetupAIFFHeader(		fRefNum,
									   2, 
									   rate44khz, 
									   16, 
									   'MADS',
									   tt,
									   tt*4);
			}
			
			FSCloseFork( fRefNum);
			
			// Life continues...
			
			curMusic->header->numInstru = MAXINSTRU;
			break;
			
		case 'MPG4':
		{
			long 	version;
			OSErr 	result;
			FSSpec	spec;
			
			result = Gestalt(gestaltQuickTime,&version);
			if ((result == noErr) && (version >= 0x06008000))
			{
				pStrcpy( spec.name, "\pPlayerPRO.AIFF");
				
				iErr = FindFolder( kOnAppropriateDisk, kTemporaryFolderType, kCreateFolder, &spec.vRefNum, &spec.parID);	//kTemporaryFolderType
				if( iErr == noErr)
				{
					iErr = FSpDelete( &spec);
					iErr = FSpCreate( &spec, 'SNPL', 'AIFF', smSystemScript);
					iErr = FSpOpenDF( &spec, fsCurPerm, &fRefNum);
					
					CreateAIFFExporting( false, fRefNum, &spec, 'MPG4', newFile);
				}
			}
			else
			{
				Erreur( 108, -6999);
			}
		}
			break;
			
		case 'AIFF':
		case 'sfil':
			CreateAIFFExporting( false, fRefNum, newFile, theType, NULL);
			break;
			
		default:
			MyP2CStr( newFile->name);
			iErr = PPExportFile( gMADLib, theTypePtr, (Ptr) newFile->name, curMusic);
			MyC2PStr( (Ptr) newFile->name);
			break;
			
	}
	
END:
	
	
	
	ReconstructShowInfo();
	
	SetCursor( GetQDGlobalsArrow( &qdarrow));
}

void CheckInstrument(void)
{
	short	i, x;
	
	if( curMusic == NULL) MyDebugStr( __LINE__, __FILE__, "Header == NULL");
	
	if( curMusic->header->speed < 0) MyDebugStr( __LINE__, __FILE__, "curMusic->header->speed");
	if( curMusic->header->tempo < 0) MyDebugStr( __LINE__, __FILE__, "curMusic->header->tempo");
	if( curMusic->header->numPat < 1) MyDebugStr( __LINE__, __FILE__, "curMusic->header->numPat");
	
	for( i = 0; i < MAXINSTRU; i++)
	{
		for( x = 0; x < 96; x++)
		{
			if( curMusic->fid[ i].what[ x])
			{
				if( curMusic->fid[ i].what[ x] >= curMusic->fid[ i].numSamples)
				{
					curMusic->fid[ i].what[ x] = 0;
				}
			}
		}
		
		for( x = 0; x < curMusic->fid[ i].numSamples; x++)
		{
			sData	*curData = curMusic->sample[ curMusic->fid[i].firstSample + x];
			
			if( curData == NULL)		MyDebugStr( __LINE__, __FILE__, "CheckIns Err");
			if( curData->data == NULL)	MyDebugStr( __LINE__, __FILE__, "CheckIns Err");
			
			if( curData->size <= 0)
			{
				curData->loopBeg = 0;
				curData->loopSize = 0;
			}
			
			if( curData->loopSize < 0) curData->loopSize = 0;
			
			if( curData->loopBeg > curData->size)
			{
				curData->loopBeg = 0;
				curData->loopSize = 0;
			}
			
			if( curData->loopBeg + curData->loopSize > curData->size)
			{
				curData->loopBeg = 0;
				curData->loopSize = 0;
			}
		}
	}
}

Boolean	ImportFile( Str255	fName, short vRefNum, long parID, OSType	theType)
{
	short					iFileRefI = 0,fRefNum = 0, i, x;
	long					sndSize, inOutCount, z, oldTracks;
	OSErr					iErr;
	FInfo					fndrInfo;
	Ptr						tempPtr = NULL;
	FSSpec					aSpec;
	Boolean					IsReading, folderFailed = false;
	Handle					TempHandle = NULL;
	char					theTypePtr[ 5];
	Str255					MissingPlugs;
	
	if( curMusic != NULL)
	{
		thePrefs.previousSpec.generalPitch	= 252;
		thePrefs.previousSpec.generalSpeed	= 252;
		thePrefs.previousSpec.generalPan	= MADDriver->globPan;
		thePrefs.previousSpec.EPitch		= MADDriver->FreqExt;
		thePrefs.previousSpec.ESpeed		= MADDriver->VExt;
		
		thePrefs.previousSpec.generalVol	= MADDriver->VolGlobal;
		for( i = 0 ; i < MAXTRACK; i++) thePrefs.previousSpec.chanPan[ i] = curMusic->header->chanPan[ i];
		for( i = 0 ; i < MAXTRACK; i++) thePrefs.previousSpec.chanVol[ i] = curMusic->header->chanVol[ i];
		
		
		thePrefs.Previous_globalFXActive 	= curMusic->header->globalFXActive;
		for( i = 0 ; i < 10; i++) 			thePrefs.Previous_globalEffect[ i] 		= curMusic->header->globalEffect[ i];
		for( i = 0 ; i < MAXTRACK; i++)
		{
			for( x = 0 ; x < 4; x++)
			{
				thePrefs.Previous_chanEffect[ i][ x] 		= curMusic->header->chanEffect[ i][ x];
			}
		}
		for( i = 0 ; i < MAXTRACK; i++) 	thePrefs.Previous_chanBus[ i] 			= curMusic->header->chanBus[ i];
		
		BlockMoveData( curMusic->sets, &thePrefs.Previous_Sets, MAXTRACK * sizeof(FXSets));
//	}
	//thePrefs.previousSpec = *curMusic->header;
	
//	if( curMusic)
//	{
		if( GereChanged() != noErr) return false;
	}
	
	StopAIFFExporting();
	
	OSType2Ptr( theType, theTypePtr);
	
	iErr = noErr;
	
	pStrcpy( aSpec.name, fName);
	aSpec.vRefNum = vRefNum;
	aSpec.parID = parID;
	
	if( theType != 'Rsrc')
	{
		HSetVol( NULL, vRefNum, parID);
		iErr = FSpGetFInfo( &aSpec, &fndrInfo);
		if( iErr != noErr)
		{
			if( iErr == fnfErr) Erreur( 62, iErr);
			else Erreur( 44, iErr);
			return false;
		}
	}
	
	if( theType == 0)  theType = fndrInfo.fdType;
	
	/*if( IsCodeOK())
	 {
	 if( 	theType != 'STCf' && 
	 theType != 'sTAT' && 
	 theType != 'Rsrc' && 
	 theType != 'STrk' && 
	 theType != 'MADF' && 
	 theType != 'MADH' &&
	 theType != 'MADI' &&
	 theType != 'XM  ' &&
	 theType != 'S3M ')
	 {
	 Erreur( 81, -2);
	 return false;
	 }
	 }
	 else
	 {
	 CallPlug( 0);
	 }*/
	
	iErr = noErr;
	
	if( curMusic != NULL) oldTracks = curMusic->header->numChn;
	else oldTracks = 0;
	
	IsReading = MADDriver->Reading;
	MADDriver->Reading = false;
	
	//MADStopDriver( MADDriver);
	
	MADPurgeTrack( MADDriver);
	MADCleanDriver( MADDriver);
	
	DisableMenuItem( FileMenu, 3);
	
	switch( theType)
	{
		case 'Rsrc':
			if( vRefNum == -55)
			{
				TempHandle = GetResource( 'MADK', 128);
				if( TempHandle)
				{
					DetachResource( TempHandle);
					HLock( TempHandle);
					iErr = MADLoadMusicPtr( &curMusic, *TempHandle);
					HUnlock( TempHandle);
					MyDisposHandle( & TempHandle);
				}
				else MyDebugStr( __LINE__, __FILE__, "Fatal MEMORY ERROR 1: NEED MORE MEMORY !");
			}
			else
			{
				MADDisposeMusic( &curMusic, MADDriver);
				
				TempHandle = GetResource( 'MADK', vRefNum);
				if( TempHandle)
				{
					DetachResource( TempHandle);
					HLock( TempHandle);
					iErr = MADLoadMusicPtr( &curMusic, *TempHandle);
					HUnlock( TempHandle);
					MyDisposHandle( & TempHandle);
				}
				else MyDebugStr( __LINE__, __FILE__, "Fatal MEMORY ERROR 2: NEED MORE MEMORY !");
			}
			/***
			 
			 IL N'Y A PAS DE PURGE DE LA PRECEDENTE MUSIQUE :
			 
			 - PREMIER CHARGEMENT !!!
			 - MUSIQUE DE REMPLACEMENT EN CAS D'ERREUR
			 
			 ******/
			break;
			
		default:
			if( !MADPlugAvailable( gMADLib, theTypePtr))
			{
				if( MADMusicIdentifyFSp( gMADLib, theTypePtr, &aSpec) != noErr)
				{
					// Try to read this file with QUICKTIME !
					
					if( !CreateQTWindow( &aSpec))
					{
						// TEST SI PKZIP !
						
						/** TEST MEMOIRE :  Environ 1 fois la taille du fichier**/
						iErr = FSpOpenDF( &aSpec, fsCurPerm, &iFileRefI);
						GetEOF( iFileRefI, &sndSize);
						
						if( sndSize > 4)
						{
							OSType	type;
							
							sndSize = 4;
							iErr = FSRead( iFileRefI, &sndSize, &type);
							if( iErr == noErr)
							{
								if( type == 'PK\3\4')
								{
									Erreur( 106, 106);
								}
							}
						}
						
						FSCloseFork (iFileRefI);
					}
					else pStrcpy( lastLoadMODListName, fName);
					return false;
				}
				else
				{
					FInfo		fndrInfo;
					OSErr	iErr;
					
					theType = Ptr2OSType( theTypePtr);
					
					if( FSpGetFInfo( &aSpec, &fndrInfo) == noErr)
					{
						if( FSpSetFInfo( &aSpec, &fndrInfo) == noErr)		// write permission?
						{
							if( fndrInfo.fdType != theType)
							{
								if( InfoL( 60))
								{
									fndrInfo.fdType		= theType;
									fndrInfo.fdCreator	= 'SNPL';
									
									FSpSetFInfo( &aSpec, &fndrInfo);
								}
							}
						}
					}
				}
			}
			SetCursor( &watchCrsr);
			
			if( IsCodeOK())
			{
				if(	theType != 'STCf' && 
				   theType != 'sTAT' && 
				   theType != 'Rsrc' && 
				   theType != 'STrk' && 
				   theType != 'MADF' && 
				   theType != 'MADH' &&
				   theType != 'MADI' &&
				   theType != 'MADK' &&
				   theType != 'XM  ' &&
				   theType != 'S3M ')
				{
					Erreur( 81, -2);
					return false;
				}
			}
			else
			{
				CallPlug( 0);
			}
			
			MADDriver->curMusic = NULL;
			MADDisposeMusic( &curMusic, MADDriver);
			
			iErr = MADLoadMusicFSpFile( gMADLib, &curMusic, theTypePtr, &aSpec);
			
			SetCursor( GetQDGlobalsArrow( &qdarrow));
			break;
			
		case 'STCf':
			OpenMODListSTCf( aSpec);
			if( thePrefs.AutomaticOpen)
			{
				if( !OpenFirst2( 0)) folderFailed = true;
			}
			else folderFailed = true;
			break;
			
		/*case 'sTAT':
			OpenMODList2( fName, 0);
			if( thePrefs.AutomaticOpen)
			{
				if( !OpenFirst2( 0)) folderFailed = true;
			}
			else folderFailed = true;
			break;*/
			
		case 'MADK':
			
			EnableMenuItem( FileMenu, 3);
			
			if( CheckFileType( aSpec, 'MADK'))
			{
				MADDriver->curMusic = NULL;
				MADDisposeMusic( &curMusic, MADDriver);
				
				/** TEST MEMOIRE :  Environ 1 fois la taille du fichier**/
				iErr = FSpOpenDF( &aSpec, fsCurPerm, &iFileRefI);
				GetEOF( iFileRefI, &sndSize);
				FSCloseFork (iFileRefI);
				
				tempPtr = MyNewPtr( (sndSize * 3L) / 2L);
				if( tempPtr == NULL) { iErr = MADNeedMemory;  break; }
				else MyDisposePtr( & tempPtr);
				/****/
				
				iErr = MADLoadMusicFilePString( gMADLib, &curMusic, "MADK", fName);
				
				if( thePrefs.AutoCreator == true)
				{
					fndrInfo.fdCreator = 'SNPL';
					fndrInfo.fdType = 'MADK';
					FSpSetFInfo( &aSpec, &fndrInfo);
				}
				
				/*	if( thePrefs.ADAPuse)
				 {
				 LoadAdaptatorsRsrc( &aSpec);
				 }*/
			} 
			else iErr = MADFileNotSupportedByThisPlug;
			break;
	}
	
	//MADStartDriver( MADDriver);
	
	/**** Updates obligatoires *******/
	
	for( i = 0; i < curMusic->header->numChn; i++)
	{
		MADDriver->Active[ i] = true;
	}
	
	curMusic->header->numInstru = MAXINSTRU;
	
	if( iErr != noErr)
	{
		MADDriver->curMusic = NULL;
		MADDisposeMusic( &curMusic, MADDriver);
		
		MaxMem( &sndSize);
		//CompactMem( maxSize);
		
		IsReading = false;
		ImportFile( "\pUntitled", -55, 0, 'Rsrc');
		pStrcpy( fName, "\pUntitled");
		
		MADErreur( iErr);
	}
	
	if( curMusic == NULL) MyDebugStr( __LINE__, __FILE__, "curMusic == NULL");
	if( curMusic->header->MAD != 'MADK') MyDebugStr( __LINE__, __FILE__, "ERR");
	
	curMusic->originalFormat = theType;
	if( curMusic->originalFormat == 'Rsrc') curMusic->originalFormat = 'MADK';
	
	if( thePrefs.DontUseFilesMix)
	{
		curMusic->header->generalPitch	= 252;//thePrefs.previousSpec.generalPitch;
		curMusic->header->generalSpeed	= 252;//thePrefs.previousSpec.generalSpeed;
		curMusic->header->generalPan	= thePrefs.previousSpec.generalPan;
		curMusic->header->EPitch	= thePrefs.previousSpec.EPitch;
		curMusic->header->ESpeed	= thePrefs.previousSpec.ESpeed;
		
		curMusic->header->generalVol	= thePrefs.previousSpec.generalVol;
		for( i = 0 ; i < MAXTRACK; i++) curMusic->header->chanPan[ i] = thePrefs.previousSpec.chanPan[ i];
		for( i = 0 ; i < MAXTRACK; i++) curMusic->header->chanVol[ i] = thePrefs.previousSpec.chanVol[ i];
		
		
		curMusic->header->globalFXActive	= thePrefs.Previous_globalFXActive;
		for( i = 0 ; i < 10; i++) 			curMusic->header->globalEffect[ i] = thePrefs.Previous_globalEffect[ i];
		for( i = 0 ; i < MAXTRACK; i++)
		{
			for( x = 0 ; x < 4; x++)
			{
				curMusic->header->chanEffect[ i][ x] = thePrefs.Previous_chanEffect[ i][ x];
			}
		}
		for( i = 0 ; i < MAXTRACK; i++) 	curMusic->header->chanBus[ i] = thePrefs.Previous_chanBus[ i];
		
		BlockMoveData(  &thePrefs.Previous_Sets, curMusic->sets,MAXTRACK * sizeof(FXSets));
	}
	
	MADAttachDriverToMusic( MADDriver, curMusic, MissingPlugs);
	
	if( curMusic->header->numChn != oldTracks)
	{
		if( vRefNum == -55 && theType == 'Rsrc')
		{
			ChangeTracksNo( thePrefs.numChn);
			curMusic->hasChanged = false;
		}
		else
		{
			thePrefs.numChn = curMusic->header->numChn;
			SetUpTracksMenu();
			SetWindowEnviron();
		}
	}
	
	if( theType != 'sTAT' && theType != 'STCf')
	{
		pStrcpy( curMusic->musicFileName, fName);
		//pStrcpy( curMusic->musicFileName, fName);
		pStrcpy( lastLoadMODListName, fName);
		SetCurrentMOD( curMusic->musicFileName);
		
		curMusic->hasChanged = false;
		MADGetMusicStatus( MADDriver, &curMusic->fullTime, &curMusic->position);
	}
	
	/************************/
	CheckInstrument();
	MADReset( MADDriver);
	MADDriver->Reading = IsReading;
	
	if( theType != 'STCf' && theType != 'sTAT')
	{
		if( theType != 'Rsrc')
		{
			curvRefNum	= vRefNum;
			curparID		= parID;
			PatchSave		= false;
		}
		else HGetVol( NULL, &curvRefNum, &curparID);
	}
	
	UPDATE_Total();
	
	ResetUndo();
	ScanTime();
	
	
	MODListSelectThisMusic( fName);
	
	if( MissingPlugs[ 0] > 0)
	{
		OtherIntErreur( 107, 22, MissingPlugs);
	}
	
	if( iErr != noErr || folderFailed == true) return false;
	else
	{
		ShowCopyrightNote();
		return true;
	}
}
