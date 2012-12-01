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

#ifdef __APPLE__
#include <PlayerPROCore/PlayerPROCore.h>
#else
#include "RDriver.h"
#include "FileUtils.h"
#endif
#include "MOD.h"
#include "MADI.h"

#ifdef WIN32
#define strlcpy(dst, src, size) strncpy_s(dst, size, src, _TRUNCATE)
#endif

#if 0
Cmd* GetMADCommand( register short PosX, register short	TrackIdX, register PatData*	tempMusicPat)
{
	if( PosX < 0) PosX = 0;
	else if( PosX >= tempMusicPat->header.size) PosX = tempMusicPat->header.size -1;
		
	return( & (tempMusicPat->Cmds[ (tempMusicPat->header.size * TrackIdX) + PosX]));
}
#endif

enum
{
	ins 	= 1,
	note	= 2,
	cmd		= 4,
	argu	= 8,
	vol		= 16
};


static oldPatData* oldDecompressPartitionMAD1( oldMADSpec *header, oldPatData* myPat)
{
	oldPatData*				finalPtr;
	Byte 					*srcPtr;
	oldCmd					*myCmd;
	short					maxCmd;
	Byte					set;
	
	finalPtr = ( oldPatData*) calloc( sizeof( oldPatHeader) + myPat->header.size * header->numChn * sizeof( oldCmd), 1);
	if( finalPtr == NULL)
	{
		//DebugStr("\pMemory3 Bug");
		return NULL;
	}
	
	memcpy(finalPtr, myPat, sizeof( oldPatHeader));

	srcPtr = (Byte*) myPat->Cmds;
	myCmd = (oldCmd*) finalPtr->Cmds;
	maxCmd = finalPtr->header.size * header->numChn;
	
	/*** Decompression Routine ***/
	
	while( maxCmd != 0)
	{
		maxCmd--;
		
		myCmd->cmd 	= 0;
		myCmd->note = 0xFF;
		myCmd->arg 	= 0;
		myCmd->ins 	= 0;
		myCmd->vol 	= 0xFF;
		
		set = *srcPtr++;
		
		if( set & ins)	myCmd->ins = *srcPtr++;
		if( set & note)	myCmd->note = *srcPtr++;
		if( set & cmd)	myCmd->cmd = *srcPtr++;
		if( set & argu)	myCmd->arg = *srcPtr++;
		if( set & vol)	myCmd->vol = *srcPtr++;
		
		myCmd++;
	}
	
	return finalPtr;
}

static oldCmd* GetOldCommand( short PosX, short	TrackIdX, oldPatData*	tempMusicPat)
{
	if( PosX < 0) PosX = 0;
	else if( PosX >= tempMusicPat->header.size) PosX = tempMusicPat->header.size -1;
	
	return( &(tempMusicPat->Cmds[ (tempMusicPat->header.size * TrackIdX) + PosX]));
}

static void MOToldsData(struct oldsData * s) {
	PPBE32(&s->size);
	PPBE32(&s->loopBeg);
	PPBE32(&s->loopSize);
	PPBE16(&s->c2spd);
}

static void MOToldPatHeader(struct oldPatHeader * p) {
	PPBE32(&p->size);
	PPBE32(&p->compMode);
	PPBE32(&p->patBytes);
	PPBE32(&p->unused2); // this is probably superfluous, but who knows
}

static void MOToldEnvRec(struct oldEnvRec * e) {
	PPBE16(&e->pos);
	PPBE16(&e->val);
}

static void MOToldInstrData(struct oldInstrData * i) {
	int j;
	PPBE16(&i->firstSample);
	PPBE16(&i->numSamples);
	PPBE16(&i->volFade);
	for(j = 0; j < 12; j++){
		MOToldEnvRec(&i->volEnv[j]);
		MOToldEnvRec(&i->pannEnv[j]);
	}
}

