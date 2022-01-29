/*	XI				*/
/*  IMPORT/EXPORT	*/
/*	v 1.0			*/
/*	1996 by ANR		*/

#include "PPPlug.h"
#include "sound.h"

#if defined(powerc) || defined(__powerc)
enum {
		PlayerPROPlug = kCStackBased
		| RESULT_SIZE(SIZE_CODE( sizeof(OSErr)))
		| STACK_ROUTINE_PARAMETER(1, SIZE_CODE(sizeof( OSType)))
		| STACK_ROUTINE_PARAMETER(2, SIZE_CODE(sizeof( InstrData*)))
		| STACK_ROUTINE_PARAMETER(3, SIZE_CODE(sizeof( sData**)))
		| STACK_ROUTINE_PARAMETER(4, SIZE_CODE(sizeof( short*)))
		| STACK_ROUTINE_PARAMETER(5, SIZE_CODE(sizeof( FSSpec*)))
		| STACK_ROUTINE_PARAMETER(6, SIZE_CODE(sizeof( PPInfoPlug*)))
};

ProcInfoType __procinfo = PlayerPROPlug;
#else
#include <A4Stuff.h>
#endif

#if defined(powerc) || defined(__powerc)
#pragma options align=mac68k
#endif

#define UWORD unsigned short
#define ULONG unsigned long
#define UBYTE Byte
#define BYTE char
#define BOOL Boolean

#define HEADERSIZE 	276
#define PHSIZE		9
#define IHSIZE		263
#define IHSIZESMALL	33
#define IHSSIZE		40

typedef struct XMPATCHHEADER{
	
	UBYTE what[96];         // (byte) Sample number for all notes
	UWORD volenv[24];       // (byte) Points for volume envelope
	UBYTE panenv[48];       // (byte) Points for panning envelope
	UBYTE volpts;           // (byte) Number of volume points
	UBYTE panpts;           // (byte) Number of panning points
	UBYTE volsus;           // (byte) Volume sustain point
	UBYTE volbeg;           // (byte) Volume loop start point
	UBYTE volend;           // (byte) Volume loop end point
	UBYTE pansus;           // (byte) Panning sustain point
	UBYTE panbeg;           // (byte) Panning loop start point
	UBYTE panend;           // (byte) Panning loop end point
	UBYTE volflg;           // (byte) Volume type: bit 0: On; 1: Sustain; 2: Loop
	UBYTE panflg;           // (byte) Panning type: bit 0: On; 1: Sustain; 2: Loop
	UBYTE vibflg;           // (byte) Vibrato type
	UBYTE vibsweep;         // (byte) Vibrato sweep
	UBYTE vibdepth;         // (byte) Vibrato depth
	UBYTE vibrate;          // (byte) Vibrato rate
	UWORD volfade;          // (word) Volume fadeout
	UWORD reserved[11];     // (word) Reserved
} XMPATCHHEADER;


typedef struct XMWAVHEADER{
	ULONG length;           // (dword) Sample length
	ULONG loopstart;        // (dword) Sample loop start
	ULONG looplength;       // (dword) Sample loop length
	UBYTE volume;           // (byte) Volume
	BYTE finetune;          // (byte) Finetune (signed byte -128..+127)
	UBYTE type;                     // (byte) Type: Bit 0-1: 0 = No loop, 1 = Forward loop,
									// 2 = Ping-pong loop;
									// 4: 16-bit sampledata
	UBYTE panning;          // (byte) Panning (0-255)
	BYTE  relnote;          // (byte) Relative note number (signed byte)
	UBYTE reserved;         // (byte) Reserved
	char  samplename[22];   // (char) Sample name
} XMWAVHEADER;

#if defined(powerc) || defined(__powerc)
#pragma options align=reset
#endif

OSErr TestXI( Ptr CC)
{
	if( *((OSType*) CC) == 'Exte') return noErr;
	else return MADFileNotSupportedByThisPlug;
}

