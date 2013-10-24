/*	XI				*/
/*  IMPORT/EXPORT	*/
/*	v 1.0			*/
/*	1996 by ANR		*/

#include <PlayerPROCore/PlayerPROCore.h>
#include <PlayerPROCore/PPPlug.h>

#ifndef WIN32
//Windows Defines
typedef SInt16			WORD;
typedef UInt16			UWORD;
typedef SInt32			HRESULT;
typedef UInt32			ULONG;
typedef void*			LPVOID;
typedef SInt32			LONG;

typedef UInt16			UINT;
typedef Boolean			BOOL;
typedef UInt32			DWORD;
typedef UInt16			USHORT;
typedef SInt16			SHORT;
typedef FourCharCode	FOURCC;
typedef SInt8			BYTE;
#endif

typedef UInt8			UBYTE;

#include "XM.h"

static OSErr TestXI( Ptr CC)
{
	OSType Ident = *((OSType*) CC);
	PPBE32(&Ident);
	
	if (Ident == 'Exte') return noErr;
	else return MADFileNotSupportedByThisPlug;
}

static OSErr MAD2KillInstrument( InstrData *curIns, sData **sample)
{
	short			i;
//	Boolean			IsReading;

	for( i = 0; i < curIns->numSamples; i++)
	{
		if (sample[ i] != NULL)
		{
			if (sample[ i]->data != NULL)
			{
				free( sample[ i]->data);
				sample[ i]->data = NULL;
			}
			free( sample[ i]);
			sample[ i] = NULL;
		}
	}
	
	
	for( i = 0; i < 32; i++) curIns->name[ i]	= 0;
	curIns->type		= 0;
	curIns->numSamples	= 0;
	
	/**/
#if 1
	memset(curIns->what, 0, sizeof(curIns->what));
	memset(curIns->volEnv, 0, sizeof(curIns->volEnv));
	memset(curIns->pannEnv, 0, sizeof(curIns->pannEnv));
	memset(curIns->pitchEnv, 0, sizeof(curIns->pitchEnv));
#else
	for( i = 0; i < 96; i++) curIns->what[ i]		= 0;

	for( i = 0; i < 12; i++)
	{
		curIns->volEnv[ i].pos		= 0;
		curIns->volEnv[ i].val		= 0;
		
		curIns->pannEnv[ i].pos	= 0;
		curIns->pannEnv[ i].val	= 0;
		
		curIns->pitchEnv[ i].pos	= 0;
		curIns->pitchEnv[ i].val	= 0;

	}
#endif
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
	
	curIns->volFade		= 0;
	curIns->vibDepth	= 0;
	curIns->vibRate		= 0;
	
	return noErr;
}

#ifdef _MAC_H
#define Tdecode16(msg_buf) CFSwapInt16LittleToHost(*(short*)msg_buf)
#define Tdecode32(msg_buf) CFSwapInt32LittleToHost(*(int*)msg_buf)
#else
#ifdef __LITTLE_ENDIAN__
#define Tdecode16(msg_buf) *(short*)msg_buf
#define Tdecode32(msg_buf) *(int*)msg_buf
#else

static inline UInt32 Tdecode32( void *msg_buf)
{
	UInt32 toswap = *((UInt32*) msg_buf);
	PPLE32(&toswap);
	return toswap;
}

static inline UInt16 Tdecode16( void *msg_buf)
{
	UInt16 toswap = *((UInt16*) msg_buf);
	PPLE16(&toswap);
	return toswap;
}

#endif
#endif

//hack around the fact that there isn't an equivalent of CFStringGetMaximumSizeOfFileSystemRepresentation for CFURLs
static CFIndex getCFURLFilePathRepresentationLength(CFURLRef theRef, Boolean resolveAgainstBase)
{
	CFURLRef toDeref = theRef;
	if (resolveAgainstBase) {
		toDeref = CFURLCopyAbsoluteURL(theRef);
	}
	CFStringRef fileString = CFURLCopyFileSystemPath(toDeref, kCFURLPOSIXPathStyle);
	CFIndex strLength = CFStringGetMaximumSizeOfFileSystemRepresentation(fileString);
	CFRelease(fileString);
	if (resolveAgainstBase) {
		CFRelease(toDeref);
	}
	return strLength;
}