static void MOToldMADSpec(oldMADSpec * m){
	PPBE32(&m->MAD);
	PPBE16(&m->speed);
	PPBE16(&m->tempo);
	PPBE32(&m->EPitch);
	PPBE32(&m->ESpeed);
}

static OSErr MADI2Mad( Ptr MADPtr, size_t size, MADMusic *theMAD, MADDriverSettings *init)
{
	short		i, x;
	long		inOutCount, OffSetToSample = 0, z;
	//OSErr		theErr = noErr;
	//Ptr			tempPtr = NULL;
	OSType MADType = 0;
	/*SInt32		finetune[16] =
	{
		8363,	8413,	8463,	8529,	8581,	8651,	8723,	8757,
		7895,	7941,	7985,	8046,	8107,	8169,	8232,	8280
	};*/
	
	
/**** Old MADH variables ****/
	
	oldMADSpec				*oldMAD;
	
	oldMAD = (oldMADSpec*) MADPtr;
	
	
/**** HEADER ****/
	MADType = oldMAD->MAD;
	PPBE32(&MADType);
	if( MADType != 'MADI') return MADFileNotSupportedByThisPlug;
	OffSetToSample += sizeof( oldMADSpec);
	PPBE32(&MADType);
	MOToldMADSpec(oldMAD);

	
// Conversion
	inOutCount = sizeof( MADSpec);
	theMAD->header = (MADSpec*) calloc( inOutCount, 1);
	if( theMAD->header == NULL) return MADNeedMemory;
	
	theMAD->header->MAD = 'MADK';
	
	memcpy(theMAD->header->name, oldMAD->name, 32);
	memcpy(theMAD->header->infos, oldMAD->infos, INFOSSIZE);
	theMAD->header->generalPan		=	oldMAD->generalPan;
	theMAD->header->MultiChanNo		=	oldMAD->MultiChanNo;
	theMAD->header->EPitch			=	oldMAD->EPitch;
	theMAD->header->ESpeed			=	oldMAD->ESpeed;
	theMAD->header->XMLinear		=	oldMAD->XMLinear;
	theMAD->header->MODMode			=	oldMAD->MODMode;
	theMAD->header->showCopyright	=	oldMAD->showCopyright;
	theMAD->header->generalPitch	=	oldMAD->generalPitch;
	theMAD->header->generalSpeed	=	oldMAD->generalSpeed;
	theMAD->header->generalVol		=	oldMAD->generalVol;
	theMAD->header->numPat			= 	oldMAD->numPat;
	theMAD->header->numChn			= 	oldMAD->numChn;
	theMAD->header->numPointers		= 	oldMAD->numPointers;
	theMAD->header->numInstru		=	oldMAD->numInstru;
	theMAD->header->numSamples		=	oldMAD->numSamples;
	memcpy(theMAD->header->oPointers, oldMAD->oPointers, 256);
	theMAD->header->speed			= 	oldMAD->speed;
	theMAD->header->tempo			= 	oldMAD->tempo;
	
	for( i = 0; i < MAXTRACK; i++) theMAD->header->chanPan[ i] = oldMAD->chanPan[ i];
	for( i = 0; i < MAXTRACK; i++) theMAD->header->chanVol[ i] = oldMAD->chanVol[ i];
	
	
	theMAD->sets = (FXSets*) calloc( MAXTRACK * sizeof(FXSets), 1);
	
	for( i = 0; i < MAXTRACK; i++) theMAD->header->chanBus[ i].copyId = i;
/**** Patterns *******/
	
	for( i = 0; i < oldMAD->numPat; i++)
	{
		struct oldPatData		*tempPat, *tempPat2;
		struct oldPatHeader		tempPatHeader;
		
		/** Lecture du header de la partition **/
		inOutCount = sizeof( struct oldPatHeader);
		memcpy(&tempPatHeader, MADPtr + OffSetToSample, inOutCount);
		
		/*************************************************/
		/** Lecture du header + contenu de la partition **/
		/*************************************************/
		
		MOToldPatHeader(&tempPatHeader);
		
		if( tempPatHeader.compMode == 'MAD1')
		{
			inOutCount = sizeof( oldPatHeader) + tempPatHeader.patBytes;
		}
		else 
		{
			inOutCount = sizeof( oldPatHeader) + oldMAD->numChn * tempPatHeader.size * sizeof( oldCmd);
		}
		
		tempPat = (struct oldPatData*) malloc( inOutCount);
		if( tempPat == NULL) //DebugStr("\pMemory Prob1");
			return MADNeedMemory;
		
		memcpy(tempPat, MADPtr + OffSetToSample, inOutCount);
		OffSetToSample += inOutCount;
		MOToldPatHeader(&tempPat->header);
		
		if( tempPat->header.compMode == 'MAD1')
		{
			tempPat2 = oldDecompressPartitionMAD1( oldMAD, tempPat);
			
			free( (Ptr) tempPat);
			
			tempPat = tempPat2;
		}
		
		/**************/
		/* CONVERSION */
		/**************/
		
		theMAD->partition[ i] = (PatData*) calloc( sizeof( PatHeader) + theMAD->header->numChn * tempPat->header.size * sizeof( Cmd), 1);
		if( theMAD->partition[ i] == NULL) return MADNeedMemory;
		
		theMAD->partition[ i]->header.size 		= tempPat->header.size;
		theMAD->partition[ i]->header.compMode 	= 'NONE';
		
		memcpy( theMAD->partition[ i]->header.name, tempPat->header.name, 20);
		
		theMAD->partition[ i]->header.patBytes = 0;		theMAD->partition[ i]->header.unused2 = 0;
		
		for( x = 0; x < theMAD->partition[ i]->header.size; x++)
		{
			for( z = 0; z < theMAD->header->numChn; z++)
			{
				struct oldCmd *oldCmd;
				Cmd	*aCmd;
				
				aCmd = GetMADCommand(  x,  z, theMAD->partition[ i]);
				
				oldCmd 	= GetOldCommand(	x,
										z,
										tempPat);
				
				aCmd->ins 		= oldCmd->ins;
				aCmd->note 		= oldCmd->note;
				aCmd->cmd 		= oldCmd->cmd;
				aCmd->arg 		= oldCmd->arg;
				aCmd->vol		= oldCmd->vol;
				aCmd->unused 	= oldCmd->unused;
			}
		}
		
		free( tempPat);
		tempPat = NULL;
	}
	for( i = theMAD->header->numPat; i < MAXPATTERN ; i++) theMAD->partition[ i] = NULL;
	
	/**** Instruments & Samples header *****/
	
	theMAD->fid = ( InstrData*) calloc( sizeof( InstrData) * (long) MAXINSTRU, 1);
	if( !theMAD->fid) return MADNeedMemory;
	
	theMAD->sample = ( sData**) calloc( sizeof( sData*) * (long) MAXINSTRU * (long) MAXSAMPLE, 1);
	if( !theMAD->sample) return MADNeedMemory;
	
	for( i = 0; i < oldMAD->numInstru; i++)
	{
		struct oldInstrData		oldIns;
		//InstrData	*curIns = &theMAD->fid[ i];
		short		d;
		
		/** Lecture des instruments **/
		inOutCount = sizeof( struct oldInstrData);
		memcpy( &oldIns, MADPtr + OffSetToSample, inOutCount);
		MOToldInstrData(&oldIns);
		OffSetToSample += inOutCount;
		
		d = oldIns.no;
		
		memcpy( theMAD->fid[ d].name, oldIns.name, 32);
		
		theMAD->fid[ d].type = oldIns.type;
		theMAD->fid[ d].numSamples = oldIns.numSamples;
		memcpy( theMAD->fid[ d].what, oldIns.what, 96);
		memcpy( theMAD->fid[ d].volEnv, oldIns.volEnv, 12 * sizeof( EnvRec));
		memcpy( theMAD->fid[ d].pannEnv, oldIns.pannEnv, 12 * sizeof( EnvRec));
		theMAD->fid[ d].volSize = oldIns.volSize;
		theMAD->fid[ d].pannSize = oldIns.pannSize;
		theMAD->fid[ d].volSus = oldIns.volSus;
		theMAD->fid[ d].volBeg = oldIns.volBeg;
		theMAD->fid[ d].volEnd = oldIns.volEnd;
		theMAD->fid[ d].pannSus = oldIns.pannSus;
		theMAD->fid[ d].pannBeg = oldIns.pannBeg;
		theMAD->fid[ d].pannEnd = oldIns.pannEnd;
		theMAD->fid[ d].volType = oldIns.volType;
		theMAD->fid[ d].pannType = oldIns.pannType;
		theMAD->fid[ d].volFade = oldIns.volFade;
		theMAD->fid[ d].vibDepth = oldIns.vibDepth;
		theMAD->fid[ d].vibRate = oldIns.vibRate;
	}
	
	for( i = 0; i < MAXINSTRU; i++) theMAD->fid[ i].firstSample = i * MAXSAMPLE;
	
	// Read Samples
	
	for( i = 0; i < MAXINSTRU ; i++)
	{
		for( x = 0; x < theMAD->fid[ i].numSamples; x++)
		{
			oldsData	 *oldcurData;
			sData		 *curData;
			
			oldcurData = (oldsData*) (MADPtr + OffSetToSample);
			MOToldsData(oldcurData);
			OffSetToSample += sizeof( oldsData);
			
			curData = theMAD->sample[ i*MAXSAMPLE + x] = (sData*) calloc( sizeof( sData), 1);
			
			curData->size		= oldcurData->size;
			curData->loopBeg 	= oldcurData->loopBeg;
			curData->loopSize 	= oldcurData->loopSize;
			curData->vol		= oldcurData->vol;
			curData->c2spd		= oldcurData->c2spd;
			curData->loopType	= oldcurData->loopType;
			curData->amp		= oldcurData->amp;
			//	curData->panning	= oldcurData->panning;
			curData->relNote	= oldcurData->relNote;
			
			memcpy( curData->name, oldcurData->name, 32);
			curData->stereo		= oldcurData->stereo;
			
			curData->data 		= malloc( curData->size);
			if( curData->data == NULL) return MADNeedMemory;
			
			memcpy( curData->data, MADPtr + OffSetToSample, curData->size);
			OffSetToSample += curData->size;
			if( curData->amp == 16)
			{
				SInt32 	ll;
				short	*shortPtr = (short*) curData->data;
				
				for( ll = 0; ll < curData->size/2; ll++) PPBE16( &shortPtr[ ll]);
			}

		}
	}
	
	return noErr;
}

