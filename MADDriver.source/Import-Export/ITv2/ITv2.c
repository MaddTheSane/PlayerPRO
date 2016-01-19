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

#if defined(__APPLE__) && !(defined(EMBEDPLUGS) && EMBEDPLUGS)
#include <PlayerPROCore/PlayerPROCore.h>
#else
#include "RDriver.h"
#include "MADFileUtils.h"
#endif
#include "ITv2.h"

#if defined(EMBEDPLUGS) && EMBEDPLUGS
#include "embeddedPlugs.h"
#endif


static inline MADByte _mm_read_UBYTE(char **samplePtr)
{
	MADByte val = *((MADByte*)*samplePtr);
	*samplePtr += 1;
	return val;
}

//TODO: byteswapping?
static inline unsigned short ReadUS(char **samplePtr)
{
	unsigned short result = _mm_read_UBYTE(samplePtr);
	result |= ((unsigned short)_mm_read_UBYTE(samplePtr)) << 8;
	return result;
}


#ifndef _MAC_H
EXP MADErr FillPlug(PlugInfo *p);
EXP MADErr PPImpExpMain(MADFourChar order, char* AlienFileName, MADMusic *MadFile, MADInfoRec *info, MADDriverSettings *init);

EXP MADErr FillPlug(PlugInfo *p)		// Function USED IN DLL - For PC & BeOS
{
	strncpy(p->type, 		"IT  ", sizeof(p->type));
	strncpy(p->MenuName, 	"IT Files", sizeof(p->MenuName));
	p->mode		= MADPlugImport;
	p->version	= 2 << 16 | 0 << 8 | 0;
	
	return MADNoErr;
}
#endif


#if defined(EMBEDPLUGS) && EMBEDPLUGS
MADErr mainIT(MADFourChar order, char *AlienFileName, MADMusic *MadFile, MADInfoRec *info, MADDriverSettings *init)
#else
extern MADErr PPImpExpMain(MADFourChar order, char *AlienFileName, MADMusic *MadFile, MADInfoRec *info, MADDriverSettings *init)
#endif
{
	MADErr	myErr = MADNoErr;
	void*	AlienFile;
	long	sndSize;
	UNFILE	iFileRefI;

	switch (order) {
			
			
		default:
			myErr = MADOrderNotImplemented;
			break;
	}
	
	return myErr;
}