static OSErr mainXI(void						*unused,
			 OSType		order,						// Order to execute
			 InstrData	*InsHeader,					// Ptr on instrument header
			 sData		**sample,					// Ptr on samples data
			 short		*sampleID,					// If you need to replace/add only a sample, not replace the entire instrument (by example for 'AIFF' sound)
			 // If sampleID == -1 : add sample else replace selected sample.
			 CFURLRef	AlienFileCFURL,			// IN/OUT file
			 PPInfoPlug	*thePPInfoPlug)
{
	OSErr	myErr = noErr;
	UNFILE	iFileRefI;
	short	x;
	long	inOutCount;
		
	char *file = NULL;
	char *fileName = NULL;
	do{
		char *longStr = NULL;
		CFIndex pathLen = getCFURLFilePathRepresentationLength(AlienFileCFURL, TRUE);
		longStr = malloc(pathLen);
		if (!longStr) {
			return MADNeedMemory;
		}
		Boolean pathOK = CFURLGetFileSystemRepresentation(AlienFileCFURL, true, (unsigned char*)longStr, pathLen);
		if (!pathOK) {
			free(longStr);
			return MADReadingErr;
		}
		size_t StrLen = strlen(longStr);
		file = malloc(++StrLen);
		if (!file) {
			file = longStr;
			break;
		}
		strlcpy(file, longStr, StrLen);
		free(longStr);
	} while (0);
	do {
		char *FileNameLong = NULL;
		CFStringRef filenam = CFURLCopyLastPathComponent(AlienFileCFURL);
		CFIndex filenamLen = CFStringGetMaximumSizeForEncoding(CFStringGetLength(filenam), kCFStringEncodingMacRoman);
		filenamLen *= 2;
		size_t filenamshortlen = 0;
		FileNameLong = malloc(filenamLen);
		CFStringGetCString(filenam, FileNameLong, filenamLen, kCFStringEncodingMacRoman);
		CFRelease(filenam);
		filenamshortlen = strlen(FileNameLong);
		fileName = malloc(++filenamshortlen);
		if (!fileName) {
			fileName = FileNameLong;
			break;
		}
		strlcpy(fileName, FileNameLong, filenamshortlen);
		free(FileNameLong);
	} while (0);
	
	switch( order)
	{
		case 'IMPL':
		{
			Ptr				theXI;
			XMPATCHHEADER	*pth;
			XMWAVHEADER		*wh = NULL;
			short			numSamples;
			
			iFileRefI = iFileOpenRead(file);
			if (iFileRefI != NULL)
			{
				
				inOutCount = iGetEOF(iFileRefI);
				
				theXI = malloc( inOutCount);
				if (theXI == NULL) myErr = MADNeedMemory;
				else
				{
					MAD2KillInstrument( InsHeader, sample);
					
					memset(InsHeader->name, '\0', 32);
					
					for( x = 0; x < 32; x++)
					{
						if (fileName[x] == '\0') {
							break;
						}
						InsHeader->name[x] = fileName[x];
						
					}
					
					//inOutCount = iGetEOF(iFileRefI);
					
					iRead(inOutCount, theXI, iFileRefI);
					
					// READ instrument header
					
					pth = (XMPATCHHEADER*) (theXI + 0x42);
					
					numSamples = *((short*) (theXI + 0x42 + sizeof( XMPATCHHEADER)));
					InsHeader->numSamples = Tdecode16( &numSamples);
					
					pth->volfade 	= Tdecode16( &pth->volfade);
					
					memcpy(InsHeader->what, pth->what, 96);
					memcpy(InsHeader->volEnv, pth->volenv, 48);
#ifdef __BIG_ENDIAN__
					for( x = 0; x < 12; x++)
					{
//						InsHeader->volEnv[ x].pos = Tdecode16( &InsHeader->volEnv[ x].pos);	// 
//						InsHeader->volEnv[ x].val = Tdecode16( &InsHeader->volEnv[ x].val);	// 00...64
						PPLE16(&InsHeader->volEnv[x].pos);
						PPLE16(&InsHeader->volEnv[x].val);
					}
#endif
					
					InsHeader->volSize	= pth->volpts;
					InsHeader->volType	= pth->volflg;
					InsHeader->volSus	= pth->volsus;
					InsHeader->volBeg	= pth->volbeg;
					InsHeader->volEnd	= pth->volend;
					InsHeader->volFade	= pth->volfade;
					
					memcpy(InsHeader->pannEnv, pth->panenv, 48);
#ifdef __BIG_ENDIAN__
					for( x = 0; x < 12; x++)
					{
						//InsHeader->pannEnv[ x].pos = Tdecode16( &InsHeader->pannEnv[ x].pos);	//
						//InsHeader->pannEnv[ x].val = Tdecode16( &InsHeader->pannEnv[ x].val);	// 00...64
						PPLE16(&InsHeader->pannEnv[x].pos);
						PPLE16(&InsHeader->pannEnv[x].val);
					}
#endif
					
					InsHeader->pannSize	= pth->panpts;
					InsHeader->pannType	= pth->panflg;
					InsHeader->pannSus	= pth->pansus;
					InsHeader->pannBeg	= pth->panbeg;
					InsHeader->pannEnd	= pth->panend;
					
					// Read SAMPLE HEADERS
					
					for( x = 0; x < InsHeader->numSamples; x++)
					{
						sData	*curData;
						long	i;
						int 	finetune[ 16] =
						{
							7895,	7941,	7985,	8046,	8107,	8169,	8232,	8280,
							8363,	8413,	8463,	8529,	8581,	8651,	8723,	8757
						};
						
						wh = (XMWAVHEADER*) (theXI + 0x42 + 0x02 + sizeof( XMPATCHHEADER) + x * sizeof( XMWAVHEADER));
						
						wh->length 		= Tdecode32( &wh->length);
						wh->loopstart 	= Tdecode32( &wh->loopstart);
						wh->looplength 	= Tdecode32( &wh->looplength);
						
						///////////
						
						curData = sample[ x] = inMADCreateSample();
						
						curData->size		= wh->length;
						curData->loopBeg 	= wh->loopstart;
						curData->loopSize 	= wh->looplength;
						
						curData->vol		= wh->volume;
						curData->c2spd		= finetune[ (wh->finetune + 128)/16];
						curData->loopType	= 0;
						curData->amp		= 8;
						if (wh->type & 0x10)		// 16 Bits
						{
							curData->amp = 16;
						}
						if (!(wh->type & 0x3))
						{
							curData->loopBeg = 0;
							curData->loopSize = 0;
						}
						
					//	curData->panning	= wh->panning;
						curData->relNote	= wh->relnote;
						for( i = 0; i < 22; i++) curData->name[ i] = wh->samplename[ i];
					}
					
					// Read SAMPLE DATA
					{
						Ptr reader = (Ptr) wh;
						
						reader += sizeof( XMWAVHEADER);
						
						for( x = 0; x < InsHeader->numSamples; x++)
						{
							sData *curData = sample[ x];
							
							curData->data = malloc( curData->size);
							if (curData->data != NULL)
							{
								memcpy(curData->data, reader, curData->size);
								
								if (curData->amp == 16)
								{
									short	*tt;
									long	tL;
									
									tt = (short*) curData->data;

									for( tL = 0; tL < curData->size/2; tL++)
									{
										*(tt + tL) = Tdecode16( (Ptr) (tt + tL));
									}
									
									{
									/* Delta to Real */
									long	oldV, newV;
									long	xL;
									
									oldV = 0;
									
									for( xL = 0; xL < curData->size/2; xL++)
									{
										newV = tt[ xL] + oldV;
										tt[ xL] = newV;
										oldV = newV;
									}
									}
								}
								else
								{
									/* Delta to Real */
									long	oldV, newV;
									long	xL;
									
									oldV = 0;

									for( xL = 0; xL < curData->size; xL++)
									{
										newV = curData->data[ xL] + oldV;
										curData->data[ xL] = newV;
										oldV = newV;
									}
								}
							}
							
							reader += curData->size;
						}
					}
				}
				
				iClose( iFileRefI);
			}
		}
		break;
		
		case 'TEST':
		{
			Ptr	theSound;
			
			iFileRefI = iFileOpenRead( file);
			if (iFileRefI != NULL)
			{
				inOutCount = 50L;
				theSound = malloc( inOutCount);
				if (theSound == NULL) myErr = MADNeedMemory;
				else
				{
					iRead( inOutCount, theSound, iFileRefI);
					
					myErr = TestXI( (Ptr) theSound);
				}
				
				free( theSound);
				
				iClose( iFileRefI);
			}
		}
		break;
		
		case 'EXPL':
			iFileCreate( file, 'XI  ');
			iFileRefI = iFileOpenWrite( file);
			
			if (iFileRefI != NULL)
			{
				// Write instrument header
				
				short			u, /*v, p,*/ i, x;
				long			inOutCount = 0;
				//long			ihsizecopy, ihssizecopy;
				XMPATCHHEADER	pth;
				char			start[ 0x42];
				
				//FIXME: get the proper escape sequences
				//BlockMoveData( "Extended Instrument:                       FastTracker v2.00   ", start, 0x42);
				
				memcpy(start, "Extended Instrument:                       FastTracker v2.00   ", 0x42);
				
				inOutCount = 0x42;
				iWrite(inOutCount, start, iFileRefI);
				
				memcpy(pth.what, InsHeader->what, 96);
				memcpy(pth.volenv, InsHeader->volEnv, 48);
				for( x = 0; x < 24; x++)
				{
					pth.volenv[ x] = Tdecode16( &pth.volenv[ x]);
				}
				
				pth.volpts = InsHeader->volSize;
				pth.volflg = InsHeader->volType;
				pth.volsus = InsHeader->volSus;
				pth.volbeg = InsHeader->volBeg;
				pth.volend = InsHeader->volEnd;
				pth.volfade = InsHeader->volFade;
				pth.volfade 	= Tdecode16( &pth.volfade);
				
				memcpy(pth.panenv, InsHeader->pannEnv, 48);
				for( x = 0; x < 24; x++)
				{
					pth.panenv[ x] = Tdecode16( &pth.panenv[ x]);
				}
				
				pth.panpts = InsHeader->pannSize;
				pth.panflg = InsHeader->pannType;
				pth.pansus = InsHeader->pannSus;
				pth.panbeg = InsHeader->pannBeg;
				pth.panend = InsHeader->pannEnd;
				
				inOutCount = sizeof( XMPATCHHEADER);
				iWrite(inOutCount, (Ptr)&pth, iFileRefI);
				
				inOutCount = 2;
				x = InsHeader->numSamples;
				x = Tdecode16( &x);
				iWrite(inOutCount, (Ptr)&x, iFileRefI);
				
				/** WRITE samples */
				
				for( u = 0 ; u < InsHeader->numSamples ; u++)
				{
					XMWAVHEADER		wh;
					sData			*curData;
					int 	finetune[ 16] =
					{
						7895,	7941,	7985,	8046,	8107,	8169,	8232,	8280,
						8363,	8413,	8463,	8529,	8581,	8651,	8723,	8757
					};
					
					curData = sample[ u];
					
					if (curData->stereo) wh.length = curData->size / 2;
					else wh.length = curData->size;
					
					inOutCount += wh.length;
					
					wh.loopstart	= curData->loopBeg;
					wh.looplength	= curData->loopSize;
					
					if (curData->stereo)
					{
						wh.loopstart /=2;
						wh.looplength /=2;
					}
					
					wh.volume		= curData->vol;
					
					wh.finetune = -128;
					if (curData->c2spd > 8757) wh.finetune = 127;
					else
					{
						while( finetune[ (wh.finetune + 128)/16] < curData->c2spd)
						{
							wh.finetune += 16;
						}
					}
					wh.type = 0;
					if (curData->amp == 16) wh.type += 0x10;
					if (curData->loopSize > 0) wh.type += 0x3;
					
				//	wh.panning = curData->panning;
					wh.relnote = curData->relNote;
					for( x = 0; x < 22; x++) wh.samplename[ x] = curData->name[ x];
					
					wh.length 		= Tdecode32( &wh.length);
					wh.loopstart 	= Tdecode32( &wh.loopstart);
					wh.looplength 	= Tdecode32( &wh.looplength);
					
					inOutCount = sizeof( wh);
					iWrite(inOutCount, (Ptr)&wh, iFileRefI);
				}
				
				for( u = 0 ; u < InsHeader->numSamples ; u++)
				{
					sData 	*curData = sample[ u];
					char	*tempPtr;
					long	dstSize;
					
					tempPtr = malloc( curData->size);
						
					/// WriteData
					if (tempPtr != NULL)
					{
						memcpy(tempPtr, curData->data, curData->size);
						
						dstSize = curData->size;
						
						if (curData->amp == 16)
						{
							short	*tt = (short*) tempPtr;
							long	tL;
							
							/* Real to Delta */
							long	oldV, newV;
							long	xL;
							
							if (curData->stereo)
							{
								for( i = 0; i < dstSize; i++) tt[ i] = tt[ i*2];
								dstSize /= 2;
							}
							
							oldV = 0;
							
							for( xL = 0; xL < dstSize/2; xL++)
							{
								newV = tt[ xL];
								tt[ xL] -= oldV;
								oldV = newV;
							}
							
							for( tL = 0; tL < dstSize/2; tL++)
							{
								*(tt + tL) = Tdecode16( (void*) (tt + tL));
							}
						}
						else
						{
							/* Real to Delta */
							long	oldV, newV;
							long	xL;
							char	*tt = (char*) tempPtr;
							
							if (curData->stereo)
							{
								for( i = 0; i < dstSize; i++) tt[ i] = tt[ i*2];
								dstSize /= 2;
							}
							
							oldV = 0;
							
							for( xL = 0; xL < dstSize; xL++)
							{
								newV = tt[ xL];
								tt[ xL] -= oldV;
								oldV = newV;
							}
						}
						
						inOutCount = dstSize;
						iWrite( inOutCount, tempPtr, iFileRefI);
						
						
						free( tempPtr);
					}
				}
				
				iClose( iFileRefI);
			}
		break;
		
		default:
			myErr = MADOrderNotImplemented;
		break;
	}
	
	free(file);
	free(fileName);
		
	return myErr;
}

#define PLUGUUID CFUUIDGetConstantUUIDWithBytes(kCFAllocatorSystemDefault, 0x62, 0x08, 0x75, 0xF5, 0x1E, 0x38, 0x45, 0xEF, 0x9F, 0xBA, 0xAA, 0xE9, 0x29, 0x50, 0x2D, 0x63)
//620875F5-1E38-45EF-9FBA-AAE929502D63
#define PLUGINFACTORY XIFactory //The factory name as defined in the Info.plist file
#define PLUGMAIN mainXI //The old main function, renamed please

#include "CFPlugin-InstrBridge.c"