OSErr MAD2KillInstrument( InstrData *curIns, sData **sample)
{
short			i;
Boolean			IsReading;

	for( i = 0; i < curIns->numSamples; i++)
	{
		if( sample[ i] != 0L)
		{
			if( sample[ i]->data != 0L)
			{
				DisposePtr( (Ptr) sample[ i]->data);
				sample[ i]->data = 0L;
			}
			DisposePtr( (Ptr) sample[ i]);
			sample[ i] = 0L;
		}
	}
	
	
	for( i = 0; i < 32; i++) curIns->name[ i]	= 0;
	curIns->type		= 0;
	curIns->numSamples	= 0;
	
	/**/
	
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
	for( i = 0; i < 12; i++)
	{
		curIns->pitchEnv[ i].pos	= 0;
		curIns->pitchEnv[ i].val	= 0;
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
	
	curIns->volFade		= 0;
	curIns->vibDepth	= 0;
	curIns->vibRate		= 0;
	
	return noErr;
}

unsigned long Tdecode32( void *msg_buf)
{
  unsigned char *buf = msg_buf;
  
  return( (unsigned long) buf[3] << 24) | ( (unsigned long) buf[2] << 16) | ( (unsigned long) buf[ 1] << 8) | ( (unsigned long) buf[0]);
}

short Tdecode16( void *msg_buf)
{
  unsigned char *buf = msg_buf;
  
  return ( (short) buf[1] << 8) | ( (short) buf[0]);
}

OSErr main(		OSType					order,						// Order to execute
				InstrData				*InsHeader,					// Ptr on instrument header
				sData					**sample,					// Ptr on samples data
				short					*sampleID,					// If you need to replace/add only a sample, not replace the entire instrument (by example for 'AIFF' sound)
																	// If sampleID == -1 : add sample else replace selected sample.
				FSSpec					*AlienFileFSSpec,			// IN/OUT file
				PPInfoPlug				*thePPInfoPlug)
{
	OSErr	myErr;
	short	iFileRefI, x;
	long	inOutCount;
	
	#ifndef powerc
		long	oldA4 = SetCurrentA4(); 			//this call is necessary for strings in 68k code resources
	#endif
	
	switch( order)
	{
		case 'IMPL':
		{
			Ptr				theXI;
			XMPATCHHEADER	*pth;
			XMWAVHEADER		*wh;
			short			numSamples;
			
			myErr = FSpOpenDF( AlienFileFSSpec, fsCurPerm, &iFileRefI);
			if( myErr == noErr)
			{
				GetEOF( iFileRefI, &inOutCount);
				
				theXI = NewPtr( inOutCount);
				if( theXI == 0L) myErr = MADNeedMemory;
				else
				{
					MAD2KillInstrument( InsHeader, sample);
					
					for( x = 0; x < 32; x++)
					{
						if( x < AlienFileFSSpec->name[ 0]) InsHeader->name[ x] = AlienFileFSSpec->name[ x + 1];
						else InsHeader->name[ x] = '\0';
					}
					
					GetEOF( iFileRefI, &inOutCount);
					
					myErr = FSRead( iFileRefI, &inOutCount, theXI);
					
					// READ instrument header
					
					pth = (XMPATCHHEADER*) (theXI + 0x42);
					
					numSamples = *((short*) (theXI + 0x42 + sizeof( XMPATCHHEADER)));
					InsHeader->numSamples = Tdecode16( &numSamples);
					
					pth->volfade 	= Tdecode16( &pth->volfade);
					
					BlockMoveData( pth->what, 		InsHeader->what, 	96);
					BlockMoveData( pth->volenv, 	InsHeader->volEnv, 	48);
					for( x = 0; x < 12; x++)
					{
						InsHeader->volEnv[ x].pos = Tdecode16( &InsHeader->volEnv[ x].pos);	// 
						InsHeader->volEnv[ x].val = Tdecode16( &InsHeader->volEnv[ x].val);	// 00...64
					}
					
					InsHeader->volSize	= pth->volpts;
					InsHeader->volType	= pth->volflg;
					InsHeader->volSus	= pth->volsus;
					InsHeader->volBeg	= pth->volbeg;
					InsHeader->volEnd	= pth->volend;
					InsHeader->volFade	= pth->volfade;
					
					BlockMoveData( pth->panenv, InsHeader->pannEnv, 	48);
					for( x = 0; x < 12; x++)
					{
						InsHeader->pannEnv[ x].pos = Tdecode16( &InsHeader->pannEnv[ x].pos);	// 
						InsHeader->pannEnv[ x].val = Tdecode16( &InsHeader->pannEnv[ x].val);	// 00...64
					}
					
					InsHeader->pannSize	= pth->panpts;
					InsHeader->pannType	= pth->panflg;
					InsHeader->pannSus	= pth->pansus;
					InsHeader->pannBeg	= pth->panbeg;
					InsHeader->pannEnd	= pth->panend;
					
					// Read SAMPLE HEADERS
					
					for( x = 0; x < InsHeader->numSamples; x++)
					{
						sData	*curData;
						long	size, i;
						long 	finetune[ 16] = 
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
						if( wh->type & 0x10)		// 16 Bits
						{
							curData->amp = 16;
						}
						if( !(wh->type & 0x3))
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
							
							curData->data = NewPtr( curData->size);
							if( curData->data != 0L)
							{
								BlockMoveData( reader, curData->data, curData->size);
								
								if( curData->amp == 16)
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
				
				FSClose( iFileRefI);
			}
		}
		break;
		
		case 'TEST':
		{
			Ptr	theSound;
			
			myErr = FSpOpenDF( AlienFileFSSpec, fsCurPerm, &iFileRefI);
			if( myErr == noErr)
			{
				inOutCount = 50L;
				theSound = NewPtr( inOutCount);
				if( theSound == 0L) myErr = MADNeedMemory;
				else
				{
					FSRead( iFileRefI, &inOutCount, theSound);
					
					myErr = TestXI( (Ptr) theSound);
				}
				
				DisposePtr( theSound);
				
				FSClose( iFileRefI);
			}
		}
		break;
		
		case 'EXPL':
			myErr = FSpCreate( AlienFileFSSpec, 'SNPL', 'XI  ', smCurrentScript);
			myErr = FSpOpenDF( AlienFileFSSpec, fsCurPerm, &iFileRefI);
			
			if( myErr == noErr)
			{
				// Write instrument header
				
				short			u, v, p, i, x;
				long			inOutCount = 0;
				long			ihsizecopy, ihssizecopy;
				XMPATCHHEADER	pth;
				char			start[ 0x42];
				
				BlockMoveData( "Extended Instrument:                       FastTracker v2.00   ", start, 0x42);
				
				inOutCount = 0x42;
				FSWrite( iFileRefI, &inOutCount, start);
				
				BlockMoveData( InsHeader->what, pth.what, 96);
				BlockMoveData( InsHeader->volEnv, pth.volenv, 48);
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
				
				BlockMoveData( InsHeader->pannEnv, pth.panenv, 48);
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
				FSWrite( iFileRefI, &inOutCount, &pth);
				
				inOutCount = 2;
				x = InsHeader->numSamples;
				x = Tdecode16( &x);
				FSWrite( iFileRefI, &inOutCount, &x);
				
				/** WRITE samples */
				
				for( u = 0 ; u < InsHeader->numSamples ; u++)
				{
					XMWAVHEADER		wh;
					sData			*curData;
					long 	finetune[ 16] = 
					{
						7895,	7941,	7985,	8046,	8107,	8169,	8232,	8280,
						8363,	8413,	8463,	8529,	8581,	8651,	8723,	8757
					};
					
					curData = sample[ u];
					
					if( curData->stereo) wh.length = curData->size / 2;
					else wh.length = curData->size;
					
					inOutCount += wh.length;
					
					wh.loopstart	= curData->loopBeg;
					wh.looplength	= curData->loopSize;
					
					if( curData->stereo)
					{
						wh.loopstart /=2;
						wh.looplength /=2;
					}
					
					wh.volume		= curData->vol;
					
					wh.finetune = -128;
					if( curData->c2spd > 8757) wh.finetune = 127;
					else
					{
						while( finetune[ (wh.finetune + 128)/16] < curData->c2spd)
						{
							wh.finetune += 16;
						}
					}
					wh.type = 0;
					if( curData->amp == 16) wh.type += 0x10;
					if( curData->loopSize > 0) wh.type += 0x3;
					
				//	wh.panning = curData->panning;
					wh.relnote = curData->relNote;
					for( x = 0; x < 22; x++) wh.samplename[ x] = curData->name[ x];
					
					wh.length 		= Tdecode32( &wh.length);
					wh.loopstart 	= Tdecode32( &wh.loopstart);
					wh.looplength 	= Tdecode32( &wh.looplength);
					
					inOutCount = sizeof( wh);
					FSWrite( iFileRefI, &inOutCount, &wh);
				}
				
				for( u = 0 ; u < InsHeader->numSamples ; u++)
				{
					sData 	*curData = sample[ u];
					Ptr		tempPtr;
					long	dstSize;
					
					tempPtr = NewPtr( curData->size);
						
					/// WriteData
					if( tempPtr != 0L)
					{
						BlockMoveData( curData->data, tempPtr, curData->size);
						
						dstSize = curData->size;
						
						if( curData->amp == 16)
						{
							short	*tt = (short*) tempPtr;
							long	tL;
							
							/* Real to Delta */
							long	oldV, newV;
							long	xL;
							
							if( curData->stereo)
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
								*(tt + tL) = Tdecode16( (Ptr) (tt + tL));
							}
						}
						else
						{
							/* Real to Delta */
							long	oldV, newV;
							long	xL;
							Ptr		tt = ( Ptr) tempPtr;
							
							if( curData->stereo)
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
						FSWrite( iFileRefI, &inOutCount, tempPtr);
						
						DisposePtr( tempPtr);
					}
				}
				
				FSClose( iFileRefI);
			}
		break;
		
		default:
			myErr = MADOrderNotImplemented;
		break;
	}
	
	#ifndef powerc
		SetA4( oldA4);
	#endif
	
	return myErr;
}
