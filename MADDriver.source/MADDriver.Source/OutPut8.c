/********************						***********************/
//
//	Player PRO 5.9 - DRIVER SOURCE CODE -
//
//	Library Version 5.9
//
//	To use with MAD Library for Mac: Symantec, CodeWarrior and MPW
//
//	Antoine ROSSET
//	20 Micheli-Du-Crest
//	1205 GENEVA
//	SWITZERLAND
//
//	COPYRIGHT ANTOINE ROSSET 1996, 1997, 1998, 1999, 2000, 2001, 2002
//
//	Thank you for your interest in PlayerPRO !
//
//	FAX:			(+41 22) 346 11 97
//	PHONE: 			(+41 79) 203 74 62
//	Internet: 		RossetAntoine@bluewin.ch
//
/********************						***********************/

#include "RDriver.h"
#include "RDriverInt.h"

void Sampler8in8AddPolyStereo( Channel *curVoice, register Ptr	ASCBuffer, short chanNo, MADDriverRec *intDriver)
{
	char 			*SndBuffer;
	long 			i = intDriver->ASCBUFFER, volChn;
	short			temp;
	Boolean			killSample = false;
	
	///
	long				aDD, aCC = curVoice->lAC, off;
	
#if defined(HAS_LONG_DOUBLE)
	{
		long double		temp = ((long double)AMIGA_CLOCKFREQ2) / (long double) (curVoice->period * (intDriver->DriverSettings.outPutRate>>16) * intDriver->DriverSettings.oversampling);
		aDD = temp * ( 1 << BYTEDIV);
	}
#else
	aDD = (AMIGA_CLOCKFREQ2 << BYTEDIV) / (curVoice->period * (intDriver->DriverSettings.outPutRate>>16) * intDriver->DriverSettings.oversampling);
#endif
	
	///
	
	if( curVoice->curPtr >= curVoice->maxPtr && curVoice->loopSize == 0) return;
	
	SndBuffer		= (char*) 	curVoice->curPtr;
	volChn			= DoVolPanning256( 3, curVoice, intDriver, true)/256;
	
	while( i-- > 0)
	{
		off = 2*(aCC>>BYTEDIV);
		if( (Ptr) SndBuffer + off >= curVoice->maxPtr)
		{
			if( curVoice->loopSize > 0) 
			{
				SndBuffer -= curVoice->loopSize;
				if( SndBuffer + off < curVoice->begPtr) SndBuffer = curVoice->begPtr - off;
			}
			else
			{
				killSample = true;
				break;
			}
		}
		temp = (volChn * *(SndBuffer + off)) >> 8;
		off++;
		temp += (volChn * *(SndBuffer + off)) >> 8;
		temp /= 2;
		
		*ASCBuffer	+= temp;			 aCC += aDD;
		ASCBuffer	+= chanNo;
	}
	
	if( !killSample) curVoice->curPtr = (Ptr) (SndBuffer + (long) 2*(aCC>>BYTEDIV));
	else
	{
		curVoice->samplePtr	= NULL;	curVoice->curPtr	= curVoice->maxPtr;
	}
	
	curVoice->lAC = aCC & ((1 << BYTEDIV) - 1);
}


void Sampler8in8AddPoly( Channel *curVoice, register Ptr	ASCBuffer, short chanNo, MADDriverRec *intDriver)
{
	char 			*SndBuffer;
	long 			i = intDriver->ASCBUFFER, volChn;
	Boolean			killSample = false;
	///
	long				aDD, aCC = curVoice->lAC, off;
	
#if defined(HAS_LONG_DOUBLE)
	{
		long double		temp = ((long double)AMIGA_CLOCKFREQ2) / (long double) (curVoice->period * (intDriver->DriverSettings.outPutRate>>16) * intDriver->DriverSettings.oversampling);
		aDD = temp * ( 1 << BYTEDIV);
	}
#else
	aDD = (AMIGA_CLOCKFREQ2 << BYTEDIV) / (curVoice->period * (intDriver->DriverSettings.outPutRate>>16) * intDriver->DriverSettings.oversampling);
#endif
	///
	
	if( curVoice->curPtr >= curVoice->maxPtr && curVoice->loopSize == 0) return;
	
	SndBuffer		= (char*) 	curVoice->curPtr;
	volChn			= DoVolPanning256( 3, curVoice, intDriver, true)/256;
	
	while( i-- > 0)
	{
		off = (aCC>>BYTEDIV);
		if( (Ptr) SndBuffer + off >= curVoice->maxPtr)
		{
			if( curVoice->loopSize > 0) 
			{
				SndBuffer -= curVoice->loopSize;
				if( SndBuffer + off < curVoice->begPtr) SndBuffer = curVoice->begPtr - off;
			}
			else
			{
				killSample = true;
				break;
			}
		}
		*ASCBuffer	+= (volChn * *(SndBuffer + off)) >> 8;	aCC += aDD;
		ASCBuffer	+= chanNo;
	}
	
	if( !killSample) curVoice->curPtr = (Ptr) (SndBuffer + (long) (aCC>>BYTEDIV));
	else
	{
		curVoice->samplePtr	= NULL;	curVoice->curPtr	= curVoice->maxPtr;
	}
	
	curVoice->lAC = aCC & ((1 << BYTEDIV) - 1);
}