static OSErr TestoldMADFile( Ptr AlienFile)
{
	OSType myMADSign = *((OSType*) AlienFile);
	PPBE32(&myMADSign);

	if(	myMADSign == 'MADI') return   noErr;
	else return  MADFileNotSupportedByThisPlug;
}

static OSErr ExtractoldMADInfo( PPInfoRec *info, Ptr AlienFile)
{
	oldMADSpec	*myMOD = ( oldMADSpec*) AlienFile;
	//long		PatternSize;
	short		i;
	//short		tracksNo;
	
	/*** Signature ***/
	
	info->signature = myMOD->MAD;
	PPBE32(&info->signature);
	
	/*** Internal name ***/
	
	myMOD->name[ 31] = '\0';
	strlcpy( info->internalFileName, myMOD->name, sizeof(myMOD->name));
	
	/*** Tracks ***/
	
	info->totalTracks = myMOD->numChn;
		
	/*** Total Patterns ***/
	
	info->totalPatterns = 0;
	for( i = 0; i < 128; i++)
	{
		if( myMOD->oPointers[ i] >= info->totalPatterns)	info->totalPatterns = myMOD->oPointers[ i];
	}
	info->totalPatterns++;
	
	/*** Partition Length ***/
	
	info->partitionLength = myMOD->numPointers;
	
	/*** Total Instruments ***/
	
	info->totalInstruments = myMOD->numInstru;
	
	strlcpy( info->formatDescription, "MADI Plug", sizeof(info->formatDescription));

	return noErr;
}

