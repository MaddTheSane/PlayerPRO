//
//  MADDriver.c
//  PPMacho
//
//  Created by C.W. Betts on 1/10/14.
//
//

#include "RDriver.h"
#include "RDriverInt.h"
#include "MADDriver.h"

void MADDriverClearChannel(MADDriverRec *theRec, int channel)
{
	if (theRec->base.chan[channel].samplePtr != NULL) {
		theRec->base.chan[channel].curPtr		= theRec->base.chan[channel].maxPtr;
		theRec->base.chan[channel].samplePtr	= NULL;
		theRec->base.chan[channel].lAC			= 0;
		theRec->base.chan[channel].loopBeg		= 0;
		theRec->base.chan[channel].loopSize 	= 0;
	}
}

MADDriverBase *MADDriverGetBase(MADDriverRecPtr theRec)
{
	return &theRec->base;
}

bool MADDriverChannelIsDonePlaying(MADDriverRec *theRec, int channel)
{
	//TODO: more work here!
	return theRec->base.chan[channel].samplePtr == NULL;
}

IntPatData MADPatDataToInt(PatData *inPat)
{
	IntPatData toRet = {0};
	toRet.header = inPat->header;
	toRet.Cmds = inPat->Cmds;
	
	return toRet;
}
