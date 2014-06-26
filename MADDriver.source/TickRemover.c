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
#include "PPPrivate.h"

void MADTickLoopFill8(Channel *curVoice, int *ASCBuffer1, int *ASCBuffer2, size_t size, short left, short right)
{
	size++;
	while (size-- > 0) {
		*ASCBuffer1 += left;
		ASCBuffer1 += 2;
		*ASCBuffer2 += right;
		ASCBuffer2 += 2;
	}
	curVoice->prevPtr = NULL;
}

void MADTickLoop8(size_t size, Channel *curVoice, int *ASCBuffer1, int *ASCBuffer2, MADDriverRec *intDriver)
{
	int		tLongL, tLongR;
	int		curLevelL = curVoice->curLevelL, curLevelR = curVoice->curLevelR, curLastWordL = curVoice->curLastWordL;
	int		curLastWordR = curVoice->curLastWordR, TICKREMOVESIZE = curVoice->TICKREMOVESIZE;
	bool	LevelDirectionL = curVoice->LevelDirectionL, LevelDirectionR = curVoice->LevelDirectionR, RemoverWorking = curVoice->RemoverWorking;
	
	if (TICKREMOVESIZE == 0)
		return;
	
	while (size-- > 0) {
		if (LevelDirectionL) {
			if (curLevelL > 0)
				curLevelL--;
			else
				RemoverWorking = false;
			tLongL = (curLastWordL * curLevelL) / TICKREMOVESIZE;
		} else {
			if (curLevelL < TICKREMOVESIZE)
				curLevelL++;
			else
				RemoverWorking = false;
			tLongL = (curLastWordL * curLevelL) / TICKREMOVESIZE;
			tLongL -= curLastWordL;
		}
		
		if (LevelDirectionR) {
			if (curLevelR > 0)
				curLevelR--;
			else
				RemoverWorking = false;
			tLongR = (curLastWordR * curLevelR) / TICKREMOVESIZE;
		} else {
			if (curLevelR < TICKREMOVESIZE)
				curLevelR++;
			else
				RemoverWorking = false;
			tLongR = (curLastWordR * curLevelR) / TICKREMOVESIZE;
			tLongR -= curLastWordR;
		}
		
		//	Double ASCBUFFER
		*ASCBuffer1 += tLongL;
		ASCBuffer1 += 2;
		*ASCBuffer2 += tLongR;
		ASCBuffer2 += 2;
	}
	
	curVoice->curLevelL = curLevelL;
	curVoice->curLevelR = curLevelR;
	curVoice->curLastWordL = curLastWordL;
	curVoice->curLastWordR = curLastWordR;
	curVoice->TICKREMOVESIZE = TICKREMOVESIZE;
	curVoice->LevelDirectionL = LevelDirectionL;
	curVoice->LevelDirectionR = LevelDirectionR;
	curVoice->RemoverWorking = RemoverWorking;
}

void MADTickRemoverStart8(Channel *curVoice, int *ASCBuffer1, int *ASCBuffer2, MADDriverRec *intDriver)
{
	int i = intDriver->ASCBUFFER;
	int curDoVol0 = DoVolPanning256(0, curVoice, intDriver, false);
	int curDoVol1 = DoVolPanning256(1, curVoice, intDriver, false);
	
	if(curVoice->prevPtr != curVoice->begPtr ||
	   (curVoice->curPtr >= curVoice->maxPtr && curVoice->loopSize == 0) ||
	   curVoice->prevVol0 != curDoVol0 ||
	   curVoice->prevVol1 != curDoVol1)
	{
		bool	DoIT = false;
		
		curVoice->LevelDirectionL = true;
		curVoice->LevelDirectionR = true;
		
		// Right Channel
		if (curVoice->prevVol0 != curDoVol0 && curVoice->prevPtr	== curVoice->begPtr && curVoice->begPtr != NULL) {
			int diff = curVoice->prevVol0 - curDoVol0;
			
			if (diff > 0) {
				if (curVoice->prevVol0 != 0)
					curVoice->lastWordR -= (curVoice->lastWordR * curDoVol0) / curVoice->prevVol0;
				curVoice->prevVol0	= curDoVol0;
				DoIT = true;
			} else if (diff < 0) {
				if (curVoice->prevVol0 != 0)
					curVoice->lastWordR -= (curVoice->lastWordR * curDoVol0) / curVoice->prevVol0;
				curVoice->lastWordR = -curVoice->lastWordR;
				curVoice->prevVol0	= curDoVol0;
				curVoice->LevelDirectionR = false;
				DoIT = true;
			}
		} else if (curVoice->prevPtr == curVoice->begPtr)
			curVoice->lastWordR = 0;
		
		// Left Channel
		if (curVoice->prevVol1 != curDoVol1 && curVoice->prevPtr == curVoice->begPtr && curVoice->begPtr != NULL) {
			int diff = curVoice->prevVol1 - curDoVol1;
			
			if (diff > 0) {
				if (curVoice->prevVol1 != 0)
					curVoice->lastWordL -= (curVoice->lastWordL * curDoVol1) / curVoice->prevVol1;
				curVoice->prevVol1	= curDoVol1;
				DoIT = true;
			} else if (diff < 0) {
				if (curVoice->prevVol1 != 0)
					curVoice->lastWordL -= (curVoice->lastWordL * curDoVol1) / curVoice->prevVol1;
				curVoice->lastWordL = -curVoice->lastWordL;
				curVoice->prevVol1	= curDoVol1;
				curVoice->LevelDirectionL = false;
				DoIT = true;
			}
		} else if (curVoice->prevPtr == curVoice->begPtr)
			curVoice->lastWordL = 0;
		
		if (curVoice->lastWordL != 0 || curVoice->lastWordR != 0 || DoIT == true) {
			curVoice->TICKREMOVESIZE = intDriver->VSYNC;
			curVoice->TICKREMOVESIZE /= intDriver->finespeed;
			curVoice->TICKREMOVESIZE *= 8000;
			curVoice->TICKREMOVESIZE /= intDriver->VExt;
			
			if (curVoice->LevelDirectionR)
				curVoice->curLevelR = curVoice->TICKREMOVESIZE;
			else
				curVoice->curLevelR = 0;
			
			if (curVoice->LevelDirectionL)
				curVoice->curLevelL = curVoice->TICKREMOVESIZE;
			else
				curVoice->curLevelL = 0;
			
			curVoice->curLastWordR		= curVoice->lastWordR;
			curVoice->curLastWordL		= curVoice->lastWordL;
			curVoice->lastWordR				= 0;
			curVoice->lastWordL				= 0;
			
			curVoice->RemoverWorking = true;
		}
		
		curVoice->prevPtr		= curVoice->begPtr;
		curVoice->prevVol0		= curDoVol0;
		curVoice->prevVol1		= curDoVol1;
	}
	
	if (curVoice->RemoverWorking)
		MADTickLoop8(i, curVoice, ASCBuffer1, ASCBuffer2, intDriver);
}