#ifndef _MAC_H

extern EXP OSErr FillPlug( PlugInfo *p);
extern EXP OSErr PPImpExpMain( OSType order, Ptr AlienFileName, MADMusic *MadFile, PPInfoRec *info, MADDriverSettings *init);

EXP OSErr FillPlug( PlugInfo *p)		// Function USED IN DLL - For PC & BeOS
{
	strlcpy( p->type, 		"MADI", sizeof(p->type));
	strlcpy( p->MenuName, 	"MADI Files", sizeof(p->MenuName));
	p->mode	=	MADPlugImport;
	p->version = 2 << 16 | 0 << 8 | 0;

	return noErr;
}
#endif


/*****************/
/* MAIN FUNCTION */
/*****************/

extern OSErr PPImpExpMain( OSType order, Ptr AlienFileName, MADMusic *MadFile, PPInfoRec *info, MADDriverSettings *init)
{
	OSErr	myErr = noErr;
	Ptr		AlienFile;
	UNFILE	iFileRefI;
	long	sndSize;
	
	switch( order)
	{
		case MADPlugImport:
			iFileRefI = iFileOpenRead( AlienFileName);
			if( iFileRefI)
			{
				sndSize = iGetEOF( iFileRefI);
				
				// ** MEMORY Test Start
				AlienFile = (Ptr)malloc( sndSize * 2L);
				if( AlienFile == NULL) myErr = MADNeedMemory;
				// ** MEMORY Test End
				
				else
				{
					free( AlienFile);
					
					AlienFile = (Ptr)malloc( sndSize);
					myErr = iRead( sndSize , AlienFile, iFileRefI);
					if( myErr == noErr)
					{
						myErr = TestoldMADFile( AlienFile);
						if( myErr == noErr)
						{
							myErr = MADI2Mad( AlienFile, sndSize, MadFile, init);
						}
					}
					free( AlienFile);	AlienFile = NULL;
				}
				iClose( iFileRefI);
			}
			else myErr = MADReadingErr;
			break;
			
		case MADPlugTest:
			iFileRefI = iFileOpenRead( AlienFileName);
			if( iFileRefI)
			{
				sndSize = 5000L;	// Read only 5000 first bytes for optimisation
				
				AlienFile = (Ptr)malloc( sndSize);
				if( AlienFile == NULL) myErr = MADNeedMemory;
				else
				{
					myErr = iRead( sndSize , AlienFile, iFileRefI);
					if(myErr == noErr) myErr = TestoldMADFile( AlienFile);
					
					free( AlienFile);	AlienFile = NULL;
				}
				iClose( iFileRefI);
			}
			else myErr = MADReadingErr;
			break;
			
		case 'INFO':
			iFileRefI = iFileOpenRead( AlienFileName);
			if( iFileRefI)
			{
				info->fileSize = iGetEOF( iFileRefI);
				
				sndSize = 5000L;	// Read only 5000 first bytes for optimisation
				
				AlienFile = (Ptr)malloc( sndSize);
				if( AlienFile == NULL) myErr = MADNeedMemory;
				else
				{
					myErr = iRead( sndSize , AlienFile, iFileRefI);
					if( myErr == noErr)
					{
						myErr = ExtractoldMADInfo( info, AlienFile);
					}
					free( AlienFile);	AlienFile = NULL;
				}
				iClose( iFileRefI);
			}
			else myErr = MADReadingErr;
			break;
			
		default:
			myErr = MADOrderNotImplemented;
			break;
	}
	
	return myErr;
}