void Sampler16in8AddPolyStereo( Channel *curVoice, register Ptr	ASCBuffer, short chanNo, MADDriverRec *intDriver)
{
	char 			*SndBuffer;
	long 			i = intDriver->ASCBUFFER, volChn;
	short			temp;
	Boolean			killSample = false;
	
	///
	long				aDD, aCC = curVoice->lAC, off;
	
#if defined(HAS_LONG_DOUBLE)
	{
		long double		temp = ((long double)AMIGA_CLOCKFREQ2) / (long double) (curVoice->period * (intDriver->DriverSettings.outPutRate>>16) * intDriver->DriverSettings.oversampling);
		aDD = temp * ( 1 << BYTEDIV);
	}
#else
	aDD = (AMIGA_CLOCKFREQ2 << BYTEDIV) / (curVoice->period * (intDriver->DriverSettings.outPutRate>>16) * intDriver->DriverSettings.oversampling);
#endif
	
	///
	
	if( curVoice->curPtr >= curVoice->maxPtr && curVoice->loopSize == 0) return;
	
	SndBuffer		= (char*) 	curVoice->curPtr;
	volChn			= DoVolPanning256( 3, curVoice, intDriver, true)/256;
	
	while( i-- > 0)
	{
		off = 4*(aCC>>BYTEDIV);
		if( (Ptr) SndBuffer + off >= curVoice->maxPtr)
		{
			if( curVoice->loopSize > 0) 
			{
				SndBuffer -= curVoice->loopSize;
				if( SndBuffer + off < curVoice->begPtr) SndBuffer = curVoice->begPtr - off;
			}
			else
			{
				killSample = true;
				break;
			}
		}
		temp = (volChn * *(SndBuffer + off)) >> 8;
		off+=2;
		temp += (volChn * *(SndBuffer + off)) >> 8;
		temp /= 2;
		
		*ASCBuffer	+= temp;			 aCC += aDD;
		ASCBuffer	+= chanNo;
	}
	
	if( !killSample) curVoice->curPtr = (Ptr) (SndBuffer + (long) 4*(aCC>>BYTEDIV));
	else
	{
		curVoice->samplePtr	= NULL;	curVoice->curPtr	= curVoice->maxPtr;
	}
	
	curVoice->lAC = aCC & ((1 << BYTEDIV) - 1);
}


void Sampler16in8AddPoly( Channel *curVoice, register Ptr	ASCBuffer, short chanNo, MADDriverRec *intDriver)
{
	char 			*SndBuffer;
	long 			i = intDriver->ASCBUFFER, volChn;
	Boolean			killSample = false;
	///
	long				aDD, aCC = curVoice->lAC, off;
	
#if defined(HAS_LONG_DOUBLE)
	{
		long double		temp = ((long double)AMIGA_CLOCKFREQ2) / (long double) (curVoice->period * (intDriver->DriverSettings.outPutRate>>16) * intDriver->DriverSettings.oversampling);
		aDD = temp * ( 1 << BYTEDIV);
	}
#else
	aDD = (AMIGA_CLOCKFREQ2 << BYTEDIV) / (curVoice->period * (intDriver->DriverSettings.outPutRate>>16) * intDriver->DriverSettings.oversampling);
#endif
	///
	
	if( curVoice->curPtr >= curVoice->maxPtr && curVoice->loopSize == 0) return;
	
	SndBuffer		= (char*) 	curVoice->curPtr;
	volChn			= DoVolPanning256( 3, curVoice, intDriver, true)/256;
	
	while( i-- > 0)
	{
		off = 2*(aCC>>BYTEDIV);
		if( (Ptr) SndBuffer + off >= curVoice->maxPtr)
		{
			if( curVoice->loopSize > 0) 
			{
				SndBuffer -= curVoice->loopSize;
				if( SndBuffer + off < curVoice->begPtr) SndBuffer = curVoice->begPtr - off;
			}
			else
			{
				killSample = true;
				break;
			}
		}
		*ASCBuffer	+= (volChn * *(SndBuffer + off)) >> 8;			 aCC += aDD;
		ASCBuffer	+= chanNo;
	}
	
	if( !killSample) curVoice->curPtr = (Ptr) (SndBuffer + (long) 2*(aCC>>BYTEDIV));
	else
	{
		curVoice->samplePtr	= NULL;	curVoice->curPtr	= curVoice->maxPtr;
	}
	
	curVoice->lAC = aCC & ((1 << BYTEDIV) - 1);
}

void Sample8BufferAddPoly( Channel *curVoice, register Ptr ASCBuffer, short chanNo, MADDriverRec *intDriver)
{
	if( curVoice->stereo)
	{
		if( curVoice->amp == 8) Sampler8in8AddPolyStereo( curVoice, ASCBuffer, chanNo, intDriver);
		else Sampler16in8AddPolyStereo( curVoice, ASCBuffer, chanNo, intDriver);
	}
	else
	{
		if( curVoice->amp == 8) Sampler8in8AddPoly( curVoice, ASCBuffer, chanNo, intDriver);
		else Sampler16in8AddPoly( curVoice, ASCBuffer, chanNo, intDriver);
	}
}

void Play8PolyPhonic( MADDriverRec *intDriver)
{
	long	i;
	//long x;
	//Byte	*alpha = (Byte*) intDriver->IntDataPtr;
	
	//	x = intDriver->ASCBUFFER * intDriver->DriverSettings.numChn;
	//	while( x-- > 0) *alpha++ = 0x80;
	
	for( i = 0 ; i < intDriver->DriverSettings.numChn; i++)	//
	{
		Sample8BufferAddPoly( &intDriver->chan[ i], intDriver->IntDataPtr + i, intDriver->DriverSettings.numChn, intDriver);
	}
}
