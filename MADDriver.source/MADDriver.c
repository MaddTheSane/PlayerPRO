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
	if (theRec->chan[channel].samplePtr != NULL) {
		theRec->chan[channel].curPtr 	= theRec->chan[channel].maxPtr;
		theRec->chan[channel].samplePtr	= NULL;
		theRec->chan[channel].lAC		= 0;
		theRec->chan[channel].loopBeg	= 0;
		theRec->chan[channel].loopSize 	= 0;
	}
}

bool MADDriverChannelIsDonePlaying(MADDriverRec *theRec, int channel)
{
	//TODO: more work here!
	return theRec->chan[channel].samplePtr == NULL;
}

IntPatData MADPatDataToInt(PatData *inPat)
{
	IntPatData toRet = {0};
	toRet.header = inPat->header;
	toRet.Cmds = inPat->Cmds;
	
	return toRet;
}
